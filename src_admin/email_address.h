/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/email_address.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EMAIL_ADDRESS_H
#define EMAIL_ADDRESS_H

#include "boolean.h"
#include "list.h"

#define EMAIL_ADDRESS_TABLE		"email_address"
#define EMAIL_ADDRESS_SELECT		"blocked_yn"
#define EMAIL_ADDRESS_INSERT		"email_address"
#define EMAIL_ADDRESS_BLOCKED_COLUMN	"blocked_yn"

typedef struct
{
	char *email_address;
	char *insert_statement;
	boolean blocked_boolean;
} EMAIL_ADDRESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EMAIL_ADDRESS *email_address_new(
		char *email_address );

/* Process */
/* ------- */
EMAIL_ADDRESS *email_address_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *email_address_insert_statement(
		const char *email_address_table,
		const char *email_address_insert,
		char *email_address );

/* Usage */
/* ----- */
EMAIL_ADDRESS *email_address_fetch(
		char *email_address );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *email_address_primary_where(
		char *email_address );

EMAIL_ADDRESS *email_address_parse(
		char *email_address,
		char *string_fetch );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *email_address_blocked_update_statement(
		const char *email_address_table,
		const char *email_address_blocked_column,
		char *email_address );

#endif
