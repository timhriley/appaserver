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
#include "entity.h"
#include "security.h"
#include "optional_column.h"
#include "sale.h"
#include "inventory_sale.h"

INVENTORY_SALE *inventory_sale_new(
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

INVENTORY_SALE *inventory_sale_parse( char *input )
{
	INVENTORY_SALE *inventory_sale;
	char inventory_name[ 128 ];
	char buffer[ 128 ];

	if ( !input || !*input ) return NULL;

	piece( inventory_name, SQL_DELIMITER, input, 0 );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	inventory_sale = inventory_sale_new( strdup( inventory_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) inventory_sale->quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) inventory_sale->retail_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) inventory_sale->discount_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) inventory_sale->extended_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		inventory_sale->cost_of_goods_sold = atof( buffer );

	inventory_sale->sale_extended_price =
		SALE_EXTENDED_PRICE(
			inventory_sale->retail_price,
			inventory_sale->quantity,
			inventory_sale->discount_amount );

	return inventory_sale;
}

char *inventory_sale_update_system_string(
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

char *inventory_sale_update_string(
		char *primary_data_string,
		double sale_extended_price )
{
	char update_string[ 1024 ];

	if ( !primary_data_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"primary_data_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		update_string,
		sizeof ( update_string ),
	 	"%s^extended_price^%.2lf\n",
		primary_data_string,
		sale_extended_price );

	return strdup( update_string );
}

LIST *inventory_sale_list(
		const char *inventory_sale_select,
		const char *inventory_sale_table,
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
	INVENTORY_SALE *inventory_sale;

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
			(char *)inventory_sale_select,
			(char *)inventory_sale_table,
			where );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		/* Shouldn't fail */
		/* -------------- */
		inventory_sale = inventory_sale_parse( input );

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

double inventory_sale_CGS_total( LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double total = 0.0;

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale = list_get( inventory_sale_list );
		total += inventory_sale->cost_of_goods_sold;

	} while( list_next( inventory_sale_list ) );

	return total;
}

LIST *inventory_sale_primary_key_list(
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
		const char delimiter,
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
			delimiter,
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
			delimiter,
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

INVENTORY_SALE *inventory_sale_fetch(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *system_string;
	char *input;
	INVENTORY_SALE *inventory_sale;

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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_primary_where(
			SALE_INVENTORY_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_name,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			INVENTORY_SALE_SELECT,
			INVENTORY_SALE_TABLE,
			primary_where );

	free( primary_where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			system_string );

	free( system_string );

	if ( !input ) return NULL;

	/* -------------- */
	/* Shouldn't fail */
	/* -------------- */
	inventory_sale = inventory_sale_parse( input );

	inventory_sale->primary_data_string =
		inventory_sale_primary_data_string(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			inventory_sale->inventory_name,
			fund_boolean,
			contact_key_boolean );

	inventory_sale->update_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_update_string(
			inventory_sale->primary_data_string,
			inventory_sale->sale_extended_price );

	inventory_sale->primary_key_list =
		inventory_sale_primary_key_list(
			SALE_INVENTORY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	inventory_sale->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_sale_update_system_string(
			INVENTORY_SALE_TABLE,
			inventory_sale->primary_key_list );

	return inventory_sale;
}

