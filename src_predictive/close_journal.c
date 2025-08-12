/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_journal.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "float.h"
#include "account.h"
#include "element.h"
#include "subclassification.h"
#include "close_journal.h"

CLOSE_JOURNAL *close_journal_new(
		const char *account_closing_key,
		const char *account_equity_key,
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		LIST *equity_subclassification_statement_list,
		char *self_full_name,
		char *self_street_address )
{
	CLOSE_JOURNAL *close_journal;

	if ( !transaction_date_close_date_time
	||   !self_full_name
	||   !self_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_journal = close_journal_calloc();

	close_journal->account_closing_entry_string =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_closing_entry_string(
			account_closing_key,
			account_equity_key,
			__FUNCTION__ );

	close_journal->journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_new(
			self_full_name,
			self_street_address,
			transaction_date_close_date_time,
			close_journal->account_closing_entry_string
				/* account_name */ );

	close_journal->element_list_debit_sum =
		element_list_debit_sum(
			element_statement_list
				/* element_list */ );

	close_journal->element_list_credit_sum =
		element_list_credit_sum(
			element_statement_list
				/* element_list */ );

	close_journal->subclassification_list_debit_sum =
		subclassification_list_debit_sum(
			equity_subclassification_statement_list,
			-1 /* element_accumulate_debit */ );

	close_journal->subclassification_list_credit_sum =
		subclassification_list_credit_sum(
			equity_subclassification_statement_list,
			- 1 /* element_accumulate_debit */ );

	close_journal->amount =
		close_journal_amount(
			close_journal->
				element_list_debit_sum,
			close_journal->
				element_list_credit_sum,
			close_journal->
				subclassification_list_debit_sum,
			close_journal->
				subclassification_list_credit_sum );

	close_journal->debit_amount =
		close_journal_debit_amount(
			close_journal->amount );

	close_journal->journal->debit_amount =
		close_journal->debit_amount;

	close_journal->credit_amount =
		close_journal_credit_amount(
			close_journal->amount );

	close_journal->journal->credit_amount =
		close_journal->credit_amount;

	if (	close_journal->journal->debit_amount == 0.0
	&&	close_journal->journal->credit_amount == 0.0 )
	{
		close_journal->journal = NULL;
	}

	return close_journal;
}

CLOSE_JOURNAL *close_journal_calloc( void )
{
	CLOSE_JOURNAL *close_journal;

	if ( ! ( close_journal =
			calloc(	1,
				sizeof ( CLOSE_JOURNAL ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return close_journal;
}

double close_journal_amount(
		double element_list_debit_sum,
		double element_list_credit_sum,
		double subclassification_list_debit_sum,
		double subclassification_list_credit_sum )
{
	double debit_sum;
	double credit_sum;

	debit_sum =
		element_list_debit_sum +
		subclassification_list_debit_sum;

	credit_sum =
		element_list_credit_sum +
		subclassification_list_credit_sum;

	return credit_sum - debit_sum;
}

double close_journal_debit_amount(
		double close_journal_amount )
{
	if ( float_virtually_same(
		close_journal_amount,
		0.0 ) )
	{
		return 0.0;
	}

	if ( close_journal_amount < 0.0 )
		return -close_journal_amount;
	else
		return 0.0;
}

double close_journal_credit_amount(
		double close_journal_amount )
{
	if ( float_virtually_same(
		close_journal_amount,
		0.0 ) )
	{
		return 0.0;
	}

	if ( close_journal_amount < 0.0 )
		return 0.0;
	else
		return close_journal_amount;
}

