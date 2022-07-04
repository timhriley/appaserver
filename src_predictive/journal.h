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

#define JOURNAL_TABLE			"journal_ledger"

#define JOURNAL_SELECT			"full_name,"		\
					"street_address,"	\
					"transaction_date_time,"\
					"account,"		\
					"previous_balance,"	\
					"debit_amount,"		\
					"credit_amount,"	\
					"balance,"		\
					"transaction_count"

#define JOURNAL_INSERT			"full_name,"		\
					"street_address,"	\
					"transaction_date_time,"\
					"account,"		\
					"previous_balance,"	\
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
	int transaction_count;
	char *memo;
	int check_number;
	boolean match_sum_taken;
	double journal_amount;
} JOURNAL;

/* Usage */
/* ----- */
LIST *journal_year_list(
			int tax_year,
			char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_year_where(
			int tax_year,
			char *account_name,
			char *predictive_preclose_time );

/* Usage */
/* ----- */
LIST *journal_list(	char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *journal_minimum_list(
			char *minimum_transaction_date_time,
			char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_minimum_where(
			char *minimum_transaction_date_time,
			char *account_name );

/* Usage */
/* ----- */
LIST *journal_entity_list(
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
JOURNAL *journal_account_fetch(
			char *transaction_date_time,
			char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_transaction_account_where(
			char *transaction_date_time,
			char *account_name );

/* Usage */
/* ----- */
void journal_propagate(	char *transaction_date_time,
			char *account_name );

LIST *journal_list_set_balance(
			LIST *journal_list_prior /* in/out */,
			boolean account_accumulate_debit );

/* Usage */
/* ----- */
void journal_list_update(
			LIST *journal_list_set_balance );

/* Process */
/* ------- */
char *journal_update_system_string(
			char *journal_table );

/* Usage */
/* ----- */
void journal_account_name_list_propagate(
			char *transaction_date_time,
			LIST *account_name_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *journal_system_list(
			char *journal_system_string,
			boolean fetch_check_number,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Usage */
/* ----- */
JOURNAL	*journal_parse(
			char *input,
			boolean fetch_check_number,
			boolean fetch_memo );

JOURNAL *journal_new(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name );

/* Usage */
/* ----- */
char *journal_transaction_cell_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *attribute_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
JOURNAL *journal_prior(	char *transaction_date_time,
			char *account_name );

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
			char *transaction_date_time_closing,
			char *account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_max_where(
			char *transaction_date_time_closing,
			char *account_name );

/* Returns heap memory or null */
/* --------------------------- */
char *journal_max_transaction_date_time(
			char *journal_max_where,
			char *journal_table );

/* Usage */
/* ----- */
LIST *journal_account_journal_list(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *journal_account_where(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time );

/* Usage */
/* ----- */
LIST *journal_POR_list(	char *account_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *journal_following_balance_zero_list(
			char *account_name );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_following_latest_zero_balance_transaction_date_time(
			char *account_name,
			char *journal_table );

/* Usage */
/* ----- */
LIST *journal_list_prior(
			JOURNAL *journal_prior,
			char *account_name );

/* Process */
/* ------- */

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

/* Usage */
/* ----- */
void journal_list_insert(
			char *appaserver_error_filename,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list );

/* Process */
/* ------- */
FILE *journal_insert_open(
			char *appaserver_error_filename,
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
void journal_list_html_display(
			LIST *journal_list,
			char *transaction_memo );

/* Process */
/* ------- */
void journal_list_pipe_display(
			FILE *output_pipe,
			char *transaction_memo,
			char *heading,
			LIST *journal_list );

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
double journal_debit_sum(
			LIST *journal_list );

/* Process */
/* ------- */

/* Private */
/* ------- */
JOURNAL *journal_calloc(
			void );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *journal_system_string(
			char *journal_select,
			char *journal_table,
			char *where );

/* Returns heap memory */
/* ------------------- */
char *journal_transaction_system_string(
			char *attribute_name,
			char *journal_transaction_table,
			char *transaction_primary_where );

JOURNAL *journal_list_latest(
			LIST *journal_list );

double journal_credit_debit_difference_sum(
			LIST *journal_list );

double journal_debit_credit_difference_sum(
			LIST *journal_list );

LIST *journal_account_distinct_entity_list(
			char *journal_table,
			LIST *account_name_list );

LIST *journal_extract_account_name_list(
			LIST *journal_list );

LIST *journal_fetch_account_name_list(
			char *journal_table,
			char *where );

char *journal_delete_system_string(
			char *journal_table,
			char *where );

LIST *journal_binary_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account );

/* Returns heap memory */
/* ------------------- */
char *journal_list_raw_display(
			LIST *journal_list );

#endif
