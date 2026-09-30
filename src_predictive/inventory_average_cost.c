/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average_cost.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "float.h"
#include "String.h"
#include "inventory_balance.h"
#include "inventory_average_cost.h"

LIST *inventory_average_cost_list( LIST *inventory_balance_list )
{
	LIST *list = list_new();
	INVENTORY_AVERAGE_COST *prior_inventory_average_cost = {0};
	INVENTORY_BALANCE *inventory_balance;
	INVENTORY_AVERAGE_COST *inventory_average_cost;

	if ( list_rewind( inventory_balance_list ) )
	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( !prior_inventory_average_cost )
		{
			int prior_quantity_on_hand;
			double prior_average_unit_cost;
			double prior_total_cost_balance;
			INVENTORY_PURCHASE *inventory_purchase;

			if ( !inventory_balance->inventory_purchase )
			{
				char message[ 1024 ];

				snprintf(
					message,
					sizeof ( message ),
			"inventory balance must begin with a purchase." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			inventory_purchase =
				inventory_balance->
					inventory_purchase;

			if ( float_money_virtually_zero(
				inventory_purchase->cost_basis ) )
			{
				char message[ 1024 ];

				snprintf(
					message,
					sizeof ( message ),
				"inventory_purchase->cost_basis is zero." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			prior_quantity_on_hand =
				/* ------------------------ */
				/* Returns either parameter */
				/* ------------------------ */
				inventory_average_cost_prior_quantity_on_hand(
					inventory_purchase->quantity_on_hand,
					inventory_purchase->ordered_quantity );

			prior_average_unit_cost =
				/* ------------------------ */
				/* Returns either parameter */
				/* ------------------------ */
				inventory_average_cost_prior_average_unit_cost(
					inventory_purchase->
						average_unit_cost,
					inventory_purchase->
						cost_basis );

			prior_total_cost_balance =
				inventory_average_cost_prior_total_cost_balance(
					prior_quantity_on_hand,
					prior_average_unit_cost );

			prior_inventory_average_cost =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				inventory_average_cost_new(
					inventory_purchase,
					(INVENTORY_SALE *)0,
					prior_quantity_on_hand,
					prior_total_cost_balance,
					prior_average_unit_cost,
					0.0 /* cost_of_goods_sold */ );

			list_set( list, prior_inventory_average_cost );
			continue;		

		} /* if ( !prior_inventory_average_cost ) */

		if ( inventory_balance->inventory_purchase )
		{
			inventory_average_cost =
				inventory_average_cost_purchase(
					prior_inventory_average_cost,
					inventory_balance->inventory_purchase );
		}
		else
		/* ---------------------- */
		/* Must be inventory_sale */
		/* ---------------------- */
		{
			inventory_average_cost =
				inventory_average_cost_sale(
					prior_inventory_average_cost,
					inventory_balance->inventory_sale );
		}

		list_set( list, inventory_average_cost );
		prior_inventory_average_cost = inventory_average_cost;

	} while ( list_next( inventory_balance_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

INVENTORY_AVERAGE_COST *inventory_average_cost_new(
		INVENTORY_PURCHASE *inventory_purchase,
		INVENTORY_SALE *inventory_sale,
		int quantity_on_hand,
		double total_cost_balance,
		double average_unit_cost,
		double cost_of_goods_sold )
{
	INVENTORY_AVERAGE_COST *inventory_average_cost;

	inventory_average_cost = inventory_average_cost_calloc();

	inventory_average_cost->inventory_purchase = inventory_purchase,
	inventory_average_cost->inventory_sale = inventory_sale,
	inventory_average_cost->quantity_on_hand = quantity_on_hand,
	inventory_average_cost->total_cost_balance = total_cost_balance,
	inventory_average_cost->average_unit_cost = average_unit_cost,
	inventory_average_cost->cost_of_goods_sold = cost_of_goods_sold;

	return inventory_average_cost;
}

INVENTORY_AVERAGE_COST *inventory_average_cost_calloc( void )
{
	INVENTORY_AVERAGE_COST *inventory_average_cost;

	if ( ! ( inventory_average_cost =
			calloc( 1,
				sizeof ( INVENTORY_AVERAGE_COST ) ) ) )
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

	return inventory_average_cost;
}

double inventory_average_cost_prior_total_cost_balance(
		int prior_quantity_on_hand,
		double prior_average_unit_cost )
{
	return (double)prior_quantity_on_hand * prior_average_unit_cost;
}

INVENTORY_AVERAGE_COST *inventory_average_cost_purchase(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_PURCHASE *inventory_purchase )
{
	double total_cost_balance;
	int quantity_on_hand;
	double average_unit_cost;

	if ( !prior_inventory_average_cost
	||   !inventory_purchase )
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

	total_cost_balance =
		inventory_average_cost_purchase_total_cost_balance(
			prior_inventory_average_cost->total_cost_balance
				/* prior_total_cost_balance */,
			inventory_purchase->ordered_quantity,
			inventory_purchase->
				cost_basis );

	quantity_on_hand =
		inventory_average_cost_purchase_quantity_on_hand(
			prior_inventory_average_cost->quantity_on_hand
				/* prior_quantity_on_hand */,
			inventory_purchase->ordered_quantity );

	average_unit_cost =
		inventory_average_cost_purchase_average_unit_cost(
			total_cost_balance,
			quantity_on_hand );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	inventory_average_cost_new(
		inventory_purchase,
		(INVENTORY_SALE *)0,
		quantity_on_hand,
		total_cost_balance,
		average_unit_cost,
		0.0 /* inventory_average_cost_of_goods_sold */ );
}

int inventory_average_cost_purchase_quantity_on_hand(
		int quantity_on_hand,
		int ordered_quantity )
{
	return quantity_on_hand + ordered_quantity;
}

double inventory_average_cost_purchase_total_cost_balance(
		double prior_total_cost_balance,
		int ordered_quantity,
		double cost_basis_amount )
{
	return
	prior_total_cost_balance +
	((double)ordered_quantity * cost_basis_amount );
}

double inventory_average_cost_purchase_average_unit_cost(
		double total_cost_balance,
		int quantity_on_hand )
{
	return (double)quantity_on_hand * total_cost_balance;
}

INVENTORY_AVERAGE_COST *inventory_average_cost_sale(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_SALE *inventory_sale )
{
	double quantity_on_hand;
	double average_unit_cost;
	double total_cost_balance;
	double cost_of_goods_sold;

	if ( !prior_inventory_average_cost
	||   !inventory_sale )
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

	quantity_on_hand =
		inventory_average_cost_sale_quantity_on_hand(
			prior_inventory_average_cost->quantity_on_hand
				/* prior_quantity_on_hand */,
			inventory_sale->quantity );

	average_unit_cost =
		inventory_average_cost_sale_average_unit_cost(
			prior_inventory_average_cost->average_unit_cost
				/* prior_average_unit_cost */ );

	total_cost_balance =
		inventory_average_cost_sale_total_cost_balance(
			quantity_on_hand,
			average_unit_cost );

	cost_of_goods_sold =
		inventory_average_cost_of_goods_sold(
			inventory_sale->quantity,
			average_unit_cost );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	inventory_average_cost_new(
		(INVENTORY_PURCHASE *)0,
		inventory_sale,
		quantity_on_hand,
		total_cost_balance,
		average_unit_cost,
		cost_of_goods_sold );
}

int inventory_average_cost_sale_quantity_on_hand(
		int prior_quantity_on_hand,
		int quantity )
{
	return prior_quantity_on_hand - quantity;
}

double inventory_average_cost_sale_average_unit_cost(
		double prior_average_unit_cost )
{
	return prior_average_unit_cost;
}

double inventory_average_cost_sale_total_cost_balance(
		int quantity_on_hand,
		double average_unit_cost )
{
	return (double)quantity_on_hand * average_unit_cost;
}

double inventory_average_cost_of_goods_sold(
		int quantity_sold,
		double average_unit_cost )
{
	return (double)quantity_sold * average_unit_cost;
}


INVENTORY_AVERAGE_COST *inventory_average_cost_sale_seek(
		LIST *inventory_average_cost_list,
		char *sale_date_time )
{
	INVENTORY_AVERAGE_COST *inventory_average_cost;

	if ( !sale_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"sale_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( inventory_average_cost_list ) )
	do {
		inventory_average_cost =
			list_get(
				inventory_average_cost_list );

		if ( inventory_average_cost->inventory_sale )
		{
			if ( strcmp(
				sale_date_time,
				inventory_average_cost->
					inventory_sale->
					sale_date_time ) == 0 )
			{
				return inventory_average_cost;
			}
		}
	} while ( list_next( inventory_average_cost_list ) );

	return NULL;
}

double inventory_average_cost_get(
		INVENTORY_AVERAGE_COST *inventory_average_cost )
{
	if ( !inventory_average_cost )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_average_cost is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return inventory_average_cost->cost_of_goods_sold;
}

int inventory_average_cost_prior_quantity_on_hand(
		int quantity_on_hand,
		int ordered_quantity )
{
	if ( quantity_on_hand )
		return quantity_on_hand;
	else
		return ordered_quantity;
}

double inventory_average_cost_prior_average_unit_cost(
		double average_unit_cost,
		double cost_basis )
{
	if ( !float_money_virtually_zero( average_unit_cost ) )
		return average_unit_cost;
	else
		return cost_basis;
}

char *inventory_average_cost_list_display(
		LIST *inventory_average_cost_list )
{
	char display[ STRING_64K ];
	char *ptr = display;
	INVENTORY_AVERAGE_COST *inventory_average_cost;
	char *cost_display;

	*ptr = '\0';

	if ( list_rewind( inventory_average_cost_list ) )
	do {
		inventory_average_cost =
			list_get(
				inventory_average_cost_list );

		cost_display =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			inventory_average_cost_display(
				inventory_average_cost );

		if ( ptr != display ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf( ptr, "%s", cost_display );

	} while ( list_next( inventory_average_cost_list ) );

	return strdup( display );
}

char *inventory_average_cost_display(
		INVENTORY_AVERAGE_COST *inventory_average_cost )
{
	static char display[ 1024 ];

	if ( !inventory_average_cost )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_average_cost is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		display,
		sizeof ( display ),
		"purchase=%s, "
		"sale=%s, "
		"quantity_on_hand=%d, "
		"total_cost_balance=%.2lf, "
		"average_unit_cost=%.2lf, "
		"cost_of_goods_sold=%.2lf",
		(inventory_average_cost->inventory_purchase)
			? inventory_average_cost->
					inventory_purchase->
					purchase_date_time
			: "",
		(inventory_average_cost->inventory_sale)
			? inventory_average_cost->
				inventory_sale->
				sale_date_time
			: "",
		inventory_average_cost->quantity_on_hand,
		inventory_average_cost->total_cost_balance,
		inventory_average_cost->average_unit_cost,
		inventory_average_cost->cost_of_goods_sold );

	return display;
}

