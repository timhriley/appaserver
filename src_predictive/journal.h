/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/journal.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef JOURNAL_H
#define JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"

#define JOURNAL_TABLE			"journal_ledger"

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

/* Process */
/* ------- */
JOURNAL *journal_new(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name );

JOURNAL *journal_calloc(
			void );

/* Usage */
/* ----- */
JOURNAL *journal_prior(	char *transaction_date_time,
			char *account_name,
			boolean fetch_account,
			boolean fetch_subclassification,
			boolean fetch_element );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_max_prior_where(
			char *transaction_date_time,
			char *account_name );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_max_prior_transaction_date_time(
			char *journal_max_prior_where,
			char *journal_table );

/* Usage */
/* ----- */
JOURNAL *journal_latest(
			char *account_name,
			char *transaction_date_time_closing,
			boolean fetch_transaction );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_less_equal_where(
			char *transaction_date_time_closing,
			char *account_name );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_max_transaction_date_time(
			char *journal_less_equal_where,
			char *journal_table );

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
			char *transaction_preclose_time );

/* Usage */
/* ----- */
LIST *journal_minimum_list(
			char *minimum_transaction_date_time,
			char *account_name,
			double journal_prior_previous_balance );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_greater_equal_where(
			char *minimum_transaction_date_time,
			char *account_name );

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

/* Process */
/* ------- */

/* Usage */
/* ----- */
void journal_account_list_getset(
			LIST *account_list /* in/out */,
			LIST *journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void journal_propagate(	char *transaction_date_time,
			char *account_name );

/* Process */
/* ------- */
LIST *journal_list_balance_set(
			LIST *journal_list_prior /* in/out */,
			boolean element_accumulate_debit );

/* Usage */
/* ----- */
void journal_list_update(
			LIST *journal_list_balance_set );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *journal_update_system_string(
			char *journal_table );

FILE *journal_update_pipe(
			char *journal_update_system_string );

/* Usage */
/* ----- */
LIST *journal_list_prior(
			JOURNAL *journal_prior,
			char *account_name );

/* Process */
/* ------- */

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

/* Process */
/* ------- */

/* Usage */
/* ----- */
void journal_list_html_display(
			LIST *journal_list,
			char *transaction_date_time,
			char *transaction_memo );

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
double journal_debit_sum(
			LIST *journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double journal_list_transaction_amount(
			LIST *journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double journal_account_list_debit_sum(
			LIST *journal_list,
			LIST *account_name_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double journal_account_list_credit_sum(
			LIST *journal_list,
			LIST *account_name_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double journal_prior_account_end_balance(
			char *feeder_load_file_minimum_date,
			char *account_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double journal_balance_sum(
			LIST *journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *journal_tax_form_list(
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *transaction_preclose_time,
			char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_tax_form_where(
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *transaction_preclose_time,
			char *account_name );

/* Usage */
/* ----- */
JOURNAL *journal_debit_new(
			char *debit_account_name,
			double debit_amount );

/* Process */
/* ------- */

/* Usage */
/* ----- */
JOURNAL *journal_credit_new(
			char *credit_account_name,
			double credit_amount );

/* Process */
/* ------- */

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *journal_system_string(
			char *journal_select,
			char *journal_table,
			char *where );

double journal_credit_debit_difference_sum(
			LIST *journal_list );

double journal_debit_credit_difference_sum(
			LIST *journal_list );

LIST *journal_account_distinct_entity_list(
			char *journal_table,
			LIST *account_name_list );

LIST *journal_extract_account_list(
			LIST *journal_list );

char *journal_delete_system_string(
			char *journal_table,
			char *where );

LIST *journal_binary_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			ACCOUNT *debit_account,
			ACCOUNT *credit_account );

int journal_transaction_count(
			char *journal_table,
			char *account_name,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing );

LIST *journal_date_time_account_name_list(
			char *journal_table,
			char *transaction_date_time );

JOURNAL *journal_seek(
			char *transaction_date_time,
			char *account_name,
			LIST *journal_system_list );

char *journal_minimum_transaction_date_time(
			char *account_name );

double journal_balance(
			double balance,
			boolean accumulate_debit );

LIST *journal_account_name_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

double journal_amount(	double debit_amount,
			double credit_amount,
			boolean element_accumulate_debit );

#endif
