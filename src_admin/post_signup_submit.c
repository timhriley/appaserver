/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_submit.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "application.h"
#include "application_log.h"
#include "piece.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "security.h"
#include "post.h"
#include "post_contact_submit.h"
#include "post_signup_receive.h"
#include "post_signup_submit.h"

POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input_new( void )
{
	POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input;

	post_signup_submit_input = post_signup_submit_input_calloc();

	post_signup_submit_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_signup_submit_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* Used when expecting a spooled file */
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	post_signup_submit_input->post_contact_submit_input_email_address =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_submit_input_email_address(
			post_signup_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_signup_submit_input->
		post_contact_submit_input_email_invalid_boolean =
		    post_contact_submit_input_email_invalid_boolean(
			post_signup_submit_input->
				post_contact_submit_input_email_address );

	if ( post_signup_submit_input->
		post_contact_submit_input_email_invalid_boolean )
	{
		return post_signup_submit_input;
	}

	post_signup_submit_input->application_key =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_signup_submit_input_application_key(
			post_signup_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_signup_submit_input->application_key_invalid_boolean =
		post_signup_submit_input_application_key_invalid_boolean(
			post_signup_submit_input->application_key );

	if ( post_signup_submit_input->application_key_invalid_boolean )
	{
		return post_signup_submit_input;
	}

	post_signup_submit_input->application_exists_boolean =
		post_signup_submit_input_application_exists_boolean(
			post_signup_submit_input->
				appaserver_parameter->
				log_directory,
			post_signup_submit_input->application_key );

	if ( post_signup_submit_input->application_exists_boolean )
	{
		return post_signup_submit_input;
	}

	post_signup_submit_input->application_title =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_signup_submit_input_application_title(
			post_signup_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_signup_submit_input->application_title_empty_boolean =
		post_signup_submit_input_application_title_empty_boolean(
			post_signup_submit_input->application_title );

	post_signup_submit_input->environment_remote_ip_address =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_remote_ip_address(
			ENVIRONMENT_REMOTE_KEY );

	post_signup_submit_input->environment_http_user_agent =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_http_user_agent(
			ENVIRONMENT_USER_AGENT_KEY );

	post_signup_submit_input->appaserver_mailname =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_mailname(
			APPASERVER_MAILNAME_FILESPECIFICATION );

	post_signup_submit_input->appaserver_error_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			APPLICATION_ADMIN_NAME );

	return post_signup_submit_input;
}

POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input_calloc( void )
{
	POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input;

	if ( ! ( post_signup_submit_input =
			calloc( 1,
				sizeof ( POST_SIGNUP_SUBMIT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_submit_input;
}

char *post_signup_submit_input_application_key( DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"application_key",
		post_dictionary );
}

boolean post_signup_submit_input_application_key_invalid_boolean(
		char *application_key )
{
	if ( !application_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	!string_mnemonic_boolean( application_key );
}

char *post_signup_submit_input_application_title( DICTIONARY *post_dictionary )
{
	char *get;

	get = dictionary_get(
		"application_title",
		post_dictionary );

	if ( !get ) return NULL;

	return
	/* ---------------------------- */
	/* Returns heap memory or datum */
	/* ---------------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		get /* datum */ );
}

boolean post_signup_submit_input_application_title_empty_boolean(
		char *application_title )
{
	if ( application_title && *application_title )
		return 0;
	else
		return 1;
}

boolean post_signup_submit_input_application_exists_boolean(
		char *log_directory,
		char *application_key )
{
	if ( !log_directory
	||   !application_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	application_exists_boolean(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_log_filename(
			APPLICATION_LOG_EXTENSION,
			application_key /* application_name */,
			log_directory ) );
}

POST_SIGNUP_SUBMIT *post_signup_submit_new( void )
{
	POST_SIGNUP_SUBMIT *post_signup_submit =
		post_signup_submit_calloc();

	post_signup_submit->post_signup_submit_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_submit_input_new();

	post_signup_submit->index_html_parameter =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		post_signup_submit_reject_parameter(
			post_signup_submit->
				post_signup_submit_input->
				post_contact_submit_input_email_invalid_boolean,
			post_signup_submit->
				post_signup_submit_input->
				application_key_invalid_boolean,
			post_signup_submit->
				post_signup_submit_input->
				application_exists_boolean,
			post_signup_submit->
				post_signup_submit_input->
				application_title_empty_boolean );

	if ( post_signup_submit->index_html_parameter )
	{
		post_signup_submit->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				(DICTIONARY *)0 /* location_website */,
				APPLICATION_ADMIN_NAME,
				post_signup_submit->index_html_parameter );

		return post_signup_submit;
	}

	post_signup_submit->post =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_new(
			APPLICATION_ADMIN_NAME,
			post_signup_submit->
				post_signup_submit_input->
				post_contact_submit_input_email_address );

	if ( post_signup_submit->post->email_address->blocked_boolean )
	{
		return post_signup_submit;
	}

	post_signup_submit->post_signup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_new(
			post_signup_submit->post->email_address->email_address,
			post_signup_submit->post->timestamp,
			post_signup_submit->
				post_signup_submit_input->
				application_key,
			post_signup_submit->
				post_signup_submit_input->
				application_title );

	post_signup_submit->post_return_email =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_return_email(
			POST_RETURN_USERNAME,
			post_signup_submit->
				post_signup_submit_input->
				appaserver_mailname );

	post_signup_submit->post_mailx_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_mailx_system_string(
			POST_SIGNUP_SUBMIT_SUBJECT,
			post_signup_submit->post->email_address->email_address,
			post_signup_submit->post_return_email );

	post_signup_submit->post_receive_url =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_receive_url(
			POST_SIGNUP_RECEIVE_EXECUTABLE,
			"cloudacus.com" /* website_domain_name */,
			post_signup_submit->
				post_signup_submit_input->
				appaserver_parameter->
				apache_cgi_directory,
			post_signup_submit->
				post_signup_submit_input->
				post_contact_submit_input_email_address,
			post_signup_submit->
				post->
				timestamp
				/* timestamp_space */ );

	appaserver_error_message_file(
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_signup_submit_log_message(
			POST_SIGNUP_SUBMIT_LOG_PROMPT,
			post_signup_submit->post_receive_url ) );

	post_signup_submit->message =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_signup_submit_message(
			POST_SIGNUP_SUBMIT_MESSAGE_PROMPT,
			post_signup_submit->post_receive_url );

	post_signup_submit->post_login_document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_login_document_new(
			(DICTIONARY *)0 /* location_website */,
			APPLICATION_ADMIN_NAME,
			POST_SIGNUP_SUBMIT_CONFIRM_PARAMETER );

	return post_signup_submit;
}

POST_SIGNUP_SUBMIT *post_signup_submit_calloc( void )
{
	POST_SIGNUP_SUBMIT *post_signup_submit;

	if ( ! ( post_signup_submit =
			calloc( 1,
				sizeof ( POST_SIGNUP_SUBMIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_submit;
}

char *post_signup_submit_reject_parameter(
		boolean email_invalid_boolean,
		boolean application_key_invalid_boolean,
		boolean application_exists_boolean,
		boolean application_title_empty_boolean )
{
	char *parameter = {0};

	if ( email_invalid_boolean )
		parameter = "signup_invalid_email_yn=y";
	else
	if ( application_key_invalid_boolean )
		parameter = "signup_invalid_application_yn=y";
	else
	if ( application_exists_boolean )
		parameter = "signup_application_exists_yn=y";
	else
	if ( application_title_empty_boolean )
		parameter = "signup_missing_title_yn=y";

	return parameter;
}

char *post_signup_submit_message(
		const char *post_signup_submit_message_prompt,
		char *receive_url )
{
	static char message[ 128 ];

	if ( !receive_url )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: receive_url is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		message,
		sizeof ( message ),
		"%s: %s",
		post_signup_submit_message_prompt,
		receive_url );

	return message;
}

char *post_signup_submit_log_message(
		const char *post_signup_submit_log_prompt,
		char *post_receive_url )
{
	static char message[ 256 ];

	snprintf(
		message,
		sizeof ( message ),
		"%s: %s",
		post_signup_submit_log_prompt,
		post_receive_url );

	return message;
}
