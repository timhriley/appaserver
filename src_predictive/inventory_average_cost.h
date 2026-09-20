/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average_cost.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "inventory_purchase.h"
#include "inventory_sale.h"

typedef struct
{
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_SALE *inventory_sale;
	int quantity_on_hand;
	double total_cost_balance;
	double average_unit_cost;
	double cost_of_goods_sold;
} INVENTORY_AVERAGE_COST;

/* Usage */
/* ----- */
LIST *inventory_average_cost_list(
		LIST *inventory_balance_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_new(
		INVENTORY_PURCHASE *inventory_purchase,
		INVENTORY_SALE *inventory_sale,
		int quantity_on_hand,
		double total_cost_balance,
		double average_unit_cost,
		double inventory_average_cost_of_goods_sold );

/* Process */
/* ------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_calloc(
		void );

/* Usage */
/* ----- */
double inventory_average_cost_prior_total_cost_balance(
		int ordered_quantity,
		double cost_basis_amount );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_purchase(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_PURCHASE *inventory_purchase );

/* Process */
/* ------- */
int inventory_average_cost_purchase_quantity_on_hand(
		int prior_quantity_on_hand,
		int ordered_quantity );

double inventory_average_cost_purchase_total_cost_balance(
		double prior_total_cost_balance,
		int ordered_quantity,
		double cost_basis_amount );

double inventory_average_cost_purchase_average_unit_cost(
		double inventory_average_cost_purchase_total_cost_balance,
		int inventory_average_cost_purchase_quantity_on_hand );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_sale(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_SALE *inventory_sale );

/* Process */
/* ------- */
int inventory_average_cost_sale_quantity_on_hand(
		int prior_quantity_on_hand,
		int quantity );

double inventory_average_cost_sale_average_unit_cost(
		double prior_average_unit_cost );

double inventory_average_cost_sale_total_cost_balance(
		int inventory_average_cost_sale_quantity_on_hand,
		double inventory_average_cost_sale_average_unit_cost );

double inventory_average_cost_of_goods_sold(
		int quantity_sold,
		double inventory_average_cost_sale_average_unit_cost );

