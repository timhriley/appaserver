/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/contra_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CONTRA_ACCOUNT_H
#define CONTRA_ACCOUNT_H

#include "boolean.h"
#include "list.h"

#define CONTRA_ACCOUNT_TABLE	"contra_account"

#define CONTRA_ACCOUNT_SELECT	"account,"		\
				"contra_to_account"

typedef struct
{
	char *account_name;
	char *contra_to_account;
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
		double account_balance,
		double contra_to_balance );

/* Returns heap memory or null */
/* --------------------------- */
char *contra_account_label(
		char *account_name,
		char *contra_to_name );

/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CONTRA_ACCOUNT *contra_account_new(
		char *account_name,
		char *contra_to_account );

/* Process */
/* ------- */
CONTRA_ACCOUNT *contra_account_calloc(
		void );


/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_seek(
		LIST *contra_account_list,
		char *account_name /* mutually exclusive */,
		char *contra_to_account /* mutually exclusive */ );

/* Usage */
/* ----- */
boolean contra_account_boolean(
		LIST *contra_account_list,
		char *account_account );

#endif
