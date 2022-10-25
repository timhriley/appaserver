/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_timriley/post_email_timriley.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "timlib.h"
#include "dictionary.h"
#include "document.h"
#include "environ.h"
#include "post2dictionary.h"
#include "post_email.h"

#define APPAHOST_DOCUMENT_ROOT	"/var/www/html/appahost_com"

int main( void )
{
	DICTIONARY *dictionary = {0};
	char *remote_email_address;
	char *remote_ip_address;
	char *reason;
	char *message;
	POST_EMAIL *post_email;

	remote_ip_address =
		environ_get_environment(
			"REMOTE_ADDR" );

	dictionary =
		post2dictionary(
			stdin,
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session */ );

	remote_email_address = dictionary_get( dictionary, "email_address" );
	reason = dictionary_get( dictionary, "reason" );
	message = dictionary_get( dictionary, "message" );

	if ( !remote_ip_address
	||   !*remote_ip_address
	||   !remote_email_address
	||   !*remote_email_address
	||   !reason
	||   !*reason
	||   !message
	||   !*message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	document_output_content_type();

	add_utility_to_path();
	add_path( "/usr2/timriley/ufw" /* path_to_add */ );

	post_email =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_email_new(
			APPAHOST_DOCUMENT_ROOT,
			remote_email_address,
			remote_ip_address,
			reason,
			message );

	if (	post_email->address_invalid_boolean
	|| 	post_email->reason_invalid_boolean
	||	post_email->message_invalid_boolean
	||	post_email->bot_generated_boolean )
	{
		sleep( POST_EMAIL_INVALID_SLEEP );
		exit( 1 );
	}

	post_email_mailx(
		post_email->mailx_command_line,
		message );

	if ( system( post_email->message_sent_system_string ) ){}

	return 0;
}

