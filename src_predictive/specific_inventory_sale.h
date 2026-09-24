/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_sale.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define SPECIFIC_INVENTORY_SALE_SELECT	"full_name,"			\
					"sale_date_time,"		\
					"inventory_name,"		\
					"serial_key,"			\
					"retail_price,"			\
					"unit_cost,"			\
					"discount_amount,"		\
					"extended_price"

#define SPECIFIC_INVENTORY_SALE_TABLE	"specific_inventory_sale"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *inventory_name;
	char *serial_key;
	double retail_price;
	double unit_cost;
	double discount_amount;
	double extended_price;
	double sale_extended_price;
	char *update_string;
} SPECIFIC_INVENTORY_SALE;

/* Usage */
/* ----- */
LIST *specific_inventory_sale_list(
		const char *specific_inventory_sale_select,
		const char *specific_inventory_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_parse(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_new(
		char *inventory_name,
		char *serial_key );

/* Process */
/* ------- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_calloc(
		void );

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_primary_where(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_update_string(
		const char sql_delimiter,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		double sale_extended_price );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_primary_data_string(
		const char sql_delimiter,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
	LIST *update_string_list;
	double extended_total;
	double CGS_total;
} SPECIFIC_INVENTORY_SALE_LIST;

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list_new(
		const char *specific_inventory_sale_select,
		const char *specific_inventory_sale_table,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *where );

/* Process */
/* ------- */
SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_list_select(
		const char *specific_inventory_sale_select,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_list_system_string(
		char *specific_inventory_sale_list_select,
		const char *specific_inventory_sale_table,
		char *where,
		const char *sale_date_time_column
			/* For order clause */ );

/* Usage */
/* ----- */
LIST *specific_inventory_sale_list_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_list_update_system_string(
		const char *specific_inventory_sale_table,
		LIST *specific_inventory_sale_list_primary_key_list );

/* Usage */
/* ----- */
LIST *specific_inventory_sale_list_update_string_list(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
double specific_inventory_sale_list_extended_total(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
double specific_inventory_sale_list_CGS_total(
		LIST *specific_inventory_sale_list );

