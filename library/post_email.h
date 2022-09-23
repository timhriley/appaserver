/* $APPASERVER_HOME/library/post_email.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef POST_EMAIL_H
#define POST_EMAIL_H

#include "boolean.h"
#include "list.h"

#define POST_EMAIL_DESTINATION_EMAIL_ADDRESS \
					"timriley@appahost.com"

#define POST_EMAIL_BOT_SCRIPT_DENY	"email_bot_script_deny.sh"
#define POST_EMAIL_ENTITY		"Appahost"
#define POST_EMAIL_MESSAGE_SENT_HTML	"message_sent.html"
#define POST_EMAIL_INVALID_SLEEP	60

typedef struct
{
	char *message_sent_system_string;
	boolean address_invalid_boolean;
	boolean reason_invalid_boolean;
	boolean message_invalid_boolean;
	boolean bot_generated_boolean;
	char *mailx_command_line;
} POST_EMAIL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
POST_EMAIL *post_email_new(
			char *document_root,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason,
			char *message );

/* Process */
/* ------- */
POST_EMAIL *post_email_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *post_email_message_sent_system_string(
			char *document_root,
			char *post_email_message_sent_html );

boolean post_email_address_invalid_boolean(
			char *remote_email_address /* in/out */ );

boolean post_email_reason_invalid_boolean(
			char *reason /* in/out */ );

boolean post_email_message_invalid_boolean(
			char *message );

/* Need to have www-data group for:	*/
/* /var/log/apache2/ and		*/
/* /var/log/apache2/access.log		*/
/* ------------------------------------ */
boolean post_email_bot_generated_boolean(
			char *post_email_bot_script_deny,
			char *remote_ip_address );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_email_mailx_command_line(
			char *post_email_entity,
			char *post_email_destination_email_address,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_email_subject(
			char *post_email_entity,
			char *remote_email_address,
			char *remote_ip_address,
			char *reason );

/* Usage */
/* ----- */
void post_email_mailx(	char *post_email_mailx_command_line,
			char *message );

/* Process */
/* ------- */
FILE *post_email_output_pipe_open(
			char *post_email_mailx_command_line );

void post_email_output_pipe_write(
			FILE *post_email_output_pipe_open,
			char *message );

#endif

