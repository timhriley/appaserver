/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_H
#define POST_H

#include "boolean.h"
#include "list.h"
#include "insert.h"
#include "session.h"
#include "appaserver_parameter.h"

#define POST_TABLE		"post"

#define POST_SELECT		"IP_address,"		\
				"form_name"

#define POST_INSERT		"timestamp,"		\
				"email_address,"	\
				"IP_address,"		\
				"form_name"

#define POST_RETURN_USERNAME	"DoNotReply"

typedef struct
{
	char *form_name;
	char *ip_address;
	char *email_address;
	char *timestamp;
	char *insert_statement;
	LIST *form_field_datum_list;
	char *post_primary_where;
} POST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST *post_new(
		const char *form_name,
		char *email_address,
		char *ip_address );

/* Process */
/* ------- */
POST *post_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *post_insert_statement(
		const char *post_table,
		const char *post_insert,
		const char *form_name,
		char *email_address,
		char *ip_address,
		char *post_timestamp );

/* Returns heap memory */
/* ------------------- */
char *post_timestamp(
		void );

/* Usage */
/* ----- */
void post_mailx(
		char *message,
		char *mailx_system_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_return_email(
		const char *post_return_username,
		char *appaserver_mailname );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_mailx_system_string(
		const char *subject,
		char *post_return_email );

/* Usage */
/* ----- */
POST *post_fetch(
		char *email_address,
		char *timestamp );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_primary_where(
		char *email_address,
		char *timestamp );

/* Usage */
/* ----- */
POST *post_parse(
		char *email_address,
		char *timestamp,
		char *post_primary_where,
		char *string_fetch );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_receive_url(
		const char *receive_executable,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp,
		char *session_key );

typedef struct
{
	char *email_address;
	char *timestamp;
	char *session_key;
	SESSION *session;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST *post;
	char *appaserver_mailname;
} POST_RECEIVE_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_RECEIVE_INPUT *post_receive_input_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_RECEIVE_INPUT *post_receive_input_calloc(
		void );

#endif
