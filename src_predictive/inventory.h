/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"

#define INVENTORY_TABLE			"inventory"

#define INVENTORY_SELECT		"inventory_name,"		\
					"inventory_account,"		\
					"cost_of_goods_sold_account,"	\
					"inventory_category,"		\
					"retail_price,"			\
					"reorder_quantity,"		\
					"quantity_on_hand,"		\
					"average_unit_cost,"		\
					"total_cost_balance"

#define INVENTORY_LABOR_CHARGE_COLUMN	"labor_charge"

typedef struct
{
	char *inventory_name;
	char *inventory_account;
	char *cost_of_goods_sold_account;
	char *inventory_category;
	double retail_price;
	int reorder_quantity;
	int quantity_on_hand;
	double average_unit_cost;
	double total_cost_balance;
	double labor_charge;
} INVENTORY;

/* Usage */
/* ----- */
INVENTORY *inventory_fetch(
		char *inventory_name );

/* Usage */
/* ----- */
INVENTORY *inventory_parse(
		boolean inventory_labor_charge_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY *inventory_new(
		char *inventory_name );

/* Process */
/* ------- */
INVENTORY *inventory_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_primary_where(
		const char *sale_inventory_column,
		char *inventory_name );

/* Usage */
/* ----- */
boolean inventory_labor_charge_boolean(
		const char *inventory_table,
		const char *inventory_labor_charge_column );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_select_string(
		const char *inventory_select,
		const char *inventory_labor_charge_column,
		boolean inventory_labor_charge_boolean );

