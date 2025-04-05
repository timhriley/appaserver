/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_admin/post_contact_receive_execute.c		*/
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
#include "sql.h"
#include "appaserver_error.h"
#include "post_contact_receive.h"

int main( int argc, char **argv )
{
	POST_CONTACT_RECEIVE *post_contact_receive;

	document_content_type_output();

	post_contact_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_receive_new(
			argc,
			argv );

	if ( post_contact_receive->post_contact )
	{
		if ( !post_contact_receive->
			post->
			confirmation_received_date )
		{
			post_mailx(
				post_contact_receive->
					post_contact->
					message,
				post_contact_receive->mailx_system_string );

			(void)sql_execute(
				SQL_EXECUTABLE,
				post_contact_receive->
					post_receive->
					appaserver_error_filename,
				(LIST *)0 /* sql_list */,
				post_contact_receive->
					post_confirmation_update_statement );
		}

		if ( system(
			post_contact_receive->
				display_system_string ) ){}
	}
	else
	{
		sleep( POST_SLEEP_SECONDS );
	}

	return 0;
}
