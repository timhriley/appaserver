/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include "predictive.h"
#include "appaserver_error.h"
#include "float.h"
#include "String.h"
#include "inventory.h"
#include "inventory_average.h"

INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *purchase_date_time,
		char *sale_date_time,
		double current_purchase_cost_basis )
{
	INVENTORY_AVERAGE *inventory_average;
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	INVENTORY_SALE_LIST *inventory_sale_list;
	boolean first_purchase_boolean = 0;

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

	if ( !purchase_date_time
	&&   !sale_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
		"both purchase_date_time and sale_date_time are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_average = inventory_average_calloc();

	inventory_average->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	inventory_average->entity_contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	inventory_average->cost_date_time =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		inventory_average_cost_date_time(
			purchase_date_time,
			sale_date_time );

	inventory_average->inventory_purchase_cost_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_purchase_cost_where(
			INVENTORY_PURCHASE_TABLE,
			SALE_INVENTORY_COLUMN,
			PURCHASE_DATE_TIME_COLUMN,
			inventory_name,
			inventory_average->cost_date_time
				/* purchase_date_time */ );

	if ( !inventory_average->inventory_purchase_cost_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_purchase_cost_where(%s,%s) returned empty.",
			inventory_name,
			inventory_average->cost_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_purchase_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_purchase_list_new(
			INVENTORY_PURCHASE_SELECT,
			INVENTORY_PURCHASE_TABLE,
			inventory_average->predictive_fund_boolean,
			inventory_average->entity_contact_key_boolean,
			inventory_average->inventory_purchase_cost_where );

	if ( purchase_date_time )
	{
		inventory_average_set_current_purchase_cost_basis(
			purchase_date_time,
			current_purchase_cost_basis,
			inventory_purchase_list->list );
	}

	inventory_average->inventory_sale_cost_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_sale_cost_where(
			INVENTORY_SALE_TABLE,
			SALE_INVENTORY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			inventory_name,
			inventory_average->cost_date_time
				/* sale_date_time */ );

	if ( !inventory_average->inventory_sale_cost_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_sale_cost_where(%s,%s) returned empty.",
			inventory_name,
			inventory_average->cost_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_sale_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_sale_list_new(
			INVENTORY_SALE_SELECT,
			INVENTORY_SALE_TABLE,
			inventory_average->predictive_fund_boolean,
			inventory_average->entity_contact_key_boolean,
			inventory_average->inventory_sale_cost_where,
			0 /* not inventory_average_boolean */ );

	inventory_average->purchase_list =
		/* ------------------------------------------- */
		/* Returns parameter (used to change the name) */
		/* ------------------------------------------- */
		inventory_average_purchase_list(
			inventory_purchase_list->list
				/* inventory_purchase_list */ );

	inventory_average->sale_list =
		/* ------------------------------------------- */
		/* Returns parameter (used to change the name) */
		/* ------------------------------------------- */
		inventory_average_sale_list(
			inventory_sale_list->list
				/* inventory_sale_list */ );

	inventory_average->inventory_balance_list =
		inventory_balance_list(
			inventory_average->purchase_list
				/* inventory_purchase_list */,
			inventory_average->sale_list
				/* inventory_sale_list */ );

	if ( !list_length( inventory_average->inventory_balance_list ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_balance_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( purchase_date_time )
	{
		first_purchase_boolean =
			inventory_average_first_purchase_boolean(
				INVENTORY_PURCHASE_TABLE,
				PURCHASE_DATE_TIME_COLUMN,
				SALE_INVENTORY_COLUMN,
				inventory_name,
				purchase_date_time );
	}

	inventory_average->inventory_average_cost_list =
		inventory_average_cost_list(
			inventory_average->inventory_balance_list,
			first_purchase_boolean );

	if ( !list_length( inventory_average->inventory_average_cost_list ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_average_cost_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( sale_date_time )
	{
		INVENTORY_AVERAGE_COST *inventory_average_cost;

		inventory_average_cost =
			inventory_average_cost_sale_seek(
				inventory_average->inventory_average_cost_list,
				sale_date_time );

		if ( !inventory_average_cost )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"inventory_average_cost_sale_seek(%s) returned empty.",
				sale_date_time );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		inventory_average->cost_of_goods_sold =
			inventory_average_cost_get(
				inventory_average_cost );
	}

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

LIST *inventory_average_purchase_list( LIST *inventory_purchase_list )
{
	return inventory_purchase_list;
}

LIST *inventory_average_sale_list( LIST *inventory_sale_list )
{
	return inventory_sale_list;
}

void inventory_average_set_current_purchase_cost_basis(
	char *purchase_date_time,
	double current_purchase_cost_basis,
	LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( float_money_virtually_zero( current_purchase_cost_basis ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"current_purchase_cost_basis is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_purchase =
		inventory_purchase_date_seek(
			purchase_date_time,
			inventory_purchase_list );

	if ( !inventory_purchase )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_purchase_date_seek(%s) returned empty.",
			purchase_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_purchase->cost_basis = current_purchase_cost_basis;
}

boolean inventory_average_first_purchase_boolean(
		const char *inventory_purchase_table,
		const char *purchase_date_time_column,
		const char *sale_inventory_column,
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
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_first_purchase_system_string(
			inventory_purchase_table,
			purchase_date_time_column,
			sale_inventory_column,
			inventory_name );

	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	if ( !input || !*input )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_system_input(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return (strcmp( input, purchase_date_time ) == 0);
}

char *inventory_average_purchase_first_system_string(
		const char *inventory_average_table,
		const char *purchase_date_time_column,
		const char *inventory_column,
		char *inventory_name )
{
	static char system_string[ 256 ];
	char *where;

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_primary_where(
			inventory_column,
			inventory_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh 'min(%s)' %s \"%s\"",
		purchase_date_time_column,
		inventory_average_table,
		where );	

	return system_string;
}

char *inventory_average_first_purchase_where(
		const char *sale_inventory_column,
		char *inventory_name )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"%s = '%s'",
		sale_inventory_column,
		inventory_name );

	return where;
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
	char *date_time_key;
	char *primary_where;
	int quantity;
	int unit_cost;
	char *prior_purchase_date_time;

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

	date_time_key =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		inventory_average_date_time_key(
			purchase_date_time,
			(char *)0 /* sale_date_time */ );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_average_primary_where(
			inventory_name,
			date_time_key );

	inventory_average_purchase->inventory_average =
		inventory_average_fetch(
			INVENTORY_AVERAGE_SELECT,
			INVENTORY_AVERAGE_TABLE,
			primary_where );

	inventory_average_purchase->update_boolean =
		inventory_average_purchase_update_boolean(
			inventory_average_purchase->
				inventory_average );

	quantity = 
		inventory_average_purchase_quantity(
			arrived_quantity,
			slippage_quantity );

	unit_cost =
		inventory_average_unit_cost(
			ordered_quantity,
			inventory_purchase_cost_basis );

	inventory_average_purchase->inventory_average =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_average_purchase_new(
			inventory_name,
			purchase_date_time,
			ordered_quantity,
			arrived_quantity,
			slippage_quantity,
			inventory_purchase_cost_basis
				/* total_cost_balance */,
			date_time_key,
			quantity,
			quantity /* quantity_on_hand */,
			unit_cost,
			inventory_purchase_cost_basis
				/* total_cost_balance */ ); 

	inventory_average_purchase->
		inventory_average_prior_purchase_date_time =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			inventory_average_prior_purchase_date_time(
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

boolean inventory_average_purchase_update_boolean(
		INVENTORY_AVERAGE *inventory_average )
{
	return
	(boolean)(unsigned int)(long)inventory_average;
}

int inventory_average_purchase_quantity(
		int arrived_quantity,
		int slippage_quantity )
{
	return arrived_quantity - slippage_quantity;
}

double inventory_average_unit_cost(
		int ordered_quantity,
		double cost_basis )
{
	if ( !ordered_quantity ) return 0.0;

	return cost_basis / (double)ordered_quantity;
}

char *inventory_average_prior_purchase_date_time(
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
		"max( %s )",
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
	static char where[ 160 ];

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
	char where[ 160 ];

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

		if ( list_first_boolean( inventory_average_list ) )
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

			inventory_average->
				inventory_average_total_cost_balance =
					inventory_average->total_cost_balance;

			inventory_average->inventory_average_unit_cost =
				inventory_average->average_unit_cost;

			inventory_average_prior = inventory_average;

			continue;

		} /* If list_first_boolean() */

		if ( inventory_average->purchase_date_time );
		{
			inventory_average->inventory_average_quantity_on_hand =
				inventory_average_purchase_quantity_on_hand(
					inventory_average_prior->
						quantity_on_hand
						/* prior_quantity_on_hand */,
					inventory_average->arrived_quantity,
					inventory_average->slippage_quantity );

			inventory_average->
				inventory_average_total_cost_balance =
				    inventory_average_total_cost_balance(
					inventory_average_prior->
						total_cost_balance
						/* prior_total_cost_balance */,
					inventory_average->ordered_quantity,
					inventory_average->average_unit_cost );

			inventory_average->inventory_average_unit_cost =
				inventory_average_unit_cost(
				    inventory_average->
					 inventory_average_quantity_on_hand,
				    inventory_average->
					 inventory_average_total_cost_balance );
		}
		else
		/* Must be inventory sale */
		{
			inventory_average->inventory_average_quantity_on_hand =
				inventory_average_sale_quantity_on_hand(
					inventory_average_prior->
						quantity_on_hand
						/* prior_quantity_on_hand */,
					inventory_average->sold_quantity );

			inventory_average->inventory_average_unit_cost =
				inventory_average_sale_unit_cost(
					inventory_average_prior->
						average_unit_cost );

			inventory_average->
				inventory_average_total_cost_balance =
				    inventory_average_sale_total_cost_balance(
					inventory_average->
					    inventory_average_quantity_on_hand,
					inventory_average->
					    inventory_average_unit_cost );

			inventory_average->
				inventory_average_cost_of_goods_sold =
				    inventory_average_cost_of_goods_sold(
					inventory_average->sold_quantity,
					inventory_average->
						inventory_average_unit_cost );
		}

		inventory_average_prior = inventory_average;

	} while ( list_next( inventory_averge_list ) );
}

int inventory_average_purchase_quantity_on_hand(
		int prior_quantity_on_hand,
		int arrived_quantity,
		int slippage_quantity )
{
	return
	prior_quantity_on_hand +
	( arrived_quantity - slippage_quantity );
}

double inventory_average_total_cost_balance(
		double prior_total_cost_balance,
		int ordered_quantity,
		double average_unit_cost )
{
	return
	prior_total_cost_balance +
	( (double)ordered_quantity * average_unit_cost );
}

double inventory_average_unit_cost(
		int quantity_on_hand,
		double total_cost_balance )
{
	if ( !quantity_on_hand ) return 0.0;

	return total_cost_balance / (double)quantity_on_hand;
}

int inventory_average_sale_quantity_on_hand(
		int prior_quantity_on_hand,
		int sold_quantity )
{
	return prior_quantity_on_hand - sold_quantity;
}

double inventory_average_sale_unit_cost( double average_unit_cost )
{
	return average_unit_cost;
}

double inventory_average_sale_total_cost_balance(
		int quantity_on_hand,
		double average_unit_cost )
{
	return (double)quantity_on_hand * average_unit_cost;
}

double inventory_average_cost_of_goods_sold(
		int sold_quantity,
		double average_unit_cost )
{
	return (double)sold_quantity * average_unit_cost;
}

