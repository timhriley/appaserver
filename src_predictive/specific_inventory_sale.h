/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_sale.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define SPECIFIC_INVENTORY_SALE_TABLE	"specific_inventory_sale"

#define SPECIFIC_INVENTORY_SALE_SELECT	"inventory_name,"		\
					"serial_key,"			\
					"retail_price,"			\
					"discount_amount,"		\
					"extended_price,"		\
					"cost_of_goods_sold"

typedef struct
{
	char *inventory_name;
	char *serial_key;
	double retail_price;
	double discount_amount;
	double extended_price;
	double cost_of_goods_sold;
	double sale_extended_price;
	LIST *update_string_list;
	LIST *primary_key_list;
	char *sale_update_system_string;
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
double specific_inventory_sale_total(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
double specific_inventory_sale_CGS_total(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
LIST *specific_inventory_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double sale_extended_price );

/* Usage */
/* ----- */
LIST *specific_inventory_sale_primary_key_list(
		const char *sale_inventory_column,
		const char *sale_serial_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

