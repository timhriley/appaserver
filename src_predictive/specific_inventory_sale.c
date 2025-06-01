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
#include "date.h"
#include "sql.h"
#include "sale.h"
#include "specific_inventory_sale.h"

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		char *serial_label )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !inventory_name
	||   !serial_label )
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

	specific_inventory_sale->full_name = full_name;
	specific_inventory_sale->street_address = street_address;
	specific_inventory_sale->sale_date_time = sale_date_time;
	specific_inventory_sale->inventory_name = inventory_name;
	specific_inventory_sale->serial_label = serial_label;

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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
	char inventory_name[ 128 ];
	char serial_label[ 128 ];
	char piece_buffer[ 128 ];

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !input
	||   !*input )
	{
		return NULL;
	}

	piece( inventory_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );

	specific_inventory_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		specific_inventory_sale_new(
			full_name,
			street_address,
			sale_date_time,
			strdup( inventory_name ),
			strdup( serial_label ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		specific_inventory_sale->retail_price =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		specific_inventory_sale->discount_amount =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		specific_inventory_sale->extended_price =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		specific_inventory_sale->cost_of_goods_sold =
			atof( piece_buffer );

	specific_inventory_sale->sale_extended_price =
		SALE_EXTENDED_PRICE(
			specific_inventory_sale->retail_price,
			1 /* quantity */,
			specific_inventory_sale->discount_amount );

	return specific_inventory_sale;
}

char *specific_inventory_sale_update_system_string(
		const char *specific_inventory_sale_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"inventory_name"
		"serial_label";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		specific_inventory_sale_table,
		key );

	return strdup( system_string );
}

void specific_inventory_sale_update(
		const char *specific_inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		char *serial_label,
		double sale_extended_price )
{
	char *system_string;
	FILE *pipe;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !inventory_name
	||   !serial_label )
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
		specific_inventory_sale_update_system_string(
			specific_inventory_sale_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	free( system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^extended_price^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		inventory_name,
		serial_label,
		sale_extended_price );

	pclose( pipe );
}

LIST *specific_inventory_sale_list(
		const char *specific_inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !full_name
	||   !street_address
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
			full_name,
			street_address,
			sale_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			SPECIFIC_INVENTORY_SALE_SELECT,
			(char *)specific_inventory_sale_table,
			where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		specific_inventory_sale =
			specific_inventory_sale_parse(
				full_name,
				street_address,
				sale_date_time,
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

SPECIFIC_INVENTORY_SALE *specific_inventory_sale_seek(
		LIST *specific_inventory_sale_list,
		char *inventory_name,
		char *serial_label )
{
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	if ( !inventory_name
	||   !serial_label )
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

	if ( list_rewind( specific_inventory_sale_list ) )
	do {
		specific_inventory_sale =
			list_get(
				specific_inventory_sale_list );

		if ( strcmp(
			inventory_name,
			specific_inventory_sale->inventory_name ) == 0
		&&   strcmp(
			serial_label,
			specific_inventory_sale->serial_label ) == 0 )
		{
			return specific_inventory_sale;
		}

	} while ( list_next( specific_inventory_sale_list ) );

	return NULL;
}

