/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "float.h"
#include "sql.h"
#include "String.h"
#include "update.h"
#include "predictive.h"
#include "inventory.h"
#include "sale.h"
#include "purchase.h"
#include "inventory_average.h"

INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *date_time_key )
{
	INVENTORY_AVERAGE *inventory_average;

	if ( !inventory_name
	||   !date_time_key )
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

	inventory_average = inventory_average_calloc();
	inventory_average->inventory_name = inventory_name;
	inventory_average->date_time_key = date_time_key;

	return inventory_average;
}

INVENTORY_AVERAGE *inventory_average_calloc( void )
{
	INVENTORY_AVERAGE *inventory_average;

	if ( ! ( inventory_average =
			calloc( 1, sizeof ( INVENTORY_AVERAGE ) ) ) )
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

	return inventory_average;
}

char *inventory_average_cost_date_time(
		char *purchase_date_time,
		char *sale_date_time )
{
	if ( purchase_date_time )
		return purchase_date_time;
	else
		return sale_date_time;
}

INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase_fetch(
		char *inventory_name,
		char *purchase_date_time,
		int ordered_quantity,
		int arrived_quantity,
		int slippage_quantity,
		double inventory_purchase_cost_basis )
{
	INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase;
	INVENTORY_AVERAGE *inventory_average;

	if ( !inventory_name
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

	if (	!ordered_quantity
	||	float_money_virtually_zero( inventory_purchase_cost_basis ) )
	{
		return NULL;
	}

	inventory_average_purchase = inventory_average_purchase_calloc();

	inventory_average_purchase->quantity_on_hand = 
		inventory_average_purchase_quantity_on_hand(
			arrived_quantity,
			slippage_quantity );

	inventory_average_purchase->unit_cost =
		inventory_average_purchase_unit_cost(
			ordered_quantity,
			inventory_purchase_cost_basis );

	inventory_average_purchase->inventory_average_date_time_key =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		inventory_average_date_time_key(
			purchase_date_time,
			(char *)0 /* sale_date_time */ );

	inventory_average_purchase->inventory_average_primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_primary_where(
			INVENTORY_COLUMN,
			INVENTORY_AVERAGE_DATE_COLUMN,
			inventory_name,
			inventory_average_purchase->
				inventory_average_date_time_key );

	inventory_average =
		inventory_average_fetch(
			INVENTORY_AVERAGE_SELECT,
			INVENTORY_AVERAGE_TABLE,
			inventory_average_purchase->
				inventory_average_primary_where );

	inventory_average_purchase->insert_boolean =
		inventory_average_purchase_insert_boolean(
			inventory_average );

	if ( inventory_average_purchase->insert_boolean )
	{
		inventory_average_purchase->insert_sql =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			inventory_average_purchase_insert_sql(
				INVENTORY_AVERAGE_TABLE,
				INVENTORY_COLUMN,
				INVENTORY_AVERAGE_DATE_COLUMN,
				inventory_name,
				inventory_average_purchase->
					inventory_average_date_time_key );
	}

	inventory_average_purchase->inventory_average_primary_key_list =
		inventory_average_primary_key_list(
			INVENTORY_COLUMN,
			INVENTORY_AVERAGE_DATE_COLUMN );

	inventory_average_purchase->predictive_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		predictive_update_system_string(
			INVENTORY_AVERAGE_TABLE /* table_name */,
			inventory_average_purchase->
				inventory_average_primary_key_list );

	inventory_average_purchase->inventory_average_primary_data_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_primary_data_string(
			SQL_DELIMITER,
			inventory_name,
			inventory_average_purchase->
				inventory_average_date_time_key );

	inventory_average_purchase->update_string_list =
		inventory_average_purchase_update_string_list(
			SQL_DELIMITER,
			purchase_date_time,
			ordered_quantity,
			arrived_quantity,
			slippage_quantity,
			inventory_purchase_cost_basis
				/* total_cost_balance */,
			inventory_average_purchase->quantity_on_hand,
			inventory_average_purchase->unit_cost
				/* average_unit_cost */,
			inventory_average_purchase->
				inventory_average_primary_data_string );

	inventory_average_purchase->
		inventory_average_prior_purchase_date_time =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			inventory_average_prior_purchase_date_time(
				INVENTORY_COLUMN,
				PURCHASE_DATE_TIME_COLUMN,
				inventory_name,
				purchase_date_time );

	return inventory_average_purchase;
}

INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase_calloc( void )
{
	INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase;

	if ( ! ( inventory_average_purchase =
			calloc( 1,
				sizeof ( INVENTORY_AVERAGE_PURCHASE ) ) ) )
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

	return inventory_average_purchase;
}

char *inventory_average_date_time_key(
		char *purchase_date_time,
		char *sale_date_time )
{
	if ( !purchase_date_time
	&&   !sale_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"both parameters are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( purchase_date_time )
		return purchase_date_time;
	else
		return sale_date_time;
}

char *inventory_average_prior_purchase_date_time(
		const char *inventory_column,
		const char *purchase_date_time_column,
		char *inventory_name,
		char *purchase_date_time )
{
	char *system_string;
	char *input;

	if ( !inventory_name
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

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_average_prior_purchase_date_system_string(
			inventory_column,
			purchase_date_time_column,
			INVENTORY_AVERAGE_TABLE,
			inventory_name,
			purchase_date_time );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input( system_string );

	free( system_string );

	return input;
}

char *inventory_average_prior_purchase_date_system_string(
		const char *inventory_column,
		const char *purchase_date_time_column,
		const char *inventory_average_table,
		char *inventory_name,
		char *purchase_date_time )
{
	char *prior_purchase_where;
	char select[ 128 ];
	char system_string[ 1024 ];

	if ( !inventory_name
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

	prior_purchase_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_prior_purchase_where(
			inventory_column,
			purchase_date_time_column,
			inventory_name,
			purchase_date_time );

	snprintf(
		select,
		sizeof ( select ),
		"max(%s)",
		purchase_date_time_column );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh '%s' %s \"%s\"",
		select,
		inventory_average_table,
		prior_purchase_where );

	return strdup( system_string );
}

char *inventory_average_prior_purchase_where(
		const char *inventory_column,
		const char *purchase_date_time_column,
		char *inventory_name,
		char *purchase_date_time )
{
	char *primary_where;
	static char where[ 256 ];

	if ( !inventory_name
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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_primary_where(
			inventory_column,
			inventory_name );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s < '%s'",
		primary_where,
		purchase_date_time_column,
		purchase_date_time );

	return where;
}

char *inventory_average_primary_where(
		const char *inventory_column,
		const char *inventory_average_date_column,
		char *inventory_name,
		char *date_time_key )
{
	char *primary_where;
	static char where[ 256 ];

	if ( !inventory_name
	||   !date_time_key )
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
		inventory_primary_where(
			inventory_column,
			inventory_name );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		primary_where,
		inventory_average_date_column,
		date_time_key );

	return where;
}

void inventory_average_list_set( LIST *inventory_average_list )
{
	INVENTORY_AVERAGE *inventory_average;
	INVENTORY_AVERAGE *inventory_average_prior = {0};

	if ( list_rewind( inventory_average_list ) )
	do {
		inventory_average =
			list_get(
				inventory_average_list );

		if ( !inventory_average_prior )
		{
			if ( !inventory_average->purchase_date_time )
			{
				char message[ 1024 ];

				snprintf(
					message,
					sizeof ( message ),
			"inventory_average_list must begin with a purchase.." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			inventory_average->inventory_average_quantity_on_hand =
				inventory_average->quantity_on_hand;

			inventory_average->inventory_average_unit_cost =
				inventory_average->average_unit_cost;

			inventory_average->
				inventory_average_total_cost_balance =
					inventory_average->total_cost_balance;

			inventory_average_prior = inventory_average;

			continue;

		} /* If !inventory_average_prior */

		if ( inventory_average->purchase_date_time )
		{
			inventory_average->inventory_average_quantity_on_hand =
			    inventory_average_list_purchase_quantity_on_hand(
				inventory_average_prior->
					quantity_on_hand
					/* prior_quantity_on_hand */,
				inventory_average->arrived_quantity,
				inventory_average->slippage_quantity );

			inventory_average->
			   inventory_average_total_cost_balance =
			     inventory_average_list_purchase_total_cost_balance(
				inventory_average_prior->
					total_cost_balance
					/* prior_total_cost_balance */,
				inventory_average->ordered_quantity,
				inventory_average->average_unit_cost );

			inventory_average->inventory_average_unit_cost =
				inventory_average_list_purchase_unit_cost(
				    inventory_average->
					 inventory_average_quantity_on_hand,
				    inventory_average->
					 inventory_average_total_cost_balance );
		}
		else
		/* Must be inventory sale */
		{
			inventory_average->inventory_average_quantity_on_hand =
				inventory_average_list_sale_quantity_on_hand(
					inventory_average_prior->
						quantity_on_hand
						/* prior_quantity_on_hand */,
					inventory_average->sold_quantity );

			inventory_average->inventory_average_unit_cost =
				inventory_average_prior->
					average_unit_cost;

			inventory_average->
			    inventory_average_total_cost_balance =
				inventory_average_list_sale_total_cost_balance(
					inventory_average->
					    inventory_average_quantity_on_hand,
					inventory_average->
					    inventory_average_unit_cost );

			inventory_average->cost_of_goods_sold =
				inventory_average_list_cost_of_goods_sold(
					inventory_average->sold_quantity,
					inventory_average->
						inventory_average_unit_cost );
		}

		inventory_average_prior = inventory_average;

	} while ( list_next( inventory_average_list ) );
}

int inventory_average_list_purchase_quantity_on_hand(
		int prior_quantity_on_hand,
		int arrived_quantity,
		int slippage_quantity )
{
	return
	prior_quantity_on_hand +
	( arrived_quantity - slippage_quantity );
}

double inventory_average_list_total_cost_balance(
		double prior_total_cost_balance,
		int ordered_quantity,
		double average_unit_cost )
{
	return
	prior_total_cost_balance +
	( (double)ordered_quantity * average_unit_cost );
}

double inventory_average_list_purchase_unit_cost(
		int quantity_on_hand,
		double total_cost_balance )
{
	if ( !quantity_on_hand ) return 0.0;

	return total_cost_balance / (double)quantity_on_hand;
}

int inventory_average_list_sale_quantity_on_hand(
		int prior_quantity_on_hand,
		int sold_quantity )
{
	return prior_quantity_on_hand - sold_quantity;
}

double inventory_average_list_sale_total_cost_balance(
		int quantity_on_hand,
		double average_unit_cost )
{
	return (double)quantity_on_hand * average_unit_cost;
}

double inventory_average_list_cost_of_goods_sold(
		int sold_quantity,
		double average_unit_cost )
{
	return (double)sold_quantity * average_unit_cost;
}

int inventory_average_purchase_quantity_on_hand(
		int arrived_quantity,
		int slippage_quantity )
{
	return arrived_quantity - slippage_quantity;
}

double inventory_average_purchase_unit_cost(
		int ordered_quantity,
		double cost_basis )
{
	if ( !ordered_quantity ) return 0.0;

	return cost_basis / (double)ordered_quantity;
}

boolean inventory_average_purchase_insert_boolean(
		INVENTORY_AVERAGE *inventory_average_fetch )
{
	if ( inventory_average_fetch )
		return 0;
	else
		return 1;
}

char *inventory_average_purchase_insert_sql(
		const char *inventory_average_table,
		const char *inventory_column,
		const char *inventory_average_date_column,
		char *inventory_name,
		char *inventory_average_date_time_key )
{
	char insert_sql[ 1024 ];

	snprintf(
		insert_sql,
		sizeof ( insert_sql ),
		"insert into %s (%s,%s) values ('%s','%s');",
		inventory_average_table,
		inventory_column,
		inventory_average_date_column,
		inventory_name,
		inventory_average_date_time_key );

	return strdup( insert_sql );
}

LIST *inventory_average_primary_key_list(
		const char *inventory_column,
		const char *inventory_average_date_column )
{
	LIST *list = list_new();

	list_set( list, (char *)inventory_column );
	list_set( list, (char *)inventory_average_date_column );

	return list;
}

char *inventory_average_primary_data_string(
		const char sql_delimiter,
		char *inventory_name,
		char *inventory_average_date_time_key )
{
	static char data_string[ 128 ];

	snprintf(
		data_string,
		sizeof ( data_string ),
		"%s%c%s",
		inventory_name,
		sql_delimiter,
		inventory_average_date_time_key );

	return data_string;
}

LIST *inventory_average_purchase_update_string_list(
		const char sql_delimiter,
		char *purchase_date_time,
		int ordered_quantity,
		int arrived_quantity,
		int slippage_quantity,
		double total_cost_balance,
		int quantity_on_hand,
		double average_unit_cost,
		char *primary_data_string )
{
	LIST *list = list_new();
	char *update_string;

	if ( !purchase_date_time
	||   !primary_data_string )
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

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_text_string(
			sql_delimiter,
			primary_data_string,
			"purchased_date_time" /* column_name */,
			purchase_date_time /* text */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			primary_data_string,
			"ordered_quantity" /* column_name */,
			ordered_quantity /* integer */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			primary_data_string,
			"arrived_quantity" /* column_name */,
			arrived_quantity /* integer */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			primary_data_string,
			"slippage_quantity" /* column_name */,
			slippage_quantity /* integer */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"total_cost_balance" /* column_name */,
			total_cost_balance /* money */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			primary_data_string,
			"quantity_on_hand" /* column_name */,
			quantity_on_hand /* integer */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"average_unit_cost" /* column_name */,
			average_unit_cost /* money */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	return list;
}

void inventory_average_purchase_save(
		char *inventory_name,
		INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase )
{
	if ( !inventory_average_purchase )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_average_purchase is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( inventory_average_purchase->insert_sql )
	{
		(void)sql_execute(
			SQL_EXECUTABLE,
			(char *)0 /* appaserver_error_filespecification */,
			(LIST *)0 /* sql_list */,
			inventory_average_purchase->insert_sql
				/* sql_statement */ );
	}

	update_string_list_execute(
		inventory_average_purchase->predictive_update_system_string,
		inventory_average_purchase->update_string_list );

	inventory_average_purchase->inventory_average_list =
		inventory_average_list_new(
			inventory_name,	
			inventory_average_purchase->
				inventory_average_date_time_key,
			inventory_average_purchase->
				inventory_average_primary_data_string,
			inventory_average_purchase->
				inventory_average_prior_purchase_date_time );

	inventory_average_list_update(
		inventory_average_purchase->
			inventory_average_list );
}

INVENTORY_AVERAGE_LIST *inventory_average_list_new(
		char *inventory_name,
		char *inventory_average_date_time_key,
		char *inventory_average_primary_data_string,
		char *prior_purchase_date_time )
{
	INVENTORY_AVERAGE_LIST *inventory_average_list;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVENTORY_AVERAGE *inventory_average;

	if ( !inventory_name
	||   !prior_purchase_date_time )
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

	inventory_average_list = inventory_average_list_calloc();
	inventory_average_list->list = list_new();

	inventory_average_list->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_list_where(
			INVENTORY_COLUMN,
			INVENTORY_AVERAGE_DATE_COLUMN,
			inventory_name,
			prior_purchase_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			INVENTORY_AVERAGE_SELECT,
			INVENTORY_AVERAGE_TABLE,
			inventory_average_list->where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		inventory_average =
			inventory_average_parse(
				input );

		if ( !inventory_average )
		{
			char message[ 1024 ];

			pclose( input_pipe );

			snprintf(
				message,
				sizeof ( message ),
				"inventory_average_parse(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( inventory_average_list->list, inventory_average );
	}

	pclose( input_pipe );

	inventory_average_list_set(
		inventory_average_list->list /* inventory_average_list */
			/* Sets each inventory_average_quantity_on_hand */
			/* Sets each inventory_average_total_cost_balance */
			/* Sets each inventory_average_unit_cost */
			/* Sets each cost_of_goods_sold */ );

	/* Set INVENTORY_AVERAGE_TABLE */
	/* --------------------------- */
	inventory_average_list->average_primary_key_list =
		folder_attribute_cache_primary_key_list(
		INVENTORY_AVERAGE_TABLE /* folder_name */ );

	inventory_average_list->average_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		predictive_update_system_string(
			INVENTORY_AVERAGE_TABLE,
			inventory_average_list->
				average_primary_key_list );

	inventory_average_list->average_update_string_list =
		inventory_average_list_average_update_string_list(
			inventory_average_list->list
				/* inventory_average_list */ );

	/* Set INVENTORY_PURCHASE_TABLE */
	/* ---------------------------- */
	inventory_average_list->purchase_primary_key_list =
		folder_attribute_cache_primary_key_list(
			INVENTORY_PURCHASE_TABLE /* folder_name */ );

	inventory_average_list->purchase_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		predictive_update_system_string(
			INVENTORY_PURCHASE_TABLE,
			inventory_average_list->
				purchase_primary_key_list );

	inventory_average_list->purchase_update_string_list =
		inventory_average_list_purchase_update_string_list(
			inventory_average_list->list
				/* inventory_average_list */ );

	/* Set INVENTORY_TABLE */
	/* ------------------- */
	inventory_average_list->inventory_primary_key_list =
		folder_attribute_cache_primary_key_list(
			INVENTORY_TABLE /* folder_name */ );

	inventory_average_list->inventory_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		predictive_update_system_string(
			INVENTORY_TABLE,
			inventory_average_list->
				inventory_primary_key_list );

	inventory_average_list->inventory_update_string_list =
		inventory_average_list_inventory_update_string_list(
			inventory_average_list->list
				/* inventory_average_list */ );

	/* Set INVENTORY_SALE_TABLE */
	/* ------------------------ */
	inventory_average_list->sale_primary_key_list =
		folder_attribute_cache_primary_key_list(
			INVENTORY_SALE_TABLE /* folder_name */ );

	inventory_average_list->sale_update_system_string =
		predictive_update_system_string(
			INVENTORY_SALE_TABLE,
			inventory_average_list->sale_primary_key_list );

	inventory_average_list->sale_update_string_list =
		inventory_average_list_sale_update_string_list(
			inventory_average_list->list
				/* inventory_average_list */ );

	return inventory_average_list;
}

INVENTORY_AVERAGE_LIST *inventory_average_list_calloc( void )
{
	INVENTORY_AVERAGE_LIST *inventory_average_list;

	if ( ! ( inventory_average_list =
			calloc( 1,
				sizeof ( INVENTORY_AVERAGE_LIST ) ) ) )
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

	return inventory_average_list;
}

char *inventory_average_list_where(
	const char *inventory_column,
	const char *inventory_average_date_column,
	char *inventory_name,
	char *prior_purchase_date_time )
{
	char *primary_where;
	static char where[ 256 ];

	if ( !inventory_name
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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_primary_where(
			inventory_column,
			inventory_name );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s >= '%s'",
		primary_where,
		inventory_average_date_column,
		prior_purchase_date_time );

	return where;
}

INVENTORY_AVERAGE_SALE *inventory_average_sale_fetch(
		char *inventory_name,
		char *sale_date_time,
		int sold_quantity )
{
	INVENTORY_AVERAGE_SALE *inventory_average_sale;
	INVENTORY_AVERAGE *inventory_average;

	if ( !inventory_name
	||   !sale_date_time )
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

	if ( !sold_quantity ) return NULL;

	inventory_average_sale = inventory_average_sale_calloc();

	inventory_average_sale->inventory_average_date_time_key =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		inventory_average_date_time_key(
			(char *)0 /* purchase_date_time */,
			sale_date_time );

	inventory_average_sale->inventory_average_primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_primary_where(
			INVENTORY_COLUMN,
			INVENTORY_AVERAGE_DATE_COLUMN,
			inventory_name,
			inventory_average_sale->
				inventory_average_date_time_key );

	inventory_average =
		inventory_average_fetch(
			INVENTORY_AVERAGE_SELECT,
			INVENTORY_AVERAGE_TABLE,
			inventory_average_sale->
				inventory_average_primary_where );

	inventory_average->insert_boolean =
		inventory_average_purchase_insert_boolean(
			inventory_average );

	if ( inventory_average->insert_boolean )
	{
		inventory_average->insert_sql =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			inventory_average_purchase_insert_sql(
				INVENTORY_AVERAGE_TABLE,
				INVENTORY_COLUMN,
				INVENTORY_AVERAGE_DATE_COLUMN,
				inventory_name,
				inventory_average_date_time_key() );
	}

	inventory_average_sale->inventory_average_primary_key_list =
		inventory_average_primary_key_list(
			INVENTORY_COLUMN,
			INVENTORY_AVERAGE_DATE_COLUMN );

	inventory_average_sale->predictive_update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		predictive_update_system_string(
			INVENTORY_AVERAGE_TABLE /* table_name */,
			inventory_average_sale->
				inventory_average_primary_key_list );

	inventory_average_sale->inventory_average_primary_data_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_primary_data_string(
			SQL_DELIMITER,
			inventory_name,
			inventory_average_sale->
				inventory_average_date_time_key );

	inventory_average_sale->update_string_list =
		inventory_average_sale_update_string_list(
			SQL_DELIMITER,
			sale_date_time,
			sold_quantity,
			inventory_average_sale->
				inventory_average_primary_data_string );

	inventory_average_sale->
		inventory_average_prior_purchase_date_time =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			inventory_average_prior_purchase_date_time(
				INVENTORY_COLUMN,
				PURCHASE_DATE_TIME_COLUMN,
				inventory_name,
				sale_date_time /* purchase_date_time */ );

	return inventory_average_sale;
}

INVENTORY_AVERAGE_SALE *inventory_average_sale_calloc( void )
{
	INVENTORY_AVERAGE_SALE *inventory_average_sale;

	if ( ! ( inventory_average_sale =
			calloc( 1,
				sizeof ( INVENTORY_AVERAGE_SALE ) ) ) )
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

	return inventory_average_sale;
}

LIST *inventory_average_sale_update_string_list(
		const char sql_delimiter,
		char *sale_date_time,
		int sold_quantity,
		char *primary_data_string )
{
	LIST *list = list_new();
	char *update_string;

	if ( !sale_date_time
	||   !primary_data_string )
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

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_text_string(
			sql_delimiter,
			primary_data_string,
			"sale_date_time" /* column_name */,
			sale_date_time /* text */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			primary_data_string,
			"sold_quantity" /* column_name */,
			sold_quantity /* integer */,
			1 /* set_boolean */ );
	list_set( list, update_string );

	return list;
}

void inventory_average_sale_save(
		char *inventory_name,
		INVENTORY_AVERAGE_SALE *inventory_average_sale )
{
	if ( !inventory_average_sale )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_average_sale is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( inventory_average_sale->insert_sql )
	{
		(void)sql_execute(
			SQL_EXECUTABLE,
			(char *)0 /* appaserver_error_filespecification */,
			(LIST *)0 /* sql_list */,
			inventory_average_sale->insert_sql
				/* sql_statement */ );
	}

	void update_string_list_execute(
		inventory_average_sale->predictive_update_system_string,
		inventory_average_sale->update_string_list );

	inventory_average_sale->inventory_average_list =
		inventory_average_list_new(
			inventory_name,	
			inventory_average_sale->
				inventory_average_date_time_key,
			inventory_average_sale->
				inventory_average_primary_data_string,
			inventory_average_sale->
				inventory_average_prior_purchase_date_time );

	inventory_average_list_update( inventory_average_list );
}

LIST *inventory_average_list_average_update_string_list(
		char *primary_data_string,
		LIST *inventory_average_list )
{
}
