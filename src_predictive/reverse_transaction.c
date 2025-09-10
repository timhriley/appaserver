/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reverse_transaction.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "close_equity.h"
#include "equity_journal.h"
#include "close_transaction.h"
#include "reverse_transaction.h"

REVERSE_TRANSACTION *reverse_transaction_new(
		char *transaction_date_reverse_date_time,
		LIST *close_transaction_journal_list,
		char *self_full_name,
		char *self_street_address )
{
	REVERSE_TRANSACTION *reverse_transaction;

	if ( !transaction_date_reverse_date_time
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

	reverse_transaction = reverse_transaction_calloc();

	reverse_transaction->equity_journal_list =
		equity_journal_list(
			CLOSE_EQUITY_TABLE,
			EQUITY_JOURNAL_SELECT
				/* probably "distinct equity_account" */,
			CLOSE_EQUITY_PRIMARY_KEY,
			ACCOUNT_CLOSING_KEY,
			ACCOUNT_EQUITY_KEY );

	reverse_transaction->close_equity_list =
		close_equity_list(
			CLOSE_EQUITY_TABLE,
			CLOSE_EQUITY_SELECT,
			CLOSE_EQUITY_PRIMARY_KEY,
			reverse_transaction->equity_journal_list );

	reverse_transaction->journal_list =
		reverse_transaction_journal_list(
			close_transaction_journal_list,
			reverse_transaction->close_equity_list );

	if ( list_length( reverse_transaction->journal_list ) )
	{
		reverse_transaction->extract_journal_list =
			reverse_transaction_extract_journal_list(
				reverse_transaction->equity_journal_list,
				reverse_transaction->journal_list );

		reverse_transaction->reverse_transaction_amount =
			close_transaction_amount(
				reverse_transaction->extract_journal_list );

		reverse_transaction->transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				self_full_name,
				self_street_address,
				transaction_date_reverse_date_time );

		reverse_transaction->transaction->transaction_amount =
			reverse_transaction->reverse_transaction_amount;

		reverse_transaction->transaction->memo =
			TRANSACTION_REVERSE_MEMO;

		reverse_transaction->transaction->journal_list =
			reverse_transaction->extract_journal_list;
	}

	return reverse_transaction;
}

REVERSE_TRANSACTION *reverse_transaction_calloc( void )
{
	REVERSE_TRANSACTION *reverse_transaction;

	if ( ! ( reverse_transaction =
			calloc( 1,
				sizeof ( REVERSE_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return reverse_transaction;
}

LIST *reverse_transaction_journal_list(
		LIST *close_transaction_journal_list,
		LIST *close_equity_list )
{
	JOURNAL *journal;
	LIST *list = list_new();

	if ( list_rewind( close_transaction_journal_list ) )
	do {
		journal = list_get( close_transaction_journal_list );

		journal =
			reverse_transaction_equity_list_journal(
				journal,
				close_equity_list /* in/out */ );

		if ( journal ) list_set( list, journal );

	} while ( list_next( close_transaction_journal_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

JOURNAL *reverse_transaction_equity_list_journal(
		JOURNAL *journal,
		LIST *close_equity_list )
{
	JOURNAL *transaction_journal = NULL;
	CLOSE_EQUITY *close_equity;

	if ( !journal
	||   !journal->account_name
	||   !journal->account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_equity =
		close_equity_seek(
			close_equity_list,
			journal->account_name );

	if (	close_equity
	&&	close_equity->reverse_boolean )
	{
		reverse_transaction_equity_accumulate(
			journal,
			close_equity /* in/out */ );

		transaction_journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			reverse_transaction_journal(
				journal->account_name,
				journal->account,
				journal->debit_amount,
				journal->credit_amount );
	}

	return transaction_journal;
}

double reverse_transaction_equity_debit_accumulate(
		double journal_credit_amount,
		double equity_debit_amount )
{
	double equity_debit_accumulate;

	if ( journal_credit_amount )
	{
		equity_debit_accumulate =
			equity_debit_amount +
			journal_credit_amount;
	}
	else
	{
		equity_debit_accumulate =
			equity_debit_amount;
	}

	return equity_debit_accumulate;
}

double reverse_transaction_equity_credit_accumulate(
		double journal_debit_amount,
		double equity_credit_amount )
{
	double equity_credit_accumulate;

	if ( journal_debit_amount )
	{
		equity_credit_accumulate =
			equity_credit_amount +
			journal_debit_amount;
	}
	else
	{
		equity_credit_accumulate =
			equity_credit_amount;
	}

	return equity_credit_accumulate;
}

JOURNAL *reverse_transaction_journal(
		char *account_name,
		ACCOUNT *account,
		double debit_amount,
		double credit_amount )
{
	JOURNAL *journal;

	if ( !account_name
	||   !account )
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

	journal = journal_calloc();

	journal->account_name = account_name;
	journal->account = account;
	journal->debit_amount = credit_amount;
	journal->credit_amount = debit_amount;

	return journal;
}

LIST *reverse_transaction_extract_journal_list(
		LIST *equity_journal_list,
		LIST *reverse_transaction_journal_list )
{
	return
	list_append_list(
		equity_journal_extract_list(
			equity_journal_list )
			/* destination_list */,
		reverse_transaction_journal_list
			/* source_list */ );
}

void reverse_transaction_equity_accumulate(
		JOURNAL *journal,
		CLOSE_EQUITY *close_equity )
{
	double accumulate;

	accumulate =
		reverse_transaction_equity_debit_accumulate(
			journal->credit_amount,
			close_equity->
				equity_journal->
				journal->
				debit_amount );

	close_equity->
		equity_journal->
		journal->
		debit_amount =
		accumulate;

	accumulate =
		reverse_transaction_equity_credit_accumulate(
			journal->debit_amount,
			close_equity->
				equity_journal->
				journal->
				debit_amount );

	close_equity->
		equity_journal->
		journal->
		credit_amount =
		accumulate;
}
