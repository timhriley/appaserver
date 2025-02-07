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
	POST_SIGNUP *post_signup;
	SESSION *session;
	char *execute_system_string_create_application;
	char *post_confirmation_update_statement;
	/* -------------- */
	/* Set externally */
	/* -------------- */
	POST_LOGIN_DOCUMENT *post_login_document;
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

/* Returns static memory */
/* --------------------- */
char *post_signup_receive_success_parameter(
		char *password );

#endif
