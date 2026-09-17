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
#include "optional_column.h"
#include "sale.h"
#include "purchase.h"
#include "inventory.h"
#include "inventory_sale.h"
#include "inventory_purchase.h"

INVENTORY_PURCHASE *inventory_purchase_new( char *inventory_name )
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

		if ( float_dollar_virtually_same( 
			inventory_purchase->
				inventory_purchase_extended_cost,
			0.0 ) )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
				"inventory_purchase->extended_cost is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		total += inventory_purchase->inventory_purchase_extended_cost;

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

INVENTORY_PURCHASE *inventory_purchase_parse(
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	char inventory_name[ 128 ];
	char buffer[ 128 ];
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !input || !*input ) return NULL;

	/* See inventory_purchase_list_select() */
	/* ------------------------------------ */
	piece( inventory_name, SQL_DELIMITER, input, 1 );

	inventory_purchase =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_purchase_new(
			strdup( inventory_name ) );

	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer ) inventory_purchase->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		inventory_purchase->purchase_date_time =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) inventory_purchase->ordered_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) inventory_purchase->arrived_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) inventory_purchase->slippage_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) inventory_purchase->unit_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) inventory_purchase->extended_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) inventory_purchase->cost_basis = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) inventory_purchase->quantity_on_hand = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer ) inventory_purchase->average_unit_cost = atof( buffer );

	if ( fund_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 11 );
		if ( *buffer )
			inventory_purchase->fund_name =
				strdup( buffer );
	}

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 12 );
		if ( *buffer )
			inventory_purchase->contact_key =
				strdup( buffer );
	}

	inventory_purchase->inventory_purchase_extended_cost =
		INVENTORY_PURCHASE_EXTENDED_COST(
			inventory_purchase->ordered_quantity,
			inventory_purchase->unit_cost );

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
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *inventory_name,
		double extended_cost,
		double inventory_purchase_extended_cost,
		double cost_basis,
		double cost_basis_amount,
		double average_unit_cost,
		double inventory_purchase_average_unit_cost )
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

	if ( !float_virtually_same(
		average_unit_cost,
		inventory_purchase_average_unit_cost ) )
	{
		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"average_unit_cost" /* column_name */,
				inventory_purchase_average_unit_cost,
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
		char *where )
{
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	INVENTORY_PURCHASE *inventory_purchase;

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

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_list_select(
			inventory_purchase_select,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_list_system_string(
			select,
			inventory_purchase_table,
			where,
			INVENTORY_ARRIVED_COLUMN
				/* For order clause */ );


	free( select );

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
			inventory_purchase_parse(
				fund_boolean,
				contact_key_boolean,
				input );

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

LIST *inventory_purchase_list_update_string_list(
		LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *update_string_list = list_new();

	if ( list_rewind( inventory_purchase_list ) )
	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		list_set_list(
			update_string_list,
			inventory_purchase->update_string_list );

	} while ( list_next( inventory_purchase_list ) );

	if ( !list_length( update_string_list ) )
	{
		list_free( update_string_list );
		update_string_list = NULL;
	}

	return update_string_list;
}

void inventory_purchase_list_set_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *inventory_purchase_list
			/* Set each update_string_list */ )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !full_name
	||   !purchase_date_time )
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


	if ( list_rewind( inventory_purchase_list ) )
	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( !inventory_purchase->cost_basis_inventory )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"inventory_purchase->cost_basis_inventory is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		inventory_purchase->update_string_list =
			inventory_purchase_update_string_list(
				sql_delimiter,
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				predictive_fund_boolean,
				entity_contact_key_boolean,
				inventory_purchase->inventory_name,
				inventory_purchase->extended_cost,
				inventory_purchase->
					inventory_purchase_extended_cost,
				inventory_purchase->cost_basis,
				inventory_purchase->
					cost_basis_inventory->
					cost_basis_amount,
				inventory_purchase->average_unit_cost,
				inventory_purchase->
					inventory_purchase_average_unit_cost );

	} while ( list_next( inventory_purchase_list ) );
}

void inventory_purchase_list_set_average_unit_cost(
		LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( list_rewind( inventory_purchase_list ) )
	do {

		inventory_purchase =
			list_get(
				inventory_purchase_list );

		if ( !inventory_purchase->cost_basis_inventory )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"inventory_purchase->cost_basis_inventory is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		inventory_purchase->inventory_purchase_average_unit_cost =
			inventory_purchase_average_unit_cost(
				inventory_purchase->ordered_quantity,
				inventory_purchase->
					cost_basis_inventory->
					cost_basis_amount );

	} while ( list_next( inventory_purchase_list ) );
}

double inventory_purchase_average_unit_cost(
		int ordered_quantity,
		double cost_basis_amount )
{
	if ( !ordered_quantity ) return 0.0;

	return
	cost_basis_amount / (double)ordered_quantity;
}

char *inventory_purchase_cost_where(
		const char *inventory_purchase_table,
		const char *sale_inventory_column,
		const char *inventory_arrived_column,
		char *inventory_name,
		char *arrived_date_time )
{
	char cost_where[ 1024 ];
	char *ptr = cost_where;
	char *prior_date_time;

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

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	if ( !arrived_date_time ) return NULL;

	ptr += sprintf( ptr,
		"%s = '%s'",
		sale_inventory_column,
		inventory_name );

	prior_date_time =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_purchase_prior_date_time(
			inventory_purchase_table,
			sale_inventory_column,
			inventory_arrived_column,
			inventory_name,
			arrived_date_time );

	if ( prior_date_time )
	{
		ptr += sprintf( ptr,
			" and arrived_date_time >= '%s'",
			prior_date_time );

		free( prior_date_time );
	}

	return strdup( cost_where );
}

char *inventory_purchase_prior_date_time(
		const char *inventory_purchase_table,
		const char *sale_inventory_column,
		const char *inventory_arrived_column,
		char *inventory_name,
		char *arrived_date_time )
{
	char system_string[ 1024 ];
	char where[ 512 ];

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

	snprintf(
		where,
		sizeof ( where ),
		"%s = '%s' and "
		"%s is not null and "
		"%s < '%s'",
		sale_inventory_column,
		inventory_name,
		inventory_arrived_column,
		inventory_arrived_column,
		arrived_date_time );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh 'max( arrived_date_time )' %s \"%s\"",
		inventory_purchase_table,
		where );

	return string_system_input( system_string );
}

char *inventory_purchase_list_select(
		const char *inventory_purchase_select,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)inventory_purchase_select /* base_string */,
			PREDICTIVE_FUND_COLUMN /* component */,
			0 /* not escape_boolean */,
			fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			ENTITY_CONTACT_KEY_COLUMN /* component */,
			0 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string;
}

char *inventory_purchase_list_system_string(
		char *inventory_purchase_list_select,
		const char *inventory_purchase_table,
		char *where,
		const char *inventory_arrived_column )
{
	char system_string[ 1024 ];

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

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" %s",
		inventory_purchase_list_select,
		inventory_purchase_table,
		where,
		inventory_arrived_column );

	return strdup( system_string );
}

