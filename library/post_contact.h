/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_contact.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_CONTACT_H
#define POST_CONTACT_H

#include "boolean.h"
#include "list.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "bot_generated.h"

#define POST_CONTACT_DESTINATION_EMAIL		"timriley@timriley.net"
#define POST_CONTACT_APPLICATION		"appahost"
#define POST_CONTACT_ENTITY			"Appahost"
#define POST_CONTACT_NOT_SENT_FILENAME		"message_not_sent.php"
#define POST_CONTACT_SENT_FILENAME		"message_sent.php"

typedef struct
{
	BOT_GENERATED *bot_generated;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *upload_filename_list;
	POST_DICTIONARY *post_dictionary;
	char *email_address;
	boolean email_invalid_boolean;
	char *reason;
	boolean reason_invalid_boolean;
	char *message;
	boolean message_empty_boolean;
	char *filespecification;
	boolean filespecification_boolean;
} POST_CONTACT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT_INPUT *post_contact_input_new(
		char *argv_0 );

/* Process */
/* ------- */
POST_CONTACT_INPUT *post_contact_input_calloc(
		void );

/* List of one */
/* ----------- */
LIST *post_contact_input_upload_filename_list(
		void );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_input_email_address(
		DICTIONARY *post_dictionary );

boolean post_contact_input_email_invalid_boolean(
		char *post_contact_input_email_address );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_input_reason(
		DICTIONARY *post_dictionary );

boolean post_contact_input_reason_invalid_boolean(
		char *post_contact_input_reason );

/* Returns component of dictionary, heap memory, or null */
/* ----------------------------------------------------- */
char *post_contact_input_message(
		DICTIONARY *post_dictionary );

boolean post_contact_input_message_empty_boolean(
		char *post_contact_input_message );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_input_filespecification(
		const char *post_dictionary_spool_prefix,
		DICTIONARY *post_dictionary );

boolean post_contact_input_filespecification_boolean(
		char *post_contact_input_filenspecification );

typedef struct
{
	POST_CONTACT_INPUT *post_contact_input;
	char *message_not_sent_system_string;
	char *mailx_system_string;
	char *message_sent_system_string;
} POST_CONTACT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT *post_contact_new(
		char *argv_0 );

/* Process */
/* ------- */
POST_CONTACT *post_contact_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_mailx_system_string(
		const char *post_contact_destination_email,
		const char *post_contact_entity,
		char *remote_ip_address,
		char *email_address,
		char *reason,
		char *filespecification,
		char *appaserver_mailname );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_contact_subject(
		const char *post_contact_entity,
		char *remote_ip_address,
		char *email_address,
		char *reason );

/* Usage */
/* ----- */
void post_contact_mailx(
		char *message,
		char *post_contact_mailx_system_string );

/* Process */
/* ------- */
void post_contact_write(
		char *message,
		FILE *appaserver_output_pipe );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_display_system_string(
		const char *message_html,
		char *document_root );

#endif

