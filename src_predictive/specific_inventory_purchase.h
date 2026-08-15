/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_purchase.h	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"

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
	LIST *update_string_list;
	LIST *primary_key_list;
	char *update_system_string;
} SPECIFIC_INVENTORY_PURCHASE;

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

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
LIST *specific_inventory_purchase_list(
		const char *specific_inventory_purchase_select,
		const char *specific_inventory_purchase_table,
		char *purchase_primary_where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_purchase_primary_where(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		char *inventory_name,
		char *serial_key,
		char *purchase_primary_where );

/* Usage */
/* ----- */
double specific_inventory_purchase_cost_basis(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		double unit_cost );

/* Usage */
/* ----- */
LIST *specific_inventory_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double specific_inventory_purchase_cost_basis );

/* Usage */
/* ----- */
LIST *specific_inventory_purchase_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_purchase_update_system_string(
		const char *specific_inventory_purchase_table,
		LIST *specific_inventory_purchase_primary_key_list );

/* Usage */
/* ----- */
double specific_inventory_purchase_total(
		LIST *specific_inventory_purchase_list );

