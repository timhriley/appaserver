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

	if ( post_signup_receive->post_signup )
	{
		if ( !post_signup_receive->
			post->
			confirmation_received_date )
		{
			session_insert(
				SESSION_TABLE,
				SESSION_INSERT,
				post_signup_receive->session->session_key,
				post_signup_receive->
					post_receive->
					email_address
						/* login_name */,
				post_signup_receive->session->login_date,
				post_signup_receive->session->login_time,
				post_signup_receive->session->http_user_agent,
				post_signup_receive->
					session->
					remote_ip_address );
	
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

			(void)sql_execute(
				post_signup_receive->
					post_receive->
					appaserver_error_filename,
				(LIST *)0 /* sql_list */,
				post_signup_receive->
					post_confirmation_update_statement );

			post_signup_receive->post_login_document =
				post_login_document_new(
					(DICTIONARY *)0 /* location_website */,
					post_signup_receive->
						post_signup->
						application_key
							/* application_name */,
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
					session->
					session_key );
		}
	}
	else
	{
		sleep( POST_SLEEP_SECONDS );
	}

	return 0;
}
