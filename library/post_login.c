/* $APPASERVER_HOME/library/post_login.c		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "post_login.h"

POST_LOGIN *post_login_new(
			int argc,
			char **argv )
{
	POST_LOGIN *post_login;

	if ( ! ( post_login = calloc( 1, sizeof( POST_LOGIN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( post_login->post_login_dictionary =
			post_login_dictionary() ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: post_login_dictionary() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (POST_LOGIN *)0;
	}

	if ( ! ( post_login->post_login_application_name =
			post_login_application_name(
				argc,
				argv,
				post_login->post_dictionary ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: post_login_application() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (POST_LOGIN *)0;
	}

	post_login->sql_injection_escape_login_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"login_name",
				post_login->
					post_login_dictionary ) );

	post_login->sql_injection_escape_password =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"password",
				post_login->
					post_login_dictionary ) );
		post_login_sql_injection_escape_name(

	if ( ( post_login->post_login_missing_name =
		post_login_missing_name(
			post_login->sql_injection_escape_login_name ) ) )
	{
		return post_login;
	}

	post_login->post_login_public_name =
		post_login_public_name(
			post_login->sql_injection_escape_login_name );

	post_login->post_login_database_password =
		post_login_database_password(
			post_login->post_login_application_name,
			post_login->sql_injection_escape_login_name );

	if ( ( post_login->post_login_missing_database_password =
		post_login_missing_database_password(
			post_login->post_login_database_password ) ) )
	{
		return post_login;
	}

	post_login->post_login_name_email_address =
		post_login_name_email_address(
			post_login->sql_injection_escape_login_name );

	post_login->post_login_password_match_return =
		post_login_password_match(
			post_login->post_login_application_name,
			post_login->post_login_name_email_address,
			post_login->post_login_public_name,
			post_login->sql_injection_escape_password,
			post_login->post_login_database_password );

	return post_login;
}

char *post_login_database_password(
			char *application_name,
			char *login_name )
{
	sprintf(sys_string,
	 	"password4appaserver_user.sh \"%s\" \"%s\" 2>>%s",
	 	login_name,
		application_name,
		appaserver_error_filename( application_name ) );

	return string_pipe_fetch( sys_string );
}

boolean post_login_missing_name(
			char *post_login_name )
{
	return (post_login_name) ? 1 : 0;
}

boolean post_login_missing_database_password(
			char *database_password )
{
	return (database_password) ? 1 : 0;
}

DICTIONARY *post_login_dictionary( void )
{
	DICTIONARY *post_dictionary;

	return
		post2dictionary(
			stdin,
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session */ );
}

char *post_login_application_name(
			int argc,
			char **argv,
			DICTIONARY *post_dictionary )
{
	char *application_name = {0};

	if ( argc == 1 )
	{
		if ( dictionary_index_data(
				&application_name,
				post_dictionary,
				"choose_application",
				0 ) == -1 )
		{
			return (char *)0;
		}
	}
	else
	if ( argc == 2 )
	{
		application_name = argv[ 1 ];
	}

	return application_name;
}

/*
boolean post_login_email_login(
			char *login_name,
			char *email_http_filename )
{
	char sys_string[ 1024 ];
	char mailfile[ 128 ];
	FILE *output_file;

	sprintf( mailfile, "/tmp/mailfile_%d", getpid() );

	if ( ! ( output_file = fopen( mailfile, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 mailfile );
		return 0;
	}

	fprintf( output_file,
		 "To: %s\n",
		 login_name );

	fprintf( output_file,
		 "From: %s\n",
		 RETURN_ADDRESS );

	fprintf( output_file,
		 "Subject: %s\n",
		 SUBJECT );

	fprintf( output_file,
		 "Appaserver login:\n%s\n",
		 email_http_filename );

	fclose( output_file );

	sprintf( sys_string,
		 "/usr/sbin/sendmail -t < %s",
		 mailfile );
	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "rm -f %s",
		 mailfile );
	if ( system( sys_string ) ){};

	return 1;
}
*/

void post_login_redraw_index_screen(	char *application_name,
					char *location,
					char *message )
{
	char local_location[ 128 ];

	if ( location )
	{
		strcpy( local_location, location );

		sprintf( local_location,
			 "%s?%s",
			 location,
			 message );
	}
	else
	{
		sprintf( local_location,
			 "/appaserver/%s/index.php?%s",
			 application_name,
			 message );
	}

	printf( 
"Content-type: text/html						\n"
"\n"
"<html>									\n"
"<script type=\"text/javascript\">					\n"
"window.location = \"%s\"						\n"
"</script>								\n"
"</html>								\n",
		local_location );

}

boolean post_login_public_name(
			char *login_name )
{
	if ( string_strncmp( login_name, "public" ) == 0
	||   string_exists( login_name, "_public" ) )
	{
		return 1;
	}
	else
		return 0;
	}
}

boolean post_login_name_email_address(
			char *login_name )
{
	return ( timlib_login_name_email_address( login_name ) );
}

enum post_login_password_match_return
	post_login_password_match(
			char *application_name,
			boolean post_login_name_email_address,
			boolean post_login_public_name,
			char *sql_injection_escape_password,
			char *database_password )
{
	if ( post_login_public_name )
	{
		/* Any password is okay, including the empty string. */
		/* ------------------------------------------------- */
		return public_login;
	}

	/* -------------------------------------- */
	/* If the user has a blanked out password */
	/* -------------------------------------- */
	if ( !database_password
	||   !*database_password
	||   string_strcmp( database_password, "null" ) == 0 )
	{
		return password_fail;
	}

	if ( post_login_name_email_address )
	{
		return email_login;
	}

	/* ----------------------------------------- */
	/* If the database password is not encrypted */
	/* ----------------------------------------- */
	if ( string_strcmp(
		database_password,
		sql_injection_escape_password ) == 0 )
	{
		return password_match;
	}

	if ( security_password_match(
			database_password,
			security_encrypted_password(
				sql_injection_escape_password,
				security_database_password_function(
					database_password ) ) )
	{
		return password_match;
	}

	return password_fail;
}

char *post_login_session(
			char *application_name,
			char *login_name )
{
	sprintf( sys_string,
		 "session_new.sh %s %s 2>>%s", 
		 application_name,
		 login_name,
		 appaserver_error_filename(
			application_name ) );

	return string_pipe_fetch( sys_string );
}

