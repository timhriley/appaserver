/* $APPASERVER_HOME/library/post_email.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "post_email.h"

POST_EMAIL *post_email_new(
			char *document_root,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason,
			char *message )
{
	POST_EMAIL *post_email;

	if ( !document_root
	||   !remote_email_address
	||   !remote_ip_address
	||   !reason
	||   !message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	post_email = post_email_calloc();

	post_email->address_invalid_boolean =
		post_email_address_invalid_boolean(
			remote_email_address );

	if ( post_email->address_invalid_boolean )
	{
		return post_email;
	}

	post_email->reason_invalid_boolean =
		post_email_reason_invalid_boolean(
			reason /* in/out */ );

	if ( post_email->reason_invalid_boolean )
	{
		return post_email;
	}

	post_email->message_invalid_boolean =
		post_email_message_invalid_boolean(
			message );

	if ( post_email->message_invalid_boolean )
	{
		return post_email;
	}

	post_email->bot_generated_boolean =
		post_email_bot_generated_boolean(
			POST_EMAIL_BOT_SCRIPT_DENY,
			remote_ip_address );

	if ( post_email->bot_generated_boolean )
	{
		return post_email;
	}

	post_email->message_sent_system_string =
		post_email_message_sent_system_string(
			document_root,
			POST_EMAIL_MESSAGE_SENT_HTML );

	post_email->mailx_command_line =
		post_email_mailx_command_line(
			POST_EMAIL_ENTITY,
			POST_EMAIL_DESTINATION_EMAIL_ADDRESS,		
			remote_email_address,
			remote_ip_address,
			reason );

	return post_email;
}

POST_EMAIL *post_email_calloc( void )
{
	POST_EMAIL *post_email;

	if ( ! ( post_email = calloc( 1, sizeof( POST_EMAIL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_email;
}

char *post_email_message_sent_system_string(
			char *document_root,
			char *message_sent_html )
{
	char system_string[ 1024 ];

	if ( !document_root
	||   !message_sent_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"cat %s/%s",
		document_root,
		message_sent_html );

	return strdup( system_string );
}

boolean post_email_address_invalid_boolean(
			char *remote_email_address )
{
	char destination[ 128 ];

	if ( !remote_email_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: remote_email_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	search_replace_string( remote_email_address, "\\", "/" );
	search_replace_string( remote_email_address, "\"", "'" );

	string_escape_character_array(
		destination,
		remote_email_address,
		"`" );

	if ( strcmp( remote_email_address, destination ) != 0 )
		return 1;
	else
		return 0;
}

boolean post_email_reason_invalid_boolean(
			char *reason )
{
	char destination[ 128 ];

	if ( !reason )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: reason is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	search_replace_string( reason, "\\", "/" );
	search_replace_string( reason, "\"", "'" );

	string_escape_character_array(
		destination,
		reason,
		"`" );

	if ( strcmp( reason, destination ) != 0 )
		return 1;
	else
		return 0;
}

boolean post_email_message_invalid_boolean( char *message )
{
	char system_string[ 1024 ];
	int word_count;

	if ( !message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: message is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"echo \"%s\" | wc -w",
		message );

	word_count =
		string_atoi(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				system_string ) );

	if ( word_count < 5 ) return 1;

	return 0;
}

boolean post_email_bot_generated_boolean(
			char *post_email_bot_script_deny,
			char *remote_ip_address )
{
	char system_string[ 1024 ];
	int return_value;

	if ( !post_email_bot_script_deny
	||   !remote_ip_address
	||   !*remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s",
		post_email_bot_script_deny,
		remote_ip_address );

	return_value = system( system_string );

	if ( return_value )
		return 1;
	else
		return 0;
}

char *post_email_mailx_command_line(
			char *post_email_entity,
			char *post_email_destination_email_address,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason )
{
	char command_line[ 1024 ];

	if ( !post_email_entity
	||   !post_email_destination_email_address
	||   !remote_email_address
	||   !remote_ip_address
	||   !reason )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(command_line,
		"mailx -s \"%s\" %s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_email_subject(
			post_email_entity,
			remote_email_address,
			remote_ip_address,
			reason ),
		 post_email_destination_email_address );

	return strdup( command_line );
}

char *post_email_subject(
			char *post_email_entity,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason )
{
	static char subject[ 512 ];

	if ( !post_email_entity
	||   !remote_email_address
	||   !remote_ip_address
	||   !reason )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(subject,
		"%s [%s] from %s %s",
		post_email_entity,
		reason,
		remote_email_address,
		remote_ip_address );

	return subject;
}

void post_email_mailx(	char *command_line,
			char *message )
{
	FILE *pipe_open;

	if ( !command_line
	||   !message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe_open =
		post_email_output_pipe_open(
			command_line );

	post_email_output_pipe_write(
		pipe_open,
		message );

	pclose( pipe_open );
}

FILE *post_email_output_pipe_open( char *command_line )
{
	return popen( command_line, "w" );
}

void post_email_output_pipe_write(
			FILE *pipe_open,
			char *message )
{
	if ( !pipe_open
	||   !message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( pipe_open ) pclose( pipe_open );
		exit( 1 );
	}

	fprintf( pipe_open, "%s\n", message );
	environ_display( pipe_open );
}

