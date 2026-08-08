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
		char *input )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	char inventory_name[ 128 ];
	char serial_key[ 128 ];
	char buffer[ 128 ];

	if ( !input || !*input ) return NULL;

	piece( inventory_name, SQL_DELIMITER, input, 0 );
	piece( serial_key, SQL_DELIMITER, input, 1 );

	specific_inventory_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		specific_inventory_sale_new(
			strdup( inventory_name ),
			strdup( serial_key ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		specific_inventory_sale->retail_price =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		specific_inventory_sale->discount_amount =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		specific_inventory_sale->extended_price =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		specific_inventory_sale->cost_of_goods_sold =
			atof( buffer );

	specific_inventory_sale->sale_extended_price =
		SALE_EXTENDED_PRICE(
			specific_inventory_sale->retail_price,
			1 /* quantity */,
			specific_inventory_sale->discount_amount );

	return specific_inventory_sale;
}

LIST *specific_inventory_sale_list(
		const char *specific_inventory_sale_select,
		const char *specific_inventory_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !full_name
	||   !sale_date_time )
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

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)specific_inventory_sale_select,
			(char *)specific_inventory_sale_table,
			where );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		specific_inventory_sale =
			specific_inventory_sale_parse(
				input );

		if ( !specific_inventory_sale )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
			"specific_inventory_sale_parse(%s) returned empty.",
				input );

			pclose( input_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, specific_inventory_sale );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

double specific_inventory_sale_total( LIST *specific_inventory_sale_list )
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

double specific_inventory_sale_CGS_total( LIST *specific_inventory_sale_list )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	double total = 0.0;

	if ( list_rewind( specific_inventory_sale_list ) )
	do {
		specific_inventory_sale =
			list_get(
				specific_inventory_sale_list );

		total += specific_inventory_sale->cost_of_goods_sold;

	} while( list_next( specific_inventory_sale_list ) );

	return total;
}

char *specific_inventory_sale_primary_where(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *escape;
	char where[ 1024 ];

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

	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_primary_where(
			sale_inventory_column,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_name,
			fund_boolean,
			contact_key_boolean );


	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	escape = security_escape( serial_key );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		primary_where,
		sale_serial_key_column,
		escape );

	free( primary_where );
	free( escape );

	return strdup( where );
}

char *specific_inventory_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean )
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

	free( optional_column->prior_return_string );

	return optional_column->return_string /* heap memory */;
}

LIST *specific_inventory_sale_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		inventory_sale_primary_key_list(
			sale_inventory_column,
			fund_boolean,
			contact_key_boolean );

	list_set(
		primary_key_list,
		(char *)sale_serial_key_column );

	return primary_key_list;
}

LIST *specific_inventory_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double sale_extended_price )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

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
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
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
			"extended_price" /* column_name */,
			sale_extended_price /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	return list;
}

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *system_string;
	char *input;
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		specific_inventory_sale_primary_where(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_name,
			serial_key,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			SPECIFIC_INVENTORY_SALE_SELECT,
			SPECIFIC_INVENTORY_SALE_TABLE,
			primary_where );

	free( primary_where );

	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	free( system_string );

	if ( !input ) return NULL;

	/* -------------- */
	/* Should succeed */
	/* -------------- */
	specific_inventory_sale = specific_inventory_sale_parse( input );

	specific_inventory_sale->update_string_list =
		specific_inventory_sale_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			specific_inventory_sale->inventory_name,
			specific_inventory_sale->serial_key,
			fund_boolean,
			contact_key_boolean,
			specific_inventory_sale->sale_extended_price );

	specific_inventory_sale->primary_key_list =
		specific_inventory_sale_primary_key_list(
			SALE_INVENTORY_COLUMN,
			SALE_SERIAL_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	specific_inventory_sale->sale_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_update_system_string(
			SPECIFIC_INVENTORY_SALE_TABLE,
			specific_inventory_sale->primary_key_list );

	return specific_inventory_sale;
}

