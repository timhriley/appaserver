/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/cost_basis.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver.h"
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
		double cost_basis_total )
{
	COST_BASIS *cost_basis;

	if ( float_money_virtually_same( cost_basis_total, 0.0 ) )
		return NULL;

	cost_basis = cost_basis_calloc();

	cost_basis->cost_basis_fixed_asset_list =
		cost_basis_fixed_asset_list(
			sales_tax,
			freight_in,
			fixed_asset_purchase_list,
			cost_basis_total );

	cost_basis->cost_basis_fixed_asset_tax_capitalized =
		cost_basis_fixed_asset_tax_capitalized(
			cost_basis->cost_basis_fixed_asset_list );

	cost_basis->cost_basis_fixed_asset_freight_capitalized =
		cost_basis_fixed_asset_freight_capitalized(
			cost_basis->cost_basis_fixed_asset_list );

	cost_basis->cost_basis_inventory_list =
		cost_basis_inventory_list(
			freight_in,
			inventory_purchase_list,
			cost_basis_total );

	cost_basis->cost_basis_inventory_freight_capitalized =
		cost_basis_inventory_freight_capitalized(
			cost_basis->cost_basis_inventory_list );

	cost_basis->cost_basis_specific_inventory_list =
		cost_basis_specific_inventory_list(
			freight_in,
			specific_inventory_purchase_list,
			cost_basis_total );

	cost_basis->cost_basis_specific_inventory_freight_capitalized =
		cost_basis_specific_inventory_freight_capitalized(
			cost_basis->cost_basis_specific_inventory_list );

	cost_basis->sales_tax_expense =
		cost_basis_sales_tax_expense(
			sales_tax,
			cost_basis->
				cost_basis_fixed_asset_tax_capitalized );

	cost_basis->freight_in_expense =
		cost_basis_freight_in_expense(
			freight_in,
			cost_basis->
			    cost_basis_fixed_asset_freight_capitalized,
			cost_basis->
			    cost_basis_inventory_freight_capitalized,
			cost_basis->
			    cost_basis_specific_inventory_freight_capitalized );

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
		double cost_basis_total )
{
	if ( float_money_virtually_same( cost_basis_total, 0.0 ) )
		return 0.0;
	else
		return cost / cost_basis_total;
}

double cost_basis_amount(
		double cost,
		double tax_capitalized,
		double freight_capitalized )
{
	return
	cost + tax_capitalized + freight_capitalized;
}

LIST *cost_basis_fixed_asset_list(
		double sales_tax,
		double freight_in,
		LIST *fixed_asset_purchase_list,
		double cost_basis_total )
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
				sales_tax,
				freight_in,
				fixed_asset_purchase->fixed_asset_cost,
				cost_basis_total );

		fixed_asset_purchase->
			cost_basis_fixed_asset =
				cost_basis_fixed_asset;

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
		double sales_tax,
		double freight_in,
		double fixed_asset_cost,
		double cost_basis_total )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;

	cost_basis_fixed_asset = cost_basis_fixed_asset_calloc();

	cost_basis_fixed_asset->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			fixed_asset_cost,
			cost_basis_total );

	cost_basis_fixed_asset->cost_basis_tax_capitalized =
		cost_basis_tax_capitalized(
			sales_tax,
			cost_basis_fixed_asset->
				cost_basis_percent_of_total );

	cost_basis_fixed_asset->cost_basis_freight_capitalized =
		cost_basis_freight_capitalized(
			freight_in,
			cost_basis_fixed_asset->
				cost_basis_percent_of_total );

	cost_basis_fixed_asset->cost_basis_amount =
		cost_basis_amount(
			fixed_asset_cost,
			cost_basis_fixed_asset->
				cost_basis_tax_capitalized,
			cost_basis_fixed_asset->
				cost_basis_freight_capitalized );

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

double cost_basis_fixed_asset_freight_capitalized(
		LIST *cost_basis_fixed_asset_list )
{
	COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset;
	double freight_capitalized = 0.0;

	if ( list_rewind( cost_basis_fixed_asset_list ) )
	do {
		cost_basis_fixed_asset =
			 list_get(
				cost_basis_fixed_asset_list );

		freight_capitalized +=
			cost_basis_fixed_asset->
				cost_basis_freight_capitalized;

	} while ( list_next( cost_basis_fixed_asset_list ) );

	return freight_capitalized;
}

LIST *cost_basis_inventory_list(
		double freight_in,
		LIST *inventory_purchase_list,
		double cost_basis_total )
{
	INVENTORY_PURCHASE *inventory_purchase;
	COST_BASIS_INVENTORY *cost_basis_inventory;
	LIST *list = list_new();

	if ( float_money_virtually_same( cost_basis_total, 0.0 ) )
		return NULL;

	if ( list_rewind( inventory_purchase_list ) )
	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		cost_basis_inventory =
			cost_basis_inventory_new(
				freight_in,
				inventory_purchase->extended_cost,
				cost_basis_total );

		inventory_purchase->
			cost_basis_inventory =
				cost_basis_inventory;

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
		double freight_in,
		double extended_cost,
		double cost_basis_total )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;

	cost_basis_inventory = cost_basis_inventory_calloc();

	cost_basis_inventory->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			extended_cost,
			cost_basis_total );

	cost_basis_inventory->cost_basis_freight_capitalized =
		cost_basis_freight_capitalized(
			freight_in,
			cost_basis_inventory->
				cost_basis_percent_of_total );

	cost_basis_inventory->cost_basis_amount =
		cost_basis_amount(
			extended_cost,
			0.0 /* cost_basis_tax_capitalized */,
			cost_basis_inventory->
				cost_basis_freight_capitalized );

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

double cost_basis_inventory_freight_capitalized(
		LIST *cost_basis_inventory_list )
{
	COST_BASIS_INVENTORY *cost_basis_inventory;
	double freight_capitalized = 0.0;

	if ( list_rewind( cost_basis_inventory_list ) )
	do {
		cost_basis_inventory =
			 list_get(
				cost_basis_inventory_list );

		freight_capitalized +=
			cost_basis_inventory->
				cost_basis_freight_capitalized;

	} while ( list_next( cost_basis_inventory_list ) );

	return freight_capitalized;
}

LIST *cost_basis_specific_inventory_list(
		double freight_in,
		LIST *specific_inventory_purchase_list,
		double cost_basis_total )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;
	LIST *list = list_new();

	if ( float_money_virtually_same( cost_basis_total, 0.0 ) )
		return NULL;

	if ( list_rewind( specific_inventory_purchase_list ) )
	do {
		specific_inventory_purchase =
			list_get(
				specific_inventory_purchase_list );

		cost_basis_specific_inventory =
			cost_basis_specific_inventory_new(
				freight_in,
				specific_inventory_purchase->unit_cost,
				cost_basis_total );

		specific_inventory_purchase->
			cost_basis_specific_inventory =
				cost_basis_specific_inventory;
			
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
		double freight_in,
		double unit_cost,
		double cost_basis_total )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;

	cost_basis_specific_inventory = cost_basis_specific_inventory_calloc();

	cost_basis_specific_inventory->cost_basis_percent_of_total =
		cost_basis_percent_of_total(
			unit_cost,
			cost_basis_total );

	cost_basis_specific_inventory->cost_basis_freight_capitalized =
		cost_basis_freight_capitalized(
			freight_in,
			cost_basis_specific_inventory->
				cost_basis_percent_of_total );

	cost_basis_specific_inventory->cost_basis_amount =
		cost_basis_amount(
			unit_cost,
			0.0 /* cost_basis_tax_capitalized */,
			cost_basis_specific_inventory->
				cost_basis_freight_capitalized );

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

double cost_basis_specific_inventory_freight_capitalized(
		LIST *cost_basis_specific_inventory_list )
{
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;
	double freight_capitalized = 0.0;

	if ( list_rewind( cost_basis_specific_inventory_list ) )
	do {
		cost_basis_specific_inventory =
			 list_get(
				cost_basis_specific_inventory_list );

		freight_capitalized +=
			cost_basis_specific_inventory->
				cost_basis_freight_capitalized;

	} while ( list_next( cost_basis_specific_inventory_list ) );

	return freight_capitalized;
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
		double purchase_sales_tax,
		double fixed_asset_tax_capitalized )
{
	return
	purchase_sales_tax -
	fixed_asset_tax_capitalized;
}

double cost_basis_freight_in_expense(
		double purchase_freight_in,
		double fixed_asset_freight_capitalized,
		double inventory_freight_capitalized,
		double specific_inventory_freight_capitalized )
{
	return
	purchase_freight_in -
	fixed_asset_freight_capitalized -
	inventory_freight_capitalized -
	specific_inventory_freight_capitalized;
}


double cost_basis_tax_capitalized(
		double sales_tax,
		double percent_of_total )
{
	return
	sales_tax * percent_of_total;
}

double cost_basis_freight_capitalized(
		double freight_in,
		double percent_of_total )
{
	return
	freight_in * percent_of_total;
}

