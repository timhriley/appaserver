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
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "folder.h"
#include "document.h"
#include "appaserver_library.h"
#include "piece.h"
#include "application.h"
#include "date.h"
#include "sql.h"
#include "appaserver_user.h"
#include "role.h"
#include "role_folder.h"
#include "session.h"

#define SLEEP_SECONDS	2

void session_message_ip_address_changed_exit(
			char *application_name,
			char *session_key,
			char *remote_ip_address,
			char *current_ip_address,
			char *login_name )
{
	APPASERVER_PARAMETER *appaserver_parameter;
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

	appaserver_parameter = appaserver_parameter_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter->
			appaserver_mount_point );

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

void session_access_failed_message_exit(
			char *application_name,
			char *current_ip_address,
			char *login_name )
{
	APPASERVER_PARAMETER *appaserver_parameter;
	char msg[ 1024 ];

	appaserver_parameter = appaserver_parameter_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter->
			appaserver_mount_point );

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
		 SESSION_TABLE_NAME,
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

char *session_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" none",
		SESSION_TABLE_NAME,
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

	/* See attribute_list appaserver_sessions */
	/* -------------------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	session->session_key = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	session->login_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	session->login_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	session->last_access_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	session->last_access_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	session->http_user_agent = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
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
			char *sql_injection_escape_application_name,
			char *sql_injection_escape_session_key,
			char *integrity_check_login_name )
{
	SESSION *session;

	environment_set(
		"APPASERVER_DATABASE",
		sql_injection_escape_application_name );

	session =
	   session_parse(
		string_pipe_fetch(
			session_system_string(
				session_primary_where(
				      sql_injection_escape_session_key ) ) ) );

	/* Always succeeds */
	/* --------------- */
	if ( !session ) session = session_calloc();

	session->session_current_ip_address =
		/* ---------------------------- */
		/* Returns heap memory or exits */
		/* ---------------------------- */
		session_current_ip_address();

	if ( !session->login_name ) return session;

	if ( string_strcmp(
		session->login_name,
		integrity_check_login_name ) != 0 )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: string_strcmp(%s,%s) returned missmatched.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			session->login_name,
			integrity_check_login_name );

		return (SESSION *)0;
	}

	session->sql_injection_escape_application_name =
		sql_injection_escape_application_name;

	session->sql_injection_escape_session_key =
		sql_injection_escape_session_key;

	return session;
}

char *session_current_ip_address( void )
{
	char *current_ip_address;

	if ( ! ( current_ip_address =
			environment_get(
				SESSION_REMOTE_IP_ADDRESS_VARIABLE ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: environment_get(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			SESSION_REMOTE_IP_ADDRESS_VARIABLE );
		exit( 1 );
	}

	return current_ip_address;
}

SESSION *session_folder_integrity_exit(
			int argc,
			char **argv,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *state )
{
	SESSION *session;

	session =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		session_fetch(
			/* ------------------------ */
			/* Sets APPASERVER_DATABASE */
			/* ------------------------ */
			security_sql_injection_escape(
				application_name ),
			security_sql_injection_escape(
				session_key ),
			login_name
				/* integrity_check_login_name */ );

	/* No row in SESSION */
	/* ----------------- */
	if ( !session->login_name )
	{
		session_access_failed_message_exit(
			application_name,
			session->session_current_ip_address,
			login_name );
	}

	appaserver_error_argv_append_file(
		argc,
		argv,
		session->sql_injection_escape_application_name );

	if ( session_remote_ip_address_changed(
		session->remote_ip_address,
		session->session_current_ip_address ) )
	{
		session_message_ip_address_changed_exit(
			session->sql_injection_escape_application_name,
			session->session_key,
			session->remote_ip_address,
			session->session_current_ip_address,
			session->login_name );
	}

	session->sql_injection_escape_role_name =
		security_sql_injection_escape(
			role_name );

	session->sql_injection_escape_folder_name =
		security_sql_injection_escape(
			folder_name );

	if ( !role_appaserver_user_fetch(
		session->login_name,
		session->sql_injection_escape_role_name ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: for IP=%s, role_appaserver_user_fetch(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			session->session_current_ip_address,
			session->login_name,
			session->sql_injection_escape_role_name );

		session_access_failed_message_exit(
			session->sql_injection_escape_application_name,
			session->session_current_ip_address,
			session->login_name );
	}

	session->session_state_integrity =
		/* -------------------------------------- */
		/* Returns state, program memory, or null */
		/* -------------------------------------- */
		session_state_integrity(
			state,
			role_folder_fetch_list(
				session->sql_injection_escape_role_name,
				session->sql_injection_escape_folder_name ) );

	if ( !session->session_state_integrity )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: session_state_integrity(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			state );

		session_access_failed_message_exit(
			session->sql_injection_escape_application_name,
			session->session_current_ip_address,
			session->login_name );
	}

	session_environment_set(
		session->sql_injection_escape_application_name );

	session_update_access_date_time( session->session_key );

	appaserver_library_purge_temporary_files(
		session->sql_injection_escape_application_name );

	return session;
}

char *session_state_integrity(
			char *state,
			LIST *role_folder_list )
{
	if ( !state || !*state ) return (char *)0;

	if ( !list_length( role_folder_list ) ) return (char *)0;

	if ( strcmp( state, "view" ) == 0 ) state = "lookup";

	if ( strcmp( state, "lookup" ) == 0 )
	{
		if ( role_folder_update( role_folder_list )
		||   role_folder_lookup( role_folder_list ) )
		{
			return state;
		}
	}

	if ( strcmp( state, "update" ) == 0 )
	{
		if ( role_folder_update( role_folder_list ) )
		{
			return state;
		}
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		if ( role_folder_insert( role_folder_list ) )
		{
			return state;
		}
	}

	return (char *)0;
}

void session_environment_set(
			char *application_name )
{
	environ_set_utc_offset( application_name );
	add_relative_source_directory_to_path( application_name );
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_local_bin_to_path();
	environ_appaserver_home();
	environ_prepend_dot_to_path();
	environ_umask();

}
