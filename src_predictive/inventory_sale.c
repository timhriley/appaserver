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
#include "float.h"
#include "sql.h"
#include "entity.h"
#include "security.h"
#include "optional_column.h"
#include "sale.h"
#include "inventory_average_cost.h"
#include "inventory_purchase.h"
#include "inventory_sale.h"

INVENTORY_SALE *inventory_sale_new( char *inventory_name )
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

	inventory_sale = inventory_sale_calloc();
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
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	INVENTORY_SALE *inventory_sale;
	char inventory_name[ 128 ];
	char buffer[ 128 ];
	int piece_offset;

	if ( !input || !*input ) return NULL;

	/* See inventory_sale_list_select() */
	/* -------------------------------- */
	piece( inventory_name, SQL_DELIMITER, input, 2 );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	inventory_sale = inventory_sale_new( strdup( inventory_name ) );

	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer ) inventory_sale->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) inventory_sale->sale_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) inventory_sale->quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) inventory_sale->retail_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) inventory_sale->discount_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) inventory_sale->extended_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer )
		inventory_sale->cost_of_goods_sold = atof( buffer );

	piece_offset = 8;

	if ( fund_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, piece_offset++ );
		if ( *buffer )
			inventory_sale->fund_name =
				strdup( buffer );
	}

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, piece_offset );
		if ( *buffer )
			inventory_sale->contact_key =
				strdup( buffer );
	}

	inventory_sale->sale_extended_price =
		SALE_EXTENDED_PRICE(
			inventory_sale->retail_price,
			inventory_sale->quantity,
			inventory_sale->discount_amount );

	inventory_sale->inventory_average =
		inventory_average_new(
			inventory_sale->inventory_name,
			(char *)0 /* arrived_date_time */,
			inventory_sale->sale_date_time );

	inventory_sale->update_string_list =
		inventory_sale_update_string_list(
			SQL_DELIMITER,
			inventory_sale->fund_name,
			inventory_sale->full_name,
			inventory_sale->contact_key,
			inventory_sale->sale_date_time,
			inventory_sale->inventory_name,
			fund_boolean,
			contact_key_boolean,
			inventory_sale->extended_price,
			inventory_sale->sale_extended_price,
			(inventory_sale->inventory_average)
				? inventory_sale->
					inventory_average->
					inventory_average_cost_list
				: NULL );

	return inventory_sale;
}

char *inventory_sale_list_update_system_string(
		const char *inventory_sale_table,
		LIST *primary_key_list )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	sale_update_system_string(
		inventory_sale_table,
		primary_key_list );
}

LIST *inventory_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double extended_price,
		double sale_extended_price,
		LIST *inventory_average_cost_list )
{
	LIST *list = list_new();
	char *primary_data_string;
	char *update_string;
	LIST *cost_quantity_update_string_list;

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

	if ( !float_money_virtually_same(
		extended_price,
		sale_extended_price ) )
	{
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
	}

	free( primary_data_string );

	cost_quantity_update_string_list =
		inventory_sale_cost_quantity_update_string_list(
			sql_delimiter,
			fund_boolean,
			contact_key_boolean,
			inventory_average_cost_list );

	list_set_list(
		list,
		cost_quantity_update_string_list );

	list_free_container( cost_quantity_update_string_list );

	return list;
}

double inventory_sale_list_extended_total( LIST *inventory_sale_list )
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

double inventory_sale_list_CGS_total( LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	INVENTORY_AVERAGE_COST *inventory_average_cost;
	double total = 0.0;

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale = list_get( inventory_sale_list );

		if ( !inventory_sale->inventory_average )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
				"inventory_sale->inventory_average is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_rewind(
			inventory_sale->
				inventory_average->
				inventory_average_cost_list ) )
		do {
			inventory_average_cost =
				list_get(
					inventory_sale->
						inventory_average->
						inventory_average_cost_list );

			if ( inventory_average_cost->inventory_sale )
			{
				total +=
					inventory_average_cost->
						cost_of_goods_sold;
			}

		} while ( list_next(
				inventory_sale->
					inventory_average->
					inventory_average_cost_list ) );

	} while( list_next( inventory_sale_list ) );

	return total;
}

LIST *inventory_sale_list_primary_key_list(
		const char *sale_inventory_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list;

	list =
		sale_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( list, (char *)sale_inventory_column );

	return list;
}

char *inventory_sale_join(
		const char *inventory_sale_table,
		const char *foreign_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *sale_date_time_column,
		boolean contact_key_boolean )
{
	char sale_join[ 1024 ];
	char *join;


	join =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_join(
			inventory_sale_table /* ENTITY_TABLE */,
			foreign_table,
			entity_full_name_column,
			entity_contact_key_column,
			contact_key_boolean );

	snprintf(
		sale_join,
		sizeof ( sale_join ),
		"%s and %s.%s = %s.%s",
		join,
		inventory_sale_table,
		sale_date_time_column,
		foreign_table,
		sale_date_time_column );

	return strdup( sale_join );
}

char *inventory_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_data_string;
	OPTIONAL_COLUMN *optional_column;

	if ( !full_name
	||   !sale_date_time
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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	optional_column =
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			primary_data_string /* base */,
			inventory_name /* component */,
			1 /* escape_boolean */,
			1 /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string /* heap memory */;
}

char *inventory_sale_primary_where(
		const char *sale_inventory_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char where[ 1024 ];
	char *primary_where;
	char *escape;

	if ( !full_name
	||   !sale_date_time
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
		sale_primary_where(
			SALE_DATE_TIME_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	escape =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_escape(
			inventory_name );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		primary_where,
		sale_inventory_column,
		escape );

	free( escape );

	return strdup( where );
}

char *inventory_sale_cost_where(
		const char *inventory_sale_table,
		const char *sale_inventory_column,
		const char *transaction_date_time_column,
		char *inventory_name,
		char *sale_date_time )
{
	char *purchase_cost_where;
	char cost_where[ 1024 ];

	if ( !inventory_name )
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

	if ( !sale_date_time ) return NULL;

	purchase_cost_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_purchase_cost_where(
			inventory_sale_table
				/* inventory_purchase_table */,
			sale_inventory_column,
			transaction_date_time_column
				/* inventory_arrived_column */,
			inventory_name,
			sale_date_time
				/* arrived_date_time */ );

	if ( !purchase_cost_where ) return NULL;

	snprintf(
		cost_where,
		sizeof ( cost_where ),
		"%s and completed_date_time is not null",
		purchase_cost_where );

	free( purchase_cost_where );

	return strdup( cost_where );
}

INVENTORY_SALE_LIST *inventory_sale_list_calloc( void )
{
	INVENTORY_SALE_LIST *inventory_sale_list;

	if ( ! ( inventory_sale_list =
			calloc( 1,
				sizeof ( INVENTORY_SALE_LIST ) ) ) )
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

	return inventory_sale_list;
}

char *inventory_sale_list_select(
		const char *inventory_sale_select,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	inventory_purchase_list_select(
		inventory_sale_select /* inventory_purchase_select */,
		fund_boolean,
		contact_key_boolean );
}

char *inventory_sale_list_system_string(
		char *inventory_sale_list_select,
		const char *inventory_sale_table,
		char *where,
		const char *sale_completed_column )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	inventory_purchase_list_system_string(
		inventory_sale_list_select /* inventory_purchase_list_select */,
		inventory_sale_table /* inventory_purchase_table */,
		where,
		sale_completed_column /* inventory_arrived_column */ );
}

INVENTORY_SALE_LIST *inventory_sale_list_new(
		const char *inventory_sale_select,
		const char *inventory_sale_table,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *where )
{
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_SALE *inventory_sale;
	INVENTORY_SALE_LIST *inventory_sale_list;

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

	inventory_sale_list = inventory_sale_list_calloc();

	inventory_sale_list->list = list_new();

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_list_select(
			inventory_sale_select,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_list_system_string(
			select,
			inventory_sale_table,
			where,
			SALE_COMPLETED_DATE_COLUMN
				/* For order clause */ );

	free( select );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		inventory_sale =
			/* -------------- */
			/* Should succeed */
			/* -------------- */
			inventory_sale_parse(
				fund_boolean,
				contact_key_boolean,
				input );

		list_set( inventory_sale_list->list, inventory_sale );
	}

	pclose( input_pipe );

	if ( !list_length( inventory_sale_list->list ) )
		return inventory_sale_list;

	inventory_sale_list->primary_key_list =
		inventory_sale_list_primary_key_list(
			SALE_INVENTORY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	inventory_sale_list->update_system_string =
		inventory_sale_list_update_system_string(
			inventory_sale_table,
			inventory_sale_list->primary_key_list );

	inventory_sale_list->update_string_list =
		inventory_sale_list_update_string_list(
			inventory_sale_list->list /* inventory_sale_list */ );

	inventory_sale_list->extended_total =
		inventory_sale_list_extended_total(
			inventory_sale_list->list /* inventory_sale_list */ );

	inventory_sale_list->CGS_total =
		inventory_sale_list_CGS_total(
		inventory_sale_list->list /* inventory_sale_list */ );

	return inventory_sale_list;
}

LIST *inventory_sale_cost_quantity_update_string_list(
		const char sql_delimiter,
		boolean fund_boolean,
		boolean contact_key_boolean,
		LIST *inventory_average_cost_list )
{

	INVENTORY_AVERAGE_COST *inventory_average_cost;
	LIST *list = list_new();
	char *primary_data_string;
	char *update_string;

	if ( list_rewind( inventory_average_cost_list ) )
	do {
		inventory_average_cost =
			list_get(
				inventory_average_cost_list );

		if ( !inventory_average_cost->inventory_sale ) continue;

		primary_data_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			inventory_sale_primary_data_string(
				sql_delimiter,
				inventory_average_cost->
					inventory_sale->
					fund_name,
				inventory_average_cost->
					inventory_sale->
					full_name,
				inventory_average_cost->
					inventory_sale->
					contact_key,
				inventory_average_cost->
					inventory_sale->
					sale_date_time,
				inventory_average_cost->
					inventory_sale->
					inventory_name,
				fund_boolean,
				contact_key_boolean );

		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_integer_string(
				sql_delimiter,
				primary_data_string,
				"quantity_on_hand" /* column_name */,
				inventory_average_cost->
					quantity_on_hand /* integer */,
				1 /* set_boolean */ );

		list_set( list, update_string );
		free( update_string );

		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"cost_of_goods_sold" /* column_name */,
				inventory_average_cost->
					cost_of_goods_sold /* money */,
				1 /* set_boolean */ );

		list_set( list, update_string );
		free( update_string );
		free( primary_data_string );

	} while ( list_next( inventory_average_cost_list ) );
	
	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *inventory_sale_list_update_string_list( LIST *inventory_sale_list )
{
	LIST *update_string_list = list_new();
	INVENTORY_SALE *inventory_sale;

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale = list_get( inventory_sale_list );

		list_set_list(
			update_string_list,
			inventory_sale->update_string_list );

	} while ( list_next( inventory_sale_list ) );

	if ( !list_length( update_string_list ) )
	{
		list_free( update_string_list );
		update_string_list = NULL;
	}

	return update_string_list;
}

