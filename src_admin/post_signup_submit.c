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
#include "piece.h"
#include "appaserver_error.h"
#include "form.h"
#include "security.h"
#include "post.h"
#include "form_field_datum.h"
#include "post_contact_submit.h"
#include "post_signup_receive.h"
#include "post_signup_submit.h"

POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input_new( void )
{
	POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input;

	post_signup_submit_input = post_signup_submit_input_calloc();

	post_signup_submit_input->
		appaserver_parameter =
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

	post_signup_submit_input_application_title_empty_boolean =
		post_signup_submit_input_application_title_empty_boolean(
			post_signup_submit_input->application_title );

	post_signup_submit_input->environment_remote_ip_address =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_remote_ip_address();

	post_signup_submit_input->environment_http_user_agent =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_http_user_agent();

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
			application_name,
			log_directory ) );
}

POST_SIGNUP_SUBMIT *post_signup_submit_new( void )
{
	POST_SIGNUP_SUBMIT *post_signup_submit =
		post_signup_submit_calloc();

	post_signup_submit->post_signup_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_submit_input_new();

	post_signup_submit->reject_index_html_message =
		post_signup_submit_reject_index_html_message(
			post_signup_submit->
				post_signup_submit_input->
				post_login_input_missing_application_boolean,
			post_signup_submit->
				post_signup_submit_input->
				post_login_input_invalid_application_boolean,
			post_signup_submit->
				post_signup_submit_input->
				application_exists_boolean,
			post_signup_submit->
				post_signup_submit_input->
				post_contact_submit_invalid_email_boolean,
			post_signup_submit->
				post_signup_submit_input->
				missing_title_boolean );

if ( post_signup_reject_index_html_message() )
{
	POST_LOGIN_DOCUMENT *post_login_document_new(
		APPLICATION_ADMIN_NAME,
		(DICTIONARY *)0 /* location_website */,
		post_signup_submit_reject_index_html_message() );

	return this;
}

POST *post_new(
	FORM_SIGNUP /* FORM_NAME */,
	post_signup_submit_input->
		email_address,
	post_signup_submit_input->
		environment_remote_ip_address );

FORM_FIELD_INSERT_LIST *
	post_signup_submit_form_field_insert_list(
		post_signup_submit_input->post->timestamp,
		post_signup_submit_input->email_address,
		post->form_name,
		post_signup_submit_input->application_key,
		post_signup_submit_input->application_title );

LIST *form_field_datum_insert_statement_list(
	FORM_FIELD_DATUM_TABLE,
	post_signup_submit_form_field_insert_list()->list
		/* form_field_insert_list */ );

SESSION *session_new(
	APPLICATION_ADMIN_NAME,
	POST_LOGIN_NAME,
	post_signup_submit_input->
		environment_http_user_agent,
	post_signup_submit_input->
		environment_remote_ip_address );

char *post_return_email(
	POST_RETURN_USERNAME,
	post_signup_submit_input->
		appaserver_mailname );

char *post_mailx_system_string(
	POST_SIGNUP_SUBMIT_SUBJECT,
	post_return_email() );

char *post_receive_url(
	POST_SIGNUP_RECEIVE_EXECUTABLE,
	post_signup_submit_input->
		apache_cgi_directory,
	post_signup_submit_input->
		email_address,
	post->timestamp,
	session_new->session_key );

char *post_signup_submit_message(
	POST_SIGNUP_SUBMIT_MESSAGE_PROMPT,
	post_signup_submit_receive_url() );

POST_LOGIN_DOCUMENT *
	post_login_document_new(
		APPLICATION_ADMIN_NAME,
		(DICTIONARY *)0 /* location_website */,
		POST_SIGNUP_SUCCESS_INDEX_HTML_MESSAGE );

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

char *post_signup_submit_reject_index_html_parameter(
		boolean missing_application_boolean,
		boolean invalid_application_boolean,
		boolean application_exists_boolean,
		boolean post_contact_submit_invalid_email_boolean,
		boolean missing_title_boolean )
{
	char *parameter = {0};

	if ( missing_application_boolean )
		message = "signup_missing_application_yn=y";
	else
	if ( invalid_application_boolean )
		message = "signup_invalid_application_yn=y";
	else
	if ( missing_login_name_boolean )
		message = "signup_missing_name_yn=y";
	else
	if ( invalid_login_name_boolean )
		message = "signup_invalid_login_yn=y";
	else
	if ( application_exists_boolean )
		message = "signup_application_exists_yn=y";
	else
	if ( missing_title_boolean )
		message = "signup_missing_title_yn=y";

	return message;
}

}

FORM_FIELD_INSERT_LIST *post_signup_submit_form_field_insert_list(
		char *email_address,
		char *application_key,
		char *application_title,
		char *form_name,
		char *timestamp )
{
	FORM_FIELD_INSERT_LIST *form_field_insert_list;
	LIST *form_field_datum_list;
	FORM_FIELD_DATUM *form_field_datum;

	if ( !email_address
	||   !form_name
	||   !application_key
	||   !application_title
	||   !timestamp )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Safely returns */
	/* -------------- */
	form_field_insert_list = form_field_insert_list_new();

	/* Set application_key */
	/* ------------------- */
	form_field_datum_list = list_new();

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"application_key" /* field_name */,
			application_key /* field_datum */,
			(char *)0 /* message_datum */,
			0 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

	form_field_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_insert_new(
			email_address,
			form_name,
			timestamp,
			form_field_datum_list /* in/out */ );

	list_set(
		form_field_insert_list->list,
		form_field_insert );

	/* Set application_title */
	/* --------------------- */
	form_field_datum_list = list_new();

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"application_title" /* field_name */,
			application_title /* field_datum */,
			(char *)0 /* message_datum */,
			0 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

	form_field_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_insert_new(
			email_address,
			form_name,
			timestamp,
			form_field_datum_list /* in/out */ );

	list_set(
		form_field_insert_list->list,
		form_field_insert );

	return form_field_insert_list;
}
