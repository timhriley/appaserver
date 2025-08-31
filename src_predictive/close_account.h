/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_ACCOUNT_H
#define CLOSE_ACCOUNT_H

#include "list.h"
#include "boolean.h"
#include "journal.h"
#include "close_equity.h"

typedef struct
{
	char *account_name;
	ACCOUNT *account;
	CLOSE_EQUITY *close_equity;
} CLOSE_ACCOUNT;

/* Usage */
/* ----- */
LIST *close_account_list(
		LIST *subclassification_list,
		LIST *close_equity_list,
		LIST *close_transaction_nominal_journal_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_ACCOUNT *close_account_new(
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
void close_account_list_journal_set(
		LIST *close_account_list /* in/out */,
		LIST *close_transaction_nominal_journal_list,
		LIST *close_transaction_subclassification_journal_list);

/* Usage */
/* ----- */
void close_account_journal_set(
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
