/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_receive.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "environ.h"
#include "application_create.h"
#include "execute_system_string.h"
#include "post_signup_receive.h"

char *post_signup_receive_success_parameter( char *password )
{
	static char parameter[ 128 ];

	if ( !password )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: password is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		parameter,
		sizeof ( parameter ),
		"signup_success_email_password=%s",
		password );

	return parameter;
}

POST_SIGNUP_RECEIVE *post_signup_receive_new(
		int argc,
		char **argv )
{
	POST_SIGNUP_RECEIVE *post_signup_receive =
		post_signup_receive_calloc();

	session_environment_set( APPLICATION_ADMIN_NAME );

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	post_signup_receive->post_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_receive_new(
			APPLICATION_ADMIN_NAME,
			APPASERVER_MAILNAME_FILESPECIFICATION,
			argc,
			argv );

	post_signup_receive->post =
		post_fetch(
			post_signup_receive->post_receive->email_address,
			post_signup_receive->post_receive->timestamp_space );

	if ( !post_signup_receive->post )
	{
		post_signup_receive->sleep_seconds =
			post_signup_receive_sleep_seconds(
				POST_SLEEP_SECONDS );

		return post_signup_receive;
	}

	post_signup_receive->post_signup =
		post_signup_fetch(
			post_signup_receive->post_receive->email_address,
			post_signup_receive->post_receive->timestamp_space );

	if ( !post_signup_receive->post_signup )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: post_signup_fetch(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			post_signup_receive->post_receive->email_address,
			post_signup_receive->post_receive->timestamp_space );
		exit( 1 );
	}

	if ( post_signup_receive->post_signup->signup_password )
	{
		post_signup_receive->success_parameter =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_signup_receive_success_parameter(
				post_signup_receive->
					post_signup->
					signup_password );

		post_signup_receive->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				(DICTIONARY *)0
				/* input_dictionary for location_website */,
				post_signup_receive->
					post_signup->
					application_key
					/* application_name */,
				post_signup_receive->success_parameter );

		return post_signup_receive;
	}

	if ( post_signup_receive->post->confirmation_received_date )
	{
		post_signup_receive->in_process_parameter =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			post_signup_receive_in_process_parameter();

		post_signup_receive->post_login_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_login_document_new(
				(DICTIONARY *)0
				/* input_dictionary for location_website */,
				post_signup_receive->
					post_signup->
					application_key
					/* application_name */,
				post_signup_receive->in_process_parameter );

		return post_signup_receive;
	}

	post_signup_receive->session =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		session_new(
			APPLICATION_ADMIN_NAME,
			post_signup_receive->
				post_receive->
				email_address
				/* login_name */,
			post_signup_receive->
				post->
				http_user_agent,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			environment_remote_ip_address(
				ENVIRONMENT_REMOTE_KEY ) );

	post_signup_receive->
		execute_system_string_create_application =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_create_application(
				APPLICATION_CREATE_EXECUTABLE,
				post_signup_receive->
					session->
					session_key,
				post_signup_receive->
					post_receive->
					email_address
					/* login_name */,
				post_signup_receive->
					post_signup->
					application_key
					/* destination_application */,
				post_signup_receive->
					post_signup->
					application_title,
				post_signup_receive->
					post_receive->
					appaserver_error_filename );

	post_signup_receive->post_confirmation_update_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_confirmation_update_statement(
			POST_TABLE,
			POST_CONFIRMATION_COLUMN,
			post_signup_receive->post_receive->email_address,
			post_signup_receive->post_receive->timestamp_space );

	return post_signup_receive;
}

POST_SIGNUP_RECEIVE *post_signup_receive_calloc( void )
{
	POST_SIGNUP_RECEIVE *post_signup_receive;

	if ( ! ( post_signup_receive =
			calloc( 1,
				sizeof ( POST_SIGNUP_RECEIVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_receive;
}

char *post_signup_receive_password_statement(
		const char *post_signup_table,
		const char *post_signup_password_column,
		char *email_address,
		char *timestamp_space,
		char *post_signup_receive_password )
{
	static char password_statement[  256 ];

	if ( !email_address
	||   !timestamp_space
	||   !post_signup_receive_password )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		password_statement,
		sizeof ( password_statement ),
		"update %s set %s = '%s' where %s;",
		post_signup_table,
		post_signup_password_column,
		post_signup_receive_password,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_primary_where(
			email_address,
			timestamp_space ) );

	return password_statement;
}

char *post_signup_receive_in_process_parameter( void )
{
	return "signup_in_process=yes";
}

char *post_signup_receive_password( char *spool_fetch )
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

int post_signup_receive_sleep_seconds( const int post_sleep_seconds )
{
	return post_sleep_seconds;
}

