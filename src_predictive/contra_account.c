/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/contra_account.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "folder.h"
#include "journal.h"
#include "account_journal.h"
#include "contra_account.h"

LIST *contra_account_list(
		const char *contra_account_select,
		const char *contra_account_table,
		char *fund_name,
		char *end_date_time_string )
{
	LIST *list;
	FILE *input_pipe;
	char *system_string;
	char input[ 1024 ];
	CONTRA_ACCOUNT *contra_account;
	ACCOUNT_JOURNAL *contra_to_account_journal;
	ACCOUNT_JOURNAL *account_journal;

	if ( !folder_column_boolean(
		(char *)contra_account_table,
		(char *)ACCOUNT_PRIMARY_KEY ) )
	{
		return NULL;
	}

	list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)contra_account_select,
			(char *)contra_account_table,
			(char *)0 /* where */ );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		contra_account =
			contra_account_parse(
				input );

		if ( !contra_account )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: contra_account_parse( %s ) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				input );

			continue;
		}

		contra_to_account_journal =
			account_journal_latest(
				JOURNAL_TABLE,
				fund_name,
				contra_account->contra_to_account
					/* account_name */,
				end_date_time_string,
				0 /* not fetch_transaction */,
				0 /* not latest_zero_balance_boolean */ );

		if ( !contra_to_account_journal )
		{
			free( contra_account );
			continue;
		}

		account_journal =
			account_journal_latest(
				JOURNAL_TABLE,
				fund_name,
				contra_account->account_name,
				end_date_time_string,
				0 /* not fetch_transaction */,
				0 /* not latest_zero_balance_boolean */ );

		if ( !account_journal )
		{
			free( contra_account );
			continue;
		}

		contra_account->net_amount =
			contra_account_net_amount(
				contra_to_account_journal->
					balance
					/* contra_to_balance */,
				account_journal->
					balance
					/* account_balance */ );

		contra_account->label =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			contra_account_label(
				contra_to_account_journal->
					account_name
					/* contra_to_name */,
				account_journal->
					account_name );

		if ( !contra_account->label )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: contra_account_label() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

			continue;
		}

		list_set( list, contra_account );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

double contra_account_net_amount(
		double contra_to_balance,
		double account_balance )
{
	return
	contra_to_balance -
	account_balance;
}

char *contra_account_label(
		char *contra_to_name,
		char *account_name )
{
	char label[ 1024 ];

	if ( !contra_to_name || !account_name ) return NULL;

	snprintf(label,
		sizeof ( label ),
		"%s Net %s",
		contra_to_name,
		account_name );

	return strdup( label );
}

CONTRA_ACCOUNT *contra_account_parse( char *input )
{
	char contra_to_account[ 128 ];
	char account_name[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See CONTRA_ACCOUNT_SELECT */
	/* -------------------------- */
	piece( contra_to_account, SQL_DELIMITER, input, 0 );
	piece( account_name, SQL_DELIMITER, input, 1 );

	return
	contra_account_new(
		strdup( contra_to_account ),
		strdup( account_name ) );
}

CONTRA_ACCOUNT *contra_account_new(
		char *contra_to_account,
		char *account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( !contra_to_account
	||   !account_name )
	{
		char message[ 1024 ];

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

	contra_account = contra_account_calloc();

	contra_account->contra_to_account = contra_to_account;
	contra_account->account_name = account_name;

	return contra_account;
}

CONTRA_ACCOUNT *contra_account_calloc( void )
{
	CONTRA_ACCOUNT *contra_account;

	if ( ! ( contra_account = calloc( 1, sizeof ( CONTRA_ACCOUNT ) ) ) )
	{
		char message[ 1024 ];

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

	return contra_account;
}

CONTRA_ACCOUNT *contra_account_seek(
		LIST *contra_account_list,
		char *contra_to_account,
		char *account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( contra_to_account
	&&   account_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
		"both contra_to_account and account_name are populated." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( contra_account_list ) )
	do {
		contra_account = list_get( contra_account_list );

		if ( contra_to_account
		&&   string_strcmp(
			contra_account->contra_to_account,
			contra_to_account ) == 0 )
		{
			return contra_account;
		}

		if ( account_name
		&&   string_strcmp(
			contra_account->account_name,
			account_name ) == 0 )
		{
			return contra_account;
		}

	} while ( list_next( contra_account_list ) );

	return NULL;
}

boolean contra_account_boolean(
		LIST *contra_account_list,
		char *account_name )
{
	if ( !account_name )
	{
		char message[ 1024 ];

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

	if ( contra_account_seek(
		contra_account_list,
		(char *)0 /* contra_to_account */,
		account_name ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
