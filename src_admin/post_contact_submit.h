/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact_submit.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_CONTACT_SUBMIT_H
#define POST_CONTACT_SUBMIT_H

#include "boolean.h"
#include "list.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "post.h"
#include "post_contact.h"

#define POST_CONTACT_SUBMIT_SUBJECT		"Contact confirmation"
#define POST_CONTACT_SUBMIT_MESSAGE_PROMPT	"Confirm contact"
#define POST_CONTACT_SUBMIT_NOT_SENT_FILENAME	"message_not_sent.php"
#define POST_CONTACT_SUBMIT_SENT_FILENAME	"message_confirmation_sent.php"

typedef struct
{
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *upload_filename_list;
	POST_DICTIONARY *post_dictionary;
	char *email_address;
	boolean email_invalid_boolean;
	char *reason;
	boolean reason_invalid_boolean;
	char *message;
	boolean message_empty_boolean;
	char *file;
	boolean file_boolean;
	char *appaserver_mailname;
	char *appaserver_error_filename;
} POST_CONTACT_SUBMIT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input_new(
		void );

/* Process */
/* ------- */
POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input_calloc(
		void );

/* List of one */
/* ----------- */
LIST *post_contact_submit_input_upload_filename_list(
		const char *attribute_name );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_submit_input_email_address(
		DICTIONARY *post_dictionary );

boolean post_contact_submit_input_email_invalid_boolean(
		char *post_contact_submit_input_email_address );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_submit_input_reason(
		DICTIONARY *post_dictionary );

boolean post_contact_submit_input_reason_invalid_boolean(
		char *post_contact_submit_input_reason );

/* Returns component of dictionary, heap memory, or null */
/* ----------------------------------------------------- */
char *post_contact_submit_input_message(
		DICTIONARY *post_dictionary );

boolean post_contact_submit_input_message_empty_boolean(
		char *post_contact_submit_input_message );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_contact_submit_input_file(
		DICTIONARY *post_dictionary );

boolean post_contact_submit_input_file_boolean(
		char *post_contact_submit_input_file );

typedef struct
{
	POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input;
	char *display_system_string;
	POST *post;
	POST_CONTACT *post_contact;
	char *post_return_email;
	char *post_mailx_system_string;
	char *post_receive_url;
	char *message;
} POST_CONTACT_SUBMIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT_SUBMIT *post_contact_submit_new(
		void );

/* Process */
/* ------- */
POST_CONTACT_SUBMIT *post_contact_submit_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_contact_submit_display_system_string(
		const char *message_filename,
		char *document_root );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_contact_submit_message(
		const char *post_contact_submit_message_prompt,
		char *post_receive_url );

#endif
