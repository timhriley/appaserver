/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "piece.h"
#include "entity.h"
#include "sql.h"
#include "security.h"
#include "sale.h"
#include "fixed_asset.h"
#include "fixed_asset_purchase.h"

FIXED_ASSET_PURCHASE *fixed_asset_purchase_calloc( void )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( ! ( fixed_asset_purchase =
			calloc( 1, sizeof ( FIXED_ASSET_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return fixed_asset_purchase;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
		char *asset_name,
		char *serial_key,
		boolean fetch_fixed_asset_boolean )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !asset_name
	||   !serial_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fixed_asset_purchase = fixed_asset_purchase_calloc();

	fixed_asset_purchase->asset_name = asset_name;
	fixed_asset_purchase->serial_key = serial_key;

	if ( fetch_fixed_asset_boolean )
	{
		fixed_asset_purchase->fixed_asset =
			fixed_asset_fetch(
				fixed_asset_purchase->asset_name );
	}

	return fixed_asset_purchase;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_key[ 128 ];
	char buffer[ 128 ];
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !input || !*input ) return NULL;

	/* See FIXED_ASSET_PURCHASE_SELECT */
	/* ------------------------------- */
	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_key, SQL_DELIMITER, input, 1 );

	fixed_asset_purchase =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_asset_purchase_new(
			strdup( asset_name ),
			strdup( serial_key ),
			0 /* not fetch_fixed_asset_boolean */ );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		fixed_asset_purchase->service_placement_date =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		fixed_asset_purchase->fixed_asset_cost =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		fixed_asset_purchase->units_produced_so_far =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		fixed_asset_purchase->disposal_date =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
		fixed_asset_purchase->recovery_class_year_string =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer )
		fixed_asset_purchase->recovery_method =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer )
		fixed_asset_purchase->recovery_convention =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer )
		fixed_asset_purchase->recovery_system =
			strdup( buffer );

/*
	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer )
		fixed_asset_purchase->depreciation_method_resolve =
			depreciation_method_resolve( buffer );
*/

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer )
		fixed_asset_purchase->estimated_useful_life_years =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 12 );
	if ( *buffer )
		fixed_asset_purchase->estimated_useful_life_units =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 13 );
	if ( *buffer )
		fixed_asset_purchase->estimated_residual_value =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 14 );
	if ( *buffer )
		fixed_asset_purchase->declining_balance_n =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 15 );
	if ( *buffer )
		fixed_asset_purchase->cost_basis =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 16 );
	if ( *buffer )
		fixed_asset_purchase->finance_accumulated_depreciation =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 17 );
	if ( *buffer )
		fixed_asset_purchase->tax_adjusted_basis =
			atof( buffer );

	return fixed_asset_purchase;
}

char *fixed_asset_purchase_system_string(
		char *select_string,
		const char *fixed_asset_purchase_table,
		char *where,
		char *order )
{
	char system_string[ 1024 ];

	if ( !select_string
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh '%s' %s \"%s\" \"%s\"",
		select_string,
		fixed_asset_purchase_table,
		where,
		(order) ? order : "" );

	return strdup( system_string );
}

LIST *fixed_asset_purchase_list(
		const char *fixed_asset_purchase_select,
		const char *fixed_asset_purchase_table,
		char *purchase_primary_where,
		boolean entity_contact_key_boolean )
{
	char *select_string;
	char *system_string;
	char input[ 2048 ];
	FILE *input_pipe;
	LIST *list = list_new();

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			fixed_asset_purchase_select,
			ENTITY_CONTACT_KEY_COLUMN,
			entity_contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_asset_purchase_system_string(
			select_string,
			fixed_asset_purchase_table,
			purchase_primary_where,
			"service_placement_date"
				/* order */ );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			fixed_asset_purchase_parse(
				input ) );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *asset_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *select_string;
	char *primary_where;
	char *where;
	char *system_string;
	char *input;

	if ( !full_name
	||   !purchase_date_time
	||   !asset_name
	||   !serial_key )
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

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			FIXED_ASSET_PURCHASE_SELECT,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		purchase_primary_where(
			PURCHASE_DATE_TIME_COLUMN,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			fund_boolean,
			contact_key_boolean );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_asset_purchase_primary_where(
			asset_name,
			serial_key,
			primary_where );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select_string,
			FIXED_ASSET_PURCHASE_TABLE,
			where );

	free( where );

	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	free( system_string );

	return fixed_asset_purchase_parse( input );
}

char *fixed_asset_purchase_primary_where(
		char *asset_name,
		char *serial_key,
		char *purchase_primary_where )
{
	char *asset_escape;
	char *serial_escape;
	char where[ 1024 ];

	if ( !asset_name
	||   !serial_key
	||   !purchase_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Returns heap memory */
	/* ------------------- */
	asset_escape = security_escape( asset_name );
	serial_escape = security_escape( serial_key );

	snprintf(
		where,
		sizeof ( where ),
		"%s and asset_name = '%s' and serial_label = '%s'",
		purchase_primary_where,
		asset_escape,
		serial_escape );

	free( asset_escape );
	free( serial_escape );

	return strdup( where );
}

double fixed_asset_purchase_total( LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	double purchase_total = {0};

	if ( list_rewind( fixed_asset_purchase_list ) )
	do {
		fixed_asset_purchase = list_get( fixed_asset_purchase_list );
		purchase_total += fixed_asset_purchase->fixed_asset_cost;

	} while ( list_next( fixed_asset_purchase_list ) );

	return purchase_total;
}

double fixed_asset_purchase_cost_basis(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		double fixed_asset_cost )
{
	double cost_basis = {0};
	PURCHASE *purchase;

	if ( !full_name
	||   !purchase_date_time )
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

	purchase =
		purchase_trigger_new(
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			APPASERVER_UPDATE_STATE,
			(char *)0 /* preupdate_fund_name */,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_contact_key */ );

	if ( purchase )
	{
		cost_basis =
			purchase_cost_basis(
				fixed_asset_cost,
				purchase->purchase_fetch->sales_tax,
				purchase->purchase_fetch->freight_in,
				purchase->fixed_asset_purchase_total,
				purchase->inventory_purchase_total,
				purchase->specific_inventory_purchase_total,
				purchase->supply_purchase_total,
				purchase->prepaid_asset_purchase_total );
	}

	return cost_basis;
}

LIST *fixed_asset_purchase_primary_key_list(
		const char *purchase_asset_column,
		const char *sale_serial_key_column,
		LIST *purchase_primary_key_list /* out */ )
{
	list_set(
		purchase_primary_key_list,
		(char *)purchase_asset_column );

	list_set(
		purchase_primary_key_list,
		(char *)sale_serial_key_column );

	return purchase_primary_key_list;
}

LIST *fixed_asset_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double cost_basis )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

	primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time /* sale_date_time */,
			fund_boolean,
			contact_key_boolean );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"cost_basis" /* column_name */,
			cost_basis /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	return list;
}

#ifdef NOT_DEFINED
char *purchase_asset_account_name(
			LIST *fixed_asset_purchase_list )
{
	double highest_cost = 0.0;
	char *asset_account_name = {0};
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (char *)0;

	do {
		fixed_asset_purchase =
			list_get( 
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->fixed_asset_cost > highest_cost )
		{
			asset_account_name =
				fixed_asset_purchase->
					fixed_asset->
					account_name;

			highest_cost = fixed_asset_purchase->fixed_asset_cost;
		}
	} while ( list_next( fixed_asset_purchase_list ) );

	return asset_account_name;
}
#endif
