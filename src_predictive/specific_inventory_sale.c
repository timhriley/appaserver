/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_sale.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sql.h"
#include "security.h"
#include "optional_column.h"
#include "sale.h"
#include "inventory_sale.h"
#include "inventory_purchase.h"
#include "specific_inventory_sale.h"

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_new(
		char *inventory_name,
		char *serial_key )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !inventory_name
	||   !serial_key )
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

	specific_inventory_sale = specific_inventory_sale_calloc();

	specific_inventory_sale->inventory_name = inventory_name;
	specific_inventory_sale->serial_key = serial_key;

	return specific_inventory_sale;
}

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_calloc( void )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( ! ( specific_inventory_sale =
			calloc( 1,
				sizeof ( SPECIFIC_INVENTORY_SALE ) ) ) )
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

	return specific_inventory_sale;
}

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_parse(
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	char inventory_name[ 128 ];
	char serial_key[ 128 ];
	char buffer[ 128 ];
	int piece_offset;

	if ( !input || !*input ) return NULL;

	piece( inventory_name, SQL_DELIMITER, input, 2 );
	piece( serial_key, SQL_DELIMITER, input, 3 );

	specific_inventory_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		specific_inventory_sale_new(
			strdup( inventory_name ),
			strdup( serial_key ) );

	piece( buffer, SQL_DELIMITER, input, 0 );
	specific_inventory_sale->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	specific_inventory_sale->sale_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		specific_inventory_sale->retail_price =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		specific_inventory_sale->unit_cost =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
		specific_inventory_sale->discount_amount =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer )
		specific_inventory_sale->extended_price =
			atof( buffer );

	piece_offset = 8;

	if ( fund_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, piece_offset++ );
		if ( *buffer )
			specific_inventory_sale->fund_name =
				strdup( buffer );
	}

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, piece_offset++ );
		if ( *buffer )
			specific_inventory_sale->contact_key =
				strdup( buffer );
	}

	specific_inventory_sale->sale_extended_price =
		SALE_EXTENDED_PRICE(
			specific_inventory_sale->retail_price,
			1 /* quantity */,
			specific_inventory_sale->discount_amount );

	specific_inventory_sale->update_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_update_string(
			SQL_DELIMITER,
			fund_boolean,
			contact_key_boolean,
			specific_inventory_sale->fund_name,
			specific_inventory_sale->full_name,
			specific_inventory_sale->contact_key,
			specific_inventory_sale->sale_date_time,
			specific_inventory_sale->inventory_name,
			specific_inventory_sale->serial_key,
			specific_inventory_sale->sale_extended_price );

	return specific_inventory_sale;
}

double specific_inventory_sale_list_extended_total(
		LIST *specific_inventory_sale_list )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	double total = 0.0;

	if ( list_rewind( specific_inventory_sale_list ) )
	do {
		specific_inventory_sale =
			list_get(
				specific_inventory_sale_list );

		total += specific_inventory_sale->extended_price;

	} while( list_next( specific_inventory_sale_list ) );

	return total;
}

double specific_inventory_sale_list_CGS_total(
		LIST *specific_inventory_sale_list )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	double total = 0.0;

	if ( list_rewind( specific_inventory_sale_list ) )
	do {
		specific_inventory_sale =
			list_get(
				specific_inventory_sale_list );

		total += specific_inventory_sale->unit_cost;

	} while( list_next( specific_inventory_sale_list ) );

	return total;
}

char *specific_inventory_sale_primary_data_string(
		const char sql_delimiter,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key )
{
	char *primary_data_string;
	OPTIONAL_COLUMN *optional_column;

	if ( !full_name
	||   !sale_date_time
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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_name,
			fund_boolean,
			contact_key_boolean );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			primary_data_string /* base */,
			serial_key /* component */,
			1 /* escape_boolean */,
			1 /* set_boolean */ );

	free( primary_data_string );

	return optional_column->return_string /* heap memory */;
}

LIST *specific_inventory_sale_list_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		inventory_sale_list_primary_key_list(
			sale_inventory_column,
			fund_boolean,
			contact_key_boolean );

	list_set(
		primary_key_list,
		(char *)sale_serial_key_column );

	return primary_key_list;
}

char *specific_inventory_sale_update_string(
		const char sql_delimiter,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		double sale_extended_price )
{
	char *primary_data_string;
	char *update_string;

	if ( !full_name
	||   !sale_date_time
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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_primary_data_string(
			sql_delimiter,
			fund_boolean,
			contact_key_boolean,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_name,
			serial_key );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"extended_price" /* column_name */,
			sale_extended_price /* money */,
			1 /* set_boolean */ );

	return update_string;
}

SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list_new(
		const char *specific_inventory_sale_select,
		const char *specific_inventory_sale_table,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *where )
{
	SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list;
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	specific_inventory_sale_list = specific_inventory_sale_list_calloc();

	specific_inventory_sale_list->list = list_new();

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_list_select(
			specific_inventory_sale_select,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_list_system_string(
			select,
			specific_inventory_sale_table,
			where,
			SALE_DATE_TIME_COLUMN
				/* For order clause */ );

	free( select );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		specific_inventory_sale =
			/* -------------- */
			/* Should succeed */
			/* -------------- */
			specific_inventory_sale_parse(
				fund_boolean,
				contact_key_boolean,
				input );

		list_set(
			specific_inventory_sale_list->list,
			specific_inventory_sale );
	}

	pclose( input_pipe );

	if ( !list_length( specific_inventory_sale_list->list ) )
		return specific_inventory_sale_list;

	specific_inventory_sale_list->primary_key_list =
		specific_inventory_sale_list_primary_key_list(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	specific_inventory_sale_list->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_list_update_system_string(
			SPECIFIC_INVENTORY_SALE_TABLE,
			specific_inventory_sale_list->primary_key_list );

	specific_inventory_sale_list->update_string_list =
		specific_inventory_sale_list_update_string_list(
			specific_inventory_sale_list->list
				/* specific_inventory_sale_list */ );

	specific_inventory_sale_list->extended_total =
		specific_inventory_sale_list_extended_total(
			specific_inventory_sale_list->list
				/* specific_inventory_sale_list */ );

	specific_inventory_sale_list->CGS_total =
		specific_inventory_sale_list_CGS_total(
			specific_inventory_sale_list->list
				/* specific_inventory_sale_list */ );

	return specific_inventory_sale_list;
}

SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list_calloc( void )
{
	SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list;

	if ( ! ( specific_inventory_sale_list =
			calloc( 1,
				sizeof ( SPECIFIC_INVENTORY_SALE_LIST ) ) ) )
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

	return specific_inventory_sale_list;
}

char *specific_inventory_sale_list_select(
		const char *specific_inventory_sale_select,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	inventory_purchase_list_select(
		specific_inventory_sale_select /* inventory_purchase_select */,
		fund_boolean,
		contact_key_boolean );
}

char *specific_inventory_sale_list_system_string(
		char *specific_inventory_sale_list_select,
		const char *specific_inventory_sale_table,
		char *where,
		const char *sale_date_time_column )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	inventory_purchase_list_system_string(
		specific_inventory_sale_list_select
			/* inventory_purchase_list_select */,
		specific_inventory_sale_table
			/* inventory_purchase_table */,
		where,
		sale_date_time_column
			/* purchase_date_time_column */ );
}

LIST *specific_inventory_sale_list_update_string_list(
		LIST *specific_inventory_sale_list )
{
	LIST *list = list_new();
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( list_rewind( specific_inventory_sale_list ) )
	do {
		specific_inventory_sale =
			list_get(
				specific_inventory_sale_list );

		list_set( list, specific_inventory_sale->update_string );

	} while ( list_next( specific_inventory_sale_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *specific_inventory_sale_list_update_system_string(
		const char *specific_inventory_sale_table,
		LIST *primary_key_list )
{
	return
	sale_update_system_string(
		specific_inventory_sale_table,
		primary_key_list );
}
