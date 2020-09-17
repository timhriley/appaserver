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

/* Constants */
/* --------- */
#define JOURNAL_TABLE			"journal_ledger"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	int transaction_count;
	int transaction_count_database;
	double previous_balance;
	double previous_balance_database;
	double debit_amount;
	double credit_amount;
	double balance;
	double balance_database;
	boolean match_sum_taken;
	char *memo;
	int check_number;
	char *property_street_address;
} JOURNAL;

/* Operations */
/* ---------- */
JOURNAL *journal_new(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name );

JOURNAL *journal_prior(	char *transaction_date_time,
			char *account_name );

JOURNAL *journal_account_fetch(
			char *account_name,
			char *transaction_date_time );

/* Safely returns heap memory */
/* -------------------------- */
char *journal_select(	void );

JOURNAL	*journal_parse(	char *input );

boolean journal_accumulate_debit(
			char *account_name );

LIST *journal_system_list(
			char *sys_string );

FILE *journal_insert_open(
			void );

/* Executes journal_propagate() */
/* ---------------------------- */
void journal_insert(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double amount,
			boolean is_debit );

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

LIST *journal_list_minimum(
			char *minimum_transaction_date_time,
			char *account_name );

LIST *journal_list_account(
			char *account_name );

/* Returns account_name_list */
/* ------------------------- */
LIST *journal_delete(	char *full_name,
			char *street_address,
			char *transaction_date_time );

LIST *journal_account_name_list(
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
			LIST *journal_list );

void journal_list_propagate_update(
			LIST *propagate_journal_list );

JOURNAL *journal_latest(
			char *account_name,
			char *as_of_date );

LIST *journal_year_list(
			int year,
			char *account_name );

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
			char *as_of_date );

LIST *journal_minimum_account_journal_list(
			char *minimum_transaction_date_time,
			char *account_name );

JOURNAL *journal_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
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

char *journal_sys_string(
			char *where );

#endif
