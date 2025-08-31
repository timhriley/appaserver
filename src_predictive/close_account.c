/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_account.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "subclassification.h"
#include "close_account.h"

LIST *close_account_list(
		LIST *subclassification_list,
		LIST *close_equity_list,
		LIST *nominal_journal_list )
{
	LIST *list = list_new();
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	CLOSE_ACCOUNT *close_account;
	JOURNAL *journal;

	if ( list_rewind( subclassification_list ) )
	do {
		subclassification = list_get( subclassification_list );

		if ( list_rewind( subclassification->account_statement_list ) )
		do {
			account =
				list_get(
					subclassification->
						account_statement_list );

			close_account = 
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				close_account_new(
					close_equity_list,
					account->account_name,
					account );

			list_set( list, close_account );

		} while ( list_next( 
				subclassification->
					account_statement_list ) );

	} while ( list_next( subclassification_list ) );

	if ( list_rewind( nominal_journal_list ) )
	do {
		journal = list_get( nominal_journal_list );

		if ( !journal->account )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"journal->account is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		close_account = 
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_account_new(
				close_equity_list,
				journal->account->account_name,
				journal->account );

		list_set( list, close_account );

	} while ( list_next( nominal_journal_list ) );

	return list;
}

CLOSE_ACCOUNT *close_account_new(
		LIST *close_equity_list,
		char *account_name,
		ACCOUNT *account )
{
	CLOSE_ACCOUNT *close_account;

	if ( !list_length( close_equity_list )
	||   !account_name
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

	close_account = close_account_calloc();

	close_account->account_name = account_name;
	close_account->account = account;

	close_account->close_equity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		close_equity_seek(
			close_equity_list,
			account_name );

	return close_account;
}

CLOSE_ACCOUNT *close_account_calloc( void )
{
	CLOSE_ACCOUNT *close_account;

	if ( ! ( close_account = calloc( 1, sizeof ( CLOSE_ACCOUNT ) ) ) )
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

	return close_account;
}

CLOSE_ACCOUNT *close_account_seek(
		LIST *close_account_list,
		char *account_name )
{
	CLOSE_ACCOUNT *close_account;

	if ( !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( close_account_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_account_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		close_account = list_get( close_account_list );

		if ( !close_account->account_name )
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

		if ( strcmp(
			close_account->account_name,
			account_name ) == 0 )
		{
			return close_account;
		}

	} while ( list_next( close_account_list ) );

	return NULL;
}

void close_account_list_journal_set(
		LIST *close_account_list,
		LIST *nominal_journal_list,
		LIST *subclassification_journal_list )
{
	JOURNAL *journal;
	CLOSE_ACCOUNT *close_account;

	if ( list_rewind( nominal_journal_list ) )
	do {
		journal = list_get( nominal_journal_list );

		if ( ! ( close_account =
				close_account_seek(
					close_account_list,
					journal->account_name ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"close_account_seek(%s) returned empty.",
				journal->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		close_account_journal_set(
			journal->balance,
			close_account /* in/out */ );

	} while ( list_next( nominal_journal_list ) );

	if ( list_rewind( subclassification_journal_list ) )
	do {
		journal =
			list_get(
				subclassification_journal_list );

		if ( ! ( close_account =
				close_account_seek(
					close_account_list,
					journal->account_name ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"close_account_seek(%s) returned empty.",
				journal->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		close_account_journal_set(
			journal->balance,
			close_account /* in/out */ );

	} while ( list_next( subclassification_journal_list ) );
}

void close_account_journal_set(
		double journal_balance,
		CLOSE_ACCOUNT *close_account )
{
	double debit_amount;
	double credit_amount;

	if ( !journal_balance )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_balance is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_account is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_account->account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_account->account is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_account->account->subclassification
	||   !close_account->account->subclassification->element )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
	"close_account->account->subclassification is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_account->close_equity )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_account->close_equity is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_account->close_equity->journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_account->close_equity->journal is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	debit_amount =
		close_account_debit_amount(
			journal_balance,
			close_account->
				account->
				subclassification->
				element->
				accumulate_debit,
			close_account->
				close_equity->
				journal->
				debit_amount );			

	close_account->
		close_equity->
		journal->
		debit_amount =
			debit_amount;

	credit_amount =
		close_account_credit_amount(
			journal_balance,
			close_account->
				account->
				subclassification->
				element->
				accumulate_debit,
			close_account->
				close_equity->
				journal->
				credit_amount );			

	close_account->
		close_equity->
		journal->
		credit_amount =
			credit_amount;
}

double close_account_debit_amount(
		double balance,
		boolean accumulate_debit,
		double close_account_debit_amount )
{
	if ( !accumulate_debit ) return close_account_debit_amount;

	return
	balance + close_account_debit_amount;
}

double close_account_credit_amount(
		double balance,
		boolean accumulate_debit,
		double close_account_credit_amount )
{
	if ( accumulate_debit ) return close_account_credit_amount;

	return
	balance + close_account_credit_amount;
}

