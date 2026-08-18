/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/cost_basis.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "purchase.h"

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
		double purchase_cost_basis_total );

/* Process */
/* ------- */
COST_BASIS *cost_basis_calloc(
		void );

double cost_basis_extra_total(
		double sales_tax,
		double freight_in );

double cost_basis_percent_total(
		double cost_basis_fixed_asset_percent_total,
		double cost_basis_inventory_percent_total,
		double cost_basis_specific_inventory_percent_total );

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
double cost_basis_percent_of_total(
		double cost,
		double purchase_cost_basis_total );

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
		double cost_basis_percent_of_total );

/* Usage */
/* ----- */
double cost_basis_freight_capitalized(
		double freight_in,
		double cost_basis_percent_of_total );

typedef struct
{
	char *asset_name;
	char *serial_key;
	double cost_basis_percent_of_total;
	double cost_basis_tax_capitalized;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
	PURCHASE *purchase;
	COST_BASIS *cost_basis;
} COST_BASIS_FIXED_ASSET;

/* Usage */
/* ----- */
LIST *cost_basis_fixed_asset_list(
		double sales_tax,
		double freight_in,
		LIST *fixed_asset_purchase_list,
		double purchase_cost_basis_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_new(
		double sales_tax,
		double freight_in,
		char *asset_name,
		char *serial_key,
		double fixed_asset_cost,
		double purchase_cost_basis_total );

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

/* Usage */
/* ----- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_seek(
		char *asset_name,
		char *serial_key,
		LIST *cost_basis_fixed_asset_list );

/* Usage */
/* ----- */
COST_BASIS_FIXED_ASSET *cost_basis_fixed_asset_fetch(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *asset_name,
		char *serial_key );

typedef struct
{
	char *inventory_name;
	double cost_basis_percent_of_total;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
	PURCHASE *purchase;
	COST_BASIS *cost_basis;
} COST_BASIS_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_inventory_list(
		double freight_in,
		LIST *inventory_purchase_list,
		double purchase_cost_basis_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_INVENTORY *cost_basis_inventory_new(
		double freight_in,
		char *inventory_name,
		double extended_cost,
		double purchase_cost_basis_total );

/* Process */
/* ------- */
COST_BASIS_INVENTORY *cost_basis_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_inventory_freight_capitalized(
		LIST *cost_basis_inventory_list );

/* Usage */
/* ----- */
COST_BASIS_INVENTORY *cost_basis_inventory_seek(
		char *inventory_name,
		LIST *cost_basis_inventory_list );

/* Usage */
/* ----- */
COST_BASIS_INVENTORY *cost_basis_inventory_fetch(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name );

typedef struct
{
	char *inventory_name;
	char *serial_key;
	double cost_basis_percent_of_total;
	double cost_basis_freight_capitalized;
	double cost_basis_amount;
	PURCHASE *purchase;
	COST_BASIS *cost_basis;
} COST_BASIS_SPECIFIC_INVENTORY;

/* Usage */
/* ----- */
LIST *cost_basis_specific_inventory_list(
		double freight_in,
		LIST *specific_inventory_purchase_list,
		double purchase_cost_basis_total );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_new(
		double freight_in,
		char *inventory_name,
		char *serial_key,
		double unit_cost,
		double purchase_cost_basis_total );

/* Process */
/* ------- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_calloc(
		void );

/* Usage */
/* ----- */
double cost_basis_specific_inventory_freight_capitalized(
		LIST *cost_basis_inventory_list );

/* Usage */
/* ----- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_seek(
		char *inventory_name,
		char *serial_key,
		LIST *cost_basis_specific_inventory_list );

/* Usage */
/* ----- */
COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory_fetch(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key );
