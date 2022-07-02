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

#define ACCOUNT_SELECT			"account,"			\
					"subclassification,"		\
					"hard_coded_account_key,"	\
					"chart_account_number,"		\
					"annual_budget"

#define ACCOUNT_TABLE			"account"

#define ACCOUNT_NET_INCOME		"net_income"
#define ACCOUNT_CHANGE_IN_NET_ASSETS	"change_in_net_assets"
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
#define ACCOUNT_UNCLEARED_CHECKS	"uncleared_checks"

typedef struct
{
	/* Input */
	/* ----- */
	char *account_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	int chart_account_number;
	int annual_budget;

	/* Attributes */
	/* ---------- */
	SUBCLASSIFICATION *subclassification;
	LIST *journal_account_journal_list;
	int percent_of_asset;
	int percent_of_revenue;
	int delta_prior;
	LIST *following_balance_zero_journal_list;
	LIST *liability_journal_list;
	double liability_due;
	char *action_string;
} ACCOUNT;

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

/* Returns heap memory */
/* ------------------- */
char *account_system_string(
			char *account_select,
			char *account_table,
			char *where );

ACCOUNT *account_parse(	char *input,
			boolean fetch_subclassification,
			boolean fetch_entity );

ACCOUNT *account_new(	char *account_name );

ACCOUNT *account_calloc(void );

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

/* Usage */
/* ----- */
LIST *account_list(	char *where,
			boolean fetch_subclassification,
			boolean fetch_entity );

ACCOUNT *account_subclassification_fetch(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time );

/* Usage */
/* ----- */
boolean account_accumulate_debit(
			char *account_name,
			boolean expect_lots );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *account_balance_sort_list(
			LIST *account_list );

/* Process */
/* ------- */
int account_balance_match_function(
			ACCOUNT *account_from_list,
			ACCOUNT *account_compare );

/* Usage */
/* ----- */
LIST *account_list_delta_prior_set(
			LIST *prior_account_list,
			LIST *account_list );

/* Process */
/* ------- */
void account_delta_prior_set(
			LIST *prior_account_list,
			ACCOUNT *account );

int account_delta_prior(
			double prior_account_latest_journal_balance,
			double account_latest_journal_balance );

/* Usage */
/* ----- */
LIST *account_list_percent_of_asset_set(
			LIST *account_list,
			double asset_total );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *account_list_percent_of_revenue_set(
			LIST *account_list,
			double revenue_total );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_balance(	LIST *journal_account_journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_debit_total(
			LIST *account_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_credit_total(
			LIST *account_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_balance_total(
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
LIST *account_following_balance_zero_journal_list(
			char *account_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_liability_due(
			LIST *liability_journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double account_receivable_due(
			LIST *receivable_journal_list );

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

char *account_depreciation_expense( void );
char *account_sales_tax_payable( void );
char *account_accumulated_depreciation( void );
char *account_revenue(	void );
char *account_receivable( void );
char *account_uncleared_checks( void );
char *account_shipping_revenue( void );
char *account_cash( void );
char *account_gain( void );
char *account_loss( void );
char *account_payable( void );
char *account_fees_expense( void );

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
