/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_equity.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "sql.h"
#include "float.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder.h"
#include "close_equity.h"

LIST *close_equity_list(
		const char *close_equity_table,
		const char *close_equity_select,
		const char *close_equity_primary_key,
		const char *account_closing_key,
		const char *account_equity_key )
{
	LIST *list = list_new();
	char *equity_account_name;
	CLOSE_EQUITY *close_equity;
	char input[ 1024 ];

	equity_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_closing_entry_string(
			account_closing_key,
			account_equity_key,
			__FUNCTION__ );

	if ( ! ( close_equity =
			close_equity_new(
				(char *)0 /* account_name */,
				equity_account_name,
				0 /* not reverse_boolean */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"close_equity_new(%s) returned empty.",
			equity_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	list_set( list, close_equity );

	if ( folder_column_boolean(
		(char *)close_equity_table,
		(char *)close_equity_primary_key ) )
	{
		FILE *pipe;

		pipe =
			appaserver_input_pipe(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				appaserver_system_string(
					(char *)close_equity_select,
					(char *)close_equity_table,
					(char *)0 /* where */ ) );

		while ( string_input( input, pipe, sizeof ( input ) ) )
		{
			if ( ! ( close_equity =
				    close_equity_parse(
					input ) ) )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"close_equity_parse() returned empty." );

				pclose( pipe );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			list_set( list, close_equity );
		}

		pclose( pipe );
	}

	return list;
}

CLOSE_EQUITY *close_equity_parse(
		char *input )
{
	char account_name[ 128 ];
	char equity_account_name[ 128 ];
	char reverse_yn[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See CLOSE_EQUITY_SELECT */
	/* ----------------------- */
	piece( account_name, SQL_DELIMITER, input, 0 );
	piece( equity_account_name, SQL_DELIMITER, input, 1 );
	piece( reverse_yn, SQL_DELIMITER, input, 2 );

	return
	close_equity_new(
		strdup( account_name ),
		strdup( equity_account_name ),
		(*reverse_yn == 'y'
		 	/* reverse_boolean */ ) );
}

CLOSE_EQUITY *close_equity_new(
		char *account_name,
		char *equity_account_name,
		boolean reverse_boolean )
{
	CLOSE_EQUITY *close_equity;

	if ( !equity_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"equity_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_equity = close_equity_calloc();

	close_equity->account_name = account_name;
	close_equity->equity_account_name = equity_account_name;
	close_equity->reverse_boolean = reverse_boolean;

	close_equity->journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			equity_account_name );

	if ( ! ( close_equity->journal->account =
			account_fetch(
				equity_account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		return NULL;
	}

	return close_equity;
}

CLOSE_EQUITY *close_equity_calloc( void )
{
	CLOSE_EQUITY *close_equity;

	if ( ! ( close_equity = calloc( 1, sizeof ( CLOSE_EQUITY ) ) ) )
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

	return close_equity;
}

CLOSE_EQUITY *close_equity_seek(
		LIST *close_equity_list,
		char *account_name )
{
	CLOSE_EQUITY *close_equity;

	if ( !list_length( close_equity_list )
	||   !account_name )
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

	if ( list_rewind( close_equity_list ) )
	do {
		close_equity = list_get( close_equity_list );

		/* First one is CLOSE_KEY account */
		/* ------------------------------ */
		if ( close_equity->account_name )
		{
			if ( strcmp(
				close_equity->account_name,
				account_name ) == 0 )
			{
				return close_equity;
			}
		}

	} while ( list_next( close_equity_list ) );

	close_equity = list_first( close_equity_list );

	return close_equity;
}

double close_equity_debit_amount(
		double debit_sum,
		double credit_sum )
{
	if ( credit_sum > debit_sum ) return 0.0;

	return debit_sum - credit_sum;
}

double close_equity_credit_amount(
		double debit_sum,
		double credit_sum )
{
	if ( debit_sum > credit_sum ) return 0.0;

	return credit_sum - debit_sum;
}

LIST *close_equity_journal_list( LIST *close_equity_list )
{
	LIST *list = list_new();
	CLOSE_EQUITY *close_equity;

	if ( list_rewind( close_equity_list ) )
	do {
		close_equity =
			list_get(
				close_equity_list );

		if ( !close_equity->journal )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"close_equity->journal is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, close_equity->journal );

	} while ( list_next( close_equity_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

void close_equity_journal_debit_credit_set( JOURNAL *journal )
{
	double difference;
	boolean virtually_same;

	if ( !journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	difference =
		close_equity_difference(
			journal->debit_amount,
			journal->credit_amount );

	virtually_same = float_virtually_same( difference, 0.0 );

	if ( virtually_same )
	{
		journal->credit_amount = 0.0;
		journal->debit_amount = 0.0;
	}
	if ( difference > 0.0 )
	{
		journal->credit_amount = difference;
		journal->debit_amount = 0.0;
	}
	else
	{
		journal->credit_amount = 0.0;
		journal->debit_amount = -difference;
	}
}

void close_equity_journal_list_debit_credit_set(
		LIST *close_equity_journal_list )
{
	JOURNAL *journal;

	if ( list_rewind( close_equity_journal_list ) )
	do {
		journal =
			list_get(
				close_equity_journal_list );

		close_equity_journal_debit_credit_set(
			journal /* in/out */ );

	} while ( list_next( close_equity_journal_list ) );
}

double close_equity_difference(
		double debit_amount,
		double credit_amount )
{
	return credit_amount - debit_amount;
}
