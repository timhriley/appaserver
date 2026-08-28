/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_calculate.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "fixed_asset_purchase.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"
#include "cost_basis.h"
#include "purchase_calculate.h"

PURCHASE_CALCULATE *purchase_calculate_new(
		double purchase_sales_tax,
		double purchase_freight_in,
		LIST *fixed_asset_purchase_list,
		boolean inventory_total_boolean,
		LIST *inventory_purchase_list,
		boolean specific_inventory_total_boolean,
		LIST *specific_inventory_purchase_list,
		LIST *supply_purchase_list,
		LIST *service_purchase_list,
		boolean prepaid_asset_total_boolean,
		LIST *prepaid_asset_purchase_list,
		boolean return_list_total_boolean,
		LIST *purchase_return_list )
{
	PURCHASE_CALCULATE *purchase_calculate;

	purchase_calculate = purchase_calculate_calloc();

	purchase_calculate->fixed_asset_purchase_list_total =
		fixed_asset_purchase_list_total(
			fixed_asset_purchase_list );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: fixed_asset_purchase_list_total=%.2lf\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	purchase_calculate->fixed_asset_purchase_list_total );
msg( (char *)0, message );
}
	if ( inventory_total_boolean )
	{
		purchase_calculate->inventory_purchase_list_total =
			inventory_purchase_list_total(
				inventory_purchase_list );
	}

	if ( specific_inventory_total_boolean )
	{
		purchase_calculate->specific_inventory_purchase_list_total =
			specific_inventory_purchase_list_total(
				specific_inventory_purchase_list );
	}

	if ( supply_purchase_list ){}
/*
	double supply_purchase_list_total(
		supply_purchase_list );
*/

	if ( service_purchase_list ){}
/*
	double service_purchase_list_total(
		service_purchase_list );
*/

	if ( prepaid_asset_total_boolean )
	{
		if ( prepaid_asset_purchase_list ){}
/*
		double prepaid_asset_purchase_list_total(
			prepaid_asset_purchase_list );
*/
	}

	if ( return_list_total_boolean )
	{
		if ( purchase_return_list ){}
/*
		double purchase_return_list_total(
			purchase_return_list );
*/
	}

	purchase_calculate->cost_basis_total =
		purchase_calculate_cost_basis_total(
			purchase_calculate->
				fixed_asset_purchase_list_total,
			purchase_calculate->
				inventory_purchase_list_total,
			purchase_calculate->
				specific_inventory_purchase_list_total,
			purchase_calculate->
				supply_purchase_list_total );

	purchase_calculate->cost_basis =
		cost_basis_new(
			purchase_sales_tax,
			purchase_freight_in,
			fixed_asset_purchase_list
				/* Sets each cost_basis_fixed_asset */,
			inventory_purchase_list
				/* Sets each cost_basis_inventory */,
			specific_inventory_purchase_list
				/* Sets each cost_basis_specific_inventory */,
			purchase_calculate->cost_basis_total );

	purchase_calculate->total =
		purchase_calculate_total(
			purchase_calculate->
				fixed_asset_purchase_list_total,
			purchase_calculate->
				inventory_purchase_list_total,
			purchase_calculate->
				specific_inventory_purchase_list_total,
			purchase_calculate->
				supply_purchase_list_total,
			purchase_calculate->
				service_purchase_list_total,
			purchase_calculate->
				prepaid_asset_purchase_list_total );

	purchase_calculate->invoice_amount =
		purchase_calculate_invoice_amount(
			purchase_sales_tax,
			purchase_freight_in,
			purchase_calculate->return_list_total,
			purchase_calculate->total );

	return purchase_calculate;
}

PURCHASE_CALCULATE *purchase_calculate_calloc( void )
{
	PURCHASE_CALCULATE *purchase_calculate;

	if ( ! ( purchase_calculate =
			calloc( 1,
				sizeof ( PURCHASE_CALCULATE ) ) ) )
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

	return purchase_calculate;
}

double purchase_calculate_cost_basis_total(
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total )
{
	return
	fixed_asset_purchase_list_total +
	inventory_purchase_list_total +
	specific_inventory_purchase_list_total +
	supply_purchase_list_total;
}

double purchase_calculate_total(
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total )
{
	return
	fixed_asset_purchase_list_total +
	inventory_purchase_list_total +
	specific_inventory_purchase_list_total +
	supply_purchase_list_total +
	service_purchase_list_total +
	prepaid_asset_purchase_list_total;
}

double purchase_calculate_invoice_amount(
		double sales_tax,
		double freight_in,
		double return_list_total,
		double purchase_calculate_total )
{
	return
	sales_tax +
	freight_in -
	return_list_total +
	purchase_calculate_total;
}
