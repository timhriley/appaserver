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
		char *preupdate_purchase_date_time )
{
	FIXED_ASSET_HOME *fixed_asset_home;
	char *system_string;
	char *input;

	if ( !asset_name
	||   !state
	||   !preupdate_full_name
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

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_system_input( system_string ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_system_fetch(%s) returned empty.",
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
			input );

	if ( fixed_asset_home->fixed_asset_cost
	&&   fixed_asset_home->asset_account_string )
	{
		if ( ! ( fixed_asset_home->debit_account =
				account_fetch(
					fixed_asset_home->
						asset_account_string,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"account_fetch(%s) returned empty.",
				fixed_asset_home->asset_account_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ! ( fixed_asset_home->credit_account =
				account_fetch(
					fixed_asset_home->
						credit_account_string,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"account_fetch(%s) returned empty.",
				fixed_asset_home->credit_account_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fixed_asset_home->journal_binary_list =
			journal_binary_list(
				(char *)0 /* fund_name */,
				(char *)0 /* full_name */,
				(char *)0 /* contact_key */,
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
			"preupdate_fund_name"
			/* preupdate_fund_name_placeholder */,
			"preupdate_full_name"
			/* preupdate_full_name_placeholder */,
			"preupdate_contact_key"
			/* preupdate_contact_key_placeholder */,
			"preupdate_purchase_date_time"
			/* preupdate_foreign_date_time_placeholder */,
			state,
			(char *)0 /* preupdate_fund_name */,
			preupdate_full_name,
			(char *)0 /* preupdate_contact_key */,
			preupdate_purchase_date_time
			/* preupdate_foreign_date_time */,
			(char*)0 /* fund_name */,
			fixed_asset_home->full_name,
			(char *)0 /* contact_key */,
			fixed_asset_home->purchase_date_time
				/* foreign_date_time */,
			0 /* not predictive_fund_boolean */,
			0 /* not entity_contact_key_boolean */,
			fixed_asset_home->journal_binary_list
				/* insert_journal_list */ );

	fixed_asset_home->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			FIXED_ASSET_HOME_TABLE
				/* foreign_table_name */,
			"fund_name"
				/* foreign_fund_name_column */,
			"full_name"
				/* foreign_full_name_column */,
			"contact_key"
				/* foreign_contact_key_column */,
			"purchase_date_time"
				/* foreign_date_time_column */,
			fixed_asset_home->journal_binary_list
				/* insert_journal_list */,
			fixed_asset_home->fixed_asset_cost
				/* foreign_amount */,
			asset_name
				/* transaction_memo */,
			fixed_asset_home->
				subsidiary_transaction_state->
				subsidiary_transaction_insert,
			fixed_asset_home->
				subsidiary_transaction_state->
				subsidiary_transaction_delete,
			0 /* not predictive_fund_boolean */,
			0 /* not entity_contact_key_boolean */ );

	free( system_string );
	free( input );

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
		fixed_asset_home->purchase_date_time =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
	{
		fixed_asset_home->fixed_asset_cost =
			atof( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		fixed_asset_home->asset_account_string =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		fixed_asset_home->credit_account_string =
			strdup( buffer );

	return fixed_asset_home;
}

FIXED_ASSET_HOME *fixed_asset_home_new( char *asset_name )
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

