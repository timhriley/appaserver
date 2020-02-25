/* ------------------------------------------------------ 	*/
/* $APPASERVER_HOME/src_appaserver/post_login_cloudacus.c      	*/
/* ------------------------------------------------------ 	*/
/* 							       	*/
/* This program is attached to the submit button on the 	*/
/* password form from cloudacus.com/index.php.    		*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------ 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "post_login_library.h"

/* Constants */
/* --------- */
#define APPLICATION_NAME		"cloudacus"

/* Prototypes */
/* ---------- */
/*
boolean application_exists_virtual_host(
				char *application_name );

void reset_password(		char *application_name,
				char *login_name,
				char *password );
*/

int email_password(		char *login_name,
				char *password );

boolean application_exists(	char *application_name,
				char *appaserver_error_directory );

int main( int argc, char **argv )
{
	char *login_name = "";
	char *password = "";
	char sys_string[ 512 ];
	char *session = {0};
	DICTIONARY *post_dictionary;
	char *signup_yn = "";
	char *login_yn = "";
	char *application_name = "";
	char *application_title = "";
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum password_match_return password_match_return;
	char *database_string = {0};

	if ( argc == 2 ) application_name = argv[ 1 ];

	post_dictionary =
		post2dictionary(
			stdin,
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session */ );

	if ( !*application_name )
	{
		dictionary_get_index_data(
				&application_name,
				post_dictionary,
				"application_key",
				0 );
	}

	if ( !*application_name )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot get application_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
		environ_set_environment(
			"DATABASE",
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
		environ_set_environment(
			"DATABASE",
			application_name );
	}

	database_string = application_name;

	appaserver_parameter_file =
		appaserver_parameter_file_application(
			application_name );

	dictionary_get_index_data(
			&login_name,
			post_dictionary,
			"login_name",
			0 );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_error_login_name_append_file(
				argc,
				argv,
				application_name,
				login_name );

	dictionary_get_index_data(
			&password,
			post_dictionary,
			"password",
			0 );

	dictionary_get_index_data(
			&signup_yn,
			post_dictionary,
			"signup_yn",
			0 );

	dictionary_get_index_data(
			&login_yn,
			post_dictionary,
			"login_yn",
			0 );

/*
	dictionary_get_index_data(
			&email_password_yn,
			post_dictionary,
			"email_password_yn",
			0 );
*/

	dictionary_get_index_data(
			&application_title,
			post_dictionary,
			"application_title",
			0 );

/*
	if ( *email_password_yn == 'y' )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );

		if ( !*application_name 
		||   !application_exists(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point ) )
		{
			post_login_redraw_index_screen(
				APPLICATION_NAME,
				CLOUDACUS_LOCATION,
				"invalid_application_key_yn=y" );
			sleep( 2 );
			exit( 0 );
		}

		reset_password(		application_name,
					login_name,
					RESETTED_PASSWORD );

		email_password(		login_name,
					RESETTED_PASSWORD );

		post_login_redraw_index_screen(
				APPLICATION_NAME,
				CLOUDACUS_LOCATION,
				"email_password_yn=y" );
		exit( 0 );
	}
	else
*/
	if ( signup_yn && *signup_yn && *signup_yn == 'y' )
	{
		char sys_string[ 1024 ];
		int results;
		char *session = {0};

		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			TEMPLATE_APPLICATION );

		sprintf( sys_string, 
			 "session_new.sh %s %s 2>>%s", 
			 TEMPLATE_APPLICATION,
			 login_name,
			 appaserver_error_get_filename(
				TEMPLATE_APPLICATION ) );

		session = get_line_system( sys_string );

		sprintf( sys_string,
"create_empty_application \"%s\" %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %c %c %c 2>>%s 1>/dev/null",
			 TEMPLATE_APPLICATION,
			 session,
			 login_name,
			 "" /* role */,
			 "create_empty_application" /* process */,
			 application_name /* destination_application */,
			 application_title,
			 'y' /* create_database_yn */,
			 'n' /* delete_application_yn */,
			 'y' /* really_yn */,
			 appaserver_error_get_filename(
				TEMPLATE_APPLICATION ) );

		results = system( sys_string );

		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );

		if ( results == 0 )
		{
			if ( character_exists( login_name, '@' ) )
			{
				post_login_redraw_index_screen(
					APPLICATION_NAME,
					CLOUDACUS_LOCATION,
					"email_application_succeeded_yn=y" );
			}
			else
			{
				post_login_redraw_index_screen(
					APPLICATION_NAME,
					CLOUDACUS_LOCATION,
					"password_application_succeeded_yn=y" );
			}
		}
		else
		{
			post_login_redraw_index_screen(
				APPLICATION_NAME,
				CLOUDACUS_LOCATION,
				"password_application_succeeded_yn=n" );
		}

		exit( 0 );
	}
	else
	if ( *login_yn == 'y' )
	{
		char *database_password;

		if ( !*application_name 
		||   !application_exists(
					application_name,
					appaserver_parameter_file->
						appaserver_error_directory ) )
		{
			post_login_redraw_index_screen(
				APPLICATION_NAME,
				CLOUDACUS_LOCATION,
				"invalid_application_key_yn=y" );
			sleep( 2 );
			exit( 0 );
		}

		sprintf(sys_string,
		 	"password4appaserver_user.sh \"%s\" \"%s\" 2>>%s",
				login_name,
			application_name,
			appaserver_error_get_filename( application_name ) );

		database_password = get_line_system( sys_string );

		password_match_return =
			post_login_password_match(
					application_name,
					login_name,
					password,
					database_password );

		if ( password_match_return == password_match
		||   password_match_return == public_login
		||   password_match_return == email_login )
		{
			/* Generate a new session */
			/* ---------------------- */
			sprintf( sys_string, 
				 "session_new.sh %s %s 2>>%s", 
				 application_name,
				 login_name,
				 appaserver_error_get_filename(
					application_name ) );
	
			session = get_line_system( sys_string );

			post_login_output_frameset(
					application_name,
					login_name,
					session,
					password_match_return );
		}
		else
		{
			char msg[ 1024 ];

			sprintf(	msg,
					"Login denied for %s from %s.",
					login_name,
					environ_get_environment(
						"REMOTE_ADDR" ) );

                       appaserver_output_error_message(
                                application_name,
                                msg,
                                login_name );

			post_login_redraw_index_screen(
					APPLICATION_NAME,
					CLOUDACUS_LOCATION,
					"invalid_login_yn=y" );
			sleep( 2 );
			exit ( 1 );
		}
	}
	exit( 0 );
} /* main() */

#ifdef NOT_DEFINED
void reset_password(	char *application_name,
			char *login_name,
			char *password )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "appaserver_user" );
	sprintf( sys_string,
		 "echo \"update %s					 "
		 "	 set password = password('%s')			 "
		 "	 where login_name = '%s';\"			|"
		 "sql.e							 ",
	table_name,
	password,
	login_name );

	system( sys_string );

	table_name = get_table_name( application_name, "role_appaserver_user" );
	sprintf( sys_string,
		 "echo \"insert into %s					 "
		 "	 (role,login_name) values ('instructor','%s');\"|"
	"sql								 ",
	table_name,
	login_name );

	system( sys_string );

} /* reset_password() */
#endif

int email_password(	char *login_name,
			char *password )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo %s | mailx -s \"Appaserver password\" %s",
		 password,
		 login_name );
	if ( system( sys_string ) ){};
	return 1;

} /* email_password() */

boolean application_exists(	char *application_name,
				char *appaserver_error_directory )
{
	return appaserver_library_application_exists(
				application_name,
				appaserver_error_directory );
} /* application_exists() */

#ifdef NOT_DEFINED
boolean application_exists_virtual_host( char *application_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		"ls -1 /etc/apache2/sites-enabled/%s.cloudacus.com 2>/dev/null",
		 application_name );

	results = pipe2string( sys_string );

	if ( results && *results )
		return 1;
	else
		return 0;
} /* application_exists_virtual_host() */
#endif

