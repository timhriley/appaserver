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
#define JOURNAL_FOLDER_NAME	"journal_ledger"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	int transaction_count;
	double previous_balance;
	double debit_amount;
	double credit_amount;
	double balance;
	boolean match_sum_taken;
} JOURNAL;

/* Operations */
/* ---------- */
JOURNAL*journal_new(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name );

char *journal_select(	void );

JOURNAL	*journal_parse(	char *input );

LIST *journal_list(	char *where_clause );

JOURNAL *journal_prior(	char *transaction_date_time,
			char *account_name );

FILE *journal_insert_pipe(
			void );

void journal_insert(	FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double amount,
			boolean is_debit );

JOURNAL *journal_account_name_fetch(
			char *account_name,
			char *transaction_date_time );

LIST *journal_account_name_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Also does a propagate for each account */
/* -------------------------------------- */
void journal_delete(	char *full_name,
			char *street_address,
			char *transaction_date_time );

void journal_account_name_propagate(
			char *transaction_date_time,
			char *account_name );

void journal_account_name_list_propagate(
			char *transaction_date_time,
			LIST *account_name_list );

LIST *journal_prior_propagate_journal_list(
			JOURNAL *prior,
			char *account_name );

LIST *journal_minimum_transaction_date_time_list(
			char *minimum_transaction_date_time,
			char *account_name );

void journal_list_transaction_date_time_propagate(
			char *transaction_date_time,
			LIST *journal_list );

void journal_list_propagate(
			LIST *journal_list,
			boolean accumulate_debit );

#endif
