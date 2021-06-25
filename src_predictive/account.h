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
#include "entity.h"
#include "journal.h"
#include "html_table.h"

/* Constants */
/* --------- */
#define ACCOUNT_NET_INCOME			\
					"net_income"

#define ACCOUNT_CHANGE_IN_NET_ASSETS		\
					"change_in_net_assets"

#define ACCOUNT_TABLE_NAME		"account"
#define ACCOUNT_NOT_SET			"not_set"

#define ACCOUNT_CASH_KEY		"cash_key"
#define ACCOUNT_GAIN_KEY		"gain_key"
#define ACCOUNT_LOSS_KEY		"loss_key"
#define ACCOUNT_DEPRECIATION_KEY 	"depreciation_expense_key"
#define ACCOUNT_ACCUMULATED_KEY		"accumulated_depreciation_key"
#define ACCOUNT_SALES_TAX_EXPENSE_KEY	"sales_tax_key"
#define ACCOUNT_SALES_TAX_PAYABLE_KEY	"sales_tax_payable_key"
#define ACCOUNT_REVENUE_KEY		"revenue_key"
#define ACCOUNT_SHIPPING_REVENUE_KEY	"shipping_revenue_key"
#define ACCOUNT_RECEIVABLE_KEY		"receivable_key"
#define ACCOUNT_PAYABLE_KEY		"payable_key"
#define ACCOUNT_CLOSING_KEY		"closing_key"
#define ACCOUNT_DRAWING_KEY		"drawing_key"
#define ACCOUNT_FEES_EXPENSE_KEY	"fees_expense_key"
#define ACCOUNT_UNCLEARED_CHECKS_KEY	"uncleared_checks_key"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *account_name;
	char *fund_name;
	char *element_name;
	char *subclassification_name;
	char *account_key;
	double chart_account_number;

	/* Process */
	/* ------- */
	LIST *journal_list;
	JOURNAL *latest_journal;
	double balance;
	double annual_budget;
	boolean accumulate_debit;
	double payment_amount;
	LIST *account_balance_zero_journal_list;
	LIST *liability_journal_list;
	double account_liability_due;
	char *account_action_string;
	double account_balance;
	int percent_of_asset;
	int percent_of_revenue;
	int delta_prior;
	boolean display_if_zero;
	double account_receivable_due;
} ACCOUNT;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *account_escape_name(
			char *account_name );
char *account_name_escape(
			char *account_name );

/* Returns static memory */
/* --------------------- */
char *account_name_format(
			char *account_name );

/* Sets account->accumulate_debit */
/* ------------------------------ */
ACCOUNT *account_fetch(	char *account_name );

ACCOUNT *account_new(	char *account_name );

ACCOUNT *account_list_seek(
			char *account_name,
			LIST *account_list );

ACCOUNT *account_seek(	
			char *account_name,
			LIST *account_list );

char *account_hard_coded_account_name(
			char *fund_name,
			char *account_key,
			boolean warning_only,
			const char *calling_function_name );

char *account_depreciation_expense(
			char *fund_name );

char *account_sales_tax_payable(
			char *fund_name );

char *account_accumulated_depreciation(
			char *fund_name );

char *account_revenue(	char *fund_name );

char *account_receivable(
			char *fund_name );

char *account_uncleared_checks(
			char *fund_name );

char *account_shipping_revenue(
			char *fund_name );

char *account_cash(	char *fund_name );

char *account_gain(	char *fund_name );

char *account_loss(	char *fund_name );

char *account_payable(	char *fund_name );

char *account_fees_expense(
			char *fund_name );

char *account_name_display(
			char *account_name );

int account_balance_match_function(
			ACCOUNT *account_from_list,
			ACCOUNT *account_compare );

/* Sets account->accumulate_debit */
/* ------------------------------ */
ACCOUNT *account_parse(	char *input );

/* Safely returns heap memory */
/* -------------------------- */
char *account_primary_where(
			char *account_name );

LIST *account_list(	void );

LIST *account_list_fetch(
			char *where );

LIST *account_system_list(
			char *sys_string );

ACCOUNT *account_key_seek(
			LIST *account_list,
			char *fund_name,
			char *account_key );

char *account_select(	void );

char *account_non_cash_account_name(
			LIST *journal_list );

void account_list_html_output(
			HTML_TABLE *html_table,
			LIST *account_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator );

LIST *account_omit_latex_row_list(
			double *total_element,
			LIST *account_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator );

void account_propagate( char *account_name,
			char *transaction_date_time );

ACCOUNT *account_getset(
			LIST *account_list,
			char *account_name );

boolean account_accumulate_debit(
			char *subclassification_name );

boolean account_name_accumulate_debit(
			char *account_name );

void account_transaction_propagate(
			char *propagate_transaction_date_time );

char *account_system_string(
			char *where );

ACCOUNT *account_key_fetch(
			char *account_key );

boolean account_name_changed(
			char *preupdate_account_name );

char *account_action_string(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date,
			char *account_name );

void account_list_action_string_set(
			LIST *account_list,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date );

double account_list_balance(
			LIST *account_list );

void account_list_percent_of_asset_set(
			LIST *account_list,
			double asset_total );

void account_list_percent_of_revenue_set(
			LIST *account_list,
			double revenue_total );

void account_list_delta_prior_set(
			LIST *prior_account_list,
			LIST *account_list );

double account_list_balance(
			LIST *account_list );

double account_debit_total(
			LIST *account_list );

double account_credit_total(
			LIST *account_list );

double account_positive_balance_total(
			LIST *account_list );

char *account_element_name(
			char *subclassification_name );

boolean account_name_accumulate_debit(
			char *account_name );

char *account_list_display(
			LIST *account_list );

LIST *account_balance_zero_journal_list(
			char *account_name );

double account_liability_due(
			LIST *liability_journal_list );

#endif
