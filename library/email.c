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
char *email_sendmail_command(	char *attachment_filename )
{
	char sendmail_command[ 2048 ];

	if ( attachment_filename
	&&   *attachment_filename
	&&   timlib_file_exists( attachment_filename ) )
	{
		sprintf( sendmail_command,
			 "/usr/sbin/sendmail -t -a %s",
			 attachment_filename );
	}
	else
	{
		strcpy( sendmail_command, "/usr/sbin/sendmail -t" );
	}

	return strdup( sendmail_command );

} /* email_sendmail_command() */

void email_sendmail(	char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *full_name,
			char *attachment_filename )
{
	FILE *output_pipe;
	char *sendmail_command;
	char *sendmail_string;

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_command = email_sendmail_command( attachment_filename );

	/* Returns heap memory. */
	/* -------------------- */
	sendmail_string =
		email_sendmail_string(
			from_address,
			to_address,
			subject,
			message,
			reply_to,
			full_name );

	output_pipe = popen( sendmail_command, "w" );
	fprintf( output_pipe, "%s\n.\n", sendmail_string );
	pclose( output_pipe );

} /* email_sendmail() */

/* Returns heap memory. */
/* -------------------- */
char *email_sendmail_string(
			char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *full_name )
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

	if ( full_name && *full_name )
	{
		ptr += sprintf( ptr, "From: %s\n", full_name );
	}

	if ( message && *message )
	{
		ptr += sprintf( ptr, "%s\n", message );
	}

	return strdup( sendmail_string );

} /* email_sendmail_string() */

