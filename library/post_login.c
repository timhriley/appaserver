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
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "session.h"
#include "date.h"
#include "post_dictionary.h"
#include "frameset.h"
#include "choose_role.h"
#include "post_login.h"

POST_LOGIN *post_login_calloc( void )
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

	return post_login;
}

POST_LOGIN *post_login_new(
			int argc,
			char **argv )
{
	POST_LOGIN *post_login = post_login_calloc();

	if ( ! ( post_login->dictionary =
			post_login_dictionary() ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: post_login_dictionary() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (POST_LOGIN *)0;
	}

	post_login->sql_injection_escape_application_name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			string_low(
				post_login_application_name(
					argc,
					argv,
					post_login->dictionary ) ) );

	if ( !post_login->sql_injection_escape_application_name )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: post_login_application_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (POST_LOGIN *)0;
	}

	post_login->sql_injection_escape_login_name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			string_low(
				dictionary_fetch(
					"login_name",
					post_login->dictionary ) ) );

	if ( ( post_login->missing_name =
		post_login_missing_name(
			post_login->sql_injection_escape_login_name ) ) )
	{
		/* Generates an error */
		/* ------------------ */
		return post_login;
	}

	post_login->sql_injection_escape_password =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"password",
				post_login->dictionary ) );

	post_login->public_name =
		post_login_public_name(
			post_login->
				sql_injection_escape_login_name );

	post_login->database_password =
		post_login_database_password(
			post_login->sql_injection_escape_login_name );

	post_login->missing_database_password =
		post_login_missing_database_password(
			post_login->database_password );

	post_login->name_email_address =
		post_login_name_email_address(
			post_login->sql_injection_escape_login_name );

	post_login->password_match_return =
		post_login_password_match(
			post_login->missing_database_password,
			post_login->name_email_address,
			post_login->public_name,
			post_login->sql_injection_escape_password,
			post_login->database_password );

	if ( post_login->password_match_return == password_match
	||   post_login->password_match_return == public_login
	||   post_login->password_match_return == email_login )
	{
		post_login->session_key =
			post_login_session_key(
				post_login->
					sql_injection_escape_application_name,
				post_login->
					sql_injection_escape_login_name );

		if ( !post_login->session_key )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: post_login_session_key() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return (POST_LOGIN *)0;
		}
	}

	return post_login;
}

char *post_login_database_password( char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( ! ( appaserver_user =
			appaserver_user_fetch(
				login_name,
				0 /* not fetch_role_name_list */,
				0 /* not fetch_session_list */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_user_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		exit( 1 );
	}

	return appaserver_user->database_password;
}

boolean post_login_missing_name(
			char *post_login_name )
{
	return (post_login_name) ? 1 : 0;
}

boolean post_login_missing_database_password(
			char *database_password )
{
	if ( !database_password || !*database_password )
		return 1;
	else
		return 0;
}

DICTIONARY *post_login_dictionary( void )
{
	POST_DICTIONARY *post_dictionary;

	post_dictionary =
		post_dictionary_stdin_new(
			(char *)0 /* upload_directory */,
			(char *)0 /* session_key */ );

	if ( post_dictionary )
	{
		return post_dictionary->original_post_dictionary;
	}
	else
	{
		return (DICTIONARY *)0;
	}
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

boolean post_login_name_email_address( char *login_name )
{
	return ( string_character_exists( login_name, '@' ) );
}

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
	{
		return 0;
	}
}

enum password_match_return
	post_login_password_match(
			boolean missing_database_password,
			boolean name_email_address,
			boolean public_name,
			char *sql_injection_escape_password,
			char *database_password )
{
	if ( public_name )
	{
		return public_login;
	}

	if ( missing_database_password )
	{
		return database_password_blank;
	}

	if ( name_email_address )
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
					database_password ) ) ) )
	{
		return password_match;
	}

	return password_fail;
}

char *post_login_session_key(
			char *application_name,
			char *login_name )
{
	char destination[ 128 ];

	return
	session_insert(
		application_name,
		login_name,
		date_now_yyyy_mm_dd_string( date_utc_offset() ),
		date_now_hhmm_string( date_utc_offset() ),
		left_string(
			destination,
			environment_http_user_agent(),
			80 ),
		environment_remote_ip_address() );
}

void post_login_frameset_output(
			char *application_name,
			char *login_name,
			char *session_key,
			enum password_match_return password_match_return,
			char *appaserver_user_default_role_name,
			LIST *appaserver_user_role_name_list )
{
	if ( ( !appaserver_user_default_role_name
	||     !*appaserver_user_default_role_name )
	&&     list_length( appaserver_user_role_name_list ) > 1 )
	{
		char *system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			choose_role_output_system_string(
				CHOOSE_ROLE_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				appaserver_error_filename(
					application_name ) );

		if ( system( system_string ) ){}
	}
	else
	if ( ( appaserver_user_default_role_name
	&&     *appaserver_user_default_role_name )
	||     list_length( appaserver_user_role_name_list ) == 1 )
	{
		char *role_name;
		char system_string[ 1024 ];

		if ( appaserver_user_default_role_name
		&&   *appaserver_user_default_role_name )
		{
			role_name = appaserver_user_default_role_name;
		}
		else
		{
			role_name =
				list_first(
					appaserver_user_role_name_list );
		}

		sprintf(system_string,
			"output_choose_role_folder_process %s %s %s 2>>%s",
			login_name,
			session_key,
			role_name,
			appaserver_error_filename(
				application_name ) );

		if ( system( system_string ) ){}
	}

	if ( password_match_return != email_login )
	{
		char system_string[ 1024 ];

		sprintf(system_string,
			"output_frameset %s %s 2>>%s",
			login_name,
			session_key,
			appaserver_error_filename(
				application_name ) );

		if ( system( system_string ) ){}
	}
	else
	{
		char system_string[ 1024 ];

		sprintf(system_string,
			"output_email_link %s %s 2>>%s",
			login_name,
			session_key,
			appaserver_error_filename(
				application_name ) );

		if ( system( system_string ) ){}
	}
}

#ifdef NOT_DEFINED
void post_login_horizontal_frameset(
			char *title,
			FRAMESET *frameset )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' > %s 2>>%s",
		frameset->session_key,
		frameset->login_name,
		frameset->
			frameset_frame_prompt->
			output_filename,
		appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_background_color( application_name ),
		 frameset->
			frameset_frame_edit->
			output_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	if ( system( "content_type_cgi.sh" ) ){};

	printf(
		"<HTML xmlns=\"http://www.w3.org/1999/xhtml\">\n"
		"<HEAD>\n"
		"<TITLE>%s</TITLE>\n"
		"</HEAD>\n"
		"%s\n"
		"</HTML>\n",
		title,
		frameset->html );
}

void post_login_vertical_frameset(
			char *title,
			FRAMESET *frameset )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' > %s 2>>%s",
		frameset->session_key,
		frameset->login_name,
		frameset->
			frameset_frame_menu->
			output_filename,
		appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" \"%s\" n > %s 2>>%s",
		 application_background_color( application_name ),
		 title,
		 frameset->
			frameset_frame_prompt->
			output_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_background_color( application_name ),
		 frameset->
			frameset_frame_edit->
			output_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	if ( system( "content_type_cgi.sh" ) ){};

	printf(
		"<HTML xmlns=\"http://www.w3.org/1999/xhtml\">\n"
		"<HEAD>\n"
		"<TITLE>%s</TITLE>\n"
		"</HEAD>\n"
		"%s\n"
		"</HTML>\n",
		title,
		frameset->html );
}
#endif
