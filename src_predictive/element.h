/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/element.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "list.h"
#include "boolean.h"

#define ELEMENT_TABLE			"element"

#define ELEMENT_SELECT			"element,accumulate_debit_yn"

#define ELEMENT_ASSET			"asset"
#define ELEMENT_LIABILITY		"liability"
#define ELEMENT_EQUITY			"equity"
#define ELEMENT_REVENUE			"revenue"
#define ELEMENT_EXPENSE			"expense"
#define ELEMENT_EXPENDITURE		"expenditure"
#define ELEMENT_GAIN			"gain"
#define ELEMENT_LOSS			"loss"

typedef struct
{
	char *element_name;
	boolean accumulate_debit;
	LIST *subclassification_statement_list;
	LIST *account_statement_list;
	double sum;
	int percent_of_asset;
	int percent_of_income;
	int delta_prior_percent;
} ELEMENT;

/* Usage */
/* ----- */
LIST *element_statement_list(
		LIST *element_name_list,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Usage */
/* ----- */
ELEMENT *element_fetch(	char *element_name );

/* Usage */
/* ----- */
LIST *element_list(
		void );

/* Usage */
/* ----- */
LIST *element_system_list(
		char *element_system_string );

/* Process */
/* ------- */
FILE *element_pipe(
		char *element_system_string );

/* Usage */
/* ----- */
ELEMENT *element_parse(
		char *input );

/* Usage */
/* ----- */
ELEMENT *element_new(
		char *element_name );

/* Process */
/* ------- */
ELEMENT *element_calloc(
		void );

/* Usage */
/* ----- */
ELEMENT *element_statement_fetch(
		char *element_name,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *element_primary_where(
			char *element_name );

/* Returns heap memory */
/* ------------------- */
char *element_system_string(
		char *element_select,
		char *element_table,
		char *where );

/* Usage */
/* ----- */
ELEMENT *element_statement_parse(
		char *input,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Usage */
/* ----- */

/* Sets element->sum */
/* ----------------- */
void element_list_sum_set(
		LIST *element_statement_list );

/* Usage */
/* ----- */
double element_sum(
		ELEMENT *element );

/* Usage */
/* ----- */
void element_list_account_statement_list_set(
		LIST *element_statement_list /* in/out */ );

/* Usage */
/* ----- */
LIST *element_account_statement_list(
		LIST *subclassification_statement_list,
		LIST *account_statement_list );

/* Usage */
/* ----- */
void element_delta_prior_percent_set(
		ELEMENT *prior_element /* in/out */,
		ELEMENT *current_element );

/* Usage */
/* ----- */
void element_account_transaction_count_set(
		LIST *element_statement_list,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
const char *element_resolve_expense(
		const char *element_expense,
		const char *element_expenditure );

/* Usage */
/* ----- */
void element_account_action_string_set(
		LIST *element_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Usage */
/* ----- */
void element_percent_of_asset_set(
		LIST *element_statement_list );

/* Usage */
/* ----- */
void element_percent_of_income_set(
		LIST *element_statement_list );

/* Process */
/* ------- */
double element_income(
		ELEMENT *element_revenue,
		ELEMENT *element_gain );

/* Usage */
/* ----- */
double element_net_income(
		LIST *element_statement_list );

/* Public */
/* ------ */
boolean element_is_nominal(
		char *element_name );

ELEMENT *element_seek(
		char *element_name,
		LIST *element_list );

double element_list_debit_sum(
		LIST *element_statement_list );

double element_list_credit_sum(
		LIST *element_statement_list );

double element_seek_sum(
		char *element_name,
		LIST *element_statement_list );

#endif

