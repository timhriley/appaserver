/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_admin/post_contact_submit_execute.c		*/
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
#include "appaserver_error.h"
#include "post_contact_submit.h"

int main( int argc, char **argv )
{
	POST_CONTACT_SUBMIT *post_contact_submit;

	session_environment_set( APPLICATION_ADMIN_NAME );

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	document_content_type_output();

	/* Safely returns */
	/* -------------- */
	post_contact_submit = post_contact_submit_new();

	if ( post_contact_submit->post )
	{
		environment_database_set( APPLICATION_ADMIN_NAME );

		session_insert(
			SESSION_TABLE,
			SESSION_INSERT,
			post_contact_submit->session->session_key,
			(char *)0 /* login_name */,
			post_contact_submit->session->login_date,
			post_contact_submit->session->login_time,
			post_contact_submit->session->http_user_agent,
			post_contact_submit->
				post_contact_submit_input->
				environment_remote_ip_address );

		(void)insert_statement_sql_execute(
			post_contact_submit->
				post_contact_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* insert_statement_list */,
			post_contact_submit->
				post->
				insert_statement );

		(void)insert_statement_sql_execute(
			post_contact_submit->
				post_contact_submit_input->
				appaserver_error_filename,
			post_contact_submit->
				form_field_datum_insert_statement_list,
				(char *)0 /* insert_statement */ );

		post_mailx(
			post_contact_submit->message,
			post_contact_submit->post_mailx_system_string );
	}

	if ( system(
		post_contact_submit->
			display_system_string ) ){}

	return 0;
}
