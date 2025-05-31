/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "subclassification.h"

#define ACCOUNT_SELECT			"account,"			\
					"subclassification,"		\
					"hard_coded_account_key,"	\
					"annual_budget"

#define ACCOUNT_CHART_ACCOUNT_NUMBER	"chart_account_number"
#define ACCOUNT_TABLE			"account"
#define ACCOUNT_PRIMARY_KEY		"account"

#define ACCOUNT_UNCLEARED_CHECKS	"uncleared_checks"
#define ACCOUNT_NET_INCOME		"net_income"
#define ACCOUNT_CHANGE_IN_NET_ASSETS	"change_in_net_assets"
#define ACCOUNT_NOT_SET			"not_set"

#define ACCOUNT_DEPRECIATION_KEY 	"depreciation"
#define ACCOUNT_ACCUMULATED_KEY		"accumulated_depreciation"
#define ACCOUNT_SALES_TAX_PAYABLE_KEY	"sales_tax_payable"
#define ACCOUNT_REVENUE_KEY		"revenue"
#define ACCOUNT_RECEIVABLE_KEY		"receivable"
#define ACCOUNT_PAYABLE_KEY		"payable"
#define ACCOUNT_UNCLEARED_CHECKS_KEY	"uncleared_checks"
#define ACCOUNT_SHIPPING_REVENUE_KEY	"shipping_revenue"
#define ACCOUNT_GAIN_KEY		"gain"
#define ACCOUNT_LOSS_KEY		"loss"
#define ACCOUNT_FEES_EXPENSE_KEY	"fees_expense"
#define ACCOUNT_SALES_TAX_EXPENSE_KEY	"sales_tax"
#define ACCOUNT_DRAWING_KEY		"drawing"
#define ACCOUNT_CREDIT_CARD_KEY		"credit_card"
#define ACCOUNT_PASSTHRU_KEY		"credit_card_passthru"
#define ACCOUNT_CLOSING_KEY		"closing_key"
#define ACCOUNT_EQUITY_KEY		"equity_key"
#define ACCOUNT_CGS_KEY			"cost_of_goods_sold"
#define ACCOUNT_INVENTORY_KEY		"inventory"

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	double previous_balance;
	double debit_amount;
	double credit_amount;
	double balance;
	TRANSACTION *transaction;
} ACCOUNT_JOURNAL;

/* Usage */
/* ----- */

/* Returns null if balance is zero */
/* ------------------------------- */
ACCOUNT_JOURNAL *account_journal_latest(
		const char *journal_table,
		char *account_name,
		char *end_date_time_string,
		boolean fetch_transaction );

/* Process */
/* ------- */
ACCOUNT_JOURNAL *account_journal_calloc(
		void );

typedef struct
{
	char *account_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	int annual_budget;
	char *chart_account_number;
	SUBCLASSIFICATION *subclassification;

	/* Set externally */
	/* -------------- */
	ACCOUNT_JOURNAL *account_journal_latest;
	int percent_of_asset;
	int percent_of_income;
	int delta_prior_percent;
	int transaction_count;
	LIST *liability_journal_list;
	double liability_due;
	char *action_string;
} ACCOUNT;

/* Usage */
/* ----- */
LIST *account_statement_list(
		char *subclassification_primary_where,
		char *end_date_time_string,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_journal_latest,
		boolean fetch_transaction );

/* Process */
/* ------- */
FILE *account_pipe(
		char *account_system_string );

/* Usage */
/* ----- */

/* ------------------------------- */
/* Returns null if balance is zero */
/* ------------------------------- */
ACCOUNT *account_statement_parse(
		char *input,
		char *end_date_time_string,
		boolean account_chart_account_boolean,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_journal_latest,
		boolean fetch_transaction );

/* Usage */
/* ----- */
ACCOUNT *account_fetch(
		char *account_name,
		boolean fetch_subclassification,
		boolean fetch_element );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *account_primary_where(
		char *account_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_key_fetch(
		const char *hard_coded_account_key,
		const char *calling_function_name );

/* Usage */
/* ----- */
LIST *account_list(
		char *where,
		boolean fetch_subclassification,
		boolean fetch_element );

/* Usage */
/* ----- */
LIST *account_system_list(
		char *account_system_string,
		boolean account_chart_account_boolean,
		boolean fetch_subclassifiction,
		boolean fetch_element );

/* Usage */
/* ----- */
LIST *account_balance_sort_list(
		LIST *account_statement_list );

/* Process */
/* ------- */
int account_balance_compare_function(
		ACCOUNT *account_from_list,
		ACCOUNT *account_compare );

/* Usage */
/* ----- */
void account_list_delta_prior_percent_set(
		LIST *prior_account_list /* in/out */,
		LIST *current_account_list );

/* Usage */
/* ----- */
void account_percent_of_asset_set(
		LIST *subclassification_account_statement_list,
		double asset_sum );

/* Usage */
/* ----- */
void account_percent_of_income_set(
		LIST *subclassification_account_statement_list,
		double element_income );

/* Usage */
/* ----- */
boolean account_accumulate_debit_boolean(
		char *account_name );
boolean account_accumulate_debit(
		char *account_name );

/* Usage */
/* ----- */
double account_list_debit_sum(
		LIST *account_list,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
double account_list_credit_sum(
		LIST *account_list,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
double account_balance_sum(
		LIST *account_list );

/* Usage */
/* ----- */

/* Returns heap memory from static list or null (maybe) */
/* ---------------------------------------------------- */
char *account_hard_coded_account_name(
		const char *hard_coded_account_key,
		boolean warning_only,
		const char *calling_function_name );

/* Process */
/* ------- */
ACCOUNT *account_key_seek(
		const char *account_key,
		LIST *account_list );

/* Usage */
/* ----- */
ACCOUNT *account_parse(
		char *input,
		boolean account_chart_account_boolean,
		boolean fetch_subclassification,
		boolean fetch_element );

/* Usage */
/* ----- */
ACCOUNT *account_new(
		char *account_name );

/* Process */
/* ------- */
ACCOUNT *account_calloc(
		void );

/* Usage */
/* ----- */
double account_list_sum(
		LIST *account_statement_list );

/* Usage */
/* ----- */
ACCOUNT *account_element_list_seek(
		char *account_name,
		LIST *element_statement_list );
/* Usage */
/* ----- */
ACCOUNT *account_subclassification_list_seek(
		char *account_name,
		LIST *subclassification_statement_list );

/* Usage */
/* ----- */
void account_list_action_string_set(
		LIST *account_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *account_action_string(
		const char *journal_table,
		const char *dictionary_separate_drillthru_prefix,
		const char *drillthru_skipped_key,
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *session,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string,
		char *account_name );

/* Usage */
/* ----- */
void account_transaction_count_set(
		LIST *account_statement_list,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Process */
/* ------- */
int account_transaction_count(
		char *account_table,
		char *account_name,
		char *transaction_begin_date_string,
		char *end_date_time_string );

/* Usage */
/* ----- */

/* ------------------------------------ */
/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_closing_entry_string(
		const char *account_closing_key,
		const char *account_equity_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_key_account_name(
		const char *account_key,
		const char *calling_function_name );

/* Usage */
/* ----- */
boolean account_chart_account_boolean(
		const char *account_table,
		const char *account_chart_account_number );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_select_string(
		const char *account_select,
		const char *account_chart_account_number,
		boolean account_chart_account_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *account_system_string(
		char *account_select_string,
		const char *account_table,
		char *where );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_drawing(
		const char *account_drawing_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_depreciation(
		const char *account_depreciation_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_accumulated_depreciation(
		const char *account_accumulated_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_sales_tax_payable(
		const char *account_sales_tax_payable_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_revenue(
		const char *account_revenue_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_receivable(
		const char *account_receivable_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_payable(
		const char *account_payable_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_uncleared_checks(
		const char *account_uncleared_checks_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_shipping_revenue(
		const char *account_shipping_revenue_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_gain(
		const char *account_gain_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_loss(
		const char *account_loss_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_fees_expense(
		const char *account_fees_expense_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_sales_tax_expense(
		const char *account_sales_tax_expense_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_equity(
		const char *account_equity_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_credit_card_passthru(
		const char *account_passthru_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_cost_of_goods_sold(
		const char *account_CGS_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT *account_inventory(
		const char *account_inventory_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list or null */
/* -------------------------------------------- */
char *account_drawing_string(
		const char *account_drawing_key );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_equity_string(
		const char *account_equity_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_uncleared_checks_string(
		const char *account_uncleared_checks_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_depreciation_string(
		const char *account_depreciation_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_accumulated_depreciation_string(
		const char *account_accumulated_depreciation_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_payable_string(
		const char *account_payable_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_receivable_string(
		const char *account_receivable_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns heap memory from static list */
/* ------------------------------------ */
char *account_loss_string(
		const char *account_loss_key,
		const char *calling_function_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *account_name_format(
		char *account_name );

/* Usage */
/* ----- */

/* Returns account_name or program memory */
/* -------------------------------------- */
char *account_name_display(
		char *account_name );

/* Usage */
/* ----- */
ACCOUNT *account_seek(
		char *account_name,
		LIST *account_list );

/* Usage */
/* ----- */
LIST *account_cash_name_list(
		const char *account_table,
		const char *subclassification_cash );

/* Usage */
/* ----- */
LIST *account_current_liability_name_list(
		const char *account_table,
		const char *subclassification_current_liability,
		const char *account_credit_card_key,
		LIST *exclude_account_name_list );

/* Usage */
/* ----- */
LIST *account_receivable_name_list(
		const char *account_table,
		const char *subclassification_receivable );

/* Usage */
/* ----- */
ACCOUNT *account_getset(
		LIST *account_list,
		ACCOUNT *account );

/* Usage */
/* ----- */
boolean account_name_changed(
		char *preupdate_account_name );

#endif
