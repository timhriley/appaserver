/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_journal.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCOUNT_JOURNAL_H
#define ACCOUNT_JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"

typedef struct
{
	char *full_name;
	char *contact_key;
	char *transaction_date_time;
	char *account_name;
	double previous_balance;
	double debit_amount;
	double credit_amount;
	double balance;
	TRANSACTION *transaction;
} ACCOUNT_JOURNAL;

/* Usage */
/* ----- */
ACCOUNT_JOURNAL *account_journal_latest(
		const char *journal_table,
		char *fund_name,
		char *account_name,
		char *end_date_time_string,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT_JOURNAL *account_journal_new(
		char *account_name,
		double balance );

/* Process */
/* ------- */
ACCOUNT_JOURNAL *account_journal_calloc(
		void );

#endif
