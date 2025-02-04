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
#include "appaserver_error.h"
#include "post_contact_receive.h"

int main( int argc, char **argv )
{
	POST_CONTACT_RECEIVE *post_contact_receive;

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	document_content_type_output();

	post_contact_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_receive_new(
			argc,
			argv );

	if ( post_contact_receive->post )
	{
		environment_database_set( APPLICATION_ADMIN_NAME );

		post_mailx(
			post_contact_receive->
				post_contact_receive_record->
				message,
			post_contact_receive->post_mailx_system_string );
	}

	if ( system(
		post_contact_receive->
			display_system_string ) ){}

	session_delete( post_receive_input->session_key );

	return 0;
}
