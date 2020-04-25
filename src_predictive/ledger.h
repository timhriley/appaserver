/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/ledger.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver ledger ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef LEDGER_H
#define LEDGER_H

#include "list.h"
#include "boolean.h"
#include "html_table.h"
#include "date.h"
#include "latex.h"
#include "hash_table.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define LEDGER_NOT_SET_ACCOUNT		"not_set"

#define LEDGER_LOCK_TRANSACTION_ATTRIBUTE		\
					"lock_transaction_yn"

#define LEDGER_BEGINNING_BALANCE_LABEL	"Equity Beginning Balance"

#define LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS		\
					"change_in_net_assets"

#define LEDGER_ACCOUNT_NET_INCOME			\
					"net_income"

#define LEDGER_SUBCLASSIFICATION_NET_ASSETS		\
					"net_assets"

#define LEDGER_SUBCLASSIFICATION_CONTRIBUTED_CAPITAL	\
					"contributed_capital"

#define LEDGER_SUBCLASSIFICATION_RETAINED_EARNINGS	\
					"retained_earnings"

#define LEDGER_CLOSING_ENTRY_MEMO	"close closing"
#define LEDGER_FOLDER_NAME		"journal_ledger"
#define TRANSACTION_FOLDER_NAME		"transaction"
#define ACCOUNT_FOLDER_NAME		"account"
#define SUBCLASSIFICATION_FOLDER_NAME	"subclassification"
#define ELEMENT_FOLDER_NAME		"element"

#define LEDGER_ASSET_ELEMENT		"asset"
#define LEDGER_LIABILITY_ELEMENT	"liability"
#define LEDGER_EQUITY_ELEMENT		"equity"
#define LEDGER_REVENUE_ELEMENT		"revenue"
#define LEDGER_EXPENSE_ELEMENT		"expense"
#define LEDGER_GAIN_ELEMENT		"gain"
#define LEDGER_LOSS_ELEMENT		"loss"
#define LEDGER_RETAINED_EARNINGS	"retained_earnings"

#define LEDGER_CLOSING_TRANSACTION_TIME	"23:59:59"
#define LEDGER_PRIOR_TRANSACTION_TIME	"23:59:58"

#define LEDGER_GENERAL_FUND		"general_fund"
#define LEDGER_CASH_KEY			"cash_key"
#define LEDGER_LOSS_KEY			"loss_key"
#define LEDGER_ACCUMULATED_KEY		"accumulated_depreciation_key"
#define LEDGER_SALES_TAX_EXPENSE_KEY	"sales_tax_key"
#define LEDGER_SALES_TAX_PAYABLE_KEY	"sales_tax_payable_key"
#define LEDGER_ACCOUNT_PAYABLE_KEY	"account_payable_key"
#define LEDGER_REVENUE_KEY		"revenue_key"
#define LEDGER_ACCOUNT_RECEIVABLE_KEY	"account_receivable_key"
#define LEDGER_CLOSING_KEY		"closing_key"
#define LEDGER_UNCLEARED_CHECKS_KEY	"uncleared_checks_key"

#define LEDGER_SEMAPHORE_KEY		18227

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	int transaction_count;
	int database_transaction_count;
	double previous_balance;
	double database_previous_balance;
	double debit_amount;
	double credit_amount;
	double balance;
	double database_balance;
	char *memo;
	char *property_street_address;
	int check_number;
	boolean match_sum_taken;
} JOURNAL_LEDGER;

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	double transaction_amount;
	double database_transaction_amount;
	char *memo;
	int check_number;
	LIST *journal_ledger_list;
	boolean lock_transaction;
	char *property_street_address;
} TRANSACTION;

typedef struct
{
	char *account_name;
	char *fund_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	JOURNAL_LEDGER *latest_ledger;
	LIST *journal_ledger_list;
	boolean accumulate_debit;
	double balance;
	double payment_amount;
} ACCOUNT;

typedef struct
{
	char *subclassification_name;
	LIST *account_list;
	double subclassification_total;
} SUBCLASSIFICATION;

typedef struct
{
	char *element_name;
	boolean accumulate_debit;

	/* -------------------------- */
	/* if !omit_subclassification */
	/* -------------------------- */
	LIST *subclassification_list;

	/* ------------------------- */
	/* if omit_subclassification */
	/* ------------------------- */
	LIST *account_list;

	double element_total;
} LEDGER_ELEMENT;

typedef struct
{
	char *account_name;
	char *contra_to_account_name;
} CONTRA_ACCOUNT;

typedef struct
{
	LIST *element_list;
	LIST *contra_account_list;
} LEDGER;

/* Operations */
/* ---------- */
LEDGER *ledger_new(			void );

LIST *ledger_get_contra_account_list(	char *application_name );

CONTRA_ACCOUNT *ledger_contra_account_new(
					char *account_name,
					char *contra_to_account_name );

JOURNAL_LEDGER *ledger_journal_ledger_calloc(
					void );

JOURNAL_LEDGER *journal_ledger_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name );

ACCOUNT *ledger_account_fetch(		char *application_name,
					char *account_name );

ACCOUNT *ledger_account_new(		char *account_name );

SUBCLASSIFICATION *ledger_new_subclassification(
					char *subclassification_name );

LEDGER_ELEMENT *ledger_new_element(	char *element_name ); 

LEDGER_ELEMENT *ledger_element_new(	char *element_name );

LEDGER_ELEMENT *ledger_account_fetch_element(
					char *application_name,
					char *account_name );

LIST *ledger_subclassification_get_account_list(
					double *subclassification_total,
					char *application_name,
					char *subclassification_name,
					char *fund_name,
					char *as_of_date );

LIST *ledger_element_get_account_list(
					double *element_total,
					char *application_name,
					char *element_name,
					char *fund_name,
					char *as_of_date );

LIST *ledger_element_get_subclassification_list(
					double *element_total,
					char *application_name,
					char *element_name,
					char *fund_name,
					char *as_of_date );

LIST *ledger_get_element_list(		char *application_name,
					LIST *filter_element_name_list,
					char *fund_name,
					char *as_of_date,
					boolean omit_subclassification );

JOURNAL_LEDGER *ledger_get_prior_ledger(char *application_name,
					char *transaction_date_time,
					char *account_name );

void ledger_load(			char **full_name,
					char **street_address,
					int *transaction_count,
					double *previous_balance,
					double *debit_amount,
					double *credit_amount,
					double *balance,
					char **memo,
					char *transaction_date_time,
					char *account_name,
					char *application_name );

boolean ledger_account_get_accumulate_debit(
					char *application_name,
					char *account_name );

boolean ledger_element_get_accumulate_debit(
					char *application_name,
					char *element_name );

void ledger_list_set_balances(		LIST *ledger_list,
					boolean accumulate_debit );

void ledger_account_list_propagate(	LIST *propagate_account_list,
					char *application_name );

void ledger_journal_ledger_list_propagate(
					LIST *journal_ledger_list,
					char *application_name );

void ledger_propagate_account_name_list(
					char *application_name,
					char *transaction_date_time,
					LIST *account_name_list );

boolean ledger_propagate_journal_ledger_list(
					char *application_name,
					char *transaction_date_time,
					LIST *journal_ledger_list );

void ledger_propagate(			char *application_name,
					char *transaction_date_time,
					char *account_name );

LIST *ledger_subclassification_quickly_get_account_list(
					char *application_name,
					char *subclassification_name );

JOURNAL_LEDGER *ledger_get_latest_ledger(
					char *application_name,
					char *account_name,
					char *as_of_date );

double ledger_get_element_value(	LIST *subclassification_list,
					boolean element_accumulate_debit );

double ledger_get_net_income(		double total_revenues,
					double total_expenses,
					double total_gains,
					double total_losses );

void ledger_output_subclassification_aggregate_net_income(
					HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities,
					double percent_denominator );

void ledger_output_net_income(		HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities,
					double percent_denominator,
					int skip_columns );

LEDGER_ELEMENT *ledger_element_seek(	LIST *element_list,
					char *element_name );

LEDGER_ELEMENT *ledger_element_list_seek(
					LIST *element_list,
					char *element_name );

char *ledger_transaction_display(	TRANSACTION *transaction );

void ledger_list_html_display(		char *transaction_memo,
					LIST *ledger_list );

void ledger_list_text_display(		char *transaction_memo,
					LIST *ledger_list );

char *ledger_list_display(		LIST *ledger_list );

char *ledger_journal_ledger_list_display(
					LIST *journal_ledger_list );

char *ledger_account_list_display(
					LIST *account_list );

TRANSACTION *ledger_transaction_seek(	LIST *transaction_list,
					char *full_name,
					char *street_address,
					char *transaction_date_time );

TRANSACTION *ledger_transaction_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *memo );

TRANSACTION *ledger_transaction_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time );

TRANSACTION *ledger_transaction_with_load_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time );

boolean ledger_transaction_load(	double *transaction_amount,
					double *database_transaction_amount,
					char **memo,
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time );

/* Returns transaction_list with transaction_date_time changed if needed. */
/* ---------------------------------------------------------------------- */
LIST *ledger_transaction_list_insert(	LIST *transaction_list,
					char *application_name );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction );

void ledger_journal_ledger_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double amount,
					boolean is_debit );

void ledger_transaction_amount_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					double database_transaction_amount );

void ledger_entity_update(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *preupdate_full_name,
					char *preupdate_street_address );

void ledger_transaction_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data );

void ledger_journal_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data );

void ledger_transaction_memo_update(	char *application_name,
					TRANSACTION *transction );

void ledger_journal_ledger_partial_parse(
					int *transaction_count,
					int *database_transaction_count,
					double *previous_balance,
					double *database_previous_balance,
					double *debit_amount,
					double *credit_amount,
					double *balance,
					double *database_balance,
					char *input_buffer );

LIST *ledger_get_year_journal_ledger_list(
					char *application_name,
					int year,
					char *account_name );

LIST *ledger_get_journal_ledger_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *minimum_transaction_date_time,
					char *account_name );

char *ledger_get_minimum_transaction_date(
					char *application_name );

double ledger_inventory_purchase_get_sum_extension(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time );

void ledger_get_purchase_order_account_names(
				char **sales_tax_expense_account,
				char **freight_in_expense_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name );

void ledger_get_account_payable_account_name(
				char **account_payable_account,
				char *application_name,
				char *fund_name );

void ledger_get_customer_sale_account_names(
				char **sales_tax_payable_account,
				char **shipping_revenue_account,
				char **account_receivable_account,
				char *application_name,
				char *fund_name );

void ledger_insert_purchase_order_journal_ledger(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					double sum_inventory_extension,
					double sum_supply_extension,
					double sum_service_extension,
					double sum_fixed_asset_extension,
					double sales_tax,
					double freight_in,
					double purchase_amount );

char *ledger_get_transaction_where(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *folder_name,
					char *date_time_column );

void ledger_get_vendor_payment_account_names(
					char **checking_account,
					char **uncleared_checks_account,
					char **account_payable_account,
					char *application_name,
					char *fund_name );

void ledger_insert_customer_payment_journal_ledger(
					char *application_name,
					char *full_name,
					char *street_address,
					char *payment_date_time,
					double payment_amount );

LIST *ledger_get_propagate_journal_ledger_list(
					char *application_name,
					JOURNAL_LEDGER *prior_ledger,
					char *account_name );

void ledger_delete(			char *application_name,
					char *folder_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time );

void ledger_journal_delete(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name );

void ledger_propagate_accounts(		LIST *propagate_account_list,
					char *application_name,
					char *propagate_transaction_date_time );

boolean ledger_transaction_exists(	char *application_name,
					char *transaction_date_time );

LIST *ledger_fetch_transaction_list(	char *application_name,
					char *where_clause );

LIST *ledger_transaction_date_time_account_name_list(
					char *application_name,
					char *transaction_date_time );

TRANSACTION *ledger_transaction_calloc(	void );

double ledger_get_account_debit_amount(	LIST *journal_ledger_list,
					char *account_name );

double ledger_get_account_credit_amount(LIST *journal_ledger_list,
					char *account_name );

void ledger_journal_ledger_update(	FILE *update_pipe,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double debit_amount,
					double credit_amount );

void ledger_debit_credit_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *debit_account_name,
					char *credit_account_name,
					double transaction_amount );

JOURNAL_LEDGER *ledger_journal_ledger_seek(
					LIST *journal_ledger_list,
					char *account_name );

FILE *ledger_open_update_pipe(		char *application_name );

char *ledger_transaction_get_select(	void );

char *ledger_get_transaction_purchase_order_join_where(
					void );

char *ledger_get_transaction_customer_sale_join_where(
					void );

char *ledger_get_journal_ledger_transaction_join_where(
					void );

char *ledger_get_journal_ledger_purchase_order_join_where(
					void );

char *ledger_get_journal_ledger_customer_sale_join_where(
					void );

char *ledger_get_transaction_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time );

void ledger_transaction_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **memo,
				char *input_buffer );

HASH_TABLE *ledger_get_transaction_hash_table(
				char *application_name,
				char *inventory_name );

void ledger_set_transaction_hash_table(
				HASH_TABLE *transaction_hash_table,
				char *input_buffer );

void ledger_set_journal_ledger_hash_table(
				HASH_TABLE *journal_ledger_hash_table,
				char *input_buffer );

char *ledger_journal_ledger_get_select(	void );

char *ledger_get_journal_ledger_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name );

void ledger_journal_ledger_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **account_name,
				int *transaction_count,
				int *database_transaction_count,
				double *previous_balance,
				double *database_previous_balance,
				double *debit_amount,
				double *credit_amount,
				double *balance,
				double *database_balance,
				char *input_buffer );

HASH_TABLE *ledger_get_journal_ledger_hash_table(
				char *application_name,
				char *inventory_name );

TRANSACTION *ledger_sale_hash_table_build_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table );

void ledger_set_transaction_hash_table(
				HASH_TABLE *transaction_hash_table,
				char *input_buffer );

void ledger_set_journal_ledger_hash_table(
				HASH_TABLE *journal_ledger_hash_table,
				char *input_buffer );

void ledger_update_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *new_transaction_date_time );

char *ledger_get_transaction_date_time(
				char *transaction_date );

char *ledger_get_shipped_date_transaction_date_time(
				char *shipped_date );

char *ledger_get_account_subclassification_name(
				char *application_name,
				char *account_name );

char *ledger_get_supply_expense_account(
				char *application_name,
				char *supply_name );

void ledger_order_entry_account_names(
				char **checking_account,
				char **receivable_account,
				char **revenue_account,
				char *application_name,
				char *fund_name );

void ledger_get_depreciation_account_names(
				char **depreciation_expense_account,
				char **accumulated_depreciation_account,
				char *application_name,
				char *fund_name );

CONTRA_ACCOUNT *ledger_seek_contra_account(
				LIST *contra_account_list,
				char *account_name );

LIST *ledger_get_subclassification_aggregate_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				char *element_name,
				double percent_denominator );

LIST *ledger_get_subclassification_display_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				char *element_name,
				boolean element_accumulate_debit,
				double percent_denominator );

LIST *ledger_get_subclassification_omit_latex_row_list(
				double *total_element,
				LIST *account_list,
				char *element_name,
				boolean element_accumulate_debit,
				double percent_denominator );

LATEX_ROW *ledger_get_subclassification_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities,
				double percent_denominator );

LATEX_ROW *ledger_get_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities,
				double percent_denominator,
				boolean omit_subclassification );

LATEX_ROW *ledger_get_latex_liabilities_plus_equity_row(
				double liabilities_plus_equity,
				int skip_columns );

char *ledger_get_hard_coded_account_name(
				char *application_name,
				char *fund_name,
				char *hard_coded_account_key,
				boolean warning_only,
				const char *calling_function_name );

char *ledger_get_hard_coded_dictionary_key(
				char *fund_name,
				char *hard_coded_account_key );

void ledger_account_load(	char **fund_name,
				char **subclassification_name,
				char **hard_coded_account_key,
				boolean *accumulate_debit,
				char *application_name,
				char *account_name );

void ledger_account_parse(	char **account_name,
				char **fund_name,
				char **subclassification_name,
				char **hard_coded_account_key,
				char *input_buffer );

LIST *ledger_fetch_account_list(char *application_name,
				char *as_of_date );

ACCOUNT *ledger_seek_account(	LIST *account_list,
				char *account_name );

ACCOUNT *ledger_account_get_or_set(
				LIST *account_list,
				char *account_name );

ACCOUNT *ledger_account_calloc(	void );

ACCOUNT *ledger_subclassification_fund_seek_account(
				LIST *account_list,
				char *subclassification_name,
				char *fund_name );

ACCOUNT *ledger_seek_hard_coded_account_key_account(
				LIST *account_list,
				char *fund_name,
				char *hard_coded_account_key );

int ledger_balance_match_function(
				ACCOUNT *account_from_list,
				ACCOUNT *account_compare );

DICTIONARY *ledger_get_hard_coded_dictionary(
				char *application_name );

DICTIONARY *ledger_get_fund_hard_coded_dictionary(
				char *application_name );

DICTIONARY *ledger_get_non_fund_hard_coded_dictionary(
				char *application_name );

char *ledger_get_closing_transaction_date_time(
				char *as_of_date );

void ledger_propagate_element_list(
				char *application_name,
				char *transaction_date_time_string,
				LIST *element_list );

char *ledger_beginning_transaction_date(
				char *application_name,
				char *fund_name,
				char *ending_transaction_date );

/* Returns begin_date_string */
/* ------------------------- */
char *ledger_get_report_title_sub_title(
				char *title,
				char *sub_title,
				char *process_name,
				char *application_name,
				char *fund_name,
				char *as_of_date,
				int fund_name_list_length,
				char *logo_filename );

LIST *ledger_get_fund_name_list(
				char *application_name );

LIST *ledger_fetch_fund_name_list(
				char *application_name );

LIST *ledger_get_beginning_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				boolean element_accumulate_debit );

LIST *ledger_get_subclassification_aggregate_beginning_row_list(
				double *total_element,
				LIST *subclassification_list,
				double percent_denominator );

/* If debit_amount or credit_amount needs to be changed.*/
/* ---------------------------------------------------- */
boolean ledger_journal_ledger_list_reset_amount(
				LIST *journal_ledger_list,
				double amount );

boolean ledger_journal_ledger_list_amount_update(
				char *application_name,
				LIST *journal_ledger_list );

LIST *ledger_get_inventory_account_name_list(
				char *application_name );

LIST *ledger_get_service_account_name_list(
				char *application_name );

ACCOUNT *ledger_element_list_account_seek(
				LIST *element_list,
				char *account_name );

DATE *ledger_prior_closing_transaction_date(
				char *application_name,
				char *fund_name,
				char *as_of_date );

boolean ledger_is_period_element(
				char *element_name );

double ledger_get_fraction_of_year(
				char *prior_date_string,
				char *date_string );

char *ledger_get_latest_zero_balance_transaction_date_time(
				char *application_name,
				char *account_name );

char *ledger_account_get_select(char *application_name );

void ledger_append_propagate_account_list(
				LIST *propagate_account_list,
				char *transaction_date_time,
				char *account_name,
				char *application_name );

ACCOUNT *ledger_account_list_seek(
				LIST *account_list,
				char *account_name );

LIST *ledger_get_after_balance_zero_journal_ledger_list(
				char *application_name,
				char *account_name );

JOURNAL_LEDGER *ledger_get_or_set_journal_ledger(
				LIST *journal_ledger_list,
				char *account_name );

int ledger_get_non_empty_subclassification_list_length(
				LIST *subclassification_list );

TRANSACTION *ledger_inventory_purchase_hash_table_build_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table );

FILE *ledger_transaction_insert_open_stream(
				char *application_name );

void ledger_transaction_insert_stream(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction );

void ledger_journal_insert_open_stream(
				FILE **debit_account_pipe,
				FILE **credit_account_pipe,
				char *application_name );

void ledger_journal_insert_stream(
				FILE *debit_output_pipe,
				FILE *credit_output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double amount,
				char *debit_account_name,
				char *credit_account_name );

char *ledger_get_fund_where(	char *application_name,
				char *fund_name );

boolean ledger_fund_attribute_exists(
				char *application_name );

boolean ledger_title_passage_rule_attribute_exists(
				char *application_name,
				char *folder_name );

boolean ledger_folder_exists(	char *application_name,
				char *folder_name );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				LIST *journal_ledger_list );

boolean ledger_propagate_account_list_exists(
				LIST *propagate_account_list,
				char *account_name );

TRANSACTION *ledger_customer_sale_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list,
				double shipping_revenue,
				double sales_tax,
				double invoice_amount,
				char *fund_name );

TRANSACTION *ledger_purchase_order_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				double sales_tax,
				double freight_in,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				char *fund_name );

TRANSACTION *ledger_specific_inventory_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *specific_inventory_purchase_list,
				char *fund_name );

TRANSACTION *ledger_inventory_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *inventory_purchase_list,
				char *fund_name );

void ledger_get_payroll_account_names(
				char **salary_wage_expense_account,
				char **payroll_tax_account,
				char **payroll_payable_account,
				char **federal_withholding_payable_account,
				char **state_withholding_payable_account,
				char **social_security_payable_account,
				char **medicare_payable_account,
				char **retirement_plan_payable_account,
				char **health_insurance_payable_account,
				char **union_dues_payable_account,
				char **federal_unemployment_tax_payable_account,
				char **state_unemployment_tax_payable_account,
				char *application_name,
				char *fund_name );

/* Returns heap memory. */
/* -------------------- */
char *ledger_max_transaction_date(
				char *application_name );

/* Returns heap memory. */
/* -------------------- */
char *ledger_fetch_max_transaction_date_time(
				char *application_name );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_journal_ledger_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				LIST *journal_ledger_list );

void ledger_journal_ledger_batch_insert(
				char *application_name,
				LIST *transaction_list );

LIST *ledger_sort_element_list(	LIST *element_list );

void ledger_get_investment_account_names(
				char **unrealized_investment,
				char **realized_gain,
				char **realized_loss,
				char **checking_account,
				char **contributed_capital_account,
				char *application_name,
				char *fund_name );

char *ledger_transaction_binary_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				char *debit_account_name,
				char *credit_account_name );

char *ledger_fetch_unique_transaction_date_time(
				char *application_name,
				char *transaction_date_time );

double ledger_output_html_account_list(
				HTML_TABLE *html_table,
				LIST *account_list,
				char *element_name,
				boolean element_accumulate_debit,
				double percent_denominator );

double ledger_output_html_subclassification_list(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				char *element_name,
				boolean element_accumulate_debit,
				double percent_denominator );

double ledger_output_net_assets_html_subclassification_list(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				boolean element_accumulate_debit );

double ledger_output_subclassification_aggregate_html_element(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				char *element_name,
				double percent_denominator );

boolean ledger_exists_net_assets_equity_subclassification(
				LIST *subclassification_list );

boolean ledger_exists_journal_ledger(
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *transaction_date,
				double transaction_amount );

/* Returns transaction_date_time */
/* ----------------------------- */
char *ledger_transaction_date_time_update(
				/* ------------------------------------------ */
				/* Sets journal_ledger->transaction_date_time */
				/* ------------------------------------------ */
				LIST *journal_ledger_list,
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *preupdate_transaction_date_time );

void ledger_journal_ledger_transaction_date_time_update(
			FILE *update_pipe,
			char *full_name,
			char *street_address,
			char *preupdate_transaction_date_time,
			char *account_name,
			char *transaction_date_time );

DICTIONARY *ledger_account_pipe2dictionary(
				char *sys_string,
				char delimiter );

LIST *ledger_get_binary_ledger_list(
				double transaction_amount,
				char *debit_account,
				char *credit_account );

LIST *ledger_consolidate_journal_ledger_list(
				char *application_name,
				LIST *journal_ledger_list );

LIST *ledger_get_unique_account_name_list(
				LIST *journal_ledger_list );

double ledger_debit_credit_get_amount(
				double debit_amount,
				double credit_amount,
				boolean accumulate_debit );

double ledger_get_amount(	JOURNAL_LEDGER *ledger,
				boolean accumulate_debit );

char *ledger_earlier_of_two_date(
				char *date1,
				char *date2 );

boolean ledger_property_street_address_attribute_exists(
				char *application_name );

boolean ledger_tax_recovery_period_attribute_exists(
				char *application_name );

void ledger_transaction_delete_propagate(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *journal_ledger_list );

TRANSACTION *ledger_binary_transaction(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *debit_account,
				char *credit_account,
				double transaction_amount,
				char *memo );

LIST *ledger_build_binary_ledger_list(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *debit_account,
				char *credit_account );

TRANSACTION *ledger_build_binary_transaction(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *debit_account,
				char *credit_account,
				double transaction_amount,
				char *memo );

void ledger_transaction_output_pipe_display(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *journal_ledger_list );

char *ledger_get_minimum_transaction_date(
				char *application_name );

char *ledger_get_existing_closing_transaction_date_time(
				char *application_name,
				char *as_of_date );

boolean ledger_transaction_date_time_exists(
				char *application_name,
				char *transaction_date_time );

char *ledger_get_non_cash_account_name(
				char *application_name,
				TRANSACTION *transaction );

char *ledger_get_account_name(	char *account_name );

double ledger_get_sales_tax(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

double ledger_get_total_payment(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

TRANSACTION *ledger_check_number_seek_transaction(
				LIST *transaction_list,
				int check_number );

JOURNAL_LEDGER *ledger_account_seek_journal_ledger(
				LIST *journal_ledger_list,
				char *account_name );

LIST *ledger_get_credit_journal_ledger_list(
				double *sales_revenue_amount,
				double *service_revenue_amount,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list );

LIST *ledger_get_check_number_journal_ledger_list(
				char *application_name,
				char *minimum_transaction_date_time,
				char *account_name );

JOURNAL_LEDGER *ledger_check_number_seek_journal_ledger(
				LIST *cash_journal_ledger_list,
				int check_number );

JOURNAL_LEDGER *ledger_seek_journal_ledger(
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name );

JOURNAL_LEDGER *ledger_seek_uncleared_journal_ledger(
				char *application_name,
				char *fund_name,
				LIST *journal_ledger_list );

boolean ledger_exists_closing_entry(
				char *application_name,
				char *as_of_date );

#endif
