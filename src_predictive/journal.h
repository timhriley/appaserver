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
JOURNAL*journal_new(		char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name );

JOURNAL *journal_prior(		char *transaction_date_time,
				char *account_name );

FILE *journal_insert_pipe(	void );

void journal_insert(		FILE *insert_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name,
				double amount,
				boolean is_debit );

JOURNAL *journal_transaction_date_time_fetch(
				char *transaction_date_time,
				char *account_name );

#endif
