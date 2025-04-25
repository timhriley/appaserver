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
#include "sql.h"
#include "appaserver_error.h"
#include "post_signup_receive.h"

int main( int argc, char **argv )
{
	POST_SIGNUP_RECEIVE *post_signup_receive;
	char *fetch;

	document_content_type_output();

	post_signup_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_signup_receive_new(
			argc,
			argv );

	if ( post_signup_receive->session )
	{
		(void)sql_execute(
			SQL_EXECUTABLE,
			post_signup_receive->
				post_receive->
				appaserver_error_filename,
			(LIST *)0 /* sql_list */,
			post_signup_receive->
				post_confirmation_update_statement );

		session_insert(
			post_signup_receive->
				session->
				insert_string,
			post_signup_receive->
				session->
				insert_system_string );
	
		fetch =
		   spool_fetch(
			post_signup_receive->
		    		execute_system_string_create_application,
			0 /* not capture_stderr_boolean */ );
	
		if ( !fetch )
		{
			fprintf(
			  stderr,
		"ERROR in %s/%s()/%d: spool_fetch(%s) returned empty.\n",
			  __FILE__,
			  __FUNCTION__,
			  __LINE__,
			  post_signup_receive->
			     execute_system_string_create_application );
			exit( 1 );
		}

		session_delete(
			SESSION_TABLE,
			APPLICATION_ADMIN_NAME,
			post_signup_receive->
				session->
				session_key );

		post_signup_receive->password =
			/* ----------------- */
			/* Returns parameter */
			/* ----------------- */
			post_signup_receive_password(
				fetch );

		post_signup_receive->success_parameter =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_signup_receive_success_parameter(
				post_signup_receive->password );

		post_signup_receive->password_statement =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_signup_receive_password_statement(
				POST_SIGNUP_TABLE,
				POST_SIGNUP_PASSWORD_COLUMN,
				post_signup_receive->
					post_receive->
					email_address,
				post_signup_receive->
					post_receive->
					timestamp_space,
				post_signup_receive->password );

		(void)sql_execute(
			SQL_EXECUTABLE,
			post_signup_receive->
				post_receive->
				appaserver_error_filename,
			(LIST *)0 /* sql_list */,
			post_signup_receive->password_statement );

		post_signup_receive->post_login_document =
			post_login_document_new(
				(DICTIONARY *)0
				/* input_dictionary for location_website */,
				post_signup_receive->
					post_signup->
					application_key /* application_name */,
				post_signup_receive->success_parameter );
	}

	if ( post_signup_receive->post_login_document )
	{
		fflush( stdout );
		printf(	"%s\n",
			post_signup_receive->
				post_login_document->
				html );
		fflush( stdout );
	}

	if ( post_signup_receive->sleep_seconds )
	{
		sleep( post_signup_receive->sleep_seconds );
	}

	return 0;
}
