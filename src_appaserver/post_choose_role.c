/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_appaserver/post_choose_role.c		*/
/* ------------------------------------------------------------ */
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "session.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "appaserver_user.h"
#include "appaserver_parameter_file.h"
#include "role.h"
#include "folder_menu.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char sys_string[ 512 ];
	ROLE *role;

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s application login session role\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];

	environ_set_utc_offset( application_name );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
			application_name,
			login_name );
	}

	if ( !session_access(	application_name,
				session,
				login_name ) )
	{
		session_access_failed_message_and_exit(
			application_name,
			session,
			login_name );
	}

	if ( !appaserver_user_exists_role(
		login_name,
		role_name ) )
	{
		session_access_failed_message_and_exit(
			application_name,
			session,
			login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	folder_menu_create_spool_file(
		application_name,
		session,
		appaserver_parameter_file_data_directory(),
		role_name,
		(role->folder_count_yn == 'y' ) );

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '' '%s' 2>>%s", 
			application_name,
			session_key,
			login_name,
			role_name,
			"y" /* content_type_yn */,
			appaserver_error_filename(
				application_name ) );

	exit( system( sys_string ) );
}

