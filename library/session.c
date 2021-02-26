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
#include "session.h"
#include "environ.h"
#include "timlib.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "folder.h"
#include "document.h"
#include "appaserver_library.h"
#include "piece.h"
#include "application.h"
#include "date.h"

#define SLEEP_SECONDS	2

static SESSION *global_session = {0};

int session_access_folder(
			char *application_name,
			char *session,
			char *folder_name_comma_list_string,
			char *role_name,
			char *permission )
{
	char folder_name[ 128 ];
	int access_ok = 0;
	int i;

	if ( strcmp( permission, "view" ) == 0 )
		permission = "lookup";

	for(	i = 0;
		piece( folder_name, ',', folder_name_comma_list_string, i );
		i++ )
	{
		access_ok = session_get_session_folder_access_ok(
					application_name,
					session,
					folder_name,
					role_name,
					permission );

		if ( !access_ok && strcmp( permission, "lookup" ) == 0 )
		{
			access_ok = session_get_session_folder_access_ok(
						application_name,
						session,
						folder_name,
						role_name,
						"update" );
		}

		if ( !access_ok
		&& ( strcmp( permission, "update" ) == 0
		||   strcmp( permission, "lookup" ) == 0 ) )
		{
			access_ok = session_get_session_folder_access_ok(
						application_name,
						session,
						folder_name,
						role_name,
						"insert" );
		}
	
		if ( !access_ok ) return 0;
	}

	return access_ok;
}

int session_get_session_folder_access_ok(
					char *application_name,
					char *session,
					char *folder_name,
					char *role_name,
					char *permission )
{
	char sys_string[ 1024 ];
	char *results_string;
	char *appaserver_sessions_table;
	char *role_appaserver_user_table;
	char *role_folder_table;

	appaserver_sessions_table =
		get_table_name(	application_name,
				"appaserver_sessions" );

	role_appaserver_user_table =
		get_table_name(	application_name,
				"role_appaserver_user" );

	role_folder_table =
		get_table_name(	application_name,
				"role_folder" );

	sprintf(sys_string,
		"echo \"select count(*)					 "
		"	from %s,%s,%s					 "
		"	where %s.login_name = %s.login_name		 "
		"	  and %s.role = %s.role				 "
		"	  and %s.permission = '%s'			 "
		"	  and %s.appaserver_session = '%s'		 "
		"	  and %s.folder = '%s'				 "
		"	  and %s.role = '%s';\"				|"
		"sql.e							 ",
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_folder_table,
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_appaserver_user_table,
		role_folder_table,
		role_folder_table,
		permission,
		appaserver_sessions_table,
		timlib_sql_injection_escape( session ),
		role_folder_table,
		timlib_sql_injection_escape( folder_name ),
		role_folder_table,
		timlib_sql_injection_escape( role_name ) );

	results_string = pipe2string( sys_string );

	if ( !results_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot fetch from sys_string\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return ( atoi( results_string ) >= 1 );
} /* session_get_session_folder_access_ok() */

int session_get_session_process_access_ok(
					char *application_name,
					char *login_name,
					char *session,
					char *process_name,
					char *role_name )
{
	char sys_string[ 2048 ];
	char *results_string;
	char *appaserver_sessions_table;
	char *role_appaserver_user_table;
	char *role_process_table;
	char login_name_and_clause[ 128 ];

	appaserver_sessions_table =
		get_table_name(	application_name,
				"appaserver_sessions" );

	role_appaserver_user_table =
		get_table_name(	application_name,
				"role_appaserver_user" );

	role_process_table =
		get_table_name(	application_name,
				"role_process" );

	if ( login_name && *login_name )
	{
		sprintf(	login_name_and_clause,
				" and %s.login_name = '%s'",
				appaserver_sessions_table,
				login_name );
	}
	else
	{
		*login_name_and_clause = '\0';
	}

	if ( role_name && *role_name )
	{
		sprintf(sys_string,
		"echo \"select count(*)					 "
		"	from %s,%s,%s					 "
		"	where %s.login_name = %s.login_name		 "
		"	  and %s.role = %s.role				 "
		"	  and %s.appaserver_session = '%s'		 "
		"	  and %s.process = '%s'				 "
		"	  and %s.role = '%s'				 "
		"	  %s;\"						|"
		"sql.e							 ",
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_process_table,
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_appaserver_user_table,
		role_process_table,
		appaserver_sessions_table,
		timlib_sql_injection_escape( session ),
		role_process_table,
		timlib_sql_injection_escape( process_name ),
		role_appaserver_user_table,
		timlib_sql_injection_escape( role_name ),
		login_name_and_clause );
	}
	else
	{
		sprintf(sys_string,
		"echo \"select count(*)					 "
		"	from %s,%s,%s					 "
		"	where %s.login_name = %s.login_name		 "
		"	  and %s.role = %s.role				 "
		"	  and %s.appaserver_session = '%s'		 "
		"	  and %s.process = '%s'				 "
		"	  %s;\"						|"
		"sql.e							 ",
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_process_table,
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_appaserver_user_table,
		role_process_table,
		appaserver_sessions_table,
		timlib_sql_injection_escape( session ),
		role_process_table,
		timlib_sql_injection_escape( process_name ),
		login_name_and_clause );
	}

	results_string = pipe2string( sys_string );

	if ( !results_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): 1) cannot fetch from sys_string\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	/* Maybe, its a process set */
	/* ------------------------ */
	if ( !atoi( results_string ) && role_name && *role_name )
	{
		char *role_process_set_member_table;

		role_process_set_member_table =
			get_table_name(	application_name,
					"role_process_set_member" );

		sprintf(sys_string,
		"echo \"select count(*)					 "
		"	from %s,%s,%s					 "
		"	where %s.login_name = %s.login_name		 "
		"	  and %s.role = %s.role				 "
		"	  and %s.appaserver_session = '%s'		 "
		"	  and %s.role = '%s'				 "
		"	  and %s.login_name = '%s'			 "
		"	  and %s.process_set = '%s';\"			|"
		"sql.e							 ",
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_process_set_member_table,
		appaserver_sessions_table,
		role_appaserver_user_table,
		role_appaserver_user_table,
		role_process_set_member_table,
		appaserver_sessions_table,
		timlib_sql_injection_escape( session ),
		role_appaserver_user_table,
		timlib_sql_injection_escape( role_name ),
		appaserver_sessions_table,
		timlib_sql_injection_escape( login_name ),
		role_process_set_member_table,
		timlib_sql_injection_escape( process_name ) );

		results_string = pipe2string( sys_string );
		if ( !results_string )
		{
			fprintf( stderr,
			 "ERROR in %s/%s(): 2) cannot fetch from sys_string\n",
			 __FILE__,
			 __FUNCTION__ );
			exit( 1 );
		}
	}

	return ( atoi( results_string ) >= 1 );

} /* session_get_session_process_access_ok() */

void session_message_ip_address_changed_exit(
				char *application_name,
				char *login_name )
{
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char msg[ 1024 ];
	char *remote_ip_address;

	remote_ip_address =
		environ_get_environment(
			SESSION_REMOTE_IP_ADDRESS_VARIABLE );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
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

}  /* session_message_ip_address_changed_exit() */

void session_access_failed_message_and_exit(	char *application_name,
						char *session_key,
						char *login_name )
{
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char msg[ 1024 ];
	char *remote_ip_address;

	remote_ip_address =
		environ_get_environment(
			SESSION_REMOTE_IP_ADDRESS_VARIABLE );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new(
		"",
		application_name );

	document->output_content_type = 1;

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_parameter_file->appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	if ( !session_key_exists( application_name, session_key ) )
	{
		if ( login_name )
		{
			if ( remote_ip_address )
			{
				sprintf(msg,
	"Warning for %s at %s: Your session has expired. Please login again.",
		 			login_name,
					remote_ip_address );
			}
			else
			{
				sprintf(msg,
	"Warning for %s: Your session has expired. Please login again.",
		 			login_name );
			}
		}
		else
		{
			if ( remote_ip_address )
			{
				sprintf(msg,
		"Warning at %s: Your session has expired. Please login again.",
					remote_ip_address );
			}
			else
			{
				sprintf(msg,
		"Warning: Your session has expired. Please login again." );
			}
		}
	}
	else
	{
		if ( remote_ip_address
		&&   isdigit( *remote_ip_address ) )
		{
			sprintf(msg,
				"Error: Permission problem for %s at %s.",
				login_name,
				remote_ip_address );
		}
		else
		{
			sprintf(msg,
				"Error: Permission problem for %s.",
				login_name );
		}
	}

	appaserver_output_error_message( application_name, msg, login_name );
	printf( "<h3>%s</h3>\n", msg );

	document_close();
	sleep( SLEEP_SECONDS );
	exit( 1 );

} /* session_access_failed_message_and_exit() */

int session_load(	char **login_name,
			char **last_access_date,
			char **last_access_time,
			char **http_user_agent,
			char *application_name,
			char *session_key )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 1024 ];
	char *input_record;
	char where[ 1024 ];
	char *select;

	sprintf( where, "appaserver_session = '%s'", session_key );
	select = "login_name,last_access_date,last_access_time,http_user_agent";

	sprintf(sys_string,
		"get_folder_data	application=\"%s\"		"
		"			folder=appaserver_sessions	"
		"			select=\"%s\"			"
		"			where=\"%s\"			",
		application_name,
		select,
		where );

	if ( ! ( input_record = pipe2string( sys_string ) ) ) return 0;
	if ( !*input_record ) return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_record, 0 );
	*login_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_record, 1 );
	*last_access_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_record, 2 );
	*last_access_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_record, 3 );
	*http_user_agent = strdup( piece_buffer );

	return 1;

} /* session_load() */

SESSION *session_new_session( void )
{
	SESSION *session;

	session = (SESSION *)calloc( 1, sizeof( SESSION ) );
	return session;
} /* session_new_session() */

int session_access_process(	char *application_name,
				char *session,
				char *process_name,
				char *login_name,
				char *role_name )
{
	int access_ok;

	access_ok = session_get_session_process_access_ok(
				application_name,
				login_name,
				session,
				process_name,
				role_name );
	return access_ok;

} /* session_access_process() */

char *session_get_login_name(		char *application_name,
					char *session )
{
	if ( global_session ) return global_session->login_name;

	global_session = session_new_session();

	if ( session_load(	&global_session->login_name,
				&global_session->last_access_date,
				&global_session->last_access_time,
				&global_session->http_user_agent,
				application_name,
				session ) )
	{
		return global_session->login_name;
	}
	else
	{
		return (char *)0;
	}
} /* session_get_login_name() */

char *session_get_http_user_agent(	char *application_name,
					char *session )
{
	if ( global_session ) return global_session->http_user_agent;

	global_session = session_new_session();

	if ( session_load(	&global_session->login_name,
				&global_session->last_access_date,
				&global_session->last_access_time,
				&global_session->http_user_agent,
				application_name,
				session ) )
	{
		return global_session->http_user_agent;
	}
	else
	{
		return (char *)0;
	}
} /* session_get_http_user_agent() */

boolean session_key_exists(	char *application_name,
				char *session_key )
{
	if ( global_session ) return 1;

	global_session = session_new_session();

	if ( session_load(	&global_session->login_name,
				&global_session->last_access_date,
				&global_session->last_access_time,
				&global_session->http_user_agent,
				application_name,
				session_key ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
} /* session_key_exists() */

char *session_degrade_state(
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name )
{
	char *state = "lookup";

	if ( session_access_folder(
			application_name,
			session,
			folder_name,
			role_name,
			state ) )
	{
		return state;
	}
	else
	{
		return (char *)0;
	}
} /* session_degrade_state() */

boolean session_access(	char *application_name,
			char *session,
			char *login_name )
{
	if ( !global_session )
	{
		global_session = session_new_session();

		if ( !session_load(	&global_session->login_name,
					&global_session->last_access_date,
					&global_session->last_access_time,
					&global_session->http_user_agent,
					application_name,
					session ) )
		{
			return 0;
		}
	}

	if ( strcmp( login_name, global_session->login_name ) != 0 )
		return 0;
	else
		return 1;
} /* session_access() */

void session_update_access_date_time(
					char *application_name,
					char *session )
{
	DATE *now;
	char sys_string[ 1024 ];
	char *table_name;

	now = date_now_new( date_get_utc_offset() );
	table_name = get_table_name( application_name, "appaserver_sessions" );
	sprintf( sys_string,
		 "echo \"update %s					 "
		 "	 set last_access_date = '%s',			 "
		 "	     last_access_time = '%s'			 "
		 "	 where appaserver_session = '%s';\"		|"
		 "sql 1>&2 &						 ",
		 table_name,
		 date_display_yyyy_mm_dd( now ),
		 date_display_hhmm( now ),
		 session );
	if ( system( sys_string ) ){}
}

boolean session_remote_ip_address_changed(
				char *application_name,
				char *session )
{
	char local_session[ 128 ];
	char where[ 128 ];
	char sys_string[ 512 ];
	char *environ_remote_ip_address;
	char *database_remote_ip_address;

	timlib_strcpy( local_session, session, 128 );

	environ_remote_ip_address =
		environ_get_environment(
			SESSION_REMOTE_IP_ADDRESS_VARIABLE );

	sprintf( where,
		 "appaserver_session = '%s'",
		 timlib_sql_injection_escape( local_session ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=remote_ip_address	"
		 "			folder=appaserver_sessions	"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	database_remote_ip_address = pipe2string( sys_string );

	return ( timlib_strcmp(	database_remote_ip_address,
				environ_remote_ip_address ) != 0 );

} /* session_ip_address_changed() */

