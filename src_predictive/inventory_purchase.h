/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"
#include "cost_basis.h"

#define INVENTORY_PURCHASE_TABLE	"inventory_purchase"

#define INVENTORY_PURCHASE_SELECT	"inventory_name,"	\
					"ordered_quantity,"	\
					"arrived_quantity,"	\
					"missing_quantity,"	\
					"unit_cost,"		\
					"extended_cost,"	\
					"retail_price,"		\
					"cost_basis,"		\
					"quantity_on_hand,"	\
					"average_unit_cost,"

typedef struct
{
	char *inventory_name;
	int ordered_quantity;
	int arrived_quantity;
	int missing_quantity;
	double unit_cost;
	double extended_cost;
	double retail_price;
	double cost_basis;
	int quantity_on_hand;
	double average_unit_cost;
	double inventory_purchase_extended_cost;

	/* Set externally */
	/* -------------- */
	COST_BASIS_INVENTORY *cost_basis_inventory;
	LIST *update_string_list;
} INVENTORY_PURCHASE;

/* Usage */
/* ----- */
INVENTORY_PURCHASE *inventory_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_PURCHASE *inventory_purchase_new(
		char *inventory_name );

/* Process */
/* ------- */
INVENTORY_PURCHASE *inventory_purchase_calloc(
		void );

/* Usage */
/* ----- */
#define INVENTORY_PURCHASE_EXTENDED_COST(			\
		ordered_quantity,				\
		unit_cost )					\
	( (double)ordered_quantity * unit_cost )

/* Usage */
/* ----- */
LIST *inventory_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		double extended_cost,
		double cost_basis,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double inventory_purchase_extended_cost,
		double cost_basis_amount );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
} INVENTORY_PURCHASE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_PURCHASE_LIST *inventory_purchase_list_new(
		const char *inventory_purchase_select,
		const char *inventory_purchase_table,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *purchase_primary_where );

/* Process */
/* ------- */
INVENTORY_PURCHASE_LIST *inventory_purchase_list_calloc(
		void );

/* Usage */
/* ----- */
LIST *inventory_purchase_list_primary_key_list(
		const char *sale_inventory_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_purchase_list_update_system_string(
		const char *inventory_purchase_table,
		LIST *inventory_purchase_list_primary_key_list );

/* Usage */
/* ----- */
double inventory_purchase_list_total(
		LIST *inventory_purchase_list );

