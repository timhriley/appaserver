/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "list.h"
#include "boolean.h"
#include "subclassification.h"
#include "journal.h"

#define ACCOUNT_SELECT			"account,"			\
					"subclassification,"		\
					"hard_coded_account_key,"	\
					"chart_account_number,"		\
					"annual_budget"

#define ACCOUNT_TABLE			"account"

#define ACCOUNT_UNCLEARED_CHECKS	"uncleared_checks"
#define ACCOUNT_NET_INCOME		"net_income"
#define ACCOUNT_CHANGE_IN_NET_ASSETS	"change_in_net_assets"
#define ACCOUNT_NOT_SET			"not_set"

#define ACCOUNT_DEPRECIATION_KEY 	"depreciation_expense_key"
#define ACCOUNT_ACCUMULATED_KEY		"accumulated_depreciation_key"
#define ACCOUNT_SALES_TAX_PAYABLE_KEY	"sales_tax_payable_key"
#define ACCOUNT_REVENUE_KEY		"revenue_key"
#define ACCOUNT_RECEIVABLE_KEY		"receivable_key"
#define ACCOUNT_PAYABLE_KEY		"payable_key"
#define ACCOUNT_UNCLEARED_CHECKS_KEY	"uncleared_checks_key"
#define ACCOUNT_SHIPPING_REVENUE_KEY	"shipping_revenue_key"
#define ACCOUNT_CASH_KEY		"cash_key"
#define ACCOUNT_GAIN_KEY		"gain_key"
#define ACCOUNT_LOSS_KEY		"loss_key"
#define ACCOUNT_FEES_EXPENSE_KEY	"fees_expense_key"
#define ACCOUNT_SALES_TAX_EXPENSE_KEY	"sales_tax_key"
#define ACCOUNT_CLOSING_KEY		"closing_key"
#define ACCOUNT_DRAWING_KEY		"drawing_key"

typedef struct
{
	char *account_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	int chart_account_number;
	int annual_budget;
	SUBCLASSIFICATION *subclassification;
	JOURNAL *journal_latest;
	int percent_of_asset;
	int percent_of_revenue;
	int delta_prior;
	int transaction_count;
	LIST *liability_journal_list;
	double liability_due;
	char *action_string;
} ACCOUNT;

/* Usage */
/* ----- */
LIST *account_statement_list(
			char *subclassification_primary_where,
			char *transaction_date_time_closing,
			boolean fetch_journal_latest,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *account_system_string(
			char *account_select,
			char *account_table,
			char *where );

FILE *account_pipe(
			char *account_system_string );

/* Usage */
/* ----- */
ACCOUNT *account_statement_parse(
			char *input,
			char *transaction_date_time_closing,
			boolean fetch_journal_latest,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Usage */
/* ----- */
ACCOUNT *account_fetch(	char *account_name,
			boolean fetch_subclassification,
			boolean fetch_entity );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *account_primary_where(
			char *account_name );

/* Usage */
/* ----- */
ACCOUNT *account_key_fetch(
			char *hard_coded_account_key,
			boolean fetch_subclassification,
			boolean fetch_entity );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *account_hard_coded_key_where(
			char *hard_coded_account_key );

/* Usage */
/* ----- */
LIST *account_system_list(
			char *account_system_string,
			boolean fetch_subclassifiction,
			boolean fetch_entity );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *account_list(	char *where,
			boolean fetch_subclassification,
			boolean fetch_element );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *account_balance_sort_list(
			LIST *account_statement_list );

/* Process */
/* ------- */
int account_balance_match_function(
			ACCOUNT *account_from_list,
			ACCOUNT *account_compare );

/* Usage */
/* ----- */
void account_list_prior_year_set(
			LIST *prior_account_list /* in/out */,
			LIST *current_account_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void account_prior_year_set(
			ACCOUNT *prior_account /* in/out */,
			ACCOUNT *current_account );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void account_list_percent_of_asset_set(
			LIST *element_list_non_nominal_account_list,
			double asset_sum );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void account_list_percent_of_revenue_set(
			LIST *element_list_nominal_account_list,
			double revenue_sum );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_list_debit_sum(
			LIST *account_list,
			boolean element_accumulate_debit );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_list_credit_sum(
			LIST *account_list,
			boolean element_accumulate_debit );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_balance_sum(
			LIST *account_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_hard_coded_account_name(
			char *account_key,
			boolean warning_only,
			const char *calling_function_name );

/* Process */
/* ------- */
ACCOUNT *account_key_seek(
			char *account_key,
			LIST *account_list );

/* Usage */
/* ----- */
ACCOUNT *account_parse(	char *input,
			boolean fetch_subclassification,
			boolean fetch_element );

/* Process */
/* ------- */
ACCOUNT *account_new(	char *account_name );

ACCOUNT *account_calloc(
			void );

/* Usage */
/* ----- */
double account_list_sum(
			LIST *account_statement_list );

/* Usage */
/* ----- */
ACCOUNT *account_element_account_seek(
			char *account_name,
			LIST *element_statement_list );
/* Process */
/* ------- */

/* Usage */
/* ----- */
ACCOUNT *account_subclassification_account_seek(
			char *account_name,
			LIST *subclassification_statement_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void account_list_action_string_set(
			LIST *account_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date );

/* Process */
/* ------- */
char *account_action_string(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *begin_date_string,
			char *as_of_date,
			char *account_name );

/* Usage */
/* ----- */
void account_transaction_count_set(
			LIST *account_statement_list,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing );

/* Process */
/* ------- */
int account_transaction_count(
			char *account_table,
			char *account_name,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *account_name_format(
			char *account_name );

/* Returns program account_name or program memory */
/* ---------------------------------------------- */
char *account_name_display(
			char *account_name );

ACCOUNT *account_seek(	char *account_name,
			LIST *account_list );

char *account_depreciation_expense(
			char *account_depreciation_key );

char *account_accumulated_depreciation(
			char *account_accumulated_key );

char *account_sales_tax_payable(
			char *account_sales_tax_payable_key );

char *account_revenue(	char *account_revenue_key );

char *account_receivable(
			char *account_receivable_key );

char *account_payable(	char *account_payable_key );

char *account_uncleared_checks(
			char *account_uncleared_checks_key );

char *account_shipping_revenue(
			char *account_shipping_revenue_key );

char *account_cash(	char *account_cash_key );

char *account_gain(	char *account_gain_key );

char *account_loss(	char *account_loss_key );

char *account_fees_expense(
			char *account_fees_expense_key );

char *account_closing_entry(
			char *account_closing_key );

char *account_drawing(
			char *account_drawing_key );

LIST *account_cash_name_list(
			char *account_table,
			char *subclassification_cash );

LIST *account_current_liability_name_list(
			char *account_table,
			char *subclassification_current_liability,
			char *account_uncleared_checks );

LIST *account_receivable_name_list(
			char *account_table,
			char *subclassification_receivable );

ACCOUNT *account_getset(
			LIST *account_list,
			char *account_name );

#endif
