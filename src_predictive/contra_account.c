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
#include "predictive.h"
#include "journal.h"
#include "account_journal.h"
#include "contra_account.h"

LIST *contra_account_fetch_list(
		const char *contra_account_select,
		const char *contra_account_table )
{
	static LIST *list = {0};
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	CONTRA_ACCOUNT *contra_account;

	/* CONTRA_ACCOUNT table may not exist */
	/* ---------------------------------- */
	if ( !folder_column_boolean(
		(char *)contra_account_table,
		(char *)ACCOUNT_PRIMARY_KEY ) )
	{
		return NULL;
	}

	if ( list ) return list;

	list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)contra_account_select,
			(char *)contra_account_table,
			(char *)0 /* where */ );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

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

		list_set( list, contra_account );
	}

	pclose( input_pipe );

	return list;
}

CONTRA_ACCOUNT *contra_account_parse( char *input )
{
	char account_name[ 128 ];
	char contra_to_account[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See CONTRA_ACCOUNT_SELECT */
	/* -------------------------- */
	piece( account_name, SQL_DELIMITER, input, 0 );
	piece( contra_to_account, SQL_DELIMITER, input, 1 );

	return
	contra_account_new(
		strdup( account_name ),
		strdup( contra_to_account ) );
}

CONTRA_ACCOUNT *contra_account_new(
		char *account_name,
		char *contra_to_account )
{
	CONTRA_ACCOUNT *contra_account;

	if ( !account_name
	||   !contra_to_account )
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

	contra_account->account_name = account_name;
	contra_account->contra_to_account = contra_to_account;

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

CONTRA_ACCOUNT_LIST *contra_account_list_new(
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *fund_name,
		char *end_date_time_string,
		LIST *contra_account_fetch_list,
		char *contra_to_account )
{
	CONTRA_ACCOUNT_LIST *contra_account_list;
	CONTRA_ACCOUNT *contra_account;

	if ( !end_date_time_string
	||   !contra_to_account )
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

	contra_account_list = contra_account_list_calloc();
	contra_account_list->list = list_new();
	contra_account_list->account_name_list = list_new();

	if ( list_rewind( contra_account_fetch_list ) )
	do {
		contra_account =
			list_get( contra_account_fetch_list );

		if ( strcmp(
			contra_to_account,
			contra_account->contra_to_account ) == 0 )
		{
			contra_account->account_journal_latest =
			    account_journal_latest(
				JOURNAL_TABLE,
				fund_name,
				contra_account->account_name,
				end_date_time_string,
				fund_boolean,
				contact_key_boolean,
				0 /* not fetch_transaction */,
				0 /* not latest_zero_balance_boolean */ );

			if ( contra_account->account_journal_latest )
			{
				contra_account_list->balance_total +=
					contra_account->
						account_journal_latest->
						balance;

				list_set(
					contra_account_list->
						account_name_list,
					contra_account->account_name );

				list_set(
					contra_account_list->list,
					contra_account );
			}
		}
	} while ( list_next( contra_account_fetch_list ) );

	if ( !list_length( contra_account_list->list ) )
	{
		free( contra_account_list->list );
		free( contra_account_list->account_name_list );
		free( contra_account_list );
		return NULL;
	}

	contra_account_list->net_label =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		contra_account_list_net_label(
			contra_to_account,
			contra_account_list->
				account_name_list );

	return contra_account_list;
}

CONTRA_ACCOUNT_LIST *contra_account_list_calloc( void )
{
	CONTRA_ACCOUNT_LIST *contra_account_list;

	if ( ! ( contra_account_list =
			calloc( 1,
				sizeof ( CONTRA_ACCOUNT_LIST ) ) ) )
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

	return contra_account_list;
}

char *contra_account_list_net_label(
		char *contra_to_account,
		LIST *account_name_list )
{
	char net_label[ 1024 ];
	char *display_delimited = {0};

	if ( !contra_to_account
	||   !list_length( account_name_list ) )
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

	snprintf(
		net_label,
		sizeof ( net_label ),
		"%s net %s",
		contra_to_account,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		( display_delimited = list_display_delimited(
			account_name_list,
			',' ) ) );

	free( display_delimited );

	return
	strdup(
		string_initial_capital(
			net_label,
			net_label ));
}

CONTRA_ACCOUNT *contra_account_seek(
		LIST *contra_account_list,
		char *account_name )
{
	CONTRA_ACCOUNT *contra_account;

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

	if ( list_rewind( contra_account_list ) )
	do {
		contra_account = list_get( contra_account_list );

		if ( string_strcmp(
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

	return
	(boolean)(unsigned int)(long)contra_account_seek(
		contra_account_list,
		account_name );
}
