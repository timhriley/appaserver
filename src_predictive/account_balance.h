/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_balance.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCOUNT_BALANCE_H
#define ACCOUNT_BALANCE_H

#include "list.h"
#include "boolean.h"
#include "spool.h"

#define ACCOUNT_BALANCE_TABLE		"account_balance"

#define ACCOUNT_BALANCE_SELECT		"date,"				\
					"balance,"			\
					"balance_change,"		\
					"balance_change_percent"

#define ACCOUNT_BALANCE_PRIMARY_KEY	"full_name,"			\
					"account_number,"		\
					"date"

#define ACCOUNT_BALANCE_MIN_FUNCTION	"min(date)"
#define ACCOUNT_BALANCE_MAX_FUNCTION	"max(date)"

typedef struct
{
	char *full_name;
	char *contact_key;
	char *account_number;
	char *date_string;
	double balance;
	double balance_change;
	int balance_change_percent;

	/* Set externally */
	/* -------------- */
	double update_balance_change;
	int update_balance_change_percent;
	char *update_change_string;
	char *update_percent_string;
} ACCOUNT_BALANCE;

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_latest(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number );

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_fetch(
		char *fund_name,
		char *date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number );

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_parse(
		char *fund_name,
		boolean entity_contact_key_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT_BALANCE *account_balance_new(
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string );

/* Process */
/* ------- */
ACCOUNT_BALANCE *account_balance_calloc(
		void );

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_prior(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number );

/* Usage */
/* ----- */
ACCOUNT_BALANCE *account_balance_next(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number );

/* Usage */
/* ----- */
void account_balance_delta_set(
		char *fund_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		ACCOUNT_BALANCE *account_balance_prior,
		ACCOUNT_BALANCE *account_balance_current /* in/out */ );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *account_balance_update_change_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string,
		double update_balance_change,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Returns heap memory */
/* ------------------- */
char *account_balance_update_percent_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string,
		int update_balance_change_percent,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

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

/* Returns heap memory */
/* ------------------- */
char *account_balance_where(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number,
		const char *relational_operator );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *account_balance_fetch_date(
		const char *account_balance_table,
		const *account_balance_min_function,
		const *account_balance_max_function,
		char *account_balance_where,
		boolean fetch_min_boolean );

/* Driver */
/* ------ */
void account_balance_update(
		char *fund_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		ACCOUNT_BALANCE *account_balance_fetch,
		ACCOUNT_BALANCE *account_balance_next );

/* Usage */
/* ----- */
void account_balance_update_spool(
		char *update_change_string,
		char *update_percent_string,
		SPOOL *spool );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_balance_update_system_string(
		const char *account_balance_table,
		const char *account_balance_primary_key,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_balance_key_string(
		const char *account_balance_primary_key,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

#endif
