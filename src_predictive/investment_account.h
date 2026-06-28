/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVESTMENT_ACCOUNT_H
#define INVESTMENT_ACCOUNT_H

#include "boolean.h"
#include "list.h"
#include "account_balance.h"

#define INVESTMENT_ACCOUNT_TABLE	"investment_account"

#define INVESTMENT_ACCOUNT_SELECT	"full_name,"			\
					"account_number,"		\
					"investment_classification,"	\
					"investment_purpose,"		\
					"certificate_maturity_months,"	\
					"certificate_maturity_date,"	\
					"interest_rate,"		\
					"balance_latest"


#define INVESTMENT_ACCOUNT_HEADING					\
"institution_full_name,account_number,As Of Date,Book Value,Certificate Maturity,Investment Purpose"

#define INVESTMENT_ACCOUNT_JUSTIFICATION				\
					"Left,left,left,right,right,left"
typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *account_number;
	char *investment_classification;
	char *investment_purpose;
	int certificate_maturity_months;
	char *certificate_maturity_date;
	double interest_rate;
	double balance_latest;
	ACCOUNT_BALANCE *account_balance_latest;
	char *primary_where;
} INVESTMENT_ACCOUNT;

/* Usage */
/* ----- */
LIST *investment_account_list(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *as_of_date,
		char *investment_purpose,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
INVESTMENT_ACCOUNT *investment_account_parse(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVESTMENT_ACCOUNT *investment_account_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number );

/* Process */
/* ------- */
INVESTMENT_ACCOUNT *investment_account_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *investment_account_primary_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *investment_account_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *investment_purpose,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *investment_account_purpose_where(
		char *investment_purpose );

/* Process */
/* ------- */
boolean investment_account_purpose_populated_boolean(
		char *investment_purpose );

/* Usage */
/* ----- */
double investment_account_asset_sum(
		LIST *investment_account_list );

/* Usage */
/* ----- */
double investment_account_liability_sum(
		const char *investment_purpose_taxable_liability,
		LIST *investment_account_list );

/* Driver */
/* ------ */

/* Returns error_string or null */
/* ---------------------------- */
char *investment_account_update(
		const char *investment_account_table,
		char *investment_account_primary_where,
		double balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *investment_account_update_statement(
		const char *investment_account_table,
		char *investment_account_primary_where,
		double balance );

/* Driver */
/* ------ */
void investment_account_list_html_output(
		const char sql_delimiter,
		const char *investment_account_heading,
		const char *investment_account_justification,
		LIST *investment_account_list,
		double investment_account_asset_sum );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *investment_account_system_string(
		const char sql_delimiter,
		const char *investment_account_heading,
		const char *investment_account_justification );

#endif
