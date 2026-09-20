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

#define INVENTORY_PURCHASE_SELECT	"full_name,"		\
					"inventory_name,"	\
					"purchase_date_time,"	\
					"ordered_quantity,"	\
					"arrived_date_time,"	\
					"arrived_quantity,"	\
					"slippage_quantity,"	\
					"unit_cost,"		\
					"extended_cost,"	\
					"cost_basis,"		\
					"quantity_on_hand,"	\
					"average_unit_cost"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *purchase_date_time;
	char *inventory_name;
	int ordered_quantity;
	char *arrived_date_time;
	int arrived_quantity;
	int slippage_quantity;
	double unit_cost;
	double extended_cost;
	double cost_basis;
	int quantity_on_hand;
	double average_unit_cost;
	double inventory_purchase_extended_cost;

	/* Set externally */
	/* -------------- */
	COST_BASIS_INVENTORY *cost_basis_inventory;
	double inventory_purchase_average_unit_cost;
	LIST *update_string_list;
} INVENTORY_PURCHASE;

/* Usage */
/* ----- */
INVENTORY_PURCHASE *inventory_purchase_parse(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
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
LIST *inventory_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *inventory_name,
		double extended_cost,
		double inventory_purchase_extended_cost,
		double cost_basis,
		double cost_basis_amount,
		double average_unit_cost,
		double inventory_purchase_average_unit_cost );

/* Usage */
/* ----- */
#define INVENTORY_PURCHASE_EXTENDED_COST(			\
		ordered_quantity,				\
		unit_cost )					\
	( (double)ordered_quantity * unit_cost )

/* Usage */
/* ----- */
double inventory_purchase_average_unit_cost(
		int ordered_quantity,
		double cost_basis_amount );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *inventory_purchase_cost_where(
		const char *inventory_purchase_table,
		const char *sale_inventory_column,
		const char *inventory_arrived_column,
		char *inventory_name,
		char *arrived_date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *inventory_purchase_prior_date_time(
		const char *inventory_purchase_table,
		const char *sale_inventory_column,
		const char *inventory_arrived_column,
		char *inventory_name,
		char *arrived_date_time );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
	LIST *update_string_list;
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
		char *where );

/* Process */
/* ------- */
INVENTORY_PURCHASE_LIST *inventory_purchase_list_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *inventory_purchase_list_select(
		const char *inventory_purchase_select,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Returns heap memory */
/* ------------------- */
char *inventory_purchase_list_system_string(
		char *inventory_purchase_list_select,
		const char *inventory_purchase_table,
		char *where,
		const char *inventory_arrived_column
			/* For order clause */ );

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

/* Usage */
/* ----- */
void inventory_purchase_list_set_average_unit_cost(
		LIST *inventory_purchase_list );

/* Usage */
/* ----- */
void inventory_purchase_list_set_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *inventory_purchase_list
			/* Set each update_string_list */ );

/* Usage */
/* ----- */
LIST *inventory_purchase_list_update_string_list(
		LIST *inventory_purchase_list );

