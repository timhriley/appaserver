/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_journal.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_JOURNAL_H
#define CLOSE_JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

typedef struct
{
	char *account_closing_entry_string;
	JOURNAL *journal;
	double element_list_debit_sum;
	double element_list_credit_sum;
	double subclassification_list_debit_sum;
	double subclassification_list_credit_sum;
	double amount;
	double debit_amount;
	double credit_amount;
} CLOSE_JOURNAL;

/* Safely returns */
/* -------------- */
CLOSE_JOURNAL *close_journal_new(
		const char *account_closing_key,
		const char *account_equity_key,
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		LIST *equity_subclassification_statement_list,
		char *self_full_name,
		char *self_street_address );

/* Process */
/* ------- */
CLOSE_JOURNAL *close_journal_calloc(
		void );

double close_journal_amount(
		double element_list_debit_sum,
		double element_list_credit_sum,
		double subclassification_list_debit_sum,
		double subclassification_list_credit_sum );

/* Usage */
/* ----- */
double close_journal_debit_amount(
		double close_journal_amount );

/* Usage */
/* ----- */
double close_journal_credit_amount(
		double close_journal_amount );

#endif
