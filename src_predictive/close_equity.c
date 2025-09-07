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
		LIST *equity_journal_list )
{
	LIST *list = list_new();

	if ( folder_column_boolean(
		(char *)close_equity_table,
		(char *)close_equity_primary_key ) )
	{
		FILE *pipe;
		CLOSE_EQUITY *close_equity;
		char input[ 1024 ];

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
					equity_journal_list,
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

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

CLOSE_EQUITY *close_equity_parse(
		LIST *equity_journal_list,
		char *input )
{
	char account_name[ 128 ];
	char equity_account_name[ 128 ];
	char reverse_yn[ 128 ];
	CLOSE_EQUITY *close_equity;

	if ( !input || !*input ) return NULL;

	/* See CLOSE_EQUITY_SELECT */
	/* ----------------------- */
	piece( account_name, SQL_DELIMITER, input, 0 );
	piece( equity_account_name, SQL_DELIMITER, input, 1 );
	piece( reverse_yn, SQL_DELIMITER, input, 2 );

	close_equity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		close_equity_new(
			strdup( account_name ),
			(*reverse_yn == 'y'
		 		/* reverse_boolean */ ) );

	if ( ! ( close_equity->equity_journal =
			equity_journal_seek(
				equity_journal_list,
				equity_account_name ) ) )
	{
		free( close_equity );
		return NULL;
	}

	return close_equity;
}

CLOSE_EQUITY *close_equity_new(
		char *account_name,
		boolean reverse_boolean )
{
	CLOSE_EQUITY *close_equity;

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

	close_equity = close_equity_calloc();

	close_equity->account_name = account_name;
	close_equity->reverse_boolean = reverse_boolean;

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

	if ( list_rewind( close_equity_list ) )
	do {
		close_equity = list_get( close_equity_list );

		if ( strcmp(
			close_equity->account_name,
			account_name ) == 0 )
		{
			return close_equity;
		}

	} while ( list_next( close_equity_list ) );

	return NULL;
}

