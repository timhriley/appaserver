/* $APPASERVER_HOME/library/email.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include "email.h"
#include "timlib.h"

EMAIL *email_new( void )
{
	EMAIL *h;

	if ( ! ( h = (EMAIL *)calloc( 1, sizeof( EMAIL ) ) ) )
	{
		fprintf( stderr, 
			 "%s/%s()/%d: Cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return h;
}

/* Returns heap memory. */
/* -------------------- */
char *email_sendmail_command(	char *to_address,
				char *subject,
				char *attachment_filename )
{
	char sendmail_command[ 2048 ];

	if ( attachment_filename
	&&   *attachment_filename
	&&   timlib_file_exists( attachment_filename ) )
	{
		sprintf( sendmail_command,
			 "mutt -s \"%s\" -a %s -- %s",
			 subject,
			 attachment_filename,
			 to_address );
	}
	else
	{
		sprintf( sendmail_command,
			 "mutt -s \"%s\" %s",
			 subject,
			 to_address );
	}

	return strdup( sendmail_command );

}

void email_sendmail(	char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *self_full_name,
			char *attachment_filename )
{
	char sys_string[ 1024 ];
	char *sendmail_command;
	char *sendmail_string;

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_command =
		email_sendmail_command(
			to_address,
			subject,
			attachment_filename );

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_string =
		email_sendmail_string(
			from_address,
			to_address,
			subject,
			message,
			reply_to,
			self_full_name );

	sprintf(	sys_string,
			"echo \"%s\" | %s",
			sendmail_string,
			sendmail_command );

	if ( system( sys_string ) ){};
}

/* Returns heap memory. */
/* -------------------- */
char *email_sendmail_string(
			char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *self_full_name )
{
	char sendmail_string[ 65536 ];
	char *ptr = sendmail_string;

	*ptr = '\0';

	if ( to_address && *to_address )
	{
		ptr += sprintf( ptr, "To: %s\n", to_address );
	}

	if ( from_address && *from_address )
	{
		ptr += sprintf( ptr, "Email: %s\n", from_address );
	}

	if ( reply_to && *reply_to )
	{
		ptr += sprintf( ptr, "Reply-To: %s\n", reply_to );
	}

	if ( subject && *subject )
	{
		ptr += sprintf( ptr, "Subject: %s\n", subject );
	}

	if ( self_full_name && *self_full_name )
	{
		ptr += sprintf( ptr, "From: %s\n", self_full_name );
	}

	if ( message && *message )
	{
		ptr += sprintf( ptr, "%s\n", message );
	}

	return strdup( sendmail_string );
}

