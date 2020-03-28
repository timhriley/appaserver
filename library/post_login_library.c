/* $APPASERVER_HOME/library/post_login_library.c			*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "timlib.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "post_login_library.h"

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

} /* post_login_email_login() */

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

} /* post_login_redraw_index_screen() */

enum password_match_return post_login_password_match(
				char *application_name,
				char *login_name,
				char *password,
				char *database_password )
{
	if ( timlib_strncmp( login_name, "public_" ) == 0
	||   timlib_exists_string( login_name, "_public" ) )
	{
		/* Any password is okay, including the empty string. */
		/* ------------------------------------------------- */
		if ( database_password )
			return public_login;
		else
		/* -------------------- */
		/* If no public account */
		/* -------------------- */
			return password_fail;
	}
	else
	/* -------------------------------------------------------- */
	/* If no user exists or the user has a blanked out password */
	/* -------------------------------------------------------- */
	if ( !database_password
	||   !*database_password
	||   timlib_strcmp( database_password, "null" ) == 0 )
	{
		return password_fail;
	}
	else
	/* ----------------------------------------- */
	/* If the database password is not encrypted */
	/* ----------------------------------------- */
	if ( timlib_strcmp( database_password, password ) == 0 )
	{
		return password_match;
	}
	else
	if ( post_login_encoded_password_ok(
				application_name,
				database_password,
				password ) )
	{
		return password_match;
	}
	else
	if ( timlib_login_name_email_address( login_name ) )
	{
		/* Any password is okay, but not the empty string. */
		/* ----------------------------------------------- */
		if ( database_password
		&&   *database_password
		&&   timlib_strcmp( database_password, "null" ) != 0 )
		{
			return email_login;
		}
	}

	return password_fail;

} /* post_login_password_match() */

void post_login_output_frameset(	char *application_name,
					char *login_name,
					char *session,
					enum password_match_return
						password_match_return )
{
	char sys_string[ 1024 ];

	if ( password_match_return == password_match
	||   password_match_return == public_login )
	{
		sprintf(sys_string,
			"output_frameset %s %s %s \"\" y 2>>%s", 
			application_name,
			session,
			login_name,
			appaserver_error_get_filename(
				application_name ) );
		if ( system( sys_string ) ){};
	}
	else
	if ( password_match_return == email_login )
	{
		char email_http_filename[ 128 ];
		char email_output_filename[ 128 ];
		APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

		appaserver_parameter_file =
			appaserver_parameter_file_new();

		sprintf(
		 email_output_filename,
		 EMAIL_OUTPUT_FILE_TEMPLATE,
		 appaserver_parameter_file->
			document_root,
		 application_name,
		 session );

		sprintf(
		 email_http_filename,
		 EMAIL_HTTP_FILE_TEMPLATE,
		 application_get_http_prefix( application_name ),
		 appaserver_library_get_server_address(),
		 application_name,
		 session );

		sprintf(sys_string,
			"output_frameset %s %s %s \"\" n >%s 2>>%s", 
			application_name,
			session,
			login_name,
			email_output_filename,
			appaserver_error_get_filename(
				application_name ) );

		if ( system( sys_string ) ){};

		if ( post_login_email_login(
			login_name,
			email_http_filename ) )
		{
			post_login_redraw_index_screen(
				application_name,
				CLOUDACUS_LOCATION,
				"emailed_login_yn=y" );
		}
		else
		{
			post_login_redraw_index_screen(
				application_name,
				CLOUDACUS_LOCATION,
				"invalid_login_yn=y" );
		}
	}

} /* post_login_output_frameset() */

boolean post_login_encoded_password_ok(
					char *application_name,
					char *database_password,
					char *password )
{
	char *encoded_password;

	if ( !password || !*password ) return 0;
	if ( !database_password || !*database_password ) return 0;

	encoded_password =
		post_login_get_encoded_password(
				application_name,
				password,
				0 /* not old_password */ );

	if ( timlib_strcmp( database_password, encoded_password ) == 0 )
		return 1;

	encoded_password =
		post_login_get_encoded_password(
				application_name,
				password,
				1 /* old_password */ );

	return ( timlib_strcmp( database_password, encoded_password ) == 0 );

} /* post_login_encoded_password_ok() */

char *post_login_get_encoded_password(
					char *application_name,
					char *password,
					boolean old_password )
{
	char *table_name;
	char sys_string[ 1024 ];
	char function_name[ 16 ];
	char *encoded_password;

	if ( old_password )
		strcpy( function_name, "old_password" );
	else
		strcpy( function_name, "password" );

	if ( !*application_name ) return "";
	if ( !*password ) return "";

	table_name = get_table_name( application_name, "application" );

	sprintf( sys_string,
		 "echo \"select %s('%s')			 "
		 "	 from %s				 "
		 "	 where application = '%s';\"		|"
		 "sql.e						 ",
		 function_name,
		 timlib_sql_injection_escape( password ),
		 table_name,
		 application_name );

	encoded_password = pipe2string( sys_string );

	return encoded_password;

} /* post_login_get_encoded_password() */

char *post_login_password_match_return_display(
			enum password_match_return password_match_return )
{
	if ( password_match_return == password_match )
		return "password_match";
	else
	if ( password_match_return == password_fail )
		return "password_fail";
	else
	if ( password_match_return == public_login )
		return "public_login";
	if ( password_match_return == email_login )
		return "email_login";
	else
		return "unknown";

} /* post_login_password_match_return_display() */

