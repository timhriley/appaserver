/* --------------------------------------------------- 	*/
/* post_choose_role_drop_down.c			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

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
int fetch_from_dictionary(	char **application_name,
				char **database_string,
				char **session,
				char **login_name,
				char **role_name,
				char **title,
				DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *title = "";
	char *role_name;
	char sys_string[ 512 ];
	DICTIONARY *post_dictionary;
	char *database_string = {0};
	ROLE *role;

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();

	/* Apache2 is sending two arguments, the second is empty? */
	/* ------------------------------------------------------ */
	if ( argc == 1 || ( argc == 2 && !*argv[ 1 ] ) )
	{
		post_dictionary =
			post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );
	
		if ( !fetch_from_dictionary(	&application_name,
						&database_string,
						&session,
						&login_name,
						&role_name,
						&title,
						post_dictionary ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s(): cannot get fetch from dictionary\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}
	}
	else
	if ( argc < 6 )
	{
		fprintf(stderr,
	"Usage: %s application database session login role [\"title\"]\n",
			argv[ 0 ] );
		exit( 1 );
	}
	else
	{
		application_name = argv[ 1 ];
		database_string = argv[ 2 ];
		session = argv[ 3 ];
		login_name = argv[ 4 ];
		role_name = argv[ 5 ];

		if ( argc == 7 ) title = argv[ 6 ];
	}

	if ( strcmp( database_string, "database" ) == 0 )
		database_string = (char *)0;

	if (	database_string && *database_string )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

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

	if ( !role_name || !*role_name )
	{
		LIST *role_list;
		role_list = role_list_fetch( login_name );
		role_name = list_get_first_string( role_list );
	}

	/* Reselect the folders when changing roles. */
	/* ----------------------------------------- */
	role = role_new_role( application_name, role_name );

	folder_menu_create_spool_file(
		application_name,
		session,
		appaserver_parameter_file_get_data_directory(),
		role_name,
		(role->folder_count_yn == 'y' ) );

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' \"%s\" '%s' 2>>%s", 
			application_name,
			session,
			login_name,
			role_name,
			title,
			"y" /* content_type_yn */,
			appaserver_error_get_filename(
				application_name ) );

	exit( system( sys_string ) );

} /* main() */

int fetch_from_dictionary(	char **application_name,
				char **database_string,
				char **session,
				char **login_name,
				char **role_name,
				char **title,
				DICTIONARY *post_dictionary )
{
	if ( dictionary_get_index_data( 	application_name,
						post_dictionary,
						"application",
						0 ) == -1 )
	{
		return 0;
	}
	if ( dictionary_get_index_data( 	session,
						post_dictionary,
						"session",
						0 ) == -1 )
	{
		return 0;
	}
	if ( dictionary_get_index_data( 	login_name,
						post_dictionary,
						"login_name",
						0 ) == -1 )
	{
		return 0;
	}
	if ( dictionary_get_index_data( 	role_name,
						post_dictionary,
						"choose_role_drop_down",
						0 ) == -1 )
	{
		dictionary_get_index_data( 	role_name,
						post_dictionary,
						"role",
						0 );
	}
	if ( dictionary_get_index_data( 	title,
						post_dictionary,
						"title",
						0 ) == -1 )
	{
		/* Let it go through */
	}
	if ( dictionary_get_index_data( 	database_string,
						post_dictionary,
						"database",
						0 ) == -1 )
	{
		/* Let it go through */
	}
	return 1;
} /* fetch_from_dictionary() */

