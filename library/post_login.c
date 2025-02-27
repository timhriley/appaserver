/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_login.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "session.h"
#include "date.h"
#include "frameset.h"
#include "post_dictionary.h"
#include "document.h"
#include "execute_system_string.h"
#include "application_create.h"
#include "post_login.h"

POST_LOGIN *post_login_calloc( void )
{
	POST_LOGIN *post_login;

	if ( ! ( post_login = calloc( 1, sizeof ( POST_LOGIN ) ) ) )
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
	POST_LOGIN *post_login;

	if ( !argc
	||   !argv )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_login = post_login_calloc();

	post_login->post_login_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_login_input_new(
			argc,
			argv );

	if ( !post_login->post_login_input->missing_application_boolean
	&&   !post_login->post_login_input->public_boolean
	&&   !post_login->post_login_input->form_password_blank_boolean
	&&   !post_login->post_login_input->missing_login_name_boolean
	&&   !post_login->post_login_input->user_not_found_boolean )
	{
		post_login->security_encrypt_password =
			/* ---------------------------------------- */
			/* Returns database_password or heap memory */
			/* ---------------------------------------- */
			post_login_security_encrypt_password(
				post_login->post_login_input->form_password,
				post_login->
					post_login_input->
					appaserver_user->
					database_password );

	}

	if ( !post_login->post_login_input->missing_application_boolean
	&&   !post_login->post_login_input->user_not_found_boolean
	&&   !post_login->post_login_input->missing_login_name_boolean )
	{
		post_login->password_fail_boolean =
			post_login_password_fail_boolean(
				post_login->
					post_login_input->
					email_address_boolean,
				post_login->
					post_login_input->
					appaserver_user->
					database_password,
				post_login->
					post_login_input->
					form_password_blank_boolean,
				post_login->security_encrypt_password );
	}

	post_login->reject_index_html_parameter =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		post_login_reject_index_html_parameter(
			post_login->
				post_login_input->
				missing_application_boolean,
			post_login->
				post_login_input->
				invalid_application_boolean,
			post_login->
				post_login_input->
				missing_login_name_boolean,
			post_login->
				post_login_input->
				invalid_login_name_boolean,
			!post_login->
				post_login_input->
				application_log_exists_boolean
				/* application_not_exists_boolean */,
			post_login->
				post_login_input->
				user_not_found_boolean,
			post_login->
				post_login_input->
				deactivated_boolean,
			post_login->password_fail_boolean );

	if ( post_login->reject_index_html_parameter )
	{
		post_login->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				post_login->
					post_login_input->
					dictionary,
				post_login->
					post_login_input->
					application_name,
				post_login->reject_index_html_parameter );

		return post_login;
	}

	post_login->post_login_success =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_login_success_new(
			post_login->
				post_login_input->
				application_name,
			post_login->
				post_login_input->
				login_name,
			post_login->
				post_login_input->
				form_password_blank_boolean,
			post_login->
				post_login_input->
				email_address_boolean );

	if ( post_login->post_login_input->email_address_boolean
	&&   post_login->post_login_input->form_password_blank_boolean )
	{
		post_login->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				post_login->
					post_login_input->
					dictionary,
				post_login->
					post_login_input->
					application_name,
				post_login->
					post_login_success->
					index_html_parameter );
	}

	return post_login;
}

boolean post_login_input_missing_login_name_boolean( char *login_name )
{
	if ( login_name && *login_name )
		return 0;
	else
		return 1;
}

DICTIONARY *post_login_input_dictionary( void )
{
	POST_DICTIONARY *post_dictionary;

	post_dictionary =
		post_dictionary_stdin_new(
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	if ( post_dictionary )
	{
		return post_dictionary->original_post_dictionary;
	}
	else
	{
		return (DICTIONARY *)0;
	}
} 

char *post_login_input_application_name(
		int argc,
		char **argv,
		DICTIONARY *dictionary )
{
	char *application_name = {0};

	if ( argc == 2 )
	{
		application_name = argv[ 1 ];
	}
	else
	{
		LIST *key_list = list_new();
		char *get;
	
		list_set( key_list, "choose_application" );
		list_set( key_list, "application_key" );
		list_set( key_list, "application_name" );
	
		get = dictionary_list_get(
			dictionary,
			key_list );
	
		if ( get )
		{
			application_name = string_low( get );
		}
	}

	return application_name;
}

boolean post_login_input_email_address_boolean( char *login_name )
{
	return
	string_email_address_boolean( login_name );
}

boolean post_login_input_public_boolean( char *login_name )
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

char *post_login_success_output_pipe_string(
		const char *post_login_email_output_template,
		char *application_name,
		boolean form_password_blank_boolean,
		boolean email_address_boolean,
		char *document_root,
		char *session_key )
{
	char output_pipe_string[ 256 ];

	if ( !application_name
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

	if ( form_password_blank_boolean && email_address_boolean )
	{
		char output_file_template[ 128 ];

		snprintf(
			output_file_template,
			sizeof ( output_file_template ),
			post_login_email_output_template,
			document_root,
			application_name,
			session_key );

		snprintf(
			output_pipe_string,
			sizeof ( output_pipe_string ),
			"| cat > %s",
			output_file_template );
	}
	else
	{
		strcpy( output_pipe_string, "| cat" );
	}

	return strdup( output_pipe_string );
}

char *post_login_success_email_link_url(
		const char *email_http_template,
		char *application_name,
		char *session_key )
{
	static char email_link_url[ 128 ];

	if ( !application_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		email_link_url,
		sizeof ( email_link_url ),
		email_http_template,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		application_http_prefix(
			application_ssl_support_boolean(
				application_name ) ),
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_server_address(),
		application_name,
		session_key );

	return email_link_url;
}

char *post_login_success_sendmail_message(
		const char *subject,
		char *login_name,
		char *return_address,
		char *email_link_url )
{
	static char sendmail_message[ 256 ];
	char *ptr = sendmail_message;

	if ( !login_name
	||   !return_address
	||   !email_link_url )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"To: %s\n",
		login_name );

	ptr += sprintf(
		ptr,
		"From: %s\n",
		return_address );

	ptr += sprintf(
		ptr,
		"Subject: %s\n\n",
		subject );

	ptr += sprintf(
		ptr,
		"Appaserver login:\n%s",
		email_link_url );

	return sendmail_message;
}

char *post_login_success_sendmail_filename(
		char *data_directory,
		pid_t process_id )
{
	static char sendmail_filename[ 64 ];

	if ( !data_directory
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		sendmail_filename,
		sizeof ( sendmail_filename ),
		"%s/mailfile_%d",
		data_directory,
		getpid() );

	return sendmail_filename;
}

char *post_login_success_email_system_string(
		char *sendmail_message,
		char *sendmail_filename )
{
	char system_string[ 256 ];
	char *ptr = system_string;
	FILE *output_file;

	if ( !sendmail_message
	||   !sendmail_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			sendmail_filename );

	fprintf(output_file,
		"%s\n",
		sendmail_message );

	fclose( output_file );

	ptr += sprintf(
		ptr,
		/* --------------------------------------------------- */
		/* -t means to extract the recepients from the message */
		/* --------------------------------------------------- */
		"/usr/sbin/sendmail -t < %s;",
		sendmail_filename );

	ptr += sprintf(
		ptr,
		"rm -f %s",
		sendmail_filename );

	return strdup( system_string );
}

char *post_login_security_encrypt_password(
		char *post_login_password,
		char *database_password )
{
	enum password_function password_function;

	/* If database_password isn't encrypted */
	/* ------------------------------------ */
	if ( string_strcmp(
		database_password,
		post_login_password ) == 0 )
	{
		return database_password;
	}

	password_function =
		security_database_password_function(
			database_password );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	security_encrypt_password(
		post_login_password,
		password_function );
}

boolean post_login_input_user_not_found_boolean(
		APPASERVER_USER *appaserver_user )
{
	if ( !appaserver_user )
		return 1;
	else
		return 0;
}

char *post_login_input_form_password( DICTIONARY *dictionary )
{
	return
	dictionary_get(
		"password",
		dictionary );
}

POST_LOGIN_SUCCESS *post_login_success_new(
		char *application_name,
		char *login_name,
		boolean form_password_blank_boolean,
		boolean email_address_boolean )
{
	POST_LOGIN_SUCCESS *post_login_success;
	char *data_directory;

	if ( !application_name
	||   !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_login_success = post_login_success_calloc();

	post_login_success->session_key =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		session_key( application_name );

	post_login_success->output_pipe_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_login_success_output_pipe_string(
			POST_LOGIN_EMAIL_OUTPUT_TEMPLATE,
			application_name,
			form_password_blank_boolean,
			email_address_boolean,
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_document_root(),
			post_login_success->session_key );

	post_login_success->appaserver_error_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			application_name );

	post_login_success->execute_system_string_frameset =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_frameset(
		   FRAMESET_EXECUTABLE,
		   post_login_success->session_key,
		   login_name,
		   post_login_success->output_pipe_string,
		   post_login_success->appaserver_error_filename );

	if ( form_password_blank_boolean && email_address_boolean )
	{
		post_login_success->return_address =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_login_success_return_address(
				POST_LOGIN_RETURN_USERNAME,
				appaserver_mailname(
				      APPASERVER_MAILNAME_FILESPECIFICATION ) );

		post_login_success->email_link_url =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_login_success_email_link_url(
				POST_LOGIN_EMAIL_HTTP_TEMPLATE,
				application_name,
				post_login_success->session_key );

		post_login_success->sendmail_message =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_login_success_sendmail_message(
				POST_LOGIN_SUBJECT,
				login_name,
				post_login_success->return_address,
				post_login_success->email_link_url );

		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		data_directory = appaserver_parameter_data_directory();

		post_login_success->sendmail_filename =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_login_success_sendmail_filename(
				data_directory,
				getpid() /* process_id */ );

		post_login_success->email_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_login_success_email_system_string(
				post_login_success->sendmail_message,
				post_login_success->sendmail_filename );

		post_login_success->index_html_parameter =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			post_login_success_index_html_parameter();
	}

	return post_login_success;
}

POST_LOGIN_SUCCESS *post_login_success_calloc( void )
{
	POST_LOGIN_SUCCESS *post_login_success;

	if ( ! ( post_login_success =
			calloc( 1, sizeof ( POST_LOGIN_SUCCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_login_success;
}

char *post_login_reject_index_html_parameter(
		boolean missing_application_boolean,
		boolean invalid_application_boolean,
		boolean missing_login_name_boolean,
		boolean invalid_login_name_boolean,
		boolean application_not_exists_boolean,
		boolean user_not_found_boolean,
		boolean deactivated_boolean,
		boolean password_fail_boolean )
{
	char *parameter = {0};

	if ( missing_application_boolean )
		parameter = "missing_application_yn=y";
	else
	if ( invalid_application_boolean )
		parameter = "invalid_application_yn=y";
	else
	if ( missing_login_name_boolean )
		parameter = "invalid_login_yn=y";
	else
	if ( invalid_login_name_boolean )
		parameter = "invalid_login_yn=y";
	else
	if ( application_not_exists_boolean )
		parameter = "invalid_application_yn=y";
	else
	if ( user_not_found_boolean )
		parameter = "invalid_login_yn=y";
	else
	if ( deactivated_boolean )
		parameter = "account_deactivated_yn=y";
	else
	if ( password_fail_boolean )
		parameter = "invalid_password_yn=y";

	return parameter;
}

char *post_login_success_index_html_parameter( void )
{
	char *parameter = "emailed_login_yn=y";
	return parameter;
}

POST_LOGIN_DOCUMENT *post_login_document_new(
		DICTIONARY *dictionary,
		char *application_name,
		char *index_html_parameter )
{
	POST_LOGIN_DOCUMENT *post_login_document;

	if ( !index_html_parameter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: index_html_parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_login_document = post_login_document_calloc();

	post_login_document->filename =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_login_document_filename(
			dictionary );

	post_login_document->location_enum =
		post_login_document_location_enum_resolve(
			post_login_document->filename );

	post_login_document->application_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		post_login_document_application_name(
			APPLICATION_TEMPLATE_NAME,
			application_name );

	post_login_document->filename_location =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_login_document_filename_location(
			post_login_document->application_name,
			index_html_parameter,
			post_login_document->location_enum );

	post_login_document->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			post_login_document->application_name );

	post_login_document->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_basic_new(
			post_login_document->application_name,
			(LIST *)0 /* javascript_filename_list */,
			post_login_document->application_title_string );

	post_login_document->document_body_open_tag =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_open_tag();

	post_login_document->document_close_tag =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_tag();

	post_login_document->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_login_document_html(
			post_login_document->location_enum,
			post_login_document->filename_location,
			post_login_document->document->html,
			post_login_document->document_body_open_tag,
			post_login_document->document_close_tag );

	return post_login_document;
}

POST_LOGIN_DOCUMENT *post_login_document_calloc( void )
{
	POST_LOGIN_DOCUMENT *post_login_document;

	if ( ! ( post_login_document =
			calloc( 1,
				sizeof ( POST_LOGIN_DOCUMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_login_document;
}

char *post_login_document_filename_location(
		char *application_name,
		char *index_html_parameter,
		enum post_login_document_location_enum location_enum )
{
	static char location[ 128 ];

	if ( !application_name
	||   !index_html_parameter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( location_enum == location_html )
	{
		string_strcpy(
			location,
			index_html_parameter,
			sizeof ( location ) );

		if ( string_exists(
			location,
			"_yn=y" ) )
		{
			*(location + strlen( location ) - 5) = '\0';
		}
	}
	else
	if ( location_enum == location_php )
	{
		snprintf(
			location,
			sizeof ( location ),
			"/appaserver/%s/index.php?%s",
			application_name,
			index_html_parameter );
	}
	else
	/* ------------------------ */
	/* Must be location_website */
	/* ------------------------ */
	{
		snprintf(
			location,
			sizeof ( location ),
			"/index.php?%s",
			index_html_parameter );
	}

	return location;
}

char *post_login_document_html(
		enum post_login_document_location_enum location_enum,
		char *post_login_document_filename_location,
		char *document_html,
		char *document_body_open_tag,
		char *document_close_tag )
{
	char html[ 1024 ];
	char destination[ 128 ];

	if ( !post_login_document_filename_location
	||   !document_html
	||   !document_body_open_tag
	||   !document_close_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( location_enum == location_html )
	{
		snprintf(
			html,
			sizeof ( html ),
			"%s\n%s\n"
			"<p>%s.\n"
			"%s",
			document_html,
			document_body_open_tag,
			string_initial_capital(
				destination,
				post_login_document_filename_location ),
			document_close_tag );
	}
	else
	{
		snprintf(
			html,
			sizeof ( html ),
			"%s\n%s\n"
			"<script type=\"text/javascript\">\n"
			"window.location = \"%s\"\n"
			"</script>\n"
			"%s",
			document_html,
			document_body_open_tag,
			post_login_document_filename_location,
			document_close_tag );
	}

	return strdup( html );
}

boolean post_login_input_missing_application_boolean( char *application_name )
{
	if ( application_name && *application_name )
		return 0;
	else
		return 1;
}

boolean post_login_input_invalid_application_boolean( char *application_name )
{
	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	!string_mnemonic_boolean( application_name );
}

boolean post_login_input_invalid_login_name_boolean( char *login_name )
{
	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	!string_mnemonic_boolean( login_name );
}

char *post_login_input_login_name(
		const char *appaserver_user_primary_key,
		DICTIONARY *dictionary )
{
	char *login_name = {0};
	char *get;

	get =
		dictionary_get(
			(char *)appaserver_user_primary_key,
			dictionary );

	if ( get ) login_name = string_low( get );

	return login_name;
}

boolean post_login_user_not_found_boolean( APPASERVER_USER *appaserver_user )
{
	if ( !appaserver_user )
		return 1;
	else
		return 0;
}

boolean post_login_input_deactivated_boolean(
		char *database_password,
		boolean deactivated_boolean,
		boolean public_boolean )
{
	if ( deactivated_boolean ) return 1;
	if ( public_boolean ) return 0;

	if ( database_password )
		return 0;
	else
		return 1;
}

boolean post_login_input_form_password_blank_boolean( char *form_password )
{
	if ( !form_password || !*form_password )
		return 1;
	else
		return 0;
}

boolean post_login_password_fail_boolean(
		boolean email_address_boolean,
		char *database_password,
		boolean form_password_blank_boolean,
		char *security_encrypt_password )
{
	if ( email_address_boolean
	&&   form_password_blank_boolean )
	{
		return 0;
	}

	return
	( string_strcmp(
		security_encrypt_password,
		database_password ) != 0 );
}

char *post_login_document_filename( DICTIONARY *dictionary )
{
	return
	dictionary_get(
		"filename",
		dictionary );
}

enum post_login_document_location_enum
	post_login_document_location_enum_resolve(
		char *post_login_document_filename )
{
	if ( !post_login_document_filename )
	{
		return location_website;
	}

	if ( strcmp(
		post_login_document_filename,
		"cloudacus_com/index.php" ) == 0 )
	{
		return location_website;
	}

	if ( strcmp(
		post_login_document_filename,
		"index.html" ) == 0 )
	{
		return location_html;
	}

	if ( strcmp(
		post_login_document_filename,
		"index.php" ) == 0 )
	{
		return location_php;
	}
	else
	{
		return location_website;
	}
}

char *post_login_success_return_address(
		const char *return_username,
		char *appaserver_mailname )
{
	static char return_address[ 64 ];

	if ( !appaserver_mailname )
	{
		fprintf(stderr,
			"%s/%s()/%d: appaserver_mailname is empty\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	snprintf(
		return_address,
		sizeof ( return_address ),
		"%s@%s",
		return_username,
		appaserver_mailname );

	return return_address;
}

POST_LOGIN_INPUT *post_login_input_calloc( void )
{
	POST_LOGIN_INPUT *post_login_input;

	if ( ! ( post_login_input = calloc( 1, sizeof ( POST_LOGIN_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_login_input;
}

POST_LOGIN_INPUT *post_login_input_new(
		int argc,
		char **argv )
{
	POST_LOGIN_INPUT *post_login_input;

	if ( !argc
	||   !argv )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_login_input = post_login_input_calloc();

	post_login_input->dictionary = post_login_input_dictionary();

	post_login_input->application_name =
	/* ------------------------------------------------------------ */
	/* Returns component of argv, component of dictionary, or null. */
	/* ------------------------------------------------------------ */
		post_login_input_application_name(
			argc,
			argv,
			post_login_input->dictionary );

	post_login_input->missing_application_boolean =
		post_login_input_missing_application_boolean(
			post_login_input->application_name );

	if ( post_login_input->missing_application_boolean )
	{
		/* Send argv to admin log file. */
		/* ---------------------------- */
		post_login_input->application_name = APPLICATION_ADMIN_NAME;
		return post_login_input;
	}

	post_login_input->remote_ip_address =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_login_input_remote_ip_address();

	if ( !post_login_input->remote_ip_address )
		return post_login_input;

	post_login_input->invalid_application_boolean =
		post_login_input_invalid_application_boolean(
			post_login_input->application_name );

	if ( post_login_input->invalid_application_boolean )
	{
		/* Send argv to admin log file. */
		/* ---------------------------- */
		post_login_input->application_name = APPLICATION_ADMIN_NAME;
		return post_login_input;
	}

	post_login_input->login_name =
		/* ----------------------------------------- */
		/* Returns component of dictionary, or null. */
		/* ----------------------------------------- */
		post_login_input_login_name(
			APPASERVER_USER_PRIMARY_KEY,
			post_login_input->dictionary );

	post_login_input->missing_login_name_boolean =
		post_login_input_missing_login_name_boolean(
			post_login_input->login_name );

	if ( post_login_input->missing_login_name_boolean )
		return post_login_input;

	session_environment_set( post_login_input->application_name );

	post_login_input->application_log_exists_boolean =
		application_log_exists_boolean(
			post_login_input->application_name,
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_log_directory() );

	if ( !post_login_input->application_log_exists_boolean )
	{
		/* Send argv to admin log file. */
		/* ---------------------------- */
		post_login_input->application_name = APPLICATION_ADMIN_NAME;
		return post_login_input;
	}

	post_login_input->email_address_boolean =
		post_login_input_email_address_boolean(
			post_login_input->login_name );

	if ( !post_login_input->email_address_boolean )
	{
		post_login_input->invalid_login_name_boolean =
			post_login_input_invalid_login_name_boolean(
				post_login_input->login_name );
	}

	if ( post_login_input->invalid_login_name_boolean )
		return post_login_input;

	post_login_input->appaserver_user =
		appaserver_user_fetch(
			post_login_input->login_name,
			0 /* not fetch_role_name_list */ );

	post_login_input->user_not_found_boolean =
		post_login_input_user_not_found_boolean(
			post_login_input->appaserver_user );

	if ( post_login_input->user_not_found_boolean )
		return post_login_input;

	post_login_input->public_boolean =
		post_login_input_public_boolean(
			post_login_input->login_name );

	post_login_input->deactivated_boolean =
		post_login_input_deactivated_boolean(
			post_login_input->
				appaserver_user->
				database_password,
			post_login_input->
				appaserver_user->
				deactivated_boolean,
			post_login_input->public_boolean );

	if ( post_login_input->deactivated_boolean )
		return post_login_input;

	if ( !post_login_input->public_boolean )
	{
		post_login_input->form_password =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			post_login_input_form_password(
				post_login_input->dictionary );

		post_login_input->form_password_blank_boolean =
			post_login_input_form_password_blank_boolean(
				post_login_input->form_password );

	}

	return post_login_input;
}

char *post_login_document_application_name(
		const char *application_template_name,
		char *post_login_input_application_name )
{
	if ( post_login_input_application_name )
		return post_login_input_application_name;
	else
		return (char *)application_template_name;

}

char *post_login_input_remote_ip_address( void )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	environment_get( "REMOTE_ADDR" );
}
