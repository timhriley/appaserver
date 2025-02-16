/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_CONTACT_H
#define POST_CONTACT_H

#include "boolean.h"
#include "list.h"

#define POST_CONTACT_TABLE		"post_contact"

#define POST_CONTACT_SELECT		"reason,"			\
					"message,"			\
					"upload_file"

#define POST_CONTACT_INSERT		"email_address,"		\
					"timestamp,"			\
					"reason,"			\
					"message,"			\
					"upload_file"

typedef struct
{
	char *email_address;
	char *timestamp;
	char *reason;
	char *message;
	char *upload_file;
	char *insert_statement;
} POST_CONTACT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT *post_contact_new(
		char *email_address,
		char *timestamp,
		char *reason,
		char *message,
		char *upload_file );

/* Process */
/* ------- */
POST_CONTACT *post_contact_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *post_contact_insert_statement(
		const char *post_contact_table,
		const char *post_contact_insert,
		char *email_address,
		char *timestamp,
		char *reason,
		char *message,
		char *upload_file );

/* Usage */
/* ----- */
POST_CONTACT *post_contact_fetch(
		char *email_address,
		char *timestamp );

/* Process */
/* ------- */
POST_CONTACT *post_contact_parse(
		char *email_address,
		char *timestamp,
		char *string_fetch );

#endif
