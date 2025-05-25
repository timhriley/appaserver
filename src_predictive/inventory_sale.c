/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.c			*/
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
#include "inventory_sale.h"

INVENTORY_SALE *inventory_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !inventory_name )
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

	inventory_sale = inventory_sale_calloc();

	inventory_sale->full_name = full_name;
	inventory_sale->street_address = street_address;
	inventory_sale->sale_date_time = sale_date_time;
	inventory_sale->inventory_name = inventory_name;

	return inventory_sale;
}

INVENTORY_SALE *inventory_sale_calloc( void )
{
	INVENTORY_SALE *inventory_sale;

	if ( ! ( inventory_sale =
			calloc( 1,
				sizeof ( INVENTORY_SALE ) ) ) )
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

	return inventory_sale;
}

INVENTORY_SALE *inventory_sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input )
{
	INVENTORY_SALE *inventory_sale;
	char inventory_name[ 128 ];
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

	inventory_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_sale_new(
			full_name,
			street_address,
			sale_date_time,
			strdup( inventory_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		inventory_sale->quantity =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		inventory_sale->retail_price =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		inventory_sale->discount_amount =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		inventory_sale->extended_price =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		inventory_sale->cost_of_goods_sold =
			atof( piece_buffer );

	return inventory_sale;
}

char *inventory_sale_update_system_string(
		const char *inventory_sale_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"inventory_name";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		inventory_sale_table,
		key );

	return strdup( system_string );
}

void inventory_sale_update(
		const char *inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		double sale_extended_price )
{
	char *system_string;
	FILE *pipe;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !inventory_name )
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
		inventory_sale_update_system_string(
			inventory_sale_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	free( system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^extended_price^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		inventory_name,
		sale_extended_price );

	pclose( pipe );
}

LIST *inventory_sale_list(
		const char *inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_SALE *inventory_sale;

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
			INVENTORY_SALE_SELECT,
			(char *)inventory_sale_table,
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
		inventory_sale =
			inventory_sale_parse(
				full_name,
				street_address,
				sale_date_time,
				input );

		if ( !inventory_sale )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
				"inventory_sale_parse(%s) returned empty.",
				input );

			pclose( input_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, inventory_sale );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

double inventory_sale_total( LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double total = 0.0;

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale = list_get( inventory_sale_list );

		total += inventory_sale->extended_price;

	} while( list_next( inventory_sale_list ) );

	return total;
}

INVENTORY_SALE *inventory_sale_seek(
		LIST *inventory_sale_list,
		char *inventory_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( !inventory_name )
	{
		char message[ 128 ];

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

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale =
			list_get(
				inventory_sale_list );

		if ( strcmp(
			inventory_name,
			inventory_sale->inventory_name ) == 0 )
		{
			return inventory_sale;
		}

	} while ( list_next( inventory_sale_list ) );

	return NULL;
}

