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
#include "predictive.h"

#define JOURNAL_TABLE			"journal_ledger"
#define JOURNAL_TRANSACTION_TABLE	"transaction"

#define JOURNAL_SELECT			"full_name,"		\
					"street_address,"	\
					"transaction_date_time,"\
					"account,"		\
					"previous_balance,"	\
					"debit_amount,"		\
					"credit_amount,"	\
					"balance,"		\
					"transaction_count"

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

/* Returns static memory */
/* --------------------- */
char *journal_transaction_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

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
LIST *journal_account_list(
			char *account_name );


/* Returns static memory */
/* --------------------- */
char *journal_account_where(
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

/* Private */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *journal_system_string(
			char *journal_select,
			char *journal_table,
			char *where );

LIST *journal_system_list(
			char *journal_system_string,
			boolean fetch_check_number,
			boolean fetch_memo );

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

/* Returns heap memory */
/* ------------------- */
char *journal_transaction_system_string(
			char *attribute_name,
			char *journal_transaction_table,
			char *journal_transaction_where );

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

/* Public */
/* ------ */
char *journal_latest_zero_balance_transaction_date_time(
			char *account_name,
			char *journal_table );

boolean journal_accumulate_debit(
			char *account_name );

FILE *journal_insert_open(
			boolean replace );

/* Executes journal_propagate() */
/* ---------------------------- */
void journal_insert(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double amount,
			boolean is_debit,
			boolean replace );

void journal_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double amount,
			boolean is_debit );

/* Executes journal_list_set_balances() */
/* ------------------------------------ */
void journal_propagate(
			char *transaction_date_time,
			char *account_name );

LIST *journal_list_prior(
			JOURNAL *prior_journal,
			char *account_name );

LIST *journal_list_set_balances(
			LIST *journal_list,
			boolean accumulate_debit );

/* Returns account_name_list */
/* ------------------------- */
LIST *journal_delete(	char *full_name,
			char *street_address,
			char *transaction_date_time );
LIST *journal_ledger_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

void journal_account_name_list_propagate(
			char *transaction_date_time,
			LIST *account_name_list );

void journal_list_transaction_date_time_propagate(
			char *transaction_date_time,
			LIST *journal_list );

char *journal_list_audit(
			LIST *journal_list );

JOURNAL *journal_account_name_getset(
			LIST *journal_list,
			char *account_name );

LIST *journal_binary_journal_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account );

/* Returns account_name_list */
/* ------------------------- */
LIST *journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list,
			boolean replace );

void journal_list_propagate_update(
			LIST *propagate_journal_list );

JOURNAL *journal_latest(
			char *account_name,
			char *date_time );

double journal_amount(
			double debit_amount,
			double credit_amount,
			boolean accumulate_debit );

JOURNAL *journal_account_name_seek(
			LIST *journal_list,
			char *account_name );

JOURNAL *journal_check_number_seek(
			LIST *journal_list,
			int check_number );

JOURNAL *journal_transaction_date_time_seek(
			LIST *journal_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

void journal_list_text_display(
			char *transaction_memo,
			LIST *journal_list );

void journal_list_html_display(
			LIST *journal_list,
			char *transaction_memo );

char *journal_list_display(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			LIST *journal_list );

void journal_list_pipe_display(
			FILE *output_pipe,
			char *transaction_memo,
			char *heading,
			LIST *journal_list );

/* Safely returns heap memory */
/* -------------------------- */
char *journal_update_sys_string(
			void );

JOURNAL *journal_account_latest(
			char *account_name,
			char *transaction_date_time_closing );

LIST *journal_minimum_account_journal_list(
			char *minimum_transaction_date_time,
			char *account_name );

LIST *journal_binary_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account );

LIST *journal_binary_journal_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account );

void journal_list_stdout(
			LIST *journal_list );

void journal_list_update(
			LIST *journal_list );

char *journal_display(
			char *account_name,
			double previous_balance,
			double debit_amount,
			double credit_amount,
			double balance );

LIST *journal_list_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list );

LIST *journal_account_name_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

JOURNAL *journal_merchant_fees_expense(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *account_fees_expense );

double journal_transaction_amount(
			LIST *journal_list );

double journal_list_debit_sum(
			LIST *journal_list );

double journal_list_credit_sum(
			LIST *journal_list );

/* Returns account_name_list */
/* ------------------------- */
LIST *journal_list_account_name_list(
			LIST *account_name_list,
			LIST *journal_list );

LIST *journal_date_time_account_name_list(
			char *transaction_date_time );

LIST *journal_entity_account_journal_list(
			char *account_name,
			char *full_name,
			char *street_address );

double journal_debit_difference_sum(
			LIST *journal_list );

double journal_credit_difference_sum(
			LIST *journal_list );

#endif
