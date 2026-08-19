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
#include "float.h"
#include "entity.h"
#include "sql.h"
#include "sale.h"
#include "cost_basis.h"
#include "purchase.h"
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
		char *serial_key )
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
			strdup( serial_key ) );

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

FIXED_ASSET_PURCHASE_LIST *fixed_asset_purchase_list_new(
		const char *fixed_asset_purchase_select,
		const char *fixed_asset_purchase_table,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *purchase_primary_where )
{
	char *system_string;
	char input[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET_PURCHASE_LIST *fixed_asset_purchase_list;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_asset_purchase_select,
			(char *)fixed_asset_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	fixed_asset_purchase_list = fixed_asset_purchase_list_calloc();
	fixed_asset_purchase_list->list = list_new();

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			fixed_asset_purchase_list->list,
			fixed_asset_purchase_parse(
				input ) );
	}

	pclose( input_pipe );

	fixed_asset_purchase_list->primary_key_list =
		fixed_asset_purchase_list_primary_key_list(
			PURCHASE_ASSET_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	fixed_asset_purchase_list->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_asset_purchase_list_update_system_string(
			fixed_asset_purchase_table,
			fixed_asset_purchase_list->primary_key_list );

	if ( !list_length( fixed_asset_purchase_list->list ) )
	{
		list_free( fixed_asset_purchase_list->list );
		fixed_asset_purchase_list->list = NULL;
	}

	return fixed_asset_purchase_list;
}

double fixed_asset_purchase_list_total( LIST *fixed_asset_purchase_list )
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

LIST *fixed_asset_purchase_list_primary_key_list(
		const char *purchase_asset_column,
		const char *sale_serial_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list;

	list =
		purchase_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			PURCHASE_DATE_TIME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set(
		list,
		(char *)purchase_asset_column );

	list_set(
		list,
		(char *)sale_serial_key_column );

	return list;
}

char *fixed_asset_purchase_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		double cost_basis,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double cost_basis_amount )
{
	char *primary_data_string;
	char *update_string;

	if ( float_virtually_same(
		cost_basis,
		cost_basis_amount ) )
	{
		return NULL;
	}

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
			cost_basis_amount /* money */,
			1 /* set_boolean */ );

	free( primary_data_string );

	return update_string;
}

char *fixed_asset_purchase_list_update_system_string(
		const char *fixed_asset_purchase_table,
		LIST *primary_key_list )
{
	return
	/* -------------------	*/
	/* Borrow SALE’s	*/
	/* Returns heap memory  */
	/* -------------------	*/
	sale_update_system_string(
		fixed_asset_purchase_table,
		primary_key_list );
}

FIXED_ASSET_PURCHASE_LIST *inventory_purchase_list_calloc( void )
{
	FIXED_ASSET_PURCHASE_LIST *fixed_asset_purchase_list;

	if ( ! ( fixed_asset_purchase_list =
			calloc( 1,
				sizeof ( FIXED_ASSET_PURCHASE_LIST ) ) ) )
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

	return fixed_asset_purchase_list;
}

LIST *fixed_assset_purchase_list_update_string_list(
		LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	LIST *update_string_list = list_new();

	if ( list_rewind( fixed_asset_purchase_list ) )
	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		list_set(
			update_string_list,
			fixed_asset_purchase->update_string );

	} while ( list_next( fixed_asset_purchase_list ) );

	if ( !list_length( update_string_list ) )
	{
		list_free( update_string_list );
		update_string_list = NULL;
	}

	return update_string_list;
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
