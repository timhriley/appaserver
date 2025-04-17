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
		char *journal_system_string,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Process */
/* ------- */
FILE *journal_input_pipe(
		char *journal_system_string );

/* Usage */
/* ----- */
JOURNAL	*journal_parse(
		char *input,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Usage */
/* ----- */
JOURNAL *journal_new(
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
		char *transaction_date_time,
		char *account_name,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_maximum_prior_where(
		char *transaction_date_time,
		char *account_name );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_maximum_prior_transaction_date_time(
		char *journal_maximum_prior_where,
		const char *journal_table );

/* Usage */
/* ----- */
JOURNAL *journal_latest(
		const char *journal_table,
		char *account_name,
		char *end_date_time_string,
		boolean fetch_transaction_boolean,
		boolean zero_balance_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_less_equal_where(
		char *transaction_date_time_closing,
		char *account_name );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_maximum_transaction_date_time(
		const char *journal_table,
		char *journal_less_equal_where );

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
		char *transaction_date_time,
		char *account_name,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_transaction );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_transaction_account_where(
		char *transaction_date_time,
		char *account_name );

/* Usage */
/* ----- */
void journal_account_list_propagate(
		char *transaction_date_time,
		LIST *journal_extract_account_list );

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
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		LIST *journal_list );

/* Process */
/* ------- */
FILE *journal_insert_pipe(
		char *journal_insert,
		char *journal_table );

void journal_insert(
		FILE *pipe,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
void journal_list_transaction_insert(
		FILE *pipe,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		LIST *journal_list );

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
double journal_list_transaction_amount(
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
		char *transaction_date_time,
		char *account_name,
		LIST *journal_system_list );

/* Usage */
/* ----- */
LIST *journal_entity_list(
		const char *journal_select,
		const char *journal_table,
		char *full_name,
		char *street_address,
		char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_entity_where(
		char *full_name,
		char *street_address,
		char *account_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_primary_where(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name );

/* Public */
/* ------ */

LIST *journal_account_distinct_entity_list(
		char *journal_table,
		LIST *account_name_list );

LIST *journal_extract_account_list(
		LIST *journal_list );

char *journal_delete_system_string(
		char *journal_table,
		char *where );

int journal_transaction_count(
		const char *journal_table,
		char *account_name,
		char *transaction_begin_date_string,
		char *transaction_date_time_closing );

LIST *journal_date_time_account_name_list(
		char *journal_table,
		char *transaction_date_time );

char *journal_minimum_transaction_date_time(
		char *account_name );

double journal_signed_balance(
		double balance,
		boolean accumulate_debit );

LIST *journal_account_name_list(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

double journal_amount(
		double debit_amount,
		double credit_amount,
		boolean element_accumulate_debit );

typedef struct
{
	JOURNAL *journal_prior;
	char *prior_transaction_date_time;
	double prior_previous_balance;
	boolean accumulate_debit;
	LIST *journal_list;
	LIST *update_statement_list;
} JOURNAL_PROPAGATE;

/* Usage */
/* ----- */
JOURNAL_PROPAGATE *journal_propagate_new(
		char *transaction_date_time,
		char *account_name );

/* Process */
/* ------- */
JOURNAL_PROPAGATE *journal_propagate_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_propagate_prior_transaction_date_time(
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
double journal_propagate_prior_previous_balance(
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
boolean journal_propagate_accumulate_debit(
		char *account_name,
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
LIST *journal_propagate_journal_list(
		char *account_name,
		char *journal_propagate_prior_transaction_date_time,
		double journal_propagate_prior_previous_balance );

/* Usage */
/* ----- */
void journal_propagate_balance_set(
		LIST *journal_list /* in/out */,
		boolean accumulate_debit );

/* Usage */
/* ----- */
double journal_propagate_balance(
		boolean accumulate_debit,
		double previous_balance,
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
void journal_propagate_previous_balance_set(
		LIST *journal_list /* in/out */,
		double end_balance );

/* Usage */
/* ----- */
double journal_propagate_previous_balance(
		double debit_amount,
		double credit_amount,
		double balance );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_propagate_greater_equal_where(
		char *account_name,
		char *prior_transaction_date_time );

/* Usage */
/* ----- */
LIST *journal_propagate_update_statement_list(
		const char *journal_table,
		LIST *journal_propagate_journal_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_propagate_update_statement(
		const char *journal_table,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double previous_balance,
		double balance );

#endif
