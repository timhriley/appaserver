/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equity_journal.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EQUITY_JOURNAL_H
#define EQUITY_JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

#define EQUITY_JOURNAL_SELECT		"distinct equity_account"

typedef struct
{
	char *equity_account_name;
	JOURNAL *journal;
} EQUITY_JOURNAL;

/* Usage */
/* ----- */
LIST *equity_journal_list(
		const char *close_equity_table,
		const char *equity_journal_select
			/* probably "distinct equity_account" */,
		const char *close_equity_primary_key,
		const char *account_closing_key,
		const char *account_equity_key );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *equity_journal_system_string(
		const char *select,
		const char *table );

/* Usage */
/* ----- */
EQUITY_JOURNAL *equity_journal_parse(
		char *input /* stack memory */ );

/* Usage */
/* ----- */
EQUITY_JOURNAL *equity_journal_new(
		char *equity_account_name );

/* Process */
/* ------- */
EQUITY_JOURNAL *equity_journal_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EQUITY_JOURNAL *equity_journal_seek(
		LIST *equity_journal_list,
		char *equity_account_name );

/* Usage */
/* ----- */
LIST *equity_journal_extract_list(
		LIST *equity_journal_list );

/* Usage */
/* ----- */
void equity_journal_list_debit_credit_set(
		LIST *equity_journal_extract_list /* in/out */ );

/* Usage */
/* ----- */
void equity_journal_debit_credit_set(
		JOURNAL *journal /* in/out */ );

/* Process */
/* ------- */
double equity_journal_difference(
		double debit_amount,
		double credit_amount );

#endif
