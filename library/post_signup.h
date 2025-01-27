/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_signup.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_SIGNUP_H
#define POST_SIGNUP_H

#include "boolean.h"
#include "list.h"
#include "post_login.h"

typedef struct
{
	BOT_GENERATED *bot_generated;
	DICTIONARY *post_login_input_dictionary;
	char *post_login_input_application_name;
	boolean post_login_input_missing_application_boolean;
	boolean post_login_input_invalid_application_boolean;
	boolean post_login_input_application_exists_boolean;
	char *post_login_input_login_name;
	boolean post_login_input_missing_login_name_boolean;
	boolean post_login_input_invalid_login_name_boolean;
	boolean post_login_input_email_address_boolean;
	char *application_title;
	boolean missing_title_boolean;
} POST_SIGNUP_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP_INPUT *post_signup_input_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_SIGNUP_INPUT *post_signup_input_calloc(
		void );

/* Usage */
/* ----- */

/* ----------------------------------------------------- */
/* Returns component of dictionary, heap memory, or null */
/* ----------------------------------------------------- */
char *post_signup_input_application_title(
		DICTIONARY *post_login_input_dictionary );

boolean post_signup_input_missing_title_boolean(
		char *post_signup_input_application_title );

typedef struct
{
	POST_SIGNUP_INPUT *post_signup_input;
	char *reject_index_html_message;
	char *session_key;
	char *execute_system_string_create_application;
	char *success_index_html_message;
	POST_LOGIN_DOCUMENT *post_login_document;
} POST_SIGNUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP *post_signup_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_SIGNUP *post_signup_calloc(
		void );

/* Returns program memory or null */
/* ------------------------------ */
char *post_signup_reject_index_html_message(
		boolean post_login_input_missing_application_boolean,
		boolean post_login_input_invalid_application_boolean,
		boolean post_login_input_missing_login_name_boolean,
		boolean post_login_input_invalid_login_name_boolean,
		boolean post_login_input_application_exists_boolean,
		boolean post_signup_input_missing_title_boolean );

/* Usage */
/* ----- */

/* Returns spool_fetch */
/* ------------------- */
char *post_signup_password(
		char *spool_fetch );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_signup_success_index_html_message(
		boolean post_login_email_address_boolean,
		char *post_signup_password );

#endif
