/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_ACCOUNT_H
#define FEEDER_ACCOUNT_H

#include "list.h"
#include "boolean.h"

#define FEEDER_ACCOUNT_TABLE		"feeder_account"
#define FEEDER_ACCOUNT_PRIMARY_KEY	"feeder_account"

#define FEEDER_ACCOUNT_SELECT		"full_name"

typedef struct
{
	char *feeder_account_name;
	char *financial_institution_full_name;
	char *financial_institution_contact_key;
} FEEDER_ACCOUNT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ACCOUNT *feeder_account_fetch(
		const char *feeder_account_select,
		const char *feeder_account_table,
		char *feeder_account_name,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ACCOUNT *feeder_account_parse(
		char *feeder_account_name,
		boolean entity_contact_key_boolean,
		char *string_pipe_fetch );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ACCOUNT *feeder_account_new(
		char *feeder_account_name );

/* Process */
/* ------- */
FEEDER_ACCOUNT *feeder_account_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *feeder_account_primary_where(
		const char *feeder_account_primary_key,
		char *feeder_account_name );

#endif
