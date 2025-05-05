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

#define FEEDER_ACCOUNT_SELECT		"full_name,"		\
					"street_address"

typedef struct
{
	char *feeder_account_name;
	char *financial_institution_full_name;
	char *financial_institution_street_address;
} FEEDER_ACCOUNT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ACCOUNT *feeder_account_fetch(
		const char *feeder_account_table,
		char *feeder_account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_account_primary_where(
		const char *feeder_account_primary_key,
		char *feeder_account_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ACCOUNT *feeder_account_parse(
		char *feeder_account_name,
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

#endif
