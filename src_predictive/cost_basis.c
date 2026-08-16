/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/cost_basis.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "float.h"
#include "fixed_asset_purchase.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"
#include "cost_basis.h"

COST_BASIS *cost_basis_new(
		double sales_tax,
		double freight_in,
		LIST *fixed_asset_purchase_list,
		LIST *inventory_purchase_list,
		LIST *specific_inventory_purchase_list,
		double purchase_total )
{
	COST_BASIS *cost_basis;

	if ( !purchase_total ) return NULL;

	cost_basis = cost_basis_calloc();

	cost_basis->extra_total =
		cost_basis_extra_total(
			sales_tax,
			freight_in );

	cost_basis->cost_basis_fixed_asset_list =
		cost_basis_fixed_asset_list(
			fixed_asset_purchase_list,
			purchase_total,
			cost_basis->extra_total );

	cost_basis->cost_basis_fixed_asset_percent_total =
		cost_basis_fixed_asset_percent_total(
			cost_basis->cost_basis_fixed_asset_list );

	cost_basis->cost_basis_inventory_list =
		cost_basis_inventory_list(
			inventory_purchase_list,
			purchase_total,
			cost_basis->extra_total );

	cost_basis->cost_basis_inventory_percent_total =
		cost_basis_inventory_percent_total(
			cost_basis->cost_basis_inventory_list );

	cost_basis->cost_basis_specific_inventory_list =
		cost_basis_specific_inventory_list(
			specific_inventory_purchase_list,
			purchase_total,
			cost_basis->extra_total );

	cost_basis->cost_basis_specific_inventory_percent_total =
		cost_basis_specific_inventory_percent_total(
			cost_basis->cost_basis_specific_inventory_list );

	cost_basis->percent_total =
		cost_basis_percent_total(
			cost_basis->
				cost_basis_fixed_asset_percent_total,
			cost_basis->
				cost_basis_inventory_percent_total,
			cost_basis->
				cost_basis_specific_inventory_percent_total );

	cost_basis->sales_tax_expense =
		cost_basis_sales_tax_expense(
			sales_tax,
			cost_basis->percent_total );

	cost_basis->freight_in_expense =
		cost_basis_freight_in_expense(
			freight_in,
			cost_basis->percent_total );

	return cost_basis;
}

COST_BASIS *cost_basis_calloc( void )
{
	COST_BASIS *cost_basis;

	if ( ! ( cost_basis = calloc( 1, sizeof ( COST_BASIS ) ) ) )
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

	return cost_basis;
}

double cost_basis_percent_of_total(
		double cost,
		double total )
{
	if ( float_money_virtually_same( total, 0.0 ) )
		return 0.0;
	else
		return cost / total;
}

double cost_basis_extra_allocated(
		double extra_total,
		double percent_of_total )
{
	return
	extra_total *
	percent_of_total;
}

double cost_basis_amount(
		double cost,
		double extra_allocated )
{
	return
	cost + extra_allocated;
}

LIST *cost_basis_fixed_asset_list(
		LIST *fixed_asset_purchase_list,
		double purchase_total,
		double cost_basis_extra_total )
{
	LIST *list = list_new();
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;

	if ( list_rewind( fixed_asset_purchase_list ) )
	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		cost_basis_fixed_asset =
			cost_basis_fixed_asset_new(
				fixed_asset_purchase->asset_name,
				fixed_asset_purchase->serial_key,
				fixed_asset_purchase->fixed_asset_cost,
				purchase_total,
				cost_basis_extra_total );

		list_set( list, cost_basis_fixed_asset );

	} while ( list_next( fixed_asset_purchase_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_new(
		char *asset_name,
		char *serial_key,
		double fixed_asset_cost,
		double purchase_total,
		double cost_basis_extra_total )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;

	cost_basis_fixed_asset = cost_basis_fixed_asset_calloc();
	cost_basis_fixed_asset->asset_name = asset_name;
	cost_basis_fixed_asset->serial_key = serial_key;

	cost_basis_fixed_asset->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			fixed_asset_cost,
			purchase_total );

	cost_basis_fixed_asset->cost_basis_extra_allocated =
		cost_basis_extra_allocated(
			cost_basis_extra_total,
			cost_basis_fixed_asset->
				cost_basis_percent_of_total );

	cost_basis_fixed_asset->cost_basis_amount =
		cost_basis_amount(
			fixed_asset_cost,
			cost_basis_fixed_asset->
				cost_basis_extra_allocated );

	return cost_basis_fixed_asset;
}

COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_calloc( void )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;

	if ( ! ( cost_basis_fixed_asset =
			calloc( 1,
				sizeof ( COST_BASIS_FIXED_ASSET ) ) ) )
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

	return cost_basis_fixed_asset;
}

double cost_basis_fixed_asset_percent_total(
		LIST *cost_basis_fixed_asset_list )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;
	double total = 0.0;

	if ( list_rewind( cost_basis_fixed_asset_list ) )
	do {
		cost_basis_fixed_asset =
			 list_get(
				cost_basis_fixed_asset_list );

		total +=
			cost_basis_fixed_asset->
				cost_basis_percent_of_total;

	} while ( list_next( cost_basis_fixed_asset_list ) );

	return total;
}

COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_seek(
		LIST *cost_basis_fixed_asset_list,
		char *asset_name,
		char *serial_key )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;

	if ( list_rewind( cost_basis_fixed_asset_list ) )
	do {
		cost_basis_fixed_asset =
			list_get(
				cost_basis_fixed_asset_list );

		/* Safely returns */
		/* -------------- */
		if ( string_strcmp(
			asset_name,
		 	cost_basis_fixed_asset->asset_name ) == 0
		&&   string_strcmp(
			serial_key,
		 	cost_basis_fixed_asset->serial_key ) == 0 )
		{
			return cost_basis_fixed_asset;
		}

	} while ( list_next( cost_basis_fixed_asset_list ) );

	return NULL;
}

LIST *cost_basis_inventory_list(
		LIST *inventory_purchase_list,
		double purchase_total,
		double cost_basis_extra_total )
{
	INVENTORY_PURCHASE *inventory_purchase;
	COST_BASIS_INVENTORY *cost_basis_inventory;
	LIST *list = list_new();

	if ( list_rewind( inventory_purchase_list ) )
	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		cost_basis_inventory =
			cost_basis_inventory_new(
				inventory_purchase->inventory_name,
				inventory_purchase->extended_cost,
				purchase_total,
				cost_basis_extra_total );

		list_set( list, cost_basis_inventory );

	} while ( list_next( inventory_purchase_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

COST_BASIS_INVENTORY *cost_basis_inventory_new(
		char *inventory_name,
		double extended_cost,
		double purchase_total,
		double cost_basis_extra_total )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;

	cost_basis_inventory = cost_basis_inventory_calloc();
	cost_basis_inventory->inventory_name = inventory_name;

	cost_basis_inventory->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			extended_cost,
			purchase_total );

	cost_basis_inventory->cost_basis_extra_allocated =
		cost_basis_extra_allocated(
			cost_basis_extra_total,
			cost_basis_inventory->
				cost_basis_percent_of_total );

	cost_basis_inventory->cost_basis_amount =
		cost_basis_amount(
			extended_cost,
			cost_basis_inventory->
				cost_basis_extra_allocated );

	return cost_basis_inventory;
}

COST_BASIS_INVENTORY *cost_basis_inventory_calloc( void )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;

	if ( ! ( cost_basis_inventory =
			calloc( 1,
				sizeof ( COST_BASIS_INVENTORY ) ) ) )
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

	return cost_basis_inventory;
}

double cost_basis_inventory_percent_total(
		LIST *cost_basis_inventory_list )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;
	double total = 0.0;

	if ( list_rewind( cost_basis_inventory_list ) )
	do {
		cost_basis_inventory =
			 list_get(
				cost_basis_inventory_list );

		total +=
			cost_basis_inventory->
				cost_basis_percent_of_total;

	} while ( list_next( cost_basis_inventory_list ) );

	return total;
}

COST_BASIS_INVENTORY *cost_basis_inventory_seek(
		LIST *cost_basis_inventory_list,
		char *inventory_name )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;

	if ( list_rewind( cost_basis_inventory_list ) )
	do {
		cost_basis_inventory =
			list_get(
				cost_basis_inventory_list );

		/* Safely returns */
		/* -------------- */
		if ( string_strcmp(
			inventory_name,
		 	cost_basis_inventory->inventory_name ) == 0 )
		{
			return cost_basis_inventory;
		}

	} while ( list_next( cost_basis_inventory_list ) );

	return NULL;
}

LIST *cost_basis_specific_inventory_list(
		LIST *specific_inventory_purchase_list,
		double purchase_total,
		double cost_basis_extra_total )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;
	LIST *list = list_new();

	if ( list_rewind( specific_inventory_purchase_list ) )
	do {
		specific_inventory_purchase =
			list_get(
				specific_inventory_purchase_list );

		cost_basis_specific_inventory =
			cost_basis_specific_inventory_new(
				specific_inventory_purchase->inventory_name,
				specific_inventory_purchase->serial_key,
				specific_inventory_purchase->unit_cost,
				purchase_total,
				cost_basis_extra_total );

		list_set( list, cost_basis_specific_inventory );

	} while ( list_next( specific_inventory_purchase_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_new(
		char *inventory_name,
		char *serial_key,
		double unit_cost,
		double purchase_total,
		double cost_basis_extra_total )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;

	cost_basis_specific_inventory = cost_basis_specific_inventory_calloc();
	cost_basis_specific_inventory->inventory_name = inventory_name;
	cost_basis_specific_inventory->serial_key = serial_key;

	cost_basis_specific_inventory->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			unit_cost,
			purchase_total );

	cost_basis_specific_inventory->cost_basis_extra_allocated =
		cost_basis_extra_allocated(
			cost_basis_extra_total,
			cost_basis_specific_inventory->
				cost_basis_percent_of_total );

	cost_basis_specific_inventory->cost_basis_amount =
		cost_basis_amount(
			unit_cost,
			cost_basis_specific_inventory->
				cost_basis_extra_allocated );

	return cost_basis_specific_inventory;
}

COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_calloc( void )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;

	if ( ! ( cost_basis_specific_inventory =
			calloc( 1,
				sizeof ( COST_BASIS_SPECIFIC_INVENTORY ) ) ) )
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

	return cost_basis_specific_inventory;
}

double cost_basis_specific_inventory_percent_total(
		LIST *cost_basis_specific_inventory_list )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;
	double total = 0.0;

	if ( list_rewind( cost_basis_specific_inventory_list ) )
	do {
		cost_basis_specific_inventory =
			 list_get(
				cost_basis_specific_inventory_list );

		total +=
			cost_basis_specific_inventory->
				cost_basis_percent_of_total;

	} while ( list_next( cost_basis_specific_inventory_list ) );

	return total;
}

COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_seek(
		LIST *cost_basis_specific_inventory_list,
		char *inventory_name,
		char *serial_key )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;

	if ( list_rewind( cost_basis_specific_inventory_list ) )
	do {
		cost_basis_specific_inventory =
			list_get(
				cost_basis_specific_inventory_list );

		/* Safely returns */
		/* -------------- */
		if ( string_strcmp(
			inventory_name,
		 	cost_basis_specific_inventory->inventory_name ) == 0
		&&   string_strcmp(
			serial_key,
		 	cost_basis_specific_inventory->serial_key ) == 0 )
		{
			return cost_basis_specific_inventory;
		}

	} while ( list_next( cost_basis_specific_inventory_list ) );

	return NULL;
}

double cost_basis_extra_total(
		double sales_tax,
		double freight_in )
{
	return
	sales_tax + freight_in;
}

double cost_basis_percent_total(
		double fixed_asset_percent_total,
		double inventory_percent_total,
		double specific_inventory_percent_total )
{
	return
	fixed_asset_percent_total +
	inventory_percent_total +
	specific_inventory_percent_total;
}

double cost_basis_sales_tax_expense(
		double sales_tax,
		double cost_basis_percent_total )
{
	return
	( 1.0 - cost_basis_percent_total ) * sales_tax;
}

double cost_basis_freight_in_expense(
		double freight_in,
		double cost_basis_percent_total )
{
	return
	( 1.0 - cost_basis_percent_total ) * freight_in;
}

