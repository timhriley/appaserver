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
#include "sql.h"
#include "security.h"
#include "sale.h"
#include "purchase.h"
#include "fixed_asset_purchase.h"
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

double specific_inventory_purchase_total(
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

SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *where;
	char *system_string;
	char *input;
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;

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
		specific_inventory_purchase_primary_where(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			inventory_name,
			serial_key,
			primary_where );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			SPECIFIC_INVENTORY_PURCHASE_SELECT,
			SPECIFIC_INVENTORY_PURCHASE_TABLE,
			where );

	free( where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	free( system_string );

	if ( !input ) return NULL;

	specific_inventory_purchase =
		specific_inventory_purchase_parse(
			input );

	specific_inventory_purchase->cost_basis =
		specific_inventory_purchase_cost_basis(
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			specific_inventory_purchase->unit_cost );

	specific_inventory_purchase->update_string_list =
		specific_inventory_purchase_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			inventory_name,
			serial_key,
			fund_boolean,
			contact_key_boolean,
			specific_inventory_purchase->cost_basis );

	specific_inventory_purchase->primary_key_list =
		specific_inventory_purchase_primary_key_list(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	specific_inventory_purchase->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_purchase_update_system_string(
			SPECIFIC_INVENTORY_PURCHASE_TABLE,
			specific_inventory_purchase->primary_key_list );

	return specific_inventory_purchase;
}

char *specific_inventory_purchase_update_system_string(
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
	if ( *buffer )
		specific_inventory_purchase->unit_cost =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		specific_inventory_purchase->retail_price =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		specific_inventory_purchase->cost_basis =
			atof( buffer );

	return specific_inventory_purchase;
}

LIST *specific_inventory_purchase_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		inventory_purchase_primary_key_list(
			sale_inventory_column,
			fund_boolean,
			contact_key_boolean );

	list_set( primary_key_list, sale_serial_key_column );

	return primary_key_list;
}

LIST *specific_inventory_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double cost_basis )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

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

	primary_data_string =
		/* -------------------- */
		/* Borrow SALE’s	*/
		/* Returns heap memory	*/
		/* -------------------- */
		specific_inventory_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time /* sale_date_time */,
			inventory_name,
			serial_key,
			predictive_fund_boolean,
			entity_contact_key_boolean );

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

double specific_inventory_purchase_cost_basis(
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

char *specific_inventory_purchase_primary_where(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		char *inventory_name,
		char *serial_key,
		char *purchase_primary_where )
{
	char *primary_where;
	char *escape;
	char purchase_primary_where[ 256 ];

	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_primary_where(
			sale_inventory_column,
			inventory_name,
			purchase_primary_where );

	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	escape = security_escape( serial_key );

	snprintf(
		purchase_primary_where,
		sizeof ( purchase_primary_where ),
		"%s and %s = '%s'",
		primary_where,
		sale_serial_key,
		security_escape() );

	free( primary_where );
	free( escape );

	return strdup( purchase_primary_where );
}

LIST *specific_inventory_purchase_list(
		const char *specific_inventory_purchase_select,
		const char *specific_inventory_purchase_table,
		char *purchase_primary_where )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
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
			specific_inventory_purchase_select,
			specific_inventory_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input[], input_pipe, sizeof ( input ) ) )
	{
		specific_inventory_purchase  =
			/* -------------- */
			/* Should succeed */
			/* -------------- */
			specific_inventory_purchase_parse(
				input );

		list_set( list, specific_inventory_purchase );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

