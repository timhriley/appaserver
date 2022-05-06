/* $APPASERVER_HOME/library/session.c			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

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
#include "appaserver_library.h"
#include "piece.h"
#include "application.h"
#include "date.h"
#include "appaserver.h"
#include "sql.h"
#include "appaserver_user.h"
#include "role.h"
#include "role_folder.h"
#include "security.h"
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
		login_name,
		session_key,
		remote_ip_address,
		current_ip_address );

	document_quick_output( application_name );

	sprintf(msg,
		"Warning for %s",
		login_name );

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

	appaserver_output_error_message( application_name, msg, login_name );

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

	document_quick_output( application_name );

	sprintf(msg,
	"Warning for %s@%s: Your session has expired. Please login again.",
		login_name,
		current_ip_address );

	appaserver_output_error_message( application_name, msg, login_name );
	printf( "<h3>%s</h3>\n", msg );

	document_close();
	sleep( SLEEP_SECONDS );
	exit( 1 );
}

SESSION *session_calloc( void )
{
	SESSION *session;

	if ( ! ( session = calloc( 1, sizeof( SESSION ) ) ) )
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

	now = date_now_new( date_get_utc_offset() );

	sprintf( system_string,
		 "echo \"update %s					 "
		 "	 set last_access_date = '%s',			 "
		 "	     last_access_time = '%s'			 "
		 "	 where appaserver_session = '%s';\"		|"
		 "sql 1>&2 &						 ",
		 SESSION_TABLE,
		 date_display_yyyy_mm_dd( now ),
		 date_display_hhmm( now ),
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

LIST *session_list_fetch( char *login_name )
{
	return session_system_list(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_system_string(
				SESSION_SELECT,
				SESSION_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_user_primary_where(
					login_name ) ) );
}

SESSION *session_parse(	char *input )
{
	SESSION *session;
	char piece_buffer[ 1024 ];

	if ( !input || !*input ) return (SESSION *)0;

	session = session_calloc();

	/* See SESSION_SELECT */
	/* ------------------ */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	session->session_key = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	session->login_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	session->login_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	session->login_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	session->last_access_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	session->last_access_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	session->http_user_agent = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	session->remote_ip_address = strdup( piece_buffer );

	return session;
}

char *session_primary_where( char *session_key )
{
	static char where[ 128 ];

	sprintf(where,
		"appaserver_session = '%s'",
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
	||   strcmp( session->login_name, login_name ) != 0 )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_current_ip_address(
				environment_remote_ip_address() ) );
	}

	session->application_name = application_name;

	session->current_ip_address =
		/* ---------------------------- */
		/* Returns heap memory or exits */
		/* ---------------------------- */
		session_current_ip_address(
				environment_remote_ip_address() );

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
			environment_remote_ip_address() );
	}

	session_environment_set( application_name );

	appaserver_error_argv_append_file(
		argc, argv, application_name );

	session_process = session_process_calloc();

	session_process->session =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			application_name,
			session_key,
			login_name );

	session_process->security_sql_injection_escape_role_name =
		security_sql_injection_escape(
			role_name );

	if ( !session_process_valid(
		security_sql_injection_escape(
			process_or_set_name ),
		role_process_list(
			session_process->
				security_sql_injection_escape_role_name ),
		role_process_set_list(
			session_process->
				security_sql_injection_escape_role_name ) ) )
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
			char *process_name,
			LIST *role_process_list )
{
	ROLE_PROCESS *role_process;

	if ( !list_rewind( role_process_list ) ) return 0;

	do {
		role_process =
			list_get(
				role_process_list );

		if ( strcmp(	role_process->process_name,
				process_name ) == 0 )
		{
			return 1;
		}
	} while ( list_next( role_process_list ) );

	return 0;
}

void session_environment_set( char *application_name )
{
	environment_set( "APPASERVER_DATABASE", application_name );
	environ_src_appaserver_to_path();
	environ_utility_to_path();
	environ_local_bin_to_path();
	environ_relative_source_directory_to_path( application_name );
	environ_append_dot_to_path();
	environ_appaserver_home();
	environ_set_utc_offset();
	environ_umask();
}

char *session_login_name( char *session_key )
{
	SESSION *session;

	if ( ! ( session =
			session_parse(
				string_pipe_fetch(
					session_system_string(
						SESSION_SELECT,
						SESSION_TABLE,
						session_primary_where(
							session_key ) ) ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: session_parse(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			session_key );
		exit( 1 );
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

	if ( ! ( escape =
			/* Returns heap memory or null */
			/* --------------------------- */
			security_sql_injection_escape(
				application_name ) ) )
	{
		return 0;
	}

	if ( strcmp( application_name, escape ) != 0 )
		return 0;
	else
		free( escape );

	if ( ! ( escape =
			security_sql_injection_escape(
				session_key ) ) )
	{
		return 0;
	}

	if ( strcmp( session_key, escape ) != 0 )
		return 0;
	else
		free( escape );

	if ( ! ( escape =
			security_sql_injection_escape(
				login_name ) ) )
	{
		return 0;
	}

	if ( strcmp( login_name, escape ) != 0 )
		return 0;
	else
		free( escape );

	if ( ! ( escape =
			security_sql_injection_escape(
				role_name ) ) )
	{
		return 0;
	}

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

	if ( !session_sql_injection_strcmp_okay(
			application_name,
			session_key,
			login_name,
			role_name ) )
	{
		session_sql_injection_message_exit(
			argv[ 0 ],
			environment_remote_ip_address() );
	}

	session_environment_set( application_name );

	appaserver_error_argv_append_file(
		argc, argv, application_name );

	session_folder = session_folder_calloc();

	session_folder->session =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			application_name,
			session_key,
			login_name );

	if ( session_folder_valid(
			security_sql_injection_escape(
				state ),
			role_folder_list(
				security_sql_injection_escape(
					role_name ),
				security_sql_injection_escape(
					folder_name ) ) ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_folder->session->current_ip_address );
	}

	return session_folder;
}

boolean session_folder_valid(
			char *state,
			LIST *role_folder_list )
{
	if ( !state || !*state )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: state is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( role_folder_list ) ) return 0;

	if ( strcmp( state, APPASERVER_VIEWONLY_STATE ) == 0 )
		state = APPASERVER_LOOKUP_STATE;

	if ( strcmp( state, APPASERVER_LOOKUP_STATE ) == 0 )
	{
		if ( role_folder_update( role_folder_list )
		||   role_folder_lookup( role_folder_list ) )
		{
			return 1;
		}
	}

	if ( strcmp( state, APPASERVER_UPDATE_STATE ) == 0 )
	{
		if ( role_folder_update( role_folder_list ) )
		{
			return 1;
		}
	}

	if ( strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
	{
		if ( role_folder_insert( role_folder_list ) )
		{
			return 1;
		}
	}

	return 0;
}

SESSION_FOLDER *session_folder_calloc( void )
{
	SESSION_FOLDER *session_folder;

	if ( ! ( session_folder = calloc( 1, sizeof( SESSION_FOLDER ) ) ) )
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

	if ( ! ( session_process = calloc( 1, sizeof( SESSION_PROCESS ) ) ) )
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

void session_insert(	char *session_key,
			char *login_name,
			char *date_now_yyyy_mm_dd_string,
			char *date_now_hhmm_string,
			char *http_user_agent,
			char *remote_ip_address )
{
	char *field_list_string;
	FILE *output_pipe;
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !date_now_yyyy_mm_dd_string
	||   !date_now_hhmm_string
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

	field_list_string =
"appaserver_session,login_name,login_date,login_time,last_access_date,last_access_time,http_user_agent,remote_ip_address";

	sprintf( system_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 SESSION_TABLE,
		 field_list_string );

	output_pipe = popen( system_string, "w" );

	fprintf(output_pipe,
		"%s|%s|%s|%s|%s|%s|%s|%s\n",
		session_key,
		login_name,
		date_now_yyyy_mm_dd_string,
		date_now_hhmm_string,
		date_now_yyyy_mm_dd_string,
		date_now_hhmm_string,
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
