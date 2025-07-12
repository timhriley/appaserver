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
#include "sql.h"
#include "session.h"
#include "appaserver_error.h"
#include "post_contact_submit.h"

int main( int argc, char **argv )
{
	POST_CONTACT_SUBMIT *post_contact_submit;

	document_content_type_output();

	post_contact_submit =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_submit_new(
			argc,
			argv );

	if ( post_contact_submit->post_contact )
	{
		char *error_string;

		error_string = sql_execute(
			SQL_EXECUTABLE,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* sql_list */,
			post_contact_submit->
				post->
				insert_statement );

		if ( error_string )
		{
			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				error_string );
		}

		error_string = sql_execute(
			SQL_EXECUTABLE,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* sql_list */,
			post_contact_submit->
				post_contact->
				insert_statement );

		if ( error_string )
		{
			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				error_string );
		}

		post_mailx(
			post_contact_submit->message,
			post_contact_submit->post_mailx_system_string );
	}

	if ( post_contact_submit->display_system_string )
	{
		if ( system(
			post_contact_submit->
				display_system_string ) ){}
	}

	if ( post_contact_submit->
		post->
		email_address->
		insert_statement )
	{
		char *error_string;

		error_string = sql_execute(
			SQL_EXECUTABLE,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_error_filename,
			(LIST *)0 /* sql_list */,
			post_contact_submit->
				post->
				email_address->
				insert_statement );

		if ( error_string )
		{
			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				error_string );
		}
	}

	if ( post_contact_submit->
		post->
		ip_deny_system_string )
	{
		if ( system(
			post_contact_submit->
				post->
				ip_deny_system_string ) ){}
	
		sleep( POST_SLEEP_SECONDS );
	}

	return 0;
}
