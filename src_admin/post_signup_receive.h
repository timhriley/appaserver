/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_receive.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_SIGNUP_RECEIVE_H
#define POST_SIGNUP_RECEIVE_H

#include "boolean.h"
#include "list.h"

#define POST_SIGNUP_RECEIVE_EXECUTABLE		"post_signup_receive_execute"

typedef struct
{
	char *application_key;
	char *application_title;
} POST_SIGNUP_RECEIVE_RECORD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_new(
		LIST *form_field_datum_list );

/* Process */
/* ------- */
POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_signup_receive_success_parameter(
		char *password );

#endif
