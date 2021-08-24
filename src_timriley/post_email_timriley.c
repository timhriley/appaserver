/* ---------------------------------------------------	*/
/* src_timriley/post_email_timriley.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "dictionary.h"
#include "document.h"
#include "environ.h"
#include "post2dictionary.h"

#define TIMRILEY_EMAIL_ADDRESS	"timriley@appahost.com"
#define APPAHOST_DOCUMENT_ROOT	"/var/www/html/appahost_com"

int main( void )
{
	DICTIONARY *dictionary = {0};
	char *reason;
	char *email_address;
	char *message;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *document_root;
	char output_process[ 128 ];
	char *remote_IP_address;
	char *https_on;

	remote_IP_address = environ_get_environment( "REMOTE_ADDR" );
	https_on = environ_get_environment( "HTTPS" );

	dictionary = post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	reason = dictionary_get( "reason", dictionary );
	email_address = dictionary_get( "email_address", dictionary );
	message = dictionary_get( "message", dictionary );

	if ( timlib_strcmp( https_on, "on" ) == 0
	&&   reason && *reason
	&&   email_address && *email_address
	&&   message && *message )
	{
		sprintf( sys_string,
		 	"mailx -s \"Appahost [%s] from %s/%s\" %s",
		 	reason,
		 	email_address,
			remote_IP_address,
		 	TIMRILEY_EMAIL_ADDRESS );

		output_pipe = popen( sys_string, "w" );
		fprintf( output_pipe, "%s\n", message );

		environ_display( output_pipe );
		pclose( output_pipe );
	}

	document_output_content_type();

	document_root = APPAHOST_DOCUMENT_ROOT;

	sprintf( output_process, "cat %s/message_sent.html", document_root );
	if ( system( output_process ) ) {};

	exit( 0 );

} /* main() */

