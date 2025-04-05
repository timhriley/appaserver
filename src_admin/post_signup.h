/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_SIGNUP_H
#define POST_SIGNUP_H

#include "boolean.h"
#include "list.h"

#define POST_SIGNUP_TABLE		"post_signup"

#define POST_SIGNUP_SELECT		"application_key,"	\
					"application_title,"	\
					"signup_password"

#define POST_SIGNUP_INSERT		"email_address,"	\
					"timestamp,"		\
					"application_key,"	\
					"application_title"

#define POST_SIGNUP_PASSWORD_COLUMN	"signup_password"

typedef struct
{
	char *email_address;
	char *timestamp;
	char *application_key;
	char *application_title;
	char *signup_password;
	char *insert_statement;
} POST_SIGNUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP *post_signup_new(
		char *email_address,
		char *timestamp,
		char *application_key,
		char *application_title );

/* Process */
/* ------- */
POST_SIGNUP *post_signup_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *post_signup_insert_statement(
		const char *post_signup_table,
		const char *post_signup_insert,
		char *email_address,
		char *timestamp,
		char *application_key,
		char *application_title );

/* Usage */
/* ----- */
POST_SIGNUP *post_signup_fetch(
		char *email_address,
		char *timestamp );

/* Process */
/* ------- */
POST_SIGNUP *post_signup_parse(
		char *email_address,
		char *timestamp,
		char *string_fetch );

#endif
