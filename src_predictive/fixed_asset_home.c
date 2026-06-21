/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/fixed_asset_home.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "piece.h"
#include "sql.h"
#include "security.h"
#include "journal.h"
#include "fixed_asset_home.h"

FIXED_ASSET_HOME *fixed_asset_home_fetch(
		char *asset_name,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_purchase_date_time )
{
	FIXED_ASSET_HOME *fixed_asset_home;
	char *system_string;
	char *pipe_fetch;

	if ( !asset_name
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_purchase_date_time )
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

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			FIXED_ASSET_HOME_SELECT,
			FIXED_ASSET_HOME_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			fixed_asset_home_primary_where(
				FIXED_ASSET_HOME_PRIMARY_KEY,
				asset_name ) );

	if ( ! ( pipe_fetch =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch( system_string ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_fetch(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fixed_asset_home =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_asset_home_parse(
			asset_name,
			pipe_fetch /* input */ );

	if ( fixed_asset_home->fixed_asset_cost
	&&   fixed_asset_home->asset_account )
	{
		if ( ! ( fixed_asset_home->debit_account =
				account_fetch(
					fixed_asset_home->asset_account,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"account_fetch(%s) returned empty.",
				fixed_asset_home->asset_account );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fixed_asset_home->account_equity_string =
			/* ------------------------------------ */
			/* Returns heap memory from static list */
			/* ------------------------------------ */
			account_equity_string(
				ACCOUNT_EQUITY_KEY,
				__FUNCTION__ );

		if ( ! ( fixed_asset_home->credit_account =
				account_fetch(
					fixed_asset_home->
						account_equity_string,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"account_fetch(%s) returned empty.",
				fixed_asset_home->account_equity_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fixed_asset_home->journal_binary_list =
			journal_binary_list(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				fixed_asset_home->fixed_asset_cost
					/* transaction_amount */,
				fixed_asset_home->debit_account,
				fixed_asset_home->credit_account );
	}

	fixed_asset_home->subsidiary_transaction_state =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_state_new(
			"preupdate_full_name"
			/* preupdate_full_name_placeholder */,
			"preupdate_street_address"
			/* preupdate_street_address_placeholder */,
			"preupdate_purchase_date_time"
			/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time
			/* preupdate_foreign_date_time */,
			fixed_asset_home->full_name,
			fixed_asset_home->street_address,
			fixed_asset_home->
				purchase_date_time
				/* foreign_date_time */,
			fixed_asset_home->journal_binary_list
				/* insert_journal_list */ );

	fixed_asset_home->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			FIXED_ASSET_HOME_TABLE
				/* foreign_table_name */,
			"full_name"
				/* foreign_full_name_column */,
			"street_address"
				/* foreign_street_addres_column */,
			"purchase_date_time"
				/* foreign_date_time_column */,
			fixed_asset_home->
				purchase_date_time
				/* prior_transaction_date_time */,
			fixed_asset_home->journal_binary_list
				/* insert_journal_list */,
			fixed_asset_home->
				fixed_asset_cost
				/* foreign_amount */,
			asset_name
				/* transaction_memo */,
			fixed_asset_home->
				subsidiary_transaction_state->
				subsidiary_transaction_insert,
			fixed_asset_home->
				subsidiary_transaction_state->
				subsidiary_transaction_delete );

	free( system_string );
	free( pipe_fetch );

	return fixed_asset_home;
}

FIXED_ASSET_HOME *fixed_asset_home_parse(
		char *asset_name,
		char *input )
{
	FIXED_ASSET_HOME *fixed_asset_home;
	char buffer[ 128 ];

	if ( !asset_name
	||   !*asset_name
	||   !input
	||   !*input )
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

	fixed_asset_home =
		fixed_asset_home_new(
			asset_name );

	/* See FIXED_ASSET_HOME_SELECT */
	/* ---------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer )
		fixed_asset_home->full_name =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		fixed_asset_home->street_address =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		fixed_asset_home->purchase_date_time =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
	{
		fixed_asset_home->fixed_asset_cost =
			atof( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		fixed_asset_home->asset_account =
			strdup( buffer );

	return fixed_asset_home;
}

FIXED_ASSET_HOME *fixed_asset_home_new(
		char *asset_name )
{
	FIXED_ASSET_HOME *fixed_asset_home;

	if ( !asset_name || !*asset_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"asset_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	fixed_asset_home = fixed_asset_home_calloc();
	fixed_asset_home->asset_name = asset_name;

	return fixed_asset_home;
}

FIXED_ASSET_HOME *fixed_asset_home_calloc( void )
{
	FIXED_ASSET_HOME *fixed_asset_home;

	if ( ! ( fixed_asset_home =
			calloc( 1,
				sizeof ( FIXED_ASSET_HOME ) ) ) )
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

	return fixed_asset_home;
}

char *fixed_asset_home_primary_where(
		const char *fixed_asset_home_primary_key,
		char *asset_name )
{
	static char where[ 128 ];
	char *tmp;

	if ( !asset_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"asset_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"%s = '%s'",
		fixed_asset_home_primary_key,
		( tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				asset_name /* datum */ ) ) );

	free( tmp );

	return where;
}

