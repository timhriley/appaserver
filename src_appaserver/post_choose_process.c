/* ---------------------------------------------------------------	*/
/* src_appaserver/post_choose_process.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "session.h"
#include "post_choose_process.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_or_set_name;

	if ( argc != 6 )
	{
		fprintf(stderr, 
		"Usage: %s application session login role process_or_set\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_or_set = argv[ 5 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	if ( !*process )
	{
		DICTIONARY *post_dictionary;

		post_dictionary =
			post2dictionary(stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

		process =
			dictionary_get_string(
				post_dictionary,
				CHOOSE_PROCESS_MENU_NAME );
	}

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_process(
				application_name,
				session,
				process,
				login_name,
				role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
		login_name,
		role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	/* Get the parameters for this process */
	/* ----------------------------------- */
	process_parameter_record_list = 
		process_parameter_list_process_parameter_record_list(
					&is_process_set,
					application_name,
					process,
					appaserver_error_get_filename(
						application_name ),
					0 /* not is_preprompt */ );

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: process = [%s], got records = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
process,
list_display( process_parameter_record_list ) );
m2( application_name, msg );
}
*/
	preprompt_process_parameter_record_list = 
		process_parameter_list_process_parameter_record_list(
					&is_process_set,
					application_name,
					process,
					appaserver_error_get_filename(
						application_name ),
					1 /* is_preprompt */ );

	/* If no parameters, then jump to the process */
	/* ---------------------------------------- */
	if ( !list_length( process_parameter_record_list )
	&&   !list_length( preprompt_process_parameter_record_list ) )
	{
		sprintf( sys_string,
"echo \"\" | post_prompt_process_form %s %s %s '%s' %s %c 2>>%s",
			 login_name,
			 application_name,
			 session,
			 process,
			 role_name,
			 'y' /* passed_preprompt_yn */,
			 appaserver_error_get_filename(
						application_name ) );
	}
	else
	{
		sprintf( sys_string, 
	"output_prompt_process_form %s %s %s '%s' '%s' n 2>>%s",
		 	 login_name,
			 application_name,
			 session,
			 process,
			 role_name,
			 appaserver_error_get_filename(
						application_name ) );
	}

	exit( system( sys_string ) );

} /* main() */
