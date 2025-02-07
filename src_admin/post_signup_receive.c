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

	if ( !post_signup_receive->post ) return post_signup_receive;

	post_signup_receive->post_signup =
		post_signup_fetch(
			post_signup_receive->post_receive->email_address,
			post_signup_receive->post_receive->timestamp_space );

	if ( !post_signup_receive->post_signup ) return post_signup_receive;

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
			post_signup_receive->
				post->
				ip_address );

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
