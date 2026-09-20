/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average_cost.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "inventory_balance.h"
#include "inventory_average_cost.h"

LIST *inventory_average_cost_list( LIST *inventory_balance_list )
{
	LIST *list = list_new();
	INVENTORY_AVERAGE_COST *prior_inventory_average_cost = {0};
	INVENTORY_BALANCE *inventory_balance;
	double prior_total_cost_balance;
	INVENTORY_AVERAGE_COST *inventory_average_cost;

	if ( list_rewind( inventory_balance_list ) )
	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( !prior_inventory_average_cost )
		{
			if ( !inventory_balance->inventory_purchase )
			{
				char message[ 1024 ];

				snprintf(
					message,
					sizeof ( message ),
			"Inventory balance must begin with a purchase." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !inventory_balance->
				inventory_purchase->
				cost_basis )
			{
				char message[ 1024 ];

				snprintf(
					message,
					sizeof ( message ),
				"inventory_purchase->cost_basis is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			prior_total_cost_balance =
				inventory_average_cost_prior_total_cost_balance(
					inventory_balance->
						inventory_purchase->
						ordered_quantity,
					inventory_balance->
						inventory_purchase->
						cost_basis_inventory->
						cost_basis_amount );

			prior_inventory_average_cost =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				inventory_average_cost_new(
					inventory_balance->
						inventory_purchase,
					(INVENTORY_SALE *)0,
					inventory_balance->
						inventory_purchase->
						ordered_quantity
						/* quantity_on_hand */,
					prior_total_cost_balance,
					inventory_balance->
						inventory_purchase->
						cost_basis_inventory->
						cost_basis_amount
						/* average_unit_cost */,
					0.0 /* cost_of_goods_sold */ );

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
		int ordered_quantity,
		double cost_basis_amount )
{
	return (double)ordered_quantity * cost_basis_amount;
}

INVENTORY_AVERAGE_COST *inventory_average_cost_purchase(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_PURCHASE *inventory_purchase )
{
	double total_cost_balance;
	int quantity_on_hand;
	double average_unit_cost;

	if ( !prior_inventory_average_cost
	||   !inventory_purchase
	||   !inventory_purchase->cost_basis_inventory )
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
				cost_basis_inventory->
				cost_basis_amount );

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

