/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_admin/post_signup_submit_execute.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "application.h"
#include "environ.h"
#include "session.h"
#include "appaserver_error.h"
#include "post_signup_submit.h"

int main( int argc, char **argv )
{
	POST_SIGNUP_SUBMIT *post_signup_submit;

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	document_content_type_output();

	/* Safely returns */
	/* -------------- */
	post_signup_submit = post_signup_submit_new();

	if ( !post_signup_submit->post_login_document )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: post_signup_submit->post_login_document is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_signup_submit->post )
	{
		environment_database_set( APPLICATION_ADMIN_NAME );

		session_insert(
			SESSION_TABLE,
			SESSION_INSERT,
			post_signup_submit->session->session_key,
			(char *)0 /* login_name */,
			post_signup_submit->session->login_date,
			post_signup_submit->session->login_time,
			post_signup_submit->session->http_user_agent,
			post_signup_submit->
				post_signup_submit_input->
				environment_remote_ip_address );

		(void)insert_statement_sql_execute(
			post_signup_submit->
				post_signup_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* insert_statement_list */,
			post_signup_submit->
				post->
				insert_statement );

		(void)insert_statement_sql_execute(
			post_signup_submit->
				post_signup_submit_input->
				appaserver_error_filename,
			post_signup_submit->
				form_field_datum_insert_statement_list,
				(char *)0 /* insert_statement */ );

		post_mailx(
			post_signup_submit->message,
			post_signup_submit->post_mailx_system_string );
	}

	printf(	"%s\n",
		post_signup_submit->
			post_login_document->
			html );

	return 0;
}
