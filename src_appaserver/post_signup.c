/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_signup.c       			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "date.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "document.h"
#include "environ.h"
#include "session.h"
#include "spool.h"
#include "application_create.h"
#include "post_signup.h"

int main( int argc, char **argv )
{
	POST_SIGNUP *post_signup;
	char destination[ 256 ];
	char *fetch;
	char *index_html_message;

	document_content_type_output();

	post_signup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_new(
			argc,
			argv );

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_CREATE_TEMPLATE_NAME,
		post_signup->
			post_signup_input->
			post_login_input_login_name );

	if ( post_signup->
		post_signup_input->
		bot_generated->
		yes_boolean )
	{
		appaserver_error_message_file(
			APPLICATION_CREATE_TEMPLATE_NAME,
			(char *)0 /* login_name */,
			post_signup->
				post_signup_input->
				bot_generated->
				message );

		sleep( BOT_GENERATED_SLEEP_SECONDS );
		exit( 1 );
	}

	if ( post_signup->post_login_document )
	{
		printf(	"%s\n",
			post_signup->
				post_login_document->
				html );

		exit( 0 );
	}

	environment_set(
		"DATABASE",
		APPLICATION_CREATE_TEMPLATE_NAME );

	session_insert(
		post_signup->session_key,
		post_signup->
			post_signup_input->
			post_login_input_login_name,
		date_now_yyyy_mm_dd( date_utc_offset() ),
		date_now_hhmm( date_utc_offset() ),
		left_string(
			destination,
			environment_http_user_agent(),
			80 ),
		post_signup->
			post_signup_input->
			bot_generated->
			remote_ip_address );

	fetch =
		spool_fetch(
			post_signup->
				execute_system_string_create_application,
			0 /* not capture_stderr_boolean */ );

	if ( !fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			"spool_fetch",
			post_signup->
				execute_system_string_create_application );
		exit( 1 );
	}

	index_html_message =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_signup_success_index_html_message(
			post_signup->
				post_signup_input->
				post_login_input_email_address_boolean,
			/* ------------------- */
			/* Returns spool_fetch */
			/* ------------------- */
			post_signup_password(
				fetch ) );

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
			index_html_message );

	printf(	"%s\n",
		post_signup->
			post_login_document->
			html );

	session_delete( post_signup->session_key );

	return 0;
}
