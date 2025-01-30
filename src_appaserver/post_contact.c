/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_contact.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "dictionary.h"
#include "application.h"
#include "document.h"
#include "application_create.h"
#include "appaserver_error.h"
#include "post_contact.h"

int main( int argc, char **argv )
{
	POST_CONTACT *post_contact;

	document_content_type_output();

	/* Stub */
	/* ---- */
	if ( argc ){}

	/* Safely returns */
	/* -------------- */
	post_contact = post_contact_new( argv[ 0 ] );

	if ( post_contact->
		post_contact_input->
		bot_generated->
		yes_boolean )
	{
		appaserver_error_message_file(
			APPLICATION_TEMPLATE_NAME,
			(char *)0 /* login_name */,
			post_contact->
				post_contact_input->
				bot_generated->
				message );

		sleep( BOT_GENERATED_SLEEP_SECONDS );
		exit( 1 );
	}

	if ( post_contact->message_not_sent_system_string )
	{
		if ( system( post_contact->message_not_sent_system_string ) ){}
	}

	if ( post_contact->mailx_system_string )
	{
		post_contact_mailx(
			post_contact->
				post_contact_input->
				message,
			post_contact->mailx_system_string );

		if ( system( post_contact->message_sent_system_string ) ){}
	}

	return 0;
}

