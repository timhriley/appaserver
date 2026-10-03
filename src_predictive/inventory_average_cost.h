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
		LIST *inventory_balance_list,
		boolean inventory_average_first_purchase_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_new(
		INVENTORY_PURCHASE *inventory_purchase
			/* mutually exclusive */,
		INVENTORY_SALE *inventory_sale
			/* mutually exclusive */,
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

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_prior_first(
		INVENTORY_PURCHASE *inventory_purchase );

/* Process */
/* ------- */
int inventory_average_cost_prior_quantity_on_hand(
		int arrived_quantity,
		int slippage_quantity );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_prior(
		INVENTORY_PURCHASE *inventory_purchase );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE_COST *inventory_average_cost_purchase(
		INVENTORY_AVERAGE_COST *prior_inventory_average_cost,
		INVENTORY_PURCHASE *inventory_purchase );

/* Usage */
/* ----- */
double inventory_average_cost_prior_total_cost_balance(
		int inventory_average_cost_prior_quantity_on_hand,
		double inventory_average_cost_prior_average_unit_cost );

/* Usage */
/* ----- */
double inventory_average_cost_purchase_unit_cost(
		double total_cost_balance,
		int quantity_on_hand );

/* Usage */
/* ----- */
double inventory_average_cost_purchase_total_cost_balance(
		double prior_total_cost_balance,
		double cost_basis );

/* Usage */
/* ----- */
int inventory_average_cost_purchase_arrived_quantity_on_hand(
		int prior_quantity_on_hand,
		int arrived_quantity,
		int slippage_quantity );

/* Usage */
/* ----- */
int inventory_average_cost_purchase_quantity_on_hand(
		int prior_quantity_on_hand,
		int ordered_quantity );

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

/* Usage */
/* ----- */
INVENTORY_AVERAGE_COST *inventory_average_cost_sale_seek(
		LIST *inventory_average_cost_list,
		char *sale_date_time );

/* Usage */
/* ----- */

/* Returns cost_of_goods_sold */
/* -------------------------- */
double inventory_average_cost_get(
		INVENTORY_AVERAGE_COST *inventory_average_cost );

/* Usage */
/* ----- */
LIST *inventory_average_cost_list_purchase_update_string_list(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *inventory_average_cost_list );

/* Usage */
/* ----- */
LIST *inventory_average_cost_list_sale_update_string_list(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *inventory_average_cost_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_average_cost_list_display(
		LIST *inventory_average_cost_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_cost_display(
		INVENTORY_AVERAGE_COST *inventory_average_cost );

