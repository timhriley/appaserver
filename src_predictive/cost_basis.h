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
	double extra_total;
	LIST *cost_basis_fixed_asset_list;
	double cost_basis_fixed_asset_percent_total;
	LIST *cost_basis_inventory_list;
	double cost_basis_inventory_percent_total;
	LIST *cost_basis_specific_inventory_list;
	double cost_basis_specific_inventory_percent_total;
	double percent_total;
	double sales_tax_expense;
	double freight_in_expense;
} COST_BASIS;

/* Usage */
/* ----- */
COST_BASIS *cost_basis_new(
		double sales_tax,
		double freight_in,
		LIST *fixed_asset_purchase_list,
		LIST *inventory_purchase_list,
		LIST *specific_inventory_purchase_list,
		double purchase_total );

/* Process */
/* ------- */
COST_BASIS *cost_basis_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_percent_of_total(
		double cost,
		double purchase_total );

/* Usage */
/* ----- */
double purchase_extra_allocated(
		double cost_basis_extra_total,
		double cost_basis_percent_of_total );

/* Usage */
/* ----- */
double cost_basis_amount(
		double cost,
		double cost_basis_extra_allocated );

typedef struct
{
	char *asset_name;
	char *serial_key;
	double cost_basis_percent_of_total;
	double cost_basis_extra_allocated;
	double cost_basis_amount;
} COST_BASIS_FIXED_ASSET;

/* Usage */
/* ----- */
LIST *cost_basis_fixed_asset_list(
		double cost_basis_extra_total,
		LIST *fixed_asset_purchase_list,
		double purchase_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_new(
		double cost_basis_extra_total,
		char *asset_name,
		char *serial_key,
		double fixed_asset_cost,
		double purchase_total );

/* Process */
/* ------- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_fixed_asset_percent_total(
		LIST *cost_basis_fixed_asset_list );


/* Usage */
/* ----- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_seek(
		LIST *cost_basis_fixed_asset_list,
		char *asset_name,
		char *serial_key );

typedef struct
{
	char *inventory_name;
	double cost_basis_percent_of_total;
	double cost_basis_extra_allocated;
	double cost_basis_amount;
} COST_BASIS_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_inventory_list(
		double cost_basis_extra_total,
		LIST *inventory_purchase_list,
		double purchase_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_INVENTORY *cost_basis_inventory_new(
		double cost_basis_extra_total,
		char *inventoryasset_name,
		double extended_cost,
		double purchase_total );

/* Process */
/* ------- */
COST_BASIS_INVENTORY *cost_basis_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_inventory_percent_total(
		LIST *cost_basis_inventory_list );


/* Usage */
/* ----- */
COST_BASIS_INVENTORY *cost_basis_inventory_seek(
		LIST *cost_basis_inventory_list,
		char *inventory_name );

typedef struct
{
	char *inventory_name;
	char *serial_key;
	double cost_basis_percent_of_total;
	double cost_basis_extra_allocated;
	double cost_basis_amount;
} COST_BASIS_SPECIFIC_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_specific_inventory_list(
		double cost_basis_extra_total,
		LIST *specific_inventory_purchase_list,
		double purchase_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_new(
		double cost_basis_extra_total,
		char *inventory_name,
		char *serial_key,
		double unit_cost,
		double purchase_total );

/* Process */
/* ------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_specific_inventory_percent_total(
		LIST *cost_basis_specific_inventory_list );

/* Usage */
/* ----- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_seek(
		LIST *cost_basis_specific_inventory_list,
		char *inventory_name,
		char *serial_key );

