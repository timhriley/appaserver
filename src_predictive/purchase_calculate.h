/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_calculate.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "cost_basis.h"

typedef struct
{
	double fixed_asset_purchase_list_total;
	double inventory_purchase_list_total;
	double specific_inventory_purchase_list_total;
	double supply_purchase_list_total;
	double service_purchase_list_total;
	double prepaid_asset_purchase_list_total;
	double return_list_total;
	double extended_total;
	double taxable_total;
	COST_BASIS *cost_basis;
	double total;
	double invoice_amount;
} PURCHASE_CALCULATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PURCHASE_CALCULATE *purchase_calculate_new(
		double purchase_sales_tax,
		double purchase_freight_in,
		LIST *fixed_asset_purchase_list
			/* Sets each cost_basis_fixed_asset */,
		boolean inventory_total_boolean,
		LIST *inventory_purchase_list
			/* Sets each cost_basis_inventory */,
		boolean specific_inventory_total_boolean,
		LIST *specific_inventory_purchase_list
			/* Sets each cost_basis_specific_inventory */,
		LIST *supply_purchase_list,
		LIST *service_purchase_list,
		boolean prepaid_asset_total_boolean,
		LIST *prepaid_asset_purchase_list,
		boolean return_total_total_boolean,
		LIST *purchase_return_list );

/* Process */
/* ------- */
PURCHASE_CALCULATE *purchase_calculate_calloc(
		void );

/* Usage */
/* ----- */
double purchase_calculate_extended_total(
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total );

/* Usage */
/* ----- */
double purchase_calculate_taxable_total(
		double fixed_asset_purchase_list_total,
		double supply_purchase_list_total );

/* Usage */
/* ----- */
double purchase_calculate_total(
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total );

/* Usage */
/* ----- */
double purchase_calculate_invoice_amount(
		double purchase_sales_tax,
		double purchase_freight_in,
		double purchase_calculate_return_total,
		double purchase_calculate_total );
