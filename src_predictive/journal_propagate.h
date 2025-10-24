/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/journal_propagate.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef JOURNAL_PROPAGATE_H
#define JOURNAL_PROPAGATE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"
#include "journal.h"

typedef struct
{
	JOURNAL *journal_prior;
	char *prior_transaction_date_time;
	double prior_previous_balance;
	boolean accumulate_debit;
	LIST *journal_list;
	LIST *update_statement_list;
} JOURNAL_PROPAGATE;

/* Usage */
/* ----- */
JOURNAL_PROPAGATE *journal_propagate_new(
		char *fund_name,
		char *transaction_date_time,
		char *account_name );

/* Process */
/* ------- */
JOURNAL_PROPAGATE *journal_propagate_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *journal_propagate_prior_transaction_date_time(
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
double journal_propagate_prior_previous_balance(
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
boolean journal_propagate_accumulate_debit(
		char *account_name,
		JOURNAL *journal_prior );

/* Usage */
/* ----- */
LIST *journal_propagate_journal_list(
		char *fund_name,
		char *account_name,
		char *journal_propagate_prior_transaction_date_time,
		double journal_propagate_prior_previous_balance );

/* Usage */
/* ----- */
void journal_propagate_balance_set(
		LIST *journal_list /* in/out */,
		boolean accumulate_debit );

/* Usage */
/* ----- */
double journal_propagate_balance(
		boolean accumulate_debit,
		double previous_balance,
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
void journal_propagate_previous_balance_set(
		LIST *journal_list /* in/out */,
		double end_balance );

/* Usage */
/* ----- */
double journal_propagate_previous_balance(
		double debit_amount,
		double credit_amount,
		double balance );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *journal_propagate_greater_equal_where(
		char *fund_name,
		char *account_name,
		char *prior_transaction_date_time );

/* Usage */
/* ----- */
LIST *journal_propagate_update_statement_list(
		const char *journal_table,
		LIST *journal_propagate_journal_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *journal_propagate_update_statement(
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double previous_balance,
		double balance );

/* Usage */
/* ----- */
void journal_propagate_account_list(
		char *fund_name,
		char *transaction_date_time,
		LIST *journal_extract_account_list );

#endif
