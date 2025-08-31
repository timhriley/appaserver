/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_equity.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_EQUITY_H
#define CLOSE_EQUITY_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

#define CLOSE_EQUITY_TABLE		"close_equity"
#define CLOSE_EQUITY_SELECT		"account,"		\
					"equity_account,"	\
					"reverse_yn"

#define CLOSE_EQUITY_PRIMARY_KEY	"account"

typedef struct
{
	char *account_name;
	char *equity_account_name;
	boolean reverse_boolean;
	JOURNAL *journal;
} CLOSE_EQUITY;

/* Usage */
/* ----- */
LIST *close_equity_list(
		const char *close_equity_table,
		const char *close_equity_select,
		const char *close_equity_primary_key,
		const char *account_closing_key,
		const char *account_equity_key );

/* Usage */
/* ----- */
CLOSE_EQUITY *close_equity_parse(
		char *input );

/* Usage */
/* ----- */
CLOSE_EQUITY *close_equity_new(
		char *account_name,
		char *equity_account_name,
		boolean reverse_boolean );

/* Process */
/* ------- */
CLOSE_EQUITY *close_equity_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_EQUITY *close_equity_seek(
		LIST *close_equity_list,
		char *account_name );

/* Usage */
/* ----- */
double close_equity_debit_amount(
		double debit_sum,
		double credit_sum );

/* Usage */
/* ----- */
double close_equity_credit_amount(
		double debit_sum,
		double credit_sum );

/* Usage */
/* ----- */
LIST *close_equity_journal_list(
		LIST *close_equity_list );


/* Usage */
/* ----- */
void close_equity_journal_list_debit_credit_set(
		LIST *close_equity_journal_list /* in/out */ );

/* Usage */
/* ----- */
void close_equity_journal_debit_credit_set(
		JOURNAL *journal /* in/out */ );

/* Process */
/* ------- */
double close_equity_difference(
		double debit_amount,
		double credit_amount );

#endif
