/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reverse_transaction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef REVERSE_TRANSACTION_H
#define REVERSE_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"
#include "close_equity.h"
#include "journal.h"

typedef struct
{
	LIST *equity_journal_list;
	LIST *close_equity_list;
	LIST *journal_list;
	LIST *extract_journal_list;
	double reverse_transaction_amount;
	TRANSACTION *transaction;
} REVERSE_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
REVERSE_TRANSACTION *reverse_transaction_new(
		char *transaction_date_reverse_date_time,
		LIST *close_transaction_journal_list,
		char *self_full_name,
		char *self_street_address );

/* Process */
/* ------- */
REVERSE_TRANSACTION *reverse_transaction_calloc(
		void );

/* Usage */
/* ----- */
LIST *reverse_transaction_journal_list(
		LIST *close_transaction_journal_list,
		LIST *close_equity_list );

/* Usage */
/* ----- */
JOURNAL *reverse_transaction_equity_list_journal(
		JOURNAL *journal,
		LIST *close_equity_list /* in/out */ );

/* Usage */
/* ----- */
void reverse_transaction_equity_accumulate(
		JOURNAL *journal,
		CLOSE_EQUITY *close_equity /* in/out */ );

/* Usage */
/* ----- */
JOURNAL *reverse_transaction_equity_list_journal(
		JOURNAL *journal,
		LIST *close_equity_list /* in/out */ );

/* Usage */
/* ----- */
double reverse_transaction_equity_debit_accumulate(
		double journal_debit_amount,
		double equity_debit_amount );

/* Usage */
/* ----- */
double reverse_transaction_equity_credit_accumulate(
		double journal_credit_amount,
		double equity_credit_amount );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *reverse_transaction_journal(
		char *account_name,
		ACCOUNT *account,
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
LIST *reverse_transaction_extract_journal_list(
		LIST *equity_journal_list,
		LIST *reverse_transaction_journal_list );

#endif
