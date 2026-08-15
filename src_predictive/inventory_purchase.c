/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "piece.h"
#include "sql.h"
#include "security.h"
#include "predictive.h"
#include "sale.h"
#include "inventory_sale.h"
#include "purchase.h"
#include "purchase_fetch.h"
#include "fixed_asset_purchase.h"
#include "inventory_purchase.h"

INVENTORY_PURCHASE *inventory_purchase_new(
		char *inventory_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !inventory_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_purchase = inventory_purchase_calloc();
	inventory_purchase->inventory_name = inventory_name;

	return inventory_purchase;
}

INVENTORY_PURCHASE *inventory_purchase_calloc( void )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( ! ( inventory_purchase =
			calloc( 1,
				sizeof ( INVENTORY_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return inventory_purchase;
}

double inventory_purchase_total( LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double total = {0};

	if ( list_rewind( inventory_purchase_list ) )
	do {

		inventory_purchase =
			list_get(
				inventory_purchase_list );

		total += inventory_purchase->extended_cost;

	} while ( list_next( inventory_purchase_list ) );

	return total;
}

INVENTORY_PURCHASE *inventory_purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *where;
	char *system_string;
	char *input;
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !full_name
	||   !purchase_date_time
	||   !inventory_name )
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
		inventory_purchase_primary_where(
			SALE_INVENTORY_COLUMN,
			inventory_name,
			primary_where );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			INVENTORY_PURCHASE_SELECT,
			INVENTORY_PURCHASE_TABLE,
			where );

	free( where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	free( system_string );

	if ( !input ) return NULL;

	inventory_purchase =
		inventory_purchase_parse(
			input );

	inventory_purchase->cost_basis =
		inventory_purchase_cost_basis(
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			inventory_purchase->extended_cost );

	inventory_purchase->update_string_list =
		inventory_purchase_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			inventory_name,
			fund_boolean,
			contact_key_boolean,
			inventory_purchase->cost_basis );

	inventory_purchase->primary_key_list =
		inventory_purchase_primary_key_list(
			SALE_INVENTORY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	inventory_purchase->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_update_system_string(
			INVENTORY_PURCHASE_TABLE,
			inventory_purchase->primary_key_list );

	return inventory_purchase;
}

char *inventory_purchase_update_system_string(
		const char *inventory_purchase_table,
		LIST *purchase_primary_key_list )
{
	return
	/* -------------------- */
	/* Borrow SALE’s	*/
	/* -------------------- */
	/* Returns heap memory	*/
	/* -------------------- */
	sale_update_system_string(
		inventory_purchase_table,
		purchase_primary_key_list );

}

INVENTORY_PURCHASE *inventory_purchase_parse( char *input )
{
	char inventory_name[ 128 ];
	char buffer[ 128 ];
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !input || !*input ) return NULL;

	/* See INVENTORY_PURCHASE_SELECT */
	/* -------------------------------------- */
	piece( inventory_name, SQL_DELIMITER, input, 0 );

	inventory_purchase =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_purchase_new(
			strdup( inventory_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) inventory_purchase->ordered_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) inventory_purchase->arrived_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) inventory_purchase->missing_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) inventory_purchase->unit_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) inventory_purchase->extended_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) inventory_purchase->retail_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) inventory_purchase->cost_basis = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) inventory_purchase->quantity_on_hand = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) inventory_purchase->average_unit_cost = atof( buffer );

	return inventory_purchase;
}

LIST *inventory_purchase_primary_key_list(
		const char *sale_inventory_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		purchase_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			PURCHASE_DATE_TIME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( primary_key_list, (char *)sale_inventory_column );

	return primary_key_list;
}

LIST *inventory_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double cost_basis )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

	if ( !full_name
	||   !purchase_date_time
	||   !inventory_name )
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

	primary_data_string =
		/* ---------------------------- */
		/* Borrow INVENTORY_SALE’s	*/
		/* Returns heap memory		*/
		/* ---------------------------- */
		inventory_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time /* sale_date_time */,
			inventory_name,
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

double inventory_purchase_cost_basis(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		double unit_cost )
{
	return
	/* ----------------------------- */
	/* Borrow FIXED_ASSET_PURCHASE’s */
	/* ----------------------------- */
	fixed_asset_purchase_cost_basis(
		fund_name,
		full_name,
		contact_key,
		purchase_date_time,
		unit_cost /* fixed_asset_cost */ );
}

char *inventory_purchase_primary_where(
		const char *sale_inventory_column,
		char *inventory_name,
		char *purchase_primary_where )
{
	char *escape;
	char primary_where[ 256 ];

	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	escape = security_escape( inventory_name );

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"%s and %s = '%s'",
		purchase_primary_where,
		sale_inventory_column,
		escape );

	free( escape );

	return strdup( primary_where );
}

LIST *inventory_purchase_list(
		const char *inventory_purchase_select,
		const char *inventory_purchase_table,
		char *purchase_primary_where )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *list = list_new();

	if ( !purchase_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"purchase_primary_where is empty." );

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
			(char *)inventory_purchase_select,
			(char *)inventory_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		/* -------------- */
		/* Should succeed */
		/* -------------- */
		inventory_purchase  = inventory_purchase_parse( input );

		list_set( list, inventory_purchase );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

