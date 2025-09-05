/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_ACCOUNT_H
#define CLOSE_ACCOUNT_H

#include "list.h"
#include "boolean.h"
#include "account.h"
#include "close_equity.h"
#include "equity_journal.h"

typedef struct
{
	char *account_name;
	ACCOUNT *account;
	CLOSE_EQUITY *close_equity;
	EQUITY_JOURNAL *equity_journal;
} CLOSE_ACCOUNT;

/* Usage */
/* ----- */
LIST *close_account_list(
		LIST *subclassification_list,
		LIST *equity_journal_list,
		LIST *close_equity_list,
		LIST *close_transaction_nominal_journal_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_ACCOUNT *close_account_new(
		LIST *equity_journal_list,
		LIST *close_equity_list,
		char *account_name,
		ACCOUNT *account );

/* Process */
/* ------- */
CLOSE_ACCOUNT *close_account_calloc(
		void );

/* Usage */
/* ----- */
CLOSE_ACCOUNT *close_account_seek(
		LIST *close_account_list,
		char *account_name );

/* Usage */
/* ----- */
void close_account_list_journal_accumulate(
		LIST *close_account_list /* in/out */,
		LIST *close_transaction_nominal_journal_list,
		LIST *close_transaction_subclassification_journal_list );

/* Process */
/* ------- */

/* Returns equity_journal or a component of close_equity */
/* ----------------------------------------------------- */
EQUITY_JOURNAL *close_account_equity_journal(
		CLOSE_EQUITY *close_equity,
		EQUITY_JOURNAL *equity_journal );

/* Usage */
/* ----- */
void close_account_journal_accumulate(
		double journal_balance,
		CLOSE_ACCOUNT *close_account /* in/out */ );

/* Usage */
/* ----- */
double close_account_debit_amount(
		double balance,
		boolean accumulate_debit,
		double close_account_debit_amount );

/* Usage */
/* ----- */
double close_account_credit_amount(
		double balance,
		boolean accumulate_debit,
		double close_account_credit_amount );

#endif
