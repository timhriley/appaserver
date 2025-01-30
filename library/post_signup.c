/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_signup.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "session.h"
#include "application.h"
#include "application_create.h"
#include "execute_system_string.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "post_signup.h"

char *post_signup_reject_index_html_message(
		boolean missing_application_boolean,
		boolean invalid_application_boolean,
		boolean missing_login_name_boolean,
		boolean invalid_login_name_boolean,
		boolean application_exists_boolean,
		boolean missing_title_boolean )
{
	char *message = {0};

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

POST_SIGNUP *post_signup_new(
		int argc,
		char **argv )
{
	POST_SIGNUP *post_signup;

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

	post_signup = post_signup_calloc();

	session_environment_set( APPLICATION_TEMPLATE_NAME );

	post_signup->post_signup_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_input_new(
			argc,
			argv );

	if ( post_signup->post_signup_input->bot_generated->yes_boolean )
		return post_signup;

	post_signup->reject_index_html_message =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		post_signup_reject_index_html_message(
			post_signup->
				post_signup_input->
				post_login_input_missing_application_boolean,
			post_signup->
				post_signup_input->
				post_login_input_invalid_application_boolean,
			post_signup->
				post_signup_input->
				post_login_input_missing_login_name_boolean,
			post_signup->
				post_signup_input->
				post_login_input_invalid_login_name_boolean,
			post_signup->
				post_signup_input->
				post_login_input_application_exists_boolean,
			post_signup->
				post_signup_input->
				missing_title_boolean );

	if ( post_signup->reject_index_html_message )
	{
		post_signup->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				post_signup->
					post_signup_input->
					post_login_input_dictionary,
				post_signup->
					post_signup_input->
					post_login_input_application_name,
				post_signup->reject_index_html_message );

		return post_signup;
	}

	post_signup->session_key =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		session_key(
			APPLICATION_TEMPLATE_NAME );

	post_signup->
		execute_system_string_create_application =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_create_application(
				APPLICATION_CREATE_EXECUTABLE,
				post_signup->session_key,
				post_signup->
					post_signup_input->
					post_login_input_login_name,
				post_signup->
					post_signup_input->
					post_login_input_application_name
					/* destination_application */,
				post_signup->
					post_signup_input->
					application_title,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				appaserver_error_filename(
					APPLICATION_TEMPLATE_NAME ) );

	return post_signup;
}

POST_SIGNUP_INPUT *post_signup_input_new(
		int argc,
		char **argv )
{
	POST_SIGNUP_INPUT *post_signup_input;

	post_signup_input = post_signup_input_calloc();

	/* Safely returns */
	/* -------------- */
	post_signup_input->bot_generated = bot_generated_new( argv[ 0 ] );

	if ( post_signup_input->bot_generated->yes_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_dictionary =
		post_login_input_dictionary();

	if ( !post_signup_input->post_login_input_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			"post_login_input_dictionary" );
		exit( 1 );
	}

	post_signup_input->post_login_input_application_name =
	/* ------------------------------------------------------------ */
	/* Returns component of argv, component of dictionary, or null. */
	/* ------------------------------------------------------------ */
		post_login_input_application_name(
			argc,
			argv,
			post_signup_input->post_login_input_dictionary );

	post_signup_input->post_login_input_missing_application_boolean =
		post_login_input_missing_application_boolean(
			post_signup_input->post_login_input_application_name );

	if ( post_signup_input->post_login_input_missing_application_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_invalid_application_boolean =
		post_login_input_invalid_application_boolean(
			post_signup_input->post_login_input_application_name );

	if ( post_signup_input->post_login_input_invalid_application_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_application_exists_boolean =
		post_login_input_application_exists_boolean(
			post_signup_input->
				post_login_input_application_name,
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_log_directory() );

	if ( post_signup_input->post_login_input_application_exists_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_login_name =
		/* ----------------------------------------- */
		/* Returns component of dictionary, or null. */
		/* ----------------------------------------- */
		post_login_input_login_name(
			APPASERVER_USER_PRIMARY_KEY,
			post_signup_input->post_login_input_dictionary );

	post_signup_input->post_login_input_missing_login_name_boolean =
		post_login_input_missing_login_name_boolean(
			post_signup_input->post_login_input_login_name );

	if ( post_signup_input->post_login_input_missing_login_name_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_invalid_login_name_boolean =
		post_login_input_invalid_login_name_boolean(
			post_signup_input->post_login_input_login_name );

	if ( post_signup_input->post_login_input_invalid_login_name_boolean )
		return post_signup_input;

	post_signup_input->post_login_input_email_address_boolean =
		post_login_input_email_address_boolean(
			post_signup_input->post_login_input_login_name );

	post_signup_input->application_title =
		/* ----------------------------------------------------- */
		/* Returns component of dictionary, heap memory, or null */
		/* ----------------------------------------------------- */
		post_signup_input_application_title(
			post_signup_input->post_login_input_dictionary );

	post_signup_input->missing_title_boolean =
		post_signup_input_missing_title_boolean(
			post_signup_input->application_title );
	
	return post_signup_input;
}

POST_SIGNUP *post_signup_calloc( void )
{
	POST_SIGNUP *post_signup;

	if ( ! ( post_signup = calloc( 1, sizeof ( POST_SIGNUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup;
}

POST_SIGNUP_INPUT *post_signup_input_calloc( void )
{
	POST_SIGNUP_INPUT *post_signup_input;

	if ( ! ( post_signup_input =
			calloc( 1, sizeof ( POST_SIGNUP_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_input;
}

char *post_signup_input_application_title( DICTIONARY *dictionary )
{
	char *get;

	get =
		dictionary_get(
			"application_title",
			dictionary );

	if ( !get ) return NULL;

	return
	/* ---------------------------- */
	/* Returns heap memory or datum */
	/* ---------------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		get /* datum */ );
}

boolean post_signup_input_missing_title_boolean( char *application_title )
{
	if ( application_title && *application_title )
		return 0;
	else
		return 1;
}

char *post_signup_success_index_html_message(
		boolean email_address_boolean,
		char *post_signup_password )
{
	static char message[ 128 ];

	if ( !post_signup_password )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: post_signup_password is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( email_address_boolean )
	{
		snprintf(
			message,
			sizeof ( message ),
			"signup_succeeded_email_password=%s",
			post_signup_password );
	}
	else
	{
		snprintf(
			message,
			sizeof ( message ),
			"signup_succeeded_password=%s",
			post_signup_password );
	}

	return message;
}

char *post_signup_password( char *spool_fetch )
{
	if ( !spool_fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: spool_fetch is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return spool_fetch;
}

