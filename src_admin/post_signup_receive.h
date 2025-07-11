/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_receive.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_SIGNUP_RECEIVE_H
#define POST_SIGNUP_RECEIVE_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "post.h"
#include "post_signup.h"
#include "post_login.h"

#define POST_SIGNUP_RECEIVE_EXECUTABLE		"post_signup_receive_execute"

typedef struct
{
	POST_RECEIVE *post_receive;
	POST *post;
	boolean post_bot_boolean;
	int sleep_seconds;
	POST_SIGNUP *post_signup;
	char *success_parameter;
	char *in_process_parameter;
	POST_LOGIN_DOCUMENT *post_login_document;
	SESSION *session;
	char *execute_system_string_create_application;
	char *post_confirmation_update_sql;
	/* -------------- */
	/* Set externally */
	/* -------------- */
	char *password;
	char *password_update_sql;
} POST_SIGNUP_RECEIVE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP_RECEIVE *post_signup_receive_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_SIGNUP_RECEIVE *post_signup_receive_calloc(
		void );

/* Usage */
/* ----- */

/* Returns parameter */
/* ----------------- */
int post_signup_receive_sleep_seconds(
		const int post_sleep_seconds );

/* Usage */
/* ----- */

/* Returns parameter */
/* ----------------- */
char *post_signup_receive_password(
		char *spool_fetch );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_signup_receive_success_parameter(
		char *password );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *post_signup_receive_in_process_parameter(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_signup_receive_password_update_sql(
		const char *post_signup_table,
		const char *post_signup_password_column,
		char *email_address,
		char *timestamp_space,
		char *post_signup_receive_password );

#endif
