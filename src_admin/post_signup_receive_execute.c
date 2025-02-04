/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_admin/post_signup_receive_execute.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "application.h"
#include "environ.h"
#include "document.h"
#include "session.h"
#include "spool.h"
#include "appaserver_error.h"
#include "post_signup_receive.h"

int main( int argc, char **argv )
{
	POST_SIGNUP_RECEIVE *post_signup_receive;
	char *fetch;

	session_environment_set( APPLICATION_ADMIN_NAME );

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	document_content_type_output();

	post_signup_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_receive_new(
			argc,
			argv );

	fetch =
		spool_fetch(
			post_signup_receive->
				execute_system_string_create_application,
			0 /* not capture_stderr_boolean */ );

	if ( !fetch )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: spool_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			post_signup_receive->
				execute_system_string_create_application );
		exit( 1 );
	}

	post_signup_receive->post_login_document =
		post_login_document_new(
			(DICTIONARY *)0 /* location_website */,
			post_signup_receive->
				post_signup_receive_record->
				application_key /* application_name */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_signup_receive_success_parameter(
				fetch /* password */ ) );

	printf(	"%s\n",
		post_signup_receive->
			post_login_document->
			html );

	session_delete(
		post_signup_receive->
			post_receive_input->
			session_key );

	return 0;
}
