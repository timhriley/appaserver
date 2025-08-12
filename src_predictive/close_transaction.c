/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_transaction.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "element.h"
#include "subclassification.h"
#include "journal.h"
#include "close_transaction.h"

CLOSE_TRANSACTION *close_transaction_new(
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		LIST *equity_subclassification_statement_list,
		char *self_full_name,
		char *self_street_address,
		JOURNAL *close_journal )
{
	CLOSE_TRANSACTION *close_transaction;

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

	close_transaction = close_transaction_calloc();

	close_transaction->nominal_journal_list =
		close_transaction_nominal_journal_list(
			transaction_date_close_date_time,
			element_statement_list,
			self_full_name,
			self_street_address );

	close_transaction->equity_journal_list =
		close_transaction_subclassification_journal_list(
			transaction_date_close_date_time,
			equity_subclassification_statement_list,
			self_full_name,
			self_street_address,
			-1 /* element_accumulate_debit */ );

	close_transaction->journal_list =
		close_transaction_journal_list(
			close_transaction->nominal_journal_list,
			close_transaction->equity_journal_list,
			close_journal );

	if ( list_length( close_transaction->journal_list ) )
	{
		close_transaction->amount =
			close_transaction_amount(
				close_transaction->journal_list );

		close_transaction->transaction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_new(
				self_full_name,
				self_street_address,
				transaction_date_close_date_time );

		close_transaction->transaction->transaction_amount =
			close_transaction->amount;

		close_transaction->transaction->memo =
			TRANSACTION_CLOSE_MEMO;

		close_transaction->transaction->journal_list =
			close_transaction->journal_list;
	}

	return close_transaction;
}

CLOSE_TRANSACTION *close_transaction_calloc( void )
{
	CLOSE_TRANSACTION *close_transaction;

	if ( ! ( close_transaction =
			calloc( 1,
				sizeof ( CLOSE_TRANSACTION ) ) ) )
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

	return close_transaction;
}

LIST *close_transaction_journal_list(
		LIST *nominal_journal_list,
		LIST *equity_journal_list,
		JOURNAL *close_journal )
{
	LIST *journal_list = list_new();
	JOURNAL *journal;

	if ( list_rewind( nominal_journal_list ) )
	do {
		journal = list_get( nominal_journal_list );
		list_set( journal_list, journal );

	} while ( list_next( nominal_journal_list ) );

	if ( list_rewind( equity_journal_list ) )
	do {
		journal = list_get( equity_journal_list );
		list_set( journal_list, journal );

	} while ( list_next( equity_journal_list ) );

	if ( close_journal ) list_set( journal_list, close_journal );

	if ( !list_length( journal_list ) )
	{
		list_free( journal_list );
		journal_list = NULL;
	}

	return journal_list;
}

double close_transaction_amount( LIST *journal_list )
{
	return
	journal_debit_sum( journal_list );
}

LIST *close_transaction_nominal_journal_list(
		char *transaction_date_close_date_time,
		LIST *element_statement_list,
		char *self_full_name,
		char *self_street_address )
{
	LIST *journal_list = list_new();
	ELEMENT *element;

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

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		list_set_list(
			journal_list,
			close_transaction_subclassification_journal_list(
				transaction_date_close_date_time,
				element->subclassification_statement_list,
				self_full_name,
				self_street_address,
				element->accumulate_debit ) );

	} while ( list_next( element_statement_list ) );

	if ( !list_length( journal_list ) )
	{
		list_free( journal_list );
		journal_list = NULL;
	}

	return journal_list;
}

LIST *close_transaction_subclassification_journal_list(
		char *transaction_date_close_date_time,
		LIST *subclassification_list,
		char *self_full_name,
		char *self_street_address,
		boolean element_accumulate_debit )
{
	LIST *journal_list = list_new();
	SUBCLASSIFICATION *subclassification;

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

	if ( list_rewind( subclassification_list ) )
	do {
		subclassification = list_get( subclassification_list );

		list_set_list(
			journal_list,
			close_transaction_account_journal_list(
				transaction_date_close_date_time,
				subclassification->account_statement_list,
				self_full_name,
				self_street_address,
				subclassification_accumulate_debit(
					element_accumulate_debit,
					subclassification->element ) ) );

	} while ( list_next( subclassification_list ) );

	if ( !list_length( journal_list ) )
	{
		list_free( journal_list );
		journal_list = NULL;
	}

	return journal_list;
}

LIST *close_transaction_account_journal_list(
		char *transaction_date_close_date_time,
		LIST *account_list,
		char *self_full_name,
		char *self_street_address,
		boolean element_accumulate_debit )
{
	LIST *journal_list = list_new();
	ACCOUNT *account;
	JOURNAL *journal;

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


	if ( list_rewind( account_list ) )
	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest->balance ) continue;

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_new(
				self_full_name,
				self_street_address,
				transaction_date_close_date_time,
				account->account_name );

		journal->account = account;

		journal->debit_amount =
			close_transaction_debit_amount(
				account->
					account_journal_latest->
					balance,
				element_accumulate_debit );

		journal->credit_amount =
			close_transaction_credit_amount(
				account->
					account_journal_latest->
					balance,
				element_accumulate_debit );

		list_set( journal_list, journal );

	} while ( list_next( account_list ) );

	if ( !list_length( journal_list ) )
	{
		list_free( journal_list );
		journal_list = NULL;
	}

	return journal_list;
}

double close_transaction_debit_amount(
		double balance,
		boolean accumulate_debit )
{
	double debit_amount;

	/* If accumulate_credit */
	/* -------------------- */
	if ( !accumulate_debit )
	{
		if ( balance < 0.0 )
			debit_amount = 0.0;
		else
			debit_amount = balance;
	}
	else
	/* If accumulate_debit */
	/* ------------------- */
	{
		if ( balance < 0.0 )
			debit_amount = -balance;
		else
			debit_amount = 0.0;
	}

	return debit_amount;
}


double close_transaction_credit_amount(
		double balance,
		boolean accumulate_debit )
{
	double credit_amount;

	if ( accumulate_debit )
	{
		if ( balance < 0.0 )
			credit_amount = 0.0;
		else
			credit_amount = balance;
	}
	else
	{
		if ( balance < 0.0 )
			credit_amount = -balance;
		else
			credit_amount = 0.0;
	}

	return credit_amount;
}

