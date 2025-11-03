/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/journal.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef JOURNAL_H
#define JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"

#define JOURNAL_TABLE			"journal"

#define JOURNAL_SELECT			"full_name,"		\
					"street_address,"	\
					"transaction_date_time,"\
					"account,"		\
					"previous_balance,"	\
					"debit_amount,"		\
					"credit_amount,"	\
					"balance"

#define JOURNAL_INSERT			"full_name,"		\
					"street_address,"	\
					"transaction_date_time,"\
					"account,"		\
					"debit_amount,"		\
					"credit_amount"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	double previous_balance;
	double debit_amount;
	double credit_amount;
	double balance;
	TRANSACTION *transaction;
	ACCOUNT *account;
} JOURNAL;

/* Usage */
/* ----- */
LIST *journal_system_list(
		char *fund_name,
		char *journal_system_string,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Usage */
/* ----- */
JOURNAL	*journal_parse(
		char *fund_name,
		char *input,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *journal_new(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name );

/* Process */
/* ------- */
JOURNAL *journal_calloc(
		void );

/* Usage */
/* ----- */
JOURNAL *journal_prior(
		char *fund_name,
		char *transaction_date_time,
		char *account_name,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element );

/* Usage */
/* ----- */
JOURNAL *journal_latest(
		const char *journal_table,
		char *fund_name,
		char *account_name,
		char *transaction_date_time,
		boolean fetch_transaction_boolean,
		boolean latest_zero_balance_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_transaction_account_where(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_max_prior_where(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_max_prior_transaction_date_time(
		const char *journal_table,
		char *journal_max_prior_where );

/* Usage */
/* ----- */
LIST *journal_year_list(
		int tax_year,
		char *account_name,
		boolean fetch_transaction );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_year_where(
		int tax_year,
		char *account_name,
		const char *transaction_date_preclose_time );

/* Usage */
/* ----- */
JOURNAL *journal_account_fetch(
		char *fund_name,
		char *transaction_date_time,
		char *account_name,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Usage */
/* ----- */
void journal_account_list_getset(
		LIST *account_list /* in/out */,
		LIST *journal_list );

/* Usage */
/* ----- */
void journal_list_update(
		LIST *update_statement_list );

/* Usage */
/* ----- */
void journal_list_insert(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		LIST *journal_list );

/* Usage */
/* ----- */
void journal_list_transaction_insert(
		FILE *pipe,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		LIST *journal_list );

/* Usage */
/* ----- */
void journal_insert(
		FILE *journal_insert_pipe,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double debit_amount,
		double credit_amount );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_insert_data_string(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double debit_amount,
		double credit_amount,
		char *transaction_fund_datum );

/* Usage */
/* ----- */
void journal_list_html_display(
		LIST *journal_list,
		char *transaction_date_time,
		char *transaction_memo,
		char *transaction_full_name );

/* Process */
/* ------- */
void journal_list_pipe_display(
		FILE *output_pipe,
		char *transaction_date_time,
		char *transaction_memo,
		char *heading,
		LIST *journal_list );

/* Usage */
/* ----- */
void journal_list_sum_html_display(
		LIST *journal_list,
		char *transaction_date_time,
		char *transaction_memo,
		double debit_sum,
		double credit_sum );

/* Usage */
/* ----- */
double journal_transaction_amount(
		LIST *journal_list );

/* Usage */
/* ----- */
double journal_account_list_debit_sum(
		LIST *journal_list,
		LIST *account_name_list );

/* Usage */
/* ----- */
double journal_account_list_credit_sum(
		LIST *journal_list,
		LIST *account_name_list );

/* Usage */
/* ----- */
double journal_first_account_balance(
		const char *journal_table,
		char *fund_name,
		char *account_name );

/* Usage */
/* ----- */
double journal_balance_sum(
		LIST *journal_list );

/* Usage */
/* ----- */
LIST *journal_tax_form_list(
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		const char *transaction_date_preclose_time,
		char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_tax_form_where(
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		const char *transaction_date_preclose_time,
		char *account_name );

/* Usage */
/* ----- */
double journal_debit_sum(
		LIST *journal_list );

/* Usage */
/* ----- */
double journal_credit_sum(
		LIST *journal_list );

/* Usage */
/* ----- */
double journal_debit_credit_sum_difference(
		boolean element_accumulate_debit,
		LIST *journal_list );

/* Usage */
/* ----- */
double journal_credit_debit_difference_sum(
		LIST *journal_list );

/* Usage */
/* ----- */
double journal_debit_credit_difference_sum(
		LIST *journal_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *journal_account_new(
		double journal_amount,
		ACCOUNT *debit_account,
		ACCOUNT *credit_account );

/* Usage */
/* ----- */
JOURNAL *journal_debit_new(
		char *debit_account_name,
		double debit_amount );

/* Usage */
/* ----- */
JOURNAL *journal_credit_new(
		char *credit_account_name,
		double credit_amount );

/* Usage */
/* ----- */
LIST *journal_binary_list(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		ACCOUNT *debit_account,
		ACCOUNT *credit_account );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_system_string(
		const char *journal_select,
		const char *journal_table,
		char *where );

/* Usage */
/* ----- */
JOURNAL *journal_seek(
		char *fund_name,
		char *transaction_date_time,
		char *account_name,
		LIST *journal_system_list );

/* Usage */
/* ----- */
LIST *journal_entity_list(
		const char *journal_select,
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *account_name );

/* Usage */
/* ----- */
LIST *journal_transaction_list(
		const char *journal_select,
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_primary_where(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_entity_where(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *account_name );

/* Usage */
/* ----- */
boolean journal_list_match_boolean(
		LIST *journal1_list,
		LIST *journal2_list );

/* Usage */
/* ----- */
boolean journal_exists_boolean(
		LIST *journal_list,
		JOURNAL *match_journal );

/* Usage */
/* ----- */
boolean journal_match_boolean(
		JOURNAL *journal1,
		JOURNAL *journal2 );

/* Usage */
/* ----- */
LIST *journal_account_distinct_entity_list(
		char *journal_table,
		LIST *account_name_list );

/* Usage */
/* ----- */
LIST *journal_extract_account_list(
		LIST *journal_list );

/* Usage */
/* ----- */
char *journal_delete_system_string(
		const char *journal_table,
		char *where );

/* Usage */
/* ----- */
int journal_transaction_count(
		const char *journal_table,
		char *account_name,
		char *transaction_begin_date_string,
		char *transaction_date_time_closing );

/* Usage */
/* ----- */
LIST *journal_date_time_account_name_list(
		char *journal_table,
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_min_transaction_date_time(
		const char *journal_table,
		char *fund_name,
		char *account_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_max_transaction_date_time(
		const char *journal_table,
		char *where );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_account_where(
		char *fund_name,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_transaction_account_where(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_less_equal_where(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_less_where(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_min_transaction_system_string(
		const char *journal_table,
		char *where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_max_transaction_system_string(
		const char *journal_table,
		char *where );

/* Usage */
/* ----- */
double journal_signed_balance(
		double balance,
		boolean accumulate_debit );

/* Usage */
/* ----- */
LIST *journal_account_name_list(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Usage */
/* ----- */
double journal_amount(
		double debit_amount,
		double credit_amount,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_list_last_memo(
		LIST *journal_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *journal_insert_pipe(
		const char *journal_table,
		char *transaction_column_list_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_insert_system_string(
		const char *journal_table,
		char *transaction_column_list_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_column_list_string(
		const char *journal_insert,
		const char *transaction_table,
		const char *transaction_fund_column );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_list_display(
		LIST *journal_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_display(
		JOURNAL *journal );

#endif
