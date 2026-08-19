/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_purchase.c	*/
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
#include "specific_inventory_sale.h"
#include "specific_inventory_purchase.h"

SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_new(
		char *inventory_name,
		char *serial_key )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;

	if ( !inventory_name
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

	specific_inventory_purchase = specific_inventory_purchase_calloc();
	specific_inventory_purchase->inventory_name = inventory_name;
	specific_inventory_purchase->serial_key = serial_key;

	return specific_inventory_purchase;
}

SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_calloc( void )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;

	if ( ! ( specific_inventory_purchase =
			calloc( 1,
				sizeof ( SPECIFIC_INVENTORY_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return specific_inventory_purchase;
}

double specific_inventory_purchase_list_total(
		LIST *specific_inventory_purchase_list )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
	double total = {0};

	if ( list_rewind( specific_inventory_purchase_list ) )
	do {

		specific_inventory_purchase =
			list_get(
				specific_inventory_purchase_list );

		total += specific_inventory_purchase->unit_cost;

	} while ( list_next( specific_inventory_purchase_list ) );

	return total;
}

char *specific_inventory_purchase_list_update_system_string(
		const char *specific_inventory_purchase_table,
		LIST *purchase_primary_key_list )
{
	return
	/* -------------------- */
	/* Borrow SALE’s	*/
	/* -------------------- */
	/* Returns heap memory	*/
	/* -------------------- */
	sale_update_system_string(
		specific_inventory_purchase_table,
		purchase_primary_key_list );
}

SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_parse( char *input )
{
	char inventory_name[ 128 ];
	char serial_key[ 128 ];
	char buffer[ 128 ];
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;

	if ( !input || !*input ) return NULL;

	/* See SPECIFIC_INVENTORY_PURCHASE_SELECT */
	/* -------------------------------------- */
	piece( inventory_name, SQL_DELIMITER, input, 0 );
	piece( serial_key, SQL_DELIMITER, input, 1 );

	specific_inventory_purchase =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		specific_inventory_purchase_new(
			strdup( inventory_name ),
			strdup( serial_key ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) specific_inventory_purchase->unit_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		specific_inventory_purchase->retail_price =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) specific_inventory_purchase->unit_cost = atof( buffer );

	return specific_inventory_purchase;
}

LIST *specific_inventory_purchase_list_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	return
	specific_inventory_sale_primary_key_list(
		sale_inventory_column,
		sale_serial_key_column,
		fund_boolean,
		contact_key_boolean );
}

char *specific_inventory_purchase_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		double cost_basis,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double cost_basis_amount )
{
	char *primary_data_string;
	char *update_string;

	if ( !full_name
	||   !purchase_date_time
	||   !inventory_name
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

	if ( float_virtually_same(
		cost_basis,
		cost_basis_amount ) )
	{
		return NULL;
	}

	primary_data_string =
		/* ------------------------------------ */
		/* Borrow SPECIFIC_INVENTORY_SALE’s	*/
		/* Returns heap memory			*/
		/* ------------------------------------ */
		specific_inventory_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time /* sale_date_time */,
			inventory_name,
			serial_key,
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

	return update_string;
}

SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list_new(
		const char *specific_inventory_purchase_select,
		const char *specific_inventory_purchase_table,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *purchase_primary_where )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list;
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;

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
			(char *)specific_inventory_purchase_select,
			(char *)specific_inventory_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	specific_inventory_purchase_list =
		specific_inventory_purchase_list_calloc();

	specific_inventory_purchase_list->list = list_new();

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		specific_inventory_purchase =
			/* -------------- */
			/* Should succeed */
			/* -------------- */
			specific_inventory_purchase_parse( input );

		list_set(
			specific_inventory_purchase_list->list,
			specific_inventory_purchase );
	}

	pclose( input_pipe );

	specific_inventory_purchase_list->primary_key_list =
		specific_inventory_purchase_list_primary_key_list(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	specific_inventory_purchase_list->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_purchase_list_update_system_string(
			specific_inventory_purchase_table,
			specific_inventory_purchase_list->primary_key_list );

	if ( !list_length( specific_inventory_purchase_list->list ) )
	{
		list_free( specific_inventory_purchase_list->list );
		specific_inventory_purchase_list->list = NULL;
	}

	return specific_inventory_purchase_list;
}

SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list_calloc(
		void )
{
	SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list;

	if ( ! ( specific_inventory_purchase_list =
			calloc(
			   1,
			   sizeof ( SPECIFIC_INVENTORY_PURCHASE_LIST ) ) ) )
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

	return specific_inventory_purchase_list;
}

LIST *specific_inventory_purchase_list_update_string_list(
		LIST *specific_inventory_purchase_list )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
	LIST *update_string_list = list_new();

	if ( list_rewind( specific_inventory_purchase_list ) )
	do {
		specific_inventory_purchase =
			list_get(
				specific_inventory_purchase_list );

		list_set(
			update_string_list,
			specific_inventory_purchase->update_string );

	} while ( list_next( specific_inventory_purchase_list ) );

	if ( !list_length( update_string_list ) )
	{
		list_free( update_string_list );
		update_string_list = NULL;
	}

	return update_string_list;
}
