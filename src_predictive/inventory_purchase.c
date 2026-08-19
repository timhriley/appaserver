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
#include "float.h"
#include "sql.h"
#include "predictive.h"
#include "sale.h"
#include "inventory_sale.h"
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

double inventory_purchase_list_total( LIST *inventory_purchase_list )
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

char *inventory_purchase_list_update_system_string(
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

LIST *inventory_purchase_list_primary_key_list(
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
		double extended_cost,
		double cost_basis,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double inventory_purchase_extended_cost,
		double cost_basis_amount )
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

	if (	float_virtually_same(
			extended_cost,
			inventory_purchase_extended_cost )
	&&	float_virtually_same(
			cost_basis,
			cost_basis_amount ) )
	{
		list_free( list );
		return NULL;
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

	if ( !float_virtually_same(
		extended_cost,
		inventory_purchase_extended_cost ) )
	{
		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"extended_cost" /* column_name */,
				inventory_purchase_extended_cost /* money */,
				1 /* set_boolean */ );

		list_set( list, update_string );
	}

	if ( !float_virtually_same(
		cost_basis,
		cost_basis_amount ) )
	{
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

		list_set( list, update_string );
	}

	return list;
}

INVENTORY_PURCHASE_LIST *inventory_purchase_list_new(
		const char *inventory_purchase_select,
		const char *inventory_purchase_table,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *purchase_primary_where )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	INVENTORY_PURCHASE *inventory_purchase;

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

	inventory_purchase_list = inventory_purchase_list_calloc();
	inventory_purchase_list->list = list_new();

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		inventory_purchase =
			/* -------------- */
			/* Should succeed */
			/* -------------- */
			inventory_purchase_parse( input );

		inventory_purchase->inventory_purchase_extended_cost =
			INVENTORY_PURCHASE_EXTENDED_COST(
				inventory_purchase->ordered_quantity,
				inventory_purchase->unit_cost );

		list_set(
			inventory_purchase_list->list,
			inventory_purchase );
	}

	pclose( input_pipe );

	inventory_purchase_list->primary_key_list =
		inventory_purchase_list_primary_key_list(
			SALE_INVENTORY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	inventory_purchase_list->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_list_update_system_string(
			inventory_purchase_table,
			inventory_purchase_list->primary_key_list );

	if ( !list_length( inventory_purchase_list->list ) )
	{
		list_free( inventory_purchase_list->list );
		inventory_purchase_list->list = NULL;
	}

	return inventory_purchase_list;
}

INVENTORY_PURCHASE_LIST *inventory_purchase_list_calloc( void )
{
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;

	if ( ! ( inventory_purchase_list =
			calloc( 1,
				sizeof ( INVENTORY_PURCHASE_LIST ) ) ) )
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

	return inventory_purchase_list;
}

