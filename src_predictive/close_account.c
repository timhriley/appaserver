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
#include "appaserver.h"
#include "subclassification.h"
#include "close_account.h"

LIST *close_account_list(
		LIST *subclassification_list,
		LIST *equity_journal_list,
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
					equity_journal_list,
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
				equity_journal_list,
				close_equity_list,
				journal->account->account_name,
				journal->account );

		list_set( list, close_account );

	} while ( list_next( nominal_journal_list ) );

	return list;
}

CLOSE_ACCOUNT *close_account_new(
		LIST *equity_journal_list,
		LIST *close_equity_list,
		char *account_name,
		ACCOUNT *account )
{
	CLOSE_ACCOUNT *close_account;

	if ( !list_length( equity_journal_list )
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

	if ( ( close_account->close_equity =
			close_equity_seek(
				close_equity_list,
				account_name ) ) )
	{
		return close_account;
	}

	close_account->equity_journal = list_first( equity_journal_list );

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

	if ( list_rewind( close_account_list ) )
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

void close_account_list_journal_accumulate(
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

		close_account_journal_accumulate(
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

		close_account_journal_accumulate(
			journal->balance,
			close_account /* in/out */ );

	} while ( list_next( subclassification_journal_list ) );
}

void close_account_journal_accumulate(
		double journal_balance,
		CLOSE_ACCOUNT *close_account )
{
	double debit_amount;
	double credit_amount;
	EQUITY_JOURNAL *equity_journal;

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

	equity_journal =
		/* ----------------------------------------------------- */
		/* Returns equity_journal or a component of close_equity */
		/* ----------------------------------------------------- */
		close_account_equity_journal(
			close_account->close_equity,
			close_account->equity_journal );

	debit_amount =
		close_account_debit_amount(
			journal_balance,
			close_account->
				account->
				subclassification->
				element->
				accumulate_debit,
			equity_journal->
				journal->
				debit_amount );			

	equity_journal->
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
			equity_journal->
				journal->
				credit_amount );			

	equity_journal->
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

EQUITY_JOURNAL *close_account_equity_journal(
		CLOSE_EQUITY *close_equity,
		EQUITY_JOURNAL *equity_journal )
{
	if ( !close_equity
	&&   !equity_journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"both close_equity and equity_journal are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( close_equity
	&&   equity_journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"both close_equity and equity_journal are populated." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( close_equity && !close_equity->equity_journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_equity->equity_journal is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( close_equity )
		return close_equity->equity_journal;
	else
		return equity_journal;
}

void close_account_list_display( LIST *close_account_list )
{
	char *system_string;
	FILE *output_pipe;
	CLOSE_ACCOUNT *close_account;

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		close_account_html_system_string(
			CLOSE_ACCOUNT_HEADING,
			CLOSE_ACCOUNT_JUSTIFY );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	if ( list_rewind( close_account_list ) )
	do {
		close_account = list_get( close_account_list );

		if ( close_account->close_equity )
		{
			close_account_display(
				output_pipe,
				close_account->account_name,
				close_account->
					close_equity->
					equity_journal->
					equity_account_name,
				close_account->
					account->
					account_journal_latest->
					balance,
				close_account->
					close_equity->
					reverse_boolean );
		}

	} while ( list_next( close_account_list ) );

	pclose( output_pipe );
}

char *close_account_html_system_string(
		const char *close_account_heading,
		const char *close_account_justify )
{
	static char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"html_table.e 'Close Equity' %s '^' %s",
		close_account_heading,
		close_account_justify );

	return system_string;
}

void close_account_display(
		FILE *appaserver_output_pipe,
		char *account_name,
		char *equity_account_name,
		double latest_balance,
		boolean reverse_boolean )
{
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !appaserver_output_pipe
	||   !account_name
	||   !equity_account_name
	||   !latest_balance )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		if ( appaserver_output_pipe )
			pclose( appaserver_output_pipe );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(
		appaserver_output_pipe,
		"%s^%s^%s^%s\n",
		string_initial_capital( buffer1, account_name ),
		string_initial_capital( buffer2, equity_account_name ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money( latest_balance ),
		(reverse_boolean) ? "Yes" : "No" );
}
