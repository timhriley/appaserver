/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_submit.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_SIGNUP_SUBMIT_H
#define POST_SIGNUP_SUBMIT_H

#include "boolean.h"
#include "list.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "session.h"
#include "post.h"
#include "form_field.h"
#include "post_login.h"

#define POST_SIGNUP_SUBMIT_CONFIRM_PARAMETER	"signup_confirm_yn"
#define POST_SIGNUP_SUBMIT_SUBJECT		"Cloudacus signup confirmation"
#define POST_SIGNUP_SUBMIT_MESSAGE_PROMPT	"Confirm Cloudacus signup"

typedef struct
{
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_DICTIONARY *post_dictionary;
	char *post_contact_submit_input_email_address;
	boolean post_contact_submit_input_email_invalid_boolean;
	char *application_key;
	boolean application_key_invalid_boolean;
	boolean application_exists_boolean;
	char *application_title;
	boolean application_title_empty_boolean;
	char *environment_remote_ip_address;
	char *environment_http_user_agent;
	char *appaserver_mailname;
	char *appaserver_error_filename;
} POST_SIGNUP_SUBMIT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input_new(
		void );

/* Process */
/* ------- */
POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input_calloc(
		void );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_signup_submit_input_application_key(
		DICTIONARY *post_dictionary );

boolean post_signup_submit_input_application_key_invalid_boolean(
		char *post_signup_submit_input_application_key );

boolean post_signup_submit_input_application_exists_boolean(
		char *log_directory,
		char *post_signup_submit_input_application_key );

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_signup_submit_input_application_title(
		DICTIONARY *post_dictionary );

boolean post_signup_submit_input_application_title_empty_boolean(
		char *post_signup_submit_input_application_title );

typedef struct
{
	POST_SIGNUP_SUBMIT_INPUT *post_signup_submit_input;
	char *index_html_parameter;
	POST_LOGIN_DOCUMENT *post_login_document;
	char *display_system_string;
	POST *post;
	FORM_FIELD_INSERT_LIST *form_field_insert_list;
	LIST *form_field_datum_insert_statement_list;
	SESSION *session;
	char *post_return_email;
	char *post_mailx_system_string;
	char *post_receive_url;
	char *message;
} POST_SIGNUP_SUBMIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP_SUBMIT *post_signup_submit_new(
		void );

/* Process */
/* ------- */
POST_SIGNUP_SUBMIT *post_signup_submit_calloc(
		void );

/* Returns program memory or null */
/* ------------------------------ */
char *post_signup_submit_reject_parameter(
		boolean post_contact_submit_input_email_invalid_boolean,
		boolean application_key_invalid_boolean,
		boolean application_exists_boolean,
		boolean application_title_empty_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_signup_submit_receive_url(
		const char *post_signup_receive_executable,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp,
		char *session_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_signup_submit_message(
		const char *post_signup_submit_message_prompt,
		char *post_signup_submit_receive_url );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_FIELD_INSERT_LIST *post_signup_submit_form_field_insert_list(
		char *email_address,
		char *application_key,
		char *application_title,
		char *form_name,
		char *timestamp );

#endif
