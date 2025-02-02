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
#include "session.h"
#include "post.h"

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
	char *filespecification;
	boolean filespecification_boolean;
	char *environment_remote_ip_address;
	char *environment_http_user_agent;
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
POST_CONTACT_SUBMIT_INPUT *post_contact_submit_calloc(
		void );

/* List of one */
/* ----------- */
LIST *post_contact_submit_input_upload_filename_list(
		void );

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
char *post_contact_submit_input_filespecification(
		const char *post_dictionary_spool_prefix,
		DICTIONARY *post_dictionary );

boolean post_contact_submit_input_filespecification_boolean(
		char *post_contact_submit_input_filenspecification );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_submit_receive_url(
		const char *post_contact_receive_executable,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp,
		char *session_key );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_submit_display_system_string(
		const char *message_filename,
		char *document_root );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_contact_submit_message(
		const char *post_contact_submit_message_prompt,
		char *post_contact_submit_receive_url );

typedef struct
{
	POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input;
	char *display_system_string;
	POST *post;
	LIST *form_field_datum_list;
	char *form_field_datum_insert_statement;
	SESSION *session;
	char *post_return_email;
	char *post_mailx_system_string;
	char *receive_url;
	char *message;
	char *display_system_string;
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

#endif
