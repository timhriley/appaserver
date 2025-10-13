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
#include "appaserver.h"
#include "role.h"
#include "role_folder.h"
#include "security.h"
#include "environ.h"
#include "session.h"

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
	sleep( SESSION_SLEEP_SECONDS );
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

	sleep( SESSION_SLEEP_SECONDS );
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
	sleep( SESSION_SLEEP_SECONDS );
	exit( 1 );
}

void session_folder_permission_message_exit(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *current_ip_address )
{
	char msg[ 1024 ];

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state
	||   !current_ip_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		(char *)0 /* process_name */ );

	snprintf(
		msg,
		sizeof ( msg ),
	"ERROR for %s@%s: You don't have %s permission for %s as role %s.",
		login_name,
		current_ip_address,
		state,
		folder_name,
		role_name );

	appaserver_error_message_file( application_name, login_name, msg );
	printf( "<h3>%s</h3>\n", msg );

	document_close();
	sleep( SESSION_SLEEP_SECONDS );
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

char *session_update_sql(
		const char *session_table,
		char *session_key,
		char *session_last_access_date,
		char *session_last_access_time )
{
	static char update_sql[ 256 ];

	if ( !session_key
	||   !session_last_access_date
	||   !session_last_access_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		update_sql,
		sizeof ( update_sql ),
		"update %s "
		"set last_access_date = '%s', "
		"last_access_time = '%s' "
		"where session = '%s';",
		session_table,
		session_last_access_date,
		session_last_access_time,
		session_key );

	return update_sql;
}

boolean session_remote_ip_address_changed_boolean(
		char *remote_ip_address,
		char *current_ip_address )
{
	return
	( string_strcmp(
		remote_ip_address,
		current_ip_address ) != 0 );
}

char *session_system_string(
		const char *session_select,
		const char *session_table,
		char *where )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh '%s' %s \"%s\" none",
		session_select,
		session_table,
		where );

	return strdup( system_string );
}

SESSION *session_parse(
		boolean appaserver_user_boolean,
		char *string_fetch )
{
	SESSION *session;
	char buffer[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];

	if ( !string_fetch ) return NULL;

	session = session_calloc();

	/* See SESSION_SELECT */
	/* ------------------ */
	piece( buffer, SQL_DELIMITER, string_fetch, 0 );
	session->session_key = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 1 );
	if ( *buffer ) strcpy( full_name, buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 2 );
	if ( *buffer ) strcpy( street_address, buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 3 );
	if ( *buffer ) session->login_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 4 );
	if ( *buffer ) session->login_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 5 );
	if ( *buffer ) session->last_access_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 6 );
	if ( *buffer ) session->last_access_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 7 );
	if ( *buffer ) session->http_user_agent = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 8 );
	if ( *buffer ) session->remote_ip_address = strdup( buffer );

	if ( appaserver_user_boolean )
	{
		if ( ! ( session->appaserver_user =
				appaserver_user_fetch(
					full_name /* stack memory */,
					street_address /* stack memory */,
					0 /* not fetch_role_name_list */ ) ) )
		{
			return NULL;
		}
	}

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
		char *login_name,
		boolean appaserver_user_boolean )
{
	SESSION *session;
	char *current_ip_address;

	session =
		session_parse(
			appaserver_user_boolean,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_fetch(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				appaserver_system_string(
					SESSION_SELECT,
					SESSION_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					session_primary_where(
					      	session_key ) ) ) );

	current_ip_address =
		session_current_ip_address(
			environment_remote_ip_address(
				ENVIRONMENT_REMOTE_ADDR_KEY ) );

	if ( !session )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			current_ip_address );
	}

	session->current_ip_address = current_ip_address;

	session->remote_ip_address_changed_boolean =
		session_remote_ip_address_changed_boolean(
			session->remote_ip_address,
			session->current_ip_address );

	if ( session->remote_ip_address_changed_boolean )
	{
		session_message_ip_address_changed_exit(
			application_name,
			session_key,
			login_name,
			session->remote_ip_address,
			session->current_ip_address );
	}

	session->last_access_date =
		/* --------------------*/
		/* Returns heap memory */
		/* --------------------*/
		session_last_access_date();

	session->last_access_time =
		/* --------------------*/
		/* Returns heap memory */
		/* --------------------*/
		session_last_access_time();

	session->update_sql =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		session_update_sql(
			SESSION_TABLE,
			session_key,
			session->last_access_date,
			session->last_access_time );

	(void)sql_execute(
		SQL_EXECUTABLE,
		(char *)0 /* appaserver_error_filespecification */,
		(LIST *)0 /* sql_list */,
		session->update_sql );

	session->purge_temporary_files_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		session_purge_temporary_files_system_string(
			application_name );

	if ( system(
		session->
			purge_temporary_files_system_string ) ){}

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
				ENVIRONMENT_REMOTE_ADDR_KEY ) );
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
			login_name,
			1 /* appaserver_user_boolean */ );

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
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"session_process_valid(%s) returned false.",
			process_or_set_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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

	if ( list_rewind( role_process_list ) )
	do {
		role_process =
			list_get(
				role_process_list );

		if ( string_strcmp(
				role_process->process_name,
				process_or_set_name ) == 0 )
		{
			return 1;
		}
	} while ( list_next( role_process_list ) );

	if ( list_rewind( role_process_set_member_list ) )
	do {
		role_process_set_member =
			list_get(
				role_process_set_member_list );

		if ( string_strcmp(
				role_process_set_member->process_set_name,
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

char *session_purge_temporary_files_system_string( char *application_name )
{
	static char system_string[ 128 ];

	if ( !application_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"appaserver_purge_temporary_files.sh %s &",
		application_name );

	return system_string;
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
		char *state1,
		char *state2 )
{
	SESSION_FOLDER *session_folder;

	session_folder = session_folder_calloc();

	if ( !argc
	||   !argv )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: argc or argv are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	if ( !application_name
	||   !*application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
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
	||   !state1 )
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

	session_folder->state1 =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			state1 );

	session_folder->state2 =
		/* ------------------------------------------------ */
		/* Returns APPASERVER_UPDATE_STATE, state2, or null */
		/* ------------------------------------------------ */
		session_folder_state2(
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE,
			state1,
			state2 );

	session_folder->session =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			session_folder->application_name,
			session_folder->session_key,
			session_folder->login_name,
			1 /* appaserver_user_boolean */ );

	session_folder->role_folder_list =
		role_folder_list(
			session_folder->role_name,
			session_folder->folder_name );

	if ( !session_folder_valid(
		session_folder->state1,
		session_folder->state2,
		session_folder->folder_name,
		session_folder->role_folder_list ) )
	{
		session_folder_permission_message_exit(
			session_folder->application_name,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			session_folder->state1,
			session_folder->session->current_ip_address );
	}

	return session_folder;
}

boolean session_folder_valid(
		char *state1,
		char *state2,
		char *folder_name,
		LIST *role_folder_list )
{
	if ( !state1
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

	if ( strcmp( state1, APPASERVER_VIEWONLY_STATE ) == 0 )
		state1 = APPASERVER_LOOKUP_STATE;

	if ( role_folder_state_boolean(
		state1,
		folder_name,
		role_folder_list ) )
	{
		return 1;
	}

	if (	state2
	&&	role_folder_state_boolean(
			state2,
			folder_name,
			role_folder_list ) )
	{
		return 1;
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

void session_delete(
		const char *session_table,
		char *application_name,
		char *session_key )
{
	if ( !application_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	(void)sql_execute(
		SQL_EXECUTABLE,
		appaserver_error_filespecification( application_name ),
		(LIST *)0 /* sql_list */,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		session_delete_sql(
			session_table,
			session_key ) );
}

char *session_delete_sql(
		const char *session_table,
		char *session_key )
{
	static char delete_sql[ 128 ];

	if ( !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		delete_sql,
		sizeof ( delete_sql ),
		"delete from %s where session = '%s';",
		session_table,
		session_key );

	return delete_sql;
}

char *session_insert_string(
		char *session_key,
		char *login_date,
		char *login_time,
		char *remote_ip_address,
		char *http_user_agent,
		APPASERVER_USER *appaserver_user )
{
	char insert_string[ 1024 ];

	if ( !session_key
	||   !login_date
	||   !login_time
	||   !remote_ip_address
	||   !http_user_agent )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		insert_string,
		sizeof ( insert_string ),
		"%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
		session_key,
		(appaserver_user) ? appaserver_user->full_name : "",
		(appaserver_user) ? appaserver_user->street_address : "",
		login_date,
		login_time,
		login_date,
		login_time,
		remote_ip_address,
		http_user_agent );

	return strdup( insert_string );
}

void session_insert(
		char *insert_string,
		char *insert_system_string )
{
	FILE *output_pipe;

	if ( !insert_string
	||   !insert_system_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			insert_system_string );

	fprintf(
		output_pipe,
		"%s\n",
		insert_string );

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

	snprintf(
		system_string,
		sizeof ( system_string ),
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

        while ( *ptr && isdigit( *ptr ) ) ptr--;

	if ( *ptr == '_' )
	{
        	*(ptr) = '\0';
	}

        return filename_session;
}

char *session_http_user_agent(
		const int session_user_agent_width,
		char *environment_http_user_agent )
{
	char destination[ 1024 ];

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
	session_login_date();
}

char *session_last_access_time( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	session_login_time();
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
		boolean appaserver_user_boolean )
{
	SESSION *session;

	if ( !application_name
	||   !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	session = session_calloc();

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
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			environment_http_user_agent(
				ENVIRONMENT_HTTP_USER_AGENT_KEY ) );

	session->remote_ip_address =
		/* ---------------------------- */
		/* Returns heap memory or exits */
		/* ---------------------------- */
		session_current_ip_address(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			environment_remote_ip_address(
				ENVIRONMENT_REMOTE_ADDR_KEY ) );

	if ( appaserver_user_boolean )
	{
		session->appaserver_user =
			appaserver_user_login_fetch(
				login_name,
				0 /* not fetch_role_name_list */ );

		if ( !session->appaserver_user )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"appaserver_user_login_fetch(%s) returned empty.",
				login_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !session->appaserver_user->full_name
		||   !*session->appaserver_user->full_name
		||   !session->appaserver_user->street_address
		||   !*session->appaserver_user->street_address )
		{
			char message[ 128 ];
	
			snprintf(
				message,
				sizeof ( message ),
				"appaserver_user is incomplete." );
	
			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	session->insert_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		session_insert_string(
			session->session_key,
			session->login_date,
			session->login_time,
			session->http_user_agent,
			session->remote_ip_address,
			session->appaserver_user );

	session->insert_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		session_insert_system_string(
			SESSION_TABLE,
			SESSION_INSERT );

	return session;
}

char *session_folder_state2(
		const char *appaserver_lookup_state,
		const char *appaserver_update_state,
		char *state1,
		char *state2 )
{
	char *folder_state2 = {0};

	if ( !state1 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"state1 is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( state2 )
	{
		folder_state2 = state2;
	}
	else
	if ( 	strcmp(
			state1,
			appaserver_lookup_state ) == 0 )
	{
		folder_state2 = (char *)appaserver_update_state;
	}

	return folder_state2;
}
