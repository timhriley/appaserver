/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_purchase.h	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"
#include "cost_basis.h"

#define SPECIFIC_INVENTORY_PURCHASE_TABLE	"specific_inventory_purchase"

#define SPECIFIC_INVENTORY_PURCHASE_SELECT	"inventory_name,"	\
						"serial_key,"		\
						"unit_cost,"		\
						"retail_price,"		\
						"cost_basis"

typedef struct
{
	char *inventory_name;
	char *serial_key;
	double unit_cost;
	double retail_price;
	double cost_basis;

	/* Set externally */
	/* -------------- */
	COST_BASIS_SPECIFIC_INVENTORY *cost_basis_specific_inventory;
	char *update_string;
} SPECIFIC_INVENTORY_PURCHASE;

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_new(
		char *inventory_name,
		char *serial_key );

/* Process */
/* ------- */
SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_purchase_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		double cost_basis,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double cost_basis_amount );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
} SPECIFIC_INVENTORY_PURCHASE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list_new(
		const char *specific_inventory_purchase_select,
		const char *specific_inventory_purchase_table,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *purchase_primary_where );

/* Process */
/* ------- */
SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list_calloc(
		void );

/* Usage */
/* ----- */
LIST *specific_inventory_purchase_list_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_purchase_list_update_system_string(
		const char *specific_inventory_purchase_table,
		LIST *specific_inventory_purchase_list_primary_key_list );

/* Usage */
/* ----- */
double specific_inventory_purchase_list_total(
		LIST *specific_inventory_purchase_list );

/* Usage */
/* ----- */
LIST *specific_inventory_purchase_list_update_string_list(
		LIST *specific_inventory_purchase_list );

