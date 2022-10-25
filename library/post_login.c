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
#include "timlib.h"
#include "application.h"
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
#include "document.h"
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

	if ( post_login->sql_injection_escape_application_name )
	{
		session_environment_set(
			post_login->
				sql_injection_escape_application_name );
	}
	else
	{
		session_environment_set(
			POST_LOGIN_TEMPLATE_APPLICATION );
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

	post_login->missing_name =
		post_login_missing_name(
			post_login->sql_injection_escape_login_name );

	post_login->sql_injection_escape_password =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"password",
				post_login->dictionary ) );

	post_login->sql_injection_escape_signup_yn =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"signup_yn",
				post_login->dictionary ) );

	post_login->sql_injection_escape_application_key =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"application_key",
				post_login->dictionary ) );

	post_login->sql_injection_escape_application_title =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			dictionary_fetch(
				"application_title",
				post_login->dictionary ) );

	post_login->signup =
		post_login_signup(
			post_login->sql_injection_escape_signup_yn,
			post_login->sql_injection_escape_application_key,
			post_login->sql_injection_escape_application_title );

	if ( post_login->signup )
	{
		post_login->signup_duplicate =
			post_login_signup_duplicate(
				post_login->
					sql_injection_escape_application_key,
				appaserver_parameter_error_directory() );
	}

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
			post_login->signup_duplicate,
			post_login->signup,
			post_login->missing_name,
			post_login->missing_database_password,
			post_login->name_email_address,
			post_login->public_name,
			post_login->sql_injection_escape_password,
			post_login->database_password );

	if ( post_login->password_match_return == email_login
	||   post_login->password_match_return == password_fail
	||   post_login->password_match_return == missing_name
	||   post_login->password_match_return == signup
	||   post_login->password_match_return == signup_duplicate )
	{
		if ( ! ( post_login->ip_address = post_login_ip_address() ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: post_login_ip_address() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( post_login->message =
				/* ------------------------------ */
				/* Returns program memory or null */
				/* ------------------------------ */
				post_login_message(
					post_login->password_match_return,
					post_login->name_email_address ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: post_login_message(%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				post_login->password_match_return );
			exit( 1 );
		}

		post_login->redraw_application_name =
			post_login_redraw_application_name(
				post_login->
					sql_injection_escape_application_name,
				post_login->
					sql_injection_escape_application_key );

		post_login->redraw_index_screen_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_login_redraw_index_screen_string(
				post_login->
					redraw_application_name,
				post_login->ip_address,
				post_login->message,
				appaserver_library_from_php(
					post_login->dictionary) );
	}

	if ( post_login->password_match_return == password_match
	||   post_login->password_match_return == public_login
	||   post_login->password_match_return == email_login )
	{
		post_login->session_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_key(
				post_login->
					sql_injection_escape_application_name );

		if ( !post_login->session_key || !*post_login->session_key )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

/*
		session_insert(
			post_login->session_key,
			post_login->sql_injection_escape_login_name,
			date_now_yyyy_mm_dd_string( date_utc_offset() ),
			date_now_hhmm_string( date_utc_offset() ),
			left_string(
				destination,
				environment_http_user_agent(),
				80 ),
			environment_remote_ip_address() );
*/

		post_login->output_pipe_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_login_output_pipe_string(
				POST_LOGIN_EMAIL_OUTPUT_TEMPLATE,
				post_login->name_email_address,
				post_login->
					sql_injection_escape_application_name,
				appaserver_parameter_document_root(),
				post_login->session_key );

		post_login->frameset_output_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			frameset_output_system_string(
			   FRAMESET_OUTPUT_EXECUTABLE,
			   post_login->session_key,
			   post_login->sql_injection_escape_login_name,
			   post_login->output_pipe_string,
			   appaserver_error_filename(
				post_login->
				     sql_injection_escape_application_name ) );
	}

	if ( post_login->password_match_return == email_login )
	{
		post_login->email_link_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_login_email_link_system_string(
				POST_LOGIN_EMAIL_HTTP_TEMPLATE,
				POST_LOGIN_RETURN_ADDRESS,
				POST_LOGIN_SUBJECT,
				post_login->
					sql_injection_escape_application_name,
				post_login->session_key,
				post_login->
					sql_injection_escape_login_name );
	}

	if ( post_login->password_match_return == signup )
	{
		post_login->session_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_key( POST_LOGIN_TEMPLATE_APPLICATION );

		if ( !post_login->session_key || !*post_login->session_key )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		post_login->empty_application_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_login_empty_application_system_string(
				POST_LOGIN_EMPTY_EXECUTABLE,
				POST_LOGIN_TEMPLATE_APPLICATION,
				post_login->session_key,
				post_login->
				      sql_injection_escape_login_name,
				post_login->
				      sql_injection_escape_application_key,
				post_login->
				      sql_injection_escape_application_title );
	}

	return post_login;
}

char *post_login_database_password( char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( !login_name || !*login_name ) return (char *)0;

	if ( ! ( appaserver_user =
			appaserver_user_fetch(
				login_name,
				0 /* not fetch_role_name_list */ ) ) )
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
	if ( post_login_name && *post_login_name )
		return 0;
	else
		return 1;
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
			boolean post_login_signup_duplicate,
			boolean post_login_signup,
			boolean post_login_missing_name,
			boolean missing_database_password,
			boolean name_email_address,
			boolean public_name,
			char *sql_injection_escape_password,
			char *database_password )
{
	if ( post_login_signup_duplicate )
	{
		return signup_duplicate;
	}
	else
	if ( post_login_signup )
	{
		return signup;
	}
	else
	if ( post_login_missing_name )
	{
		return missing_name;
	}
	else
	if ( public_name )
	{
		return public_login;
	}
	else
	if ( missing_database_password )
	{
		return database_password_blank;
	}
	else
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

char *post_login_output_pipe_string(
			char *post_login_email_output_template,
			boolean name_email_address,
			char *application_name,
			char *document_root,
			char *session_key )
{
	char output_pipe_string[ 256 ];

	if ( !post_login_email_output_template
	||   !application_name
	||   !document_root
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( name_email_address )
	{
		char output_file_template[ 128 ];

		sprintf(output_file_template,
			post_login_email_output_template,
			document_root,
			application_name,
			session_key );

		sprintf(output_pipe_string,
			"| cat > %s",
			output_file_template );
	}
	else
	{
		strcpy( output_pipe_string, "| cat" );
	}

	return strdup( output_pipe_string );
}

char *post_login_ip_address( void )
{
	return appaserver_library_server_address();
}

/*
char *post_login_location(
			char *cloudacus_location,
			char *ip_address )
{
	if ( string_strncmp( ip_address, "204.13.232" ) == 0 )
		return cloudacus_location;
	else
		return (char *)0;
}
*/

char *post_login_message(
			enum password_match_return password_match_return,
			boolean name_email_address )
{
	if ( password_match_return == signup && !name_email_address )
		return "password_application_succeeded_yn=y";
	else
	if ( password_match_return == signup && name_email_address )
		return "email_application_succeeded_yn=y";
	else
	if ( password_match_return == signup_duplicate )
		return "invalid_application_key_yn=y";
	else
	if ( password_match_return == email_login )
		return "emailed_login_yn=y";
	else
	if ( password_match_return == password_fail )
		return "invalid_login_yn=y";
	else
	if ( password_match_return == database_password_blank )
		return "account_deactivated_yn=y";
	else
	if ( password_match_return == missing_name )
		return "missing_name_yn=y";
	else
		return (char *)0;
}

char *post_login_redraw_index_screen_string(
			char *redraw_application_name,
			char *ip_address,
			char *message,
			boolean from_php )
{
	char screen_string[ 1024 ];
	char *ptr = screen_string;
	char location[ 128 ];
	DOCUMENT *document;

	if ( !redraw_application_name
	||   !ip_address
	||   !message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( from_php )
	{
		sprintf(location,
			"%s/appaserver/%s/index.php?%s",
			ip_address,
			redraw_application_name,
			message );
	}
	else
	{
		*location = '\0';
	}

	document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_quick_new(
			redraw_application_name,
			application_title_string( redraw_application_name ) );

	ptr += sprintf(
		ptr,
		"%s\n%s\n%s\n",
		document_content_type_html(),
		document->html,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_tag( (char *)0, (char *)0, (char *)0 ) );

	if ( !from_php )
	{
		ptr += sprintf( 
			ptr,
			"<p>Please try again.\n"
			"%s",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_html() );
	}
	else
	{
		ptr += sprintf( 
			ptr,
			"<script type=\"text/javascript\">\n"
			"window.location = \"%s\"\n"
			"</script>\n"
			"%s",
			location,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_html() );
	}

	return strdup( screen_string );
}

char *post_login_email_link_system_string(
			char *email_http_template,
			char *return_address,
			char *subject,
			char *application_name,
			char *session_key,
			char *login_name )
{
	char system_string[ 1024 ];
	char *ptr = system_string;
	char email_http_filename[ 128 ];
	char mailfile[ 128 ];
	FILE *output_file;

	sprintf( mailfile, "/tmp/mailfile_%d", getpid() );

	if ( ! ( output_file = fopen( mailfile, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: fopen(%s,'w') returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 mailfile );
		exit( 1 );
	}

	fprintf(output_file,
		"To: %s\n",
		login_name );

	fprintf(output_file,
		"From: %s\n",
		return_address );

	fprintf(output_file,
		"Subject: %s\n",
		subject );

	sprintf(email_http_filename,
		email_http_template,
		application_http_prefix( application_name ),
		appaserver_library_server_address(),
		application_name,
		session_key );

	fprintf(output_file,
		"Appaserver login:\n%s\n",
		email_http_filename );

	fclose( output_file );

	ptr += sprintf(
		ptr,
		"/usr/sbin/sendmail -t < %s;",
		mailfile );

	ptr += sprintf(
		ptr,
		"rm -f %s",
		mailfile );

	return strdup( system_string );
}

char *post_login_empty_application_system_string(
			char *empty_executable,
			char *template_application,
			char *session_key,
			char *login_name,
			char *application_key,
			char *application_title )
{
	char system_string[ 1024 ];

	if ( !empty_executable
	||   !template_application
	||   !session_key
	||   !login_name
	||   !application_key
	||   !application_title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s '%s' %s %s \"%s\" %c %c %c 2>>%s 1>/dev/null",
		empty_executable,
		template_application,
		session_key,
		login_name,
		"" /* role */,
		empty_executable /* process */,
		application_key /* destination_application */,
		application_title,
		'y' /* create_database_yn */,
		'n' /* delete_application_yn */,
		'y' /* really_yn */,
		appaserver_error_filename( template_application ) );

	return strdup( system_string );
}

boolean post_login_signup(
			char *signup_yn,
			char *application_key,
			char *application_title )
{
	if ( !signup_yn
	||   !application_key
	||   !*application_key
	||   !application_title
	||   !*application_title )
	{
		return 0;
	}

	if ( *signup_yn == 'y' )
		return 1;
	else
		return 0;
}

boolean post_login_signup_duplicate(
			char *application_key,
			char *appaserver_parameter_error_directory )
{
	return appaserver_library_application_exists(
			application_key,
			appaserver_parameter_error_directory );
}

char *post_login_redraw_application_name(
			char *application_name,
			char *application_key )
{
	if ( application_key && *application_key )
		return application_key;
	else
		return application_name;
}
