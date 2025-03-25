/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/session.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "list.h"
#include "environ.h"
#include "String.h"
#include "appaserver_error.h"
#include "folder.h"
#include "document.h"
#include "piece.h"
#include "application.h"
#include "date.h"
#include "appaserver.h"
#include "sql.h"
#include "appaserver_user.h"
#include "role.h"
#include "role_folder.h"
#include "security.h"
#include "environ.h"
#include "session.h"

#define SLEEP_SECONDS	2

void session_message_ip_address_changed_exit(
		char *application_name,
		char *session_key,
		char *login_name,
		char *remote_ip_address,
		char *current_ip_address )
{
	char msg[ 1024 ];

	fprintf(stderr,
"ERROR in %s/%s()/%d: IP address changed for application=%s, login_name=%s, session_key=%s, remote_ip_address=%s, current_ip_address=%s\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		application_name,
		(login_name) ? login_name : "[login_name missing]",
		session_key,
		remote_ip_address,
		current_ip_address );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		(char *)0 /* process_name */ );

	sprintf(msg,
		"Warning for %s",
		(login_name) ? login_name : "[login_name missing]" );

	if (	remote_ip_address
	&&	*remote_ip_address
	&&	isdigit( *remote_ip_address ) )
	{
		sprintf( msg + strlen( msg ),
			 "@%s",
		 	remote_ip_address );
	}

	sprintf( msg + strlen( msg ),
		 ": A security trigger occurred. Please login again." );

	appaserver_error_message_file( application_name, login_name, msg );

	printf( "<h3>%s</h3>\n", msg );

	document_close();
	sleep( SLEEP_SECONDS );
	exit( 1 );
}

void session_sql_injection_message_exit(
		char *argv_0,
		char *environment_remote_ip_address )
{
	fprintf(stderr,
	"ERROR in %s/%s()/%d: %s had an illegal character sent in from %s.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		argv_0,
		environment_remote_ip_address );

	sleep( SLEEP_SECONDS );
	exit( 1 );
}

void session_access_failed_message_exit(
		char *application_name,
		char *login_name,
		char *current_ip_address )
{
	char msg[ 1024 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		(char *)0 /* process_name */ );

	sprintf(msg,
	"Warning for %s@%s: Your session has expired. Please login again.",
		login_name,
		current_ip_address );

	appaserver_error_message_file( application_name, login_name, msg );
	printf( "<h3>%s</h3>\n", msg );

	document_close();
	sleep( SLEEP_SECONDS );
	exit( 1 );
}

SESSION *session_calloc( void )
{
	SESSION *session;

	if ( ! ( session = calloc( 1, sizeof ( SESSION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}

	return session;
}

void session_update_access_date_time( char *session )
{
	DATE *now;
	char system_string[ 1024 ];

	now = date_now_new( date_utc_offset() );

	sprintf( system_string,
		 "echo \"update %s					 "
		 "	 set last_access_date = '%s',			 "
		 "	     last_access_time = '%s'			 "
		 "	 where session = '%s';\"			|"
		 "sql 1>&2 &						 ",
		 SESSION_TABLE,
		 date_yyyy_mm_dd_static_display( now ),
		 date_hhmm_static_display( now ),
		 session );

	if ( system( system_string ) ){}
}

boolean session_remote_ip_address_changed(
		char *remote_ip_address,
		char *current_ip_address )
{
	return ( string_strcmp(
			remote_ip_address,
			current_ip_address ) != 0 );
}

LIST *session_system_list( char *system_string )
{
	FILE *pipe;
	SESSION *session;
	char input[ 1024 ];
	LIST *list = {0};

	pipe = popen( system_string, "r" );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( session = session_parse( input ) ) )
		{
			if ( !list ) list = list_new();
			list_set( list, session );
		}
	}

	pclose( pipe );
	return list;
}

char *session_system_string(
		char *session_select,
		char *session_table,
		char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" none",
		session_select,
		session_table,
		where );

	return strdup( system_string );
}

SESSION *session_parse(	char *input )
{
	SESSION *session;
	char buffer[ 1024 ];

	if ( !input || !*input ) return (SESSION *)0;

	session = session_calloc();

	/* See SESSION_SELECT */
	/* ------------------ */
	piece( buffer, SQL_DELIMITER, input, 0 );
	session->session_key = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) session->login_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) session->login_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) session->login_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) session->last_access_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) session->last_access_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) session->http_user_agent = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) session->remote_ip_address = strdup( buffer );

	return session;
}

char *session_primary_where( char *session_key )
{
	static char where[ 128 ];
	char *escape;

	if ( !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	escape =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			session_key /* datum */ );

	if ( strcmp( escape, session_key ) != 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid session_key=[%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			session_key );
		exit( 1 );
	}

	sprintf(where,
		"session = '%s'",
		session_key );

	return where;
}

SESSION *session_fetch(
		char *application_name,
		char *session_key,
		char *login_name )
{
	SESSION *session;

	session =
		session_parse(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				session_system_string(
					SESSION_SELECT,
					SESSION_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					session_primary_where(
					      	session_key ) ) ) );

	if ( !session
	||   ( login_name
	&&     string_strcmp(
		session->login_name,
		login_name ) != 0 ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_current_ip_address(
				environment_remote_ip_address(
					ENVIRONMENT_REMOTE_KEY ) ) );
	}

	session->application_name = application_name;

	session->current_ip_address =
		/* ---------------------------- */
		/* Returns heap memory or exits */
		/* ---------------------------- */
		session_current_ip_address(
			environment_remote_ip_address(
				ENVIRONMENT_REMOTE_KEY ) );

	if ( session_remote_ip_address_changed(
		session->remote_ip_address,
		session->current_ip_address ) )
	{
		session_message_ip_address_changed_exit(
			application_name,
			session_key,
			login_name,
			session->remote_ip_address,
			session->current_ip_address );
	}

	session_update_access_date_time( session_key );
	session_purge_temporary_files( application_name );

	return session;
}

char *session_current_ip_address( char *environment_remote_ip_address )
{
	if ( !environment_remote_ip_address
	||   !*environment_remote_ip_address )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: environment_remote_ip_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return environment_remote_ip_address;
}

SESSION_PROCESS *session_process_integrity_exit(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name )
{
	SESSION_PROCESS *session_process;

	if ( !session_sql_injection_strcmp_okay(
		application_name,
		session_key,
		login_name,
		role_name ) )
	{
		session_sql_injection_message_exit(
			argv[ 0 ],
			environment_remote_ip_address(
				ENVIRONMENT_REMOTE_KEY ) );
	}

	session_environment_set( application_name );

	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		login_name );

	session_process = session_process_calloc();

	session_process->session =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			application_name,
			session_key,
			login_name );

	session_process->role_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			role_name /* datum */ );

	if ( !session_process_valid(
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			process_or_set_name ),
		role_process_list(
			session_process->role_name,
			0 /* not fetch_process */ ),
		role_process_set_member_list(
			session_process->role_name,
			0 /* not fetch_process_set */ ) ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_process->session->current_ip_address );
	}

	session_process->process_name =
		process_name_fetch(
			process_or_set_name );

	if ( !session_process->process_name )
	{
		session_process->process_set_name = process_or_set_name;
	}

	return session_process;
}

boolean session_process_valid(
		char *process_or_set_name,
		LIST *role_process_list,
		LIST *role_process_set_member_list )
{
	ROLE_PROCESS *role_process;
	ROLE_PROCESS_SET_MEMBER *role_process_set_member;

	if ( !list_rewind( role_process_list )
	&&   !list_rewind( role_process_set_member_list ) )
	{
		return 0;
	}

	do {
		role_process =
			list_get(
				role_process_list );

		if ( strcmp(	role_process->process_name,
				process_or_set_name ) == 0 )
		{
			return 1;
		}
	} while ( list_next( role_process_list ) );

	do {
		role_process_set_member =
			list_get(
				role_process_set_member_list );

		if ( strcmp(	role_process_set_member->process_set_name,
				process_or_set_name ) == 0 )
		{
			return 1;
		}
	} while ( list_next( role_process_set_member_list ) );

	return 0;
}

void session_environment_set( char *application_name )
{
	environment_session_set( application_name );
}

char *session_login_name( char *session_key )
{
	SESSION *session;

	if ( ! ( session =
			session_parse(
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				string_pipe_fetch(
					session_system_string(
						SESSION_SELECT,
						SESSION_TABLE,
						session_primary_where(
							session_key ) ) ) ) ) )
	{
		return NULL;
	}

	return session->login_name;
}

void session_purge_temporary_files( char *application_name )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "appaserver_purge_temporary_files.sh %s &",
		 application_name );

	if ( system( sys_string ) ){};
}

boolean session_sql_injection_strcmp_okay(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name )
{
	char *escape;

	escape =
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name /* datum */ );

	if ( !escape ) return 0;

	if ( strcmp( application_name, escape ) != 0 )
		return 0;
	else
		free( escape );

	escape =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			session_key /* datum */ );

	if ( !escape ) return 0;

	if ( strcmp( session_key, escape ) != 0 )
		return 0;
	else
		free( escape );

	escape =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			login_name /* datum */ );

	if ( !escape ) return 0;

	if ( strcmp( login_name, escape ) != 0 )
		return 0;
	else
		free( escape );

	escape =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			role_name /* datum */ );

	if ( !escape ) return 0;

	if ( strcmp( role_name, escape ) != 0 )
		return 0;
	else
		free( escape );

	return 1;
}

SESSION_FOLDER *session_folder_integrity_exit(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state )
{
	SESSION_FOLDER *session_folder;

	session_folder = session_folder_calloc();

	if ( !argc
	||   !argv
	||   !application_name
	||   !*application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	session_folder->application_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name );

	session_environment_set( session_folder->application_name );

	appaserver_error_argv_append_file(
		argc,
		argv,
		session_folder->application_name );

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	session_folder->session_key =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			session_key );

	session_folder->login_name =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			login_name );

	session_folder->role_name =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			role_name );

	session_folder->folder_name =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name );

	session_folder->state =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			state );

	session_folder->session =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			session_folder->application_name,
			session_folder->session_key,
			session_folder->login_name );

	session_folder->role_folder_list =
		role_folder_list(
			session_folder->role_name,
			session_folder->folder_name );

	if ( !session_folder_valid(
		session_folder->state,
		session_folder->folder_name,
		session_folder->role_folder_list ) )
	{
		session_access_failed_message_exit(
			session_folder->application_name,
			session_folder->login_name,
			session_folder->session->current_ip_address );
	}

	return session_folder;
}

boolean session_folder_valid(
		char *state,
		char *folder_name,
		LIST *role_folder_list )
{
	if ( !state
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( role_folder_list ) ) return 0;

	if ( strcmp( state, APPASERVER_VIEWONLY_STATE ) == 0 )
		state = ROLE_PERMISSION_LOOKUP;

	if ( strcmp( state, ROLE_PERMISSION_LOOKUP ) == 0 )
	{
		if ( role_folder_lookup_boolean(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			folder_name,
			role_folder_list ) )
		{
			return 1;
		}
	}

	if ( strcmp( state, APPASERVER_UPDATE_STATE ) == 0 )
	{
		if ( role_folder_update_boolean(
			ROLE_PERMISSION_UPDATE,
			folder_name,
			role_folder_list ) )
		{
			return 1;
		}
	}

	if ( strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
	{
		if ( role_folder_insert_boolean(
			ROLE_PERMISSION_INSERT,
			folder_name,
			role_folder_list ) )
		{
			return 1;
		}
	}

	return 0;
}

SESSION_FOLDER *session_folder_calloc( void )
{
	SESSION_FOLDER *session_folder;

	if ( ! ( session_folder = calloc( 1, sizeof ( SESSION_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}

	return session_folder;
}

SESSION_PROCESS *session_process_calloc( void )
{
	SESSION_PROCESS *session_process;

	if ( ! ( session_process = calloc( 1, sizeof ( SESSION_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}

	return session_process;
}

void session_delete( char *session_key )
{
	char system_string[ 1024 ];

	if ( !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"echo \"delete from %s where session = '%s';\" | sql",
		SESSION_TABLE,
		session_key );

	if ( system( system_string ) ){}
}

void session_insert(
		const char *session_table,
		const char *session_insert_columns,
		char *session_key,
		char *login_name,
		char *login_date,
		char *login_time,
		char *http_user_agent,
		char *remote_ip_address )
{
	FILE *output_pipe;

	if ( !session_key
	||   !login_time
	||   !login_time
	||   !http_user_agent
	||   !remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_insert_system_string(
				session_table,
				session_insert_columns ) );

	fprintf(output_pipe,
		"%s|%s|%s|%s|%s|%s|%s|%s\n",
		session_key,
		(login_name) ? login_name : "",
		login_date,
		login_time,
		login_date,
		login_time,
		http_user_agent,
		remote_ip_address );

	pclose( output_pipe );
}

char *session_key( char *application_name )
{
	char system_string[ 128 ];
	char *key;

	if ( !application_name || !*application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"session_number_new.sh %s",
		application_name );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( key = string_pipe_fetch( system_string ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: string_pipe_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );
		exit( 1 );
	}

	return key;
}

char *session_right_trim( char *filename_session )
{
        char *ptr;

        if ( !filename_session || !*filename_session ) return filename_session;

        ptr = filename_session + strlen( filename_session ) - 1;

        while ( *ptr && isdigit( *ptr ) )
        	ptr--;

	if ( *ptr == '_' )
	{
        	*(ptr) = '\0';
	}

        return filename_session;
}

void session_access_or_exit(
		char *application_name,
		char *session_key,
		char *login_name )
{
	(void)session_fetch(
		application_name,
		session_key,
		login_name );
}

char *session_http_user_agent(
		const int session_user_agent_width,
		char *environment_http_user_agent )
{
	char destination[ 81 ];

	if ( !environment_http_user_agent )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: environment_http_user_agent is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	strdup(
		/* Returns destination */
		/* ------------------- */
		string_left(
			destination,
			environment_http_user_agent,
			session_user_agent_width /* how_many */ ) );
}

char *session_login_date( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_yyyy_mm_dd(
		date_utc_offset() );
}

char *session_login_time( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_hhmm(
		date_utc_offset() );
}

char *session_last_access_date( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_yyyy_mm_dd(
		date_utc_offset() );
}

char *session_last_access_time( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_hhmm(
		date_utc_offset() );
}

char *session_insert_system_string(
		const char *session_table,
		const char *session_insert_columns )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement.e table=%s field=\"%s\" |"
		"tee_appaserver.sh |"
		"sql.e",
		session_table,
		session_insert_columns );

	return strdup( system_string );
}

SESSION *session_new(
		char *application_name,
		char *login_name,
		char *environment_http_user_agent,
		char *environment_remote_ip_address )
{
	SESSION *session;

	if ( !application_name
	||   !environment_http_user_agent
	||   !environment_remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	session = session_calloc();

	session->application_name = application_name;
	session->login_name = login_name;
	session->environment_http_user_agent = environment_http_user_agent;
	session->remote_ip_address = environment_remote_ip_address;

	/* Returns heap memory */
	/* ------------------- */
	session->session_key = session_key( application_name );

	/* Returns heap memory */
	/* --------------------*/
	session->login_date = session_login_date();

	/* Returns heap memory */
	/* --------------------*/
	session->login_time = session_login_time();

	session->http_user_agent =
		/* --------------------*/
		/* Returns heap memory */
		/* --------------------*/
		session_http_user_agent(
			SESSION_USER_AGENT_WIDTH,
			environment_http_user_agent );

	return session;
}

