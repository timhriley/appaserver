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
	POST_SIGNUP_SUBMIT *post_signup_submit;


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

char *post_signup_submit_reject_index_html_message(
		boolean missing_application_boolean,
		boolean invalid_application_boolean,
		boolean application_exists_boolean,
		boolean post_contact_submit_invalid_email_boolean,
		boolean missing_title_boolean )
{
}

LIST *post_signup_submit_form_field_datum_list(
		char *timestamp,
		char *email_address,
		char *form_name,
		char *application_key,
		char *application_title )
{
	LIST *form_field_datum_list = list_new();
	FORM_FIELD_DATUM *form_field_datum;

	if ( !timestamp
	||   !email_address
	||   !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"timestamp" /* field_name */,
			timestamp /* field_datum */,
			(char *)0 /* message_datum */,
			1 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"email_address" /* field_name */,
			email_address /* field_datum */,
			(char *)0 /* message_datum */,
			2 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"form_name" /* field_name */,
			form_name /* field_datum */,
			(char *)0 /* message_datum */,
			3 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"application_key" /* field_name */,
			application_key /* field_datum */,
			(char *)0 /* message_datum */,
			4 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"application_title" /* field_name */,
			application_title /* field_datum */,
			(char *)0 /* message_datum */,
			4 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	return form_field_datum_list;
}
