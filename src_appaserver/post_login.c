/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/post_login.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* This program is attached to the submit button on the */
/* password form. 				       	*/
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

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
#include "basename.h"
#include "boolean.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "security.h"
#include "post_login_library.h"

#define SECONDS_TO_SLEEP		2

int main( int argc, char **argv )
{
	char *login_name = "";
	char sys_string[ 512 ];
	char *application_name = "";
	char *database_string = {0};
	char *password = "";
	char *database_password = "";
	char *session;
	DICTIONARY *post_dictionary;
	boolean login_denied = 0;
	enum password_match_return password_match_return;

	if ( ! ( post_dictionary =
			post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get post_dictionary from stdin.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( argc == 1 )
	{
		if ( dictionary_get_index_data(
				&application_name,
				post_dictionary,
				"choose_application",
				0 ) == -1 )
		{
			login_denied = 1;
		}
	}
	else
	if ( argc == 2 )
	{
		application_name = argv[ 1 ];
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

	database_string = low_string( application_name );

	add_utility_to_path();
	add_src_appaserver_to_path();

	if ( dictionary_get_index_data(
			&login_name,
			post_dictionary,
			"login_name",
			0 ) == -1 )
	{
		login_denied = 1;
	}

	login_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			login_name );

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

	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	sprintf(sys_string,
	 	"password4appaserver_user.sh \"%s\" \"%s\" 2>>%s",
	 	login_name,
		application_name,
		appaserver_error_get_filename( application_name ) );

	database_password = get_line_system( sys_string );

	if ( !database_password || !*database_password )
	{
		password_match_return = password_fail;
	}
	else
	{
		password_match_return =
			post_login_password_match(
				application_name,
				login_name,
				password,
				database_password );
	}

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
		login_denied = 1;
	}

	if ( login_denied )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"Login denied for %s from %s",
			login_name,
			environ_get_environment( "REMOTE_ADDR" ) );

		appaserver_output_error_message(
			application_name,
			msg,
			login_name );

		if ( !appaserver_library_get_from_php( post_dictionary ) )
		{
			printf( 
"Content-type: text/html					\n"
"\n"
"		<HTML>						\n"
"		<HEAD>						\n"
"			<TITLE>%s Please Try Again</TITLE>   	\n"
"		</HEAD>						\n"
"								\n"
"		<p>Please try again.				\n"
"		</HTML>						\n",
			application_name );
		}
		else
		{
			post_login_redraw_index_screen(
				application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	return 0;
}

