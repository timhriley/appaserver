/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/prior_fixed_asset.c			*/
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
#include "account.h"
#include "prior_fixed_asset.h"

PRIOR_FIXED_ASSET *
	prior_fixed_asset_fetch(
		char *asset_name,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_purchase_date_time,
		char *preupdate_fixed_asset_cost,
		char *preupdate_asset_account )
{
	PRIOR_FIXED_ASSET *prior_fixed_asset;
	char *system_string;
	char *pipe_fetch;
	char *credit_account_name;

	if ( !asset_name
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_purchase_date_time
	||   !preupdate_fixed_asset_cost
	||   !preupdate_asset_account )
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
			PRIOR_FIXED_ASSET_SELECT,
			PRIOR_FIXED_ASSET_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			prior_fixed_asset_primary_where(
				PRIOR_FIXED_ASSET_PRIMARY_KEY,
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

	prior_fixed_asset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prior_fixed_asset_parse(
			asset_name,
			pipe_fetch /* input */ );

	prior_fixed_asset->subsidiary_transaction_state =
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
			"preupdate_fixed_asset_cost"
			/* preupdate_foreign_amount_placeholder */,
			"preupdate_asset_account"
			/* preupdate_account_name_placeholder */,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time
			/* preupdate_foreign_date_time */,
			preupdate_fixed_asset_cost
			/* preupdate_foreign_amount */,
			preupdate_asset_account
			/* preupdate_account_name */,
			prior_fixed_asset->
				full_name,
			prior_fixed_asset->
				street_address,
			prior_fixed_asset->
				purchase_date_time
				/* foreign_date_time */,
			prior_fixed_asset->
				fixed_asset_cost_string
				/* foreign_amount_string */,
			prior_fixed_asset->
				asset_account
				/* account_name */ );

	credit_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_hard_coded_account_name(
			ACCOUNT_EQUITY_KEY,
			0 /* not warning_only */,
			__FUNCTION__ /* calling_function_name */ );

	prior_fixed_asset->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			PRIOR_FIXED_ASSET_TABLE
				/* foreign_table_name */,
			"full_name"
				/* foreign_full_name_column */,
			"street_address"
				/* foreign_street_addres_column */,
			"purchase_date_time"
				/* foreign_date_time_column */,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time
				/* preupdate_foreign_date_time */,
			preupdate_fixed_asset_cost
				/* preupdate_foreign_amount */,
			preupdate_asset_account
				/* preupdate_account_name */,
			prior_fixed_asset->
				full_name,
			prior_fixed_asset->
				street_address,
			prior_fixed_asset->
				purchase_date_time
				/* foreign_date_time */,
			prior_fixed_asset->
				fixed_asset_cost
				/* foreign_amount */,
			prior_fixed_asset->
				asset_account
				/* account_name */,
			prior_fixed_asset->
				subsidiary_transaction_state->
				preupdate_change_full_name,
			prior_fixed_asset->
				subsidiary_transaction_state->
				preupdate_change_street_address,
			prior_fixed_asset->
				subsidiary_transaction_state->
				preupdate_change_foreign_date_time,
			prior_fixed_asset->
				subsidiary_transaction_state->
				preupdate_change_foreign_amount,
			prior_fixed_asset->
				subsidiary_transaction_state->
				preupdate_change_account_name,
			(char *)0 /* debit_account_name */
				/* Mutually exclusive */,
			credit_account_name
				/* Mutually exclusive */,
			asset_name /* transaction_memo */ );

	free( system_string );
	free( pipe_fetch );

	return prior_fixed_asset;
}

PRIOR_FIXED_ASSET *prior_fixed_asset_parse(
		char *asset_name,
		char *input )
{
	PRIOR_FIXED_ASSET *prior_fixed_asset;
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

	prior_fixed_asset =
		prior_fixed_asset_new(
			asset_name );

	/* See PRIOR_FIXED_ASSET_SELECT */
	/* ---------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer )
		prior_fixed_asset->full_name =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		prior_fixed_asset->street_address =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		prior_fixed_asset->purchase_date_time =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
	{
		prior_fixed_asset->fixed_asset_cost_string =
			strdup( buffer );

		prior_fixed_asset->fixed_asset_cost =
			atof( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		prior_fixed_asset->asset_account =
			strdup( buffer );

	return prior_fixed_asset;
}

PRIOR_FIXED_ASSET *prior_fixed_asset_new(
		char *asset_name )
{
	PRIOR_FIXED_ASSET *prior_fixed_asset;

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


	prior_fixed_asset = prior_fixed_asset_calloc();
	prior_fixed_asset->asset_name = asset_name;

	return prior_fixed_asset;
}

PRIOR_FIXED_ASSET *prior_fixed_asset_calloc( void )
{
	PRIOR_FIXED_ASSET *prior_fixed_asset;

	if ( ! ( prior_fixed_asset =
			calloc( 1,
				sizeof ( PRIOR_FIXED_ASSET ) ) ) )
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

	return prior_fixed_asset;
}

char *prior_fixed_asset_primary_where(
		const char *prior_fixed_asset_primary_key,
		char *asset_name )
{
	static char where[ 128 ];

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
		prior_fixed_asset_primary_key,
		asset_name );

	return where;
}

