/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/cost_basis.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

typedef struct
{
	LIST *cost_basis_fixed_asset_list;
	double cost_basis_fixed_asset_tax_capitalized;
	double cost_basis_fixed_asset_freight_capitalized;
	LIST *cost_basis_inventory_list;
	double cost_basis_inventory_freight_capitalized;
	LIST *cost_basis_specific_inventory_list;
	double cost_basis_specific_inventory_freight_capitalized;
	double sales_tax_expense;
	double freight_in_expense;
} COST_BASIS;

/* Usage */
/* ----- */
COST_BASIS *cost_basis_new(
		double purchase_sales_tax,
		double purchase_freight_in,
		LIST *fixed_asset_purchase_list,
		LIST *inventory_purchase_list,
		LIST *specific_inventory_purchase_list,
		double purchase_calculate_taxable_total,
		double purchase_calculate_extended_total );

/* Process */
/* ------- */
COST_BASIS *cost_basis_calloc(
		void );

double cost_basis_sales_tax_expense(
		double purchase_sales_tax,
		double cost_basis_fixed_asset_tax_capitalized );

double cost_basis_freight_in_expense(
		double purchase_freight_in,
		double cost_basis_fixed_asset_freight_capitalized,
		double cost_basis_inventory_freight_capitalized,
		double cost_basis_specific_inventory_freight_capitalized );

/* Usage */
/* ----- */
double cost_basis_taxable_percent_of_total(
		double cost,
		double purchase_calculate_taxable_total );

/* Usage */
/* ----- */
double cost_basis_extended_percent_of_total(
		double cost,
		double purchase_calculate_extended_total );

/* Usage */
/* ----- */
double cost_basis_amount(
		double cost,
		double cost_basis_tax_capitalized,
		double cost_basis_freight_capitalized );

/* Usage */
/* ----- */
double cost_basis_tax_capitalized(
		double sales_tax,
		double cost_basis_tax_percent_of_total );

/* Usage */
/* ----- */
double cost_basis_freight_capitalized(
		double freight_in,
		double cost_basis_extended_percent_of_total );

typedef struct
{
	double cost_basis_taxable_percent_of_total;
	double cost_basis_tax_capitalized;
	double cost_basis_extended_percent_of_total;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
} COST_BASIS_FIXED_ASSET;

/* Usage */
/* ----- */
LIST *cost_basis_fixed_asset_list(
		double sales_tax,
		double freight_in,
		LIST *fixed_asset_purchase_list,
		double purchase_calculate_tax_total,
		double purchase_calculate_extended_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_new(
		double sales_tax,
		double freight_in,
		double fixed_asset_cost,
		double purchase_calculate_taxable_total,
		double purchase_calculate_extended_total );

/* Process */
/* ------- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_fixed_asset_tax_capitalized(
		LIST *cost_basis_fixed_asset_list );

/* Usage */
/* ----- */
double cost_basis_fixed_asset_freight_capitalized(
		LIST *cost_basis_fixed_asset_list );

typedef struct
{
	double cost_basis_extended_percent_of_total;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
} COST_BASIS_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_inventory_list(
		double freight_in,
		LIST *inventory_purchase_list,
		double purchase_calculate_extended_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_INVENTORY *cost_basis_inventory_new(
		double freight_in,
		double inventory_purchase_extended_cost,
		double purchase_calculate_extended_total );

/* Process */
/* ------- */
COST_BASIS_INVENTORY *cost_basis_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_inventory_freight_capitalized(
		LIST *cost_basis_inventory_list );

typedef struct
{
	double cost_basis_extended_percent_of_total;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
} COST_BASIS_SPECIFIC_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_specific_inventory_list(
		double freight_in,
		LIST *specific_inventory_purchase_list,
		double purchase_calculate_extended_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_new(
		double freight_in,
		double unit_cost,
		double purchase_calculate_extended_total );

/* Process */
/* ------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_specific_inventory_freight_capitalized(
		LIST *cost_basis_inventory_list );
