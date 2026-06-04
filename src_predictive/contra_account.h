/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/contra_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CONTRA_ACCOUNT_H
#define CONTRA_ACCOUNT_H

#include "boolean.h"
#include "list.h"
#include "account.h"

#define CONTRA_ACCOUNT_TABLE	"contra_account"

#define CONTRA_ACCOUNT_SELECT	"contra_to_account,"	\
				"account"

typedef struct
{
	char *contra_to_account;
	char *account_name;
	ACCOUNT_JOURNAL *contra_to_account_journal;
	ACCOUNT_JOURNAL *account_journal;
	double net_amount;
	char *label;
} CONTRA_ACCOUNT;

/* Usage */
/* ----- */
LIST *contra_account_list(
		const char *contra_account_select,
		const char *contra_account_table,
		char *fund_name,
		char *end_date_time_string );

/* Process */
/* ------- */
double contra_account_net_amount(
		double contra_to_balance,
		double account_balance );

/* Returns heap memory or null */
/* --------------------------- */
char *contra_account_label(
		char *contra_to_name,
		char *account_name );

/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CONTRA_ACCOUNT *contra_account_new(
		char *contra_to_account,
		char *account_name );

/* Process */
/* ------- */
CONTRA_ACCOUNT *contra_account_calloc(
		void );


/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_seek(
		LIST *contra_account_list,
		char *contra_to_account /* mutually exclusing */,
		char *account_name /* mutually exclusive */ );

#endif
