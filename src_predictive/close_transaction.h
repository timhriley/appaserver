/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_transaction.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_TRANSACTION_H
#define CLOSE_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "journal.h"
#include "transaction.h"

typedef struct
{
	LIST *nominal_journal_list;
	LIST *equity_journal_list;
	LIST *journal_list;
	double amount;
	TRANSACTION *transaction;
} CLOSE_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_TRANSACTION *close_transaction_new(
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		LIST *equity_subclassification_statement_list,
		char *self_full_name,
		char *self_street_address,
		JOURNAL *close_journal );

/* Process */
/* ------- */
CLOSE_TRANSACTION *close_transaction_calloc(
		void );

LIST *close_transaction_journal_list(
		LIST *close_transaction_nominal_journal_list,
		LIST *equity_journal_list,
		JOURNAL *close_journal );

/* Usage */
/* ----- */
LIST *close_transaction_nominal_journal_list(
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		char *self_full_name,
		char *self_street_address );

/* Usage */
/* ----- */
LIST *close_transaction_subclassification_journal_list(
		char *transaction_date_close_date_time,
		LIST *subclassification_list,
		char *self_full_name,
		char *self_street_address,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
LIST *close_transaction_account_journal_list(
		char *transaction_date_close_date_time,
		LIST *account_list,
		char *self_full_name,
		char *self_street_address,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
double close_transaction_debit_amount(
		double balance,
		boolean element_accumulate_debit );


/* Usage */
/* ----- */
double close_transaction_credit_amount(
		double balance,
		boolean element_accumulate_debit );

/* Usage */
/* ----- */
double close_transaction_amount(
		LIST *close_transaction_journal_list );

#endif

