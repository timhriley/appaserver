/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equity_journal.c			*/
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
#include "equity_journal.h"

LIST *equity_journal_list(
		const char *close_equity_table,
		const char *equity_journal_select,
		const char *close_equity_primary_key,
		const char *account_closing_key,
		const char *account_equity_key )
{
	LIST *list = list_new();
	char *equity_account_name;
	EQUITY_JOURNAL *equity_journal;
	char input[ 1024 ];

	equity_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_closing_entry_string(
			account_closing_key,
			account_equity_key,
			__FUNCTION__ );

	if ( ! ( equity_journal =
			equity_journal_new(
				equity_account_name ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"equity_journal_new(%s) returned empty.",
			equity_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set( list, equity_journal );

	if ( folder_column_boolean(
		(char *)close_equity_table,
		(char *)close_equity_primary_key ) )
	{
		char *system_string;
		FILE *pipe;

		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			equity_journal_system_string(
				equity_journal_select,
				close_equity_table );

		pipe = appaserver_input_pipe( system_string );

		free( system_string );

		while ( string_input( input, pipe, sizeof ( input ) ) )
		{
			if ( ! ( equity_journal =
				    equity_journal_parse(
					input ) ) )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"equity_journal_parse() returned empty." );

				pclose( pipe );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			list_set( list, equity_journal );
		}

		pclose( pipe );
	}

	return list;
}

EQUITY_JOURNAL *equity_journal_parse( char *input /* stack memory */ )
{
	if ( !input || !*input ) return NULL;

	return
	equity_journal_new(
		/* ------------------------- */
		/* See EQUITY_JOURNAL_SELECT */
		/* ------------------------- */
		strdup( input ) /* equity_account_name */ );
}

EQUITY_JOURNAL *equity_journal_new( char *equity_account_name )
{
	EQUITY_JOURNAL *equity_journal;

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

	equity_journal = equity_journal_calloc();

	equity_journal->equity_account_name = equity_account_name;

	equity_journal->journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_new(
			(char *)0 /* fund_name */,
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			equity_account_name );

	if ( ! ( equity_journal->journal->account =
			account_fetch(
				equity_account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		return NULL;
	}

	return equity_journal;
}

EQUITY_JOURNAL *equity_journal_calloc( void )
{
	EQUITY_JOURNAL *equity_journal;

	if ( ! ( equity_journal = calloc( 1, sizeof ( EQUITY_JOURNAL ) ) ) )
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

	return equity_journal;
}

EQUITY_JOURNAL *equity_journal_seek(
		LIST *equity_journal_list,
		char *equity_account_name )
{
	EQUITY_JOURNAL *equity_journal;

	if ( !list_rewind( equity_journal_list ) /* at least one */
	||   !equity_account_name )
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

	do {
		equity_journal = list_get( equity_journal_list );

		/* First one is ACCOUNT_CLOSING_KEY account */
		/* ---------------------------------------- */
		if ( !list_first_boolean( equity_journal_list ) )
		{
			if ( strcmp(
				equity_journal->equity_account_name,
				equity_account_name ) == 0 )
			{
				return equity_journal;
			}
		}

	} while ( list_next( equity_journal_list ) );

	equity_journal = list_first( equity_journal_list );

	return equity_journal;
}

LIST *equity_journal_extract_list( LIST *equity_journal_list )
{
	LIST *list = list_new();
	EQUITY_JOURNAL *equity_journal;

	if ( list_rewind( equity_journal_list ) )
	do {
		equity_journal =
			list_get(
				equity_journal_list );

		if ( !equity_journal->journal )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"equity_journal->journal is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, equity_journal->journal );

	} while ( list_next( equity_journal_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

void equity_journal_debit_credit_set( JOURNAL *journal )
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
		equity_journal_difference(
			journal->debit_amount,
			journal->credit_amount );

	virtually_same = float_virtually_same( difference, 0.0 );

	if ( virtually_same )
	{
		journal->credit_amount = 0.0;
		journal->debit_amount = 0.0;
	}
	else
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

void equity_journal_list_debit_credit_set(
		LIST *equity_journal_extract_list )
{
	JOURNAL *journal;

	if ( list_rewind( equity_journal_extract_list ) )
	do {
		journal =
			list_get(
				equity_journal_extract_list );

		equity_journal_debit_credit_set(
			journal /* in/out */ );

	} while ( list_next( equity_journal_extract_list ) );
}

double equity_journal_difference(
		double debit_amount,
		double credit_amount )
{
	return credit_amount - debit_amount;
}

char *equity_journal_system_string(
		const char *select,
		const char *table )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s",
		select,
		table );

	return strdup( system_string );
}

