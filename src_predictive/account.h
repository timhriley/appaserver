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
#include "journal.h"
#include "html_table.h"

/* Constants */
/* --------- */
#define ACCOUNT_NET_INCOME			\
					"net_income"

#define ACCOUNT_CHANGE_IN_NET_ASSETS		\
					"change_in_net_assets"

#define ACCOUNT_FOLDER_NAME		"account"
#define ACCOUNT_TABLE_NAME		ACCOUNT_FOLDER_NAME
#define ACCOUNT_NOT_SET			"not_set"

#define ACCOUNT_CASH_KEY		"cash_key"
#define ACCOUNT_LOSS_KEY		"loss_key"
#define ACCOUNT_ACCUMULATED_KEY		"accumulated_depreciation_key"
#define ACCOUNT_SALES_TAX_EXPENSE_KEY	"sales_tax_key"
#define ACCOUNT_SALES_TAX_PAYABLE_KEY	"sales_tax_payable_key"
#define ACCOUNT_ACCOUNT_PAYABLE_KEY	"account_payable_key"
#define ACCOUNT_REVENUE_KEY		"revenue_key"
#define ACCOUNT_ACCOUNT_RECEIVABLE_KEY	"account_receivable_key"
#define ACCOUNT_CLOSING_KEY		"closing_key"
#define ACCOUNT_DRAWING_KEY		"drawing_key"
#define ACCOUNT_UNCLEARED_CHECKS_KEY	"uncleared_checks_key"

/* Structures */
/* ---------- */
typedef struct
{
	char *account_name;
	char *fund_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	double chart_account_number;
	JOURNAL *latest_journal;
	boolean accumulate_debit;
	double balance;
	double annual_budget;
	LIST *journal_ledger_list;
} ACCOUNT;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *account_escape_name(
			char *account_name );
char *account_name_escape(
			char *account_name );

ACCOUNT *account_fetch(	char *account_name );

ACCOUNT *account_new(	char *account_name );

ACCOUNT *account_seek(	LIST *account_list,
			char *account_name );

char *account_receivable(
			void );

char *account_uncleared_checks(
			void );

char *account_cash(	void );

char *account_payable(	void );

char *account_fees_expense(
			void );

char *account_name_display(
			char *account_name );

int account_balance_match_function(
			ACCOUNT *account_from_list,
			ACCOUNT *account_compare );

ACCOUNT *account_parse(	char *input );

/* Safely returns heap memory */
/* -------------------------- */
char *account_primary_where(
			char *account_name );

char *account_hard_coded_account_name(
			char *fund_name,
			char *hard_coded_account_key,
			boolean warning_only,
			const char *calling_function_name );

LIST *account_list(	void );

LIST *account_list_fetch(
			char *where );

LIST *account_system_list(
			char *sys_string );

ACCOUNT *account_key_seek(
			LIST *account_list,
			char *fund_name,
			char *hard_coded_account_key );

char *account_select(	void );

char *account_non_cash_account_name(
			LIST *journal_list );

double account_list_html_output(
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

#endif
