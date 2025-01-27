/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVESTMENT_ACCOUNT_H
#define INVESTMENT_ACCOUNT_H

#include "boolean.h"
#include "list.h"

#define INVESTMENT_ACCOUNT_TABLE	"investment_account"

#define INVESTMENT_ACCOUNT_SELECT	"investment_classification,"	\
					"investment_purpose,"		\
					"certificate_maturity_months,"	\
					"certificate_maturity_date,"	\
					"interest_rate,"		\
					"balance_latest"

typedef struct
{
	char *full_name;
	char *street_address;
	char *account_number;
	char *primary_where;
	char *investment_classification;
	char *investment_purpose;
	int certificate_maturity_months;
	char *certificate_maturity_date;
	double interest_rate;
	double balance_latest;
	LIST *account_balance_list;
	double update_balance_latest;
} INVESTMENT_ACCOUNT;

/* Usage */
/* ----- */
INVESTMENT_ACCOUNT *investment_account_fetch(
		char *full_name,
		char *street_address,
		char *account_number,
		boolean fetch_account_balance_list );

/* Process */
/* ------- */
double investment_account_update_balance_latest(
		LIST *account_balance_update_set );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *investment_account_primary_where(
		char *full_name,
		char *street_address,
		char *account_number );

/* Usage */
/* ----- */
INVESTMENT_ACCOUNT *investment_account_parse(
		char *full_name,
		char *street_address,
		char *account_number,
		char *investment_account_primary_where,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVESTMENT_ACCOUNT *investment_account_new(
		char *full_name,
		char *street_address,
		char *account_number,
		char *investment_account_primary_where );

/* Process */
/* ------- */
INVESTMENT_ACCOUNT *investment_account_calloc(
		void );

/* Driver */
/* ------ */

/* Returns error_string or null */
/* ---------------------------- */
char *investment_account_update(
		const char *investment_account_table,
		char *investment_account_primary_where,
		double update_balance_latest );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *investment_account_update_statement(
		const char *investment_account_table,
		char *investment_account_primary_where,
		double update_balance_latest );

#endif
