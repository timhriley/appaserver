/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_balance.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCOUNT_BALANCE_H
#define ACCOUNT_BALANCE_H

#include "list.h"
#include "boolean.h"

#define ACCOUNT_BALANCE_TABLE		"account_balance"

#define ACCOUNT_BALANCE_SELECT		"date,"				\
					"balance,"			\
					"balance_change,"		\
					"balance_change_percent"

#define ACCOUNT_BALANCE_PRIMARY_KEY	"full_name,"			\
					"street_address,"		\
					"account_number,"		\
					"date"

typedef struct
{
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_string;
	double balance;
	double balance_change;
	int balance_change_percent;

	/* Set externally */
	/* -------------- */
	double update_balance_change;
	int update_balance_change_percent;
} ACCOUNT_BALANCE;

/* Usage */
/* ----- */
LIST *account_balance_list(
		char *full_name,
		char *street_address,
		char *account_number,
		char *investment_account_primary_where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_balance_system_string(
		const char *select,
		const char *account_balance_table,
		char *where );

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_parse(
		char *full_name,
		char *street_address,
		char *account_number,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT_BALANCE *account_balance_new(
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string );

/* Process */
/* ------- */
ACCOUNT_BALANCE *account_balance_calloc(
		void );

/* Usage */
/* ----- */
LIST *account_balance_update_set(
		LIST *account_balance_list /* in/out */ );

/* Usage */
/* ----- */
double account_balance_update_change(
		double prior_balance,
		double balance );

/* Usage */
/* ----- */
int account_balance_update_change_percent(
		double prior_balance,
		double balance_change );

/* Usage */
/* ----- */

/* Returns error_string */
/* -------------------- */
char *account_balance_list_update(
		LIST *account_balance_update_set );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *account_balance_update_change_string(
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string,
		double balance_change );

/* Returns static memory */
/* --------------------- */
char *account_balance_update_percent_string(
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string,
		int balance_change_percent );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_balance_update_system_string(
		const char *account_balance_table,
		const char *account_balance_primary_key );

#endif
