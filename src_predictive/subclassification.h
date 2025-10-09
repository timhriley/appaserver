/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subclassification.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SUBCLASSIFICATION_H
#define SUBCLASSIFICATION_H

#include "list.h"
#include "boolean.h"
#include "element.h"

#define SUBCLASSIFICATION_TABLE			"subclassification"

#define SUBCLASSIFICATION_SELECT		"subclassification,"	\
						"element,"		\
						"display_order"

#define SUBCLASSIFICATION_NET_ASSETS		"net_assets"
#define SUBCLASSIFICATION_CONTRIBUTED_CAPITAL	"contributed_capital"
#define SUBCLASSIFICATION_RETAINED_EARNINGS	"retained_earnings"
#define SUBCLASSIFICATION_DRAWING		"drawing"
#define SUBCLASSIFICATION_CURRENT_LIABILITY	"current_liability"
#define SUBCLASSIFICATION_RECEIVABLE		"receivable"
#define SUBCLASSIFICATION_CASH			"cash_or_equivalent"

#define SUBCLASSIFICATION_NOTANUMBER		"NAN"

#define SUBCLASSIFICATION_CHANGE_IN_NET_ASSETS	"change_in_net_assets"

#define SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL	\
						"Equity Beginning Balance"

#define SUBCLASSIFICATION_NET_ASSETS		"net_assets"
#define SUBCLASSIFICATION_DRAWING		"drawing"
#define SUBCLASSIFICATION_APPROPRIATION		"appropriation"
#define SUBCLASSIFICATION_ENCUMBRANCE		"encumbrance"
#define SUBCLASSIFICATION_ESTIMATED_INTERFUND	"estimated_interfund"
#define SUBCLASSIFICATION_ESTIMATED_REVENUE	"estimated_revenue"
#define SUBCLASSIFICATION_INTERFUND		"interfund"

typedef struct
{
	char *subclassification_name;
	char *element_name;
	int display_order;
	ELEMENT *element;
	LIST *account_statement_list;
	double sum;
	int percent_of_asset;
	int percent_of_income;
	int delta_prior_percent;
} SUBCLASSIFICATION;

/* Usage */
/* ----- */
LIST *subclassification_where_statement_list(
		char *where,
		char *end_date_time_string,
		boolean fetch_element,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *subclassification_system_string(
		char *subclassification_select,
		char *subclassification_table,
		char *where );

FILE *subclassification_pipe(
		char *subclassification_system_string );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_statement_parse(
		char *input,
		char *end_date_time_string,
		boolean fetch_element,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_fetch(
		char *subclassification_name,
		boolean fetch_element );

/* Process */
/* ------- */
SUBCLASSIFICATION *subclassification_seek(
		char *subclassification_name,
		LIST *subclassification_list );

/* Returns static memory */
/* --------------------- */
char *subclassification_primary_where(
		char *subclassification_name );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_parse(
		char *input,
		boolean fetch_element );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_new(
		char *subclassification_name );

/* Process */
/* ------- */
SUBCLASSIFICATION *subclassification_calloc(
		void );

/* Usage */
/* ----- */
LIST *subclassification_account_statement_list(
		LIST *subclassification_statement_list );

/* Usage */
/* ----- */
LIST *subclassification_list(
		char *where,
		boolean fetch_element );

/* Usage */
/* ----- */
LIST *subclassification_system_list(
		char *subclassification_system_string,
		boolean fetch_element );

/* Usage */
/* ----- */

/* --------------------------- */
/* Sets subclassification->sum */
/* --------------------------- */
double subclassification_list_sum_set(
		LIST *subclassification_statement_list );

/* Usage */
/* ----- */
void subclassification_list_delta_prior_percent_set(
		LIST *prior_subclassification_list /* in/out */,
		LIST *current_subclassification_list );

/* Usage */
/* ----- */
void subclassification_delta_prior_percent_set(
		SUBCLASSIFICATION *prior_subclassification /* in/out */,
		SUBCLASSIFICATION *current_subclassification );

/* Usage */
/* ----- */
void subclassification_account_transaction_count_set(
		LIST *subclassification_statement_list,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Usage */
/* ----- */
void subclassification_account_action_string_set(
		LIST *subclassification_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Usage */
/* ----- */
void subclassification_percent_of_asset_set(
		LIST *subclassification_statement_list,
		double element_asset_sum );

/* Usage */
/* ----- */
void subclassification_percent_of_income_set(
		LIST *subclassification_statement_list,
		double element_income );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_element_list_seek(
		char *subclassification_name,
		LIST *element_statement_list );

/* Usage */
/* ----- */
double subclassification_list_debit_sum(
		LIST *subclassification_statement_list,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
double subclassification_list_credit_sum(
		LIST *subclassification_statement_list,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
boolean subclassification_current_liability_boolean(
		char *subclassification_current_liability,
		char *subclassification_name );

/* Usage */
/* ----- */
boolean subclassification_receivable_boolean(
		char *subclassification_receivable,
		char *subclassification_name );

/* Usage */
/* ----- */
boolean subclassification_accumulate_debit(
		boolean element_accumulate_debit,
		ELEMENT *element );

#endif
