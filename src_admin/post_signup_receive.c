/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_receive.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "form_field.h"
#include "application.h"
#include "application_create.h"
#include "execute_system_string.h"
#include "post_signup_receive.h"

POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_new(
		LIST *form_field_datum_list )
{
	POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record;
	FORM_FIELD_DATUM *form_field_datum;

	post_signup_receive_record =
		post_signup_receive_record_calloc();

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"application_key" ) ) )
	{
		post_signup_receive_record->application_key =
			form_field_datum->field_datum;
	}

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"application_title" ) ) )
	{
		post_signup_receive_record->application_title =
			form_field_datum->field_datum;
	}

	return post_signup_receive_record;
}

POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_calloc( void )
{
	POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record;

	if ( ! ( post_signup_receive_record =
			calloc( 1,
				sizeof ( POST_SIGNUP_RECEIVE_RECORD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_receive_record;
}

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
		"signup_succeed_email_password=%s",
		password );

	return parameter;
}

POST_SIGNUP_RECEIVE *post_signup_receive_new(
		int argc,
		char **argv )
{
	POST_SIGNUP_RECEIVE *post_signup_receive =
		post_signup_receive_calloc();

	post_signup_receive->post_receive_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_receive_input_new(
			argc,
			argv );

	post_signup_receive->post_signup_receive_record =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_receive_record_new(
			post_signup_receive->
				post_receive_input->
				post->
				form_field_datum_list );

	post_signup_receive->
		execute_system_string_create_application =
			execute_system_string_create_application(
				APPLICATION_CREATE_EXECUTABLE,
				post_signup_receive->
					post_receive_input->
					session_key,
				post_signup_receive->
					post_receive_input->
					post->
					email_address
					/* login_name */,
				post_signup_receive->
					post_signup_receive_record->
					application_key
					/* destination_application */,
				post_signup_receive->
					post_signup_receive_record->
					application_title,
				post_signup_receive->
					post_receive_input->
					appaserver_error_filename );

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
