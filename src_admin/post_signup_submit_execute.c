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

	if ( post_signup_submit->post_signup )
	{
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
			(LIST *)0 /* insert_statement_list */,
			post_signup_submit->
				post_signup->
				insert_statement );

		post_mailx(
			post_signup_submit->message,
			post_signup_submit->post_mailx_system_string );
	}

	if ( post_signup_submit->post_login_document )
	{
		printf(	"%s\n",
			post_signup_submit->
				post_login_document->
				html );
	}

	if ( post_signup_submit->
		post->
		email_address->
		insert_statement )
	{
		(void)insert_statement_sql_execute(
			post_signup_submit->
				post_signup_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* insert_statement_list */,
			post_signup_submit->
				post->
				email_address->
				insert_statement );
	}

	if ( post_signup_submit->
		post->
		ip_deny_system_string )
	{
		if ( system(
			post_signup_submit->
				post->
				ip_deny_system_string ) ){}
	
		sleep( POST_SLEEP_SECONDS );
	}

	return 0;
}
