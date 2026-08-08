/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define INVENTORY_SALE_TABLE			"inventory_sale"

#define INVENTORY_SALE_SELECT			"inventory_name,"	\
						"quantity,"		\
						"retail_price,"		\
						"discount_amount,"	\
						"extended_price,"	\
						"cost_of_goods_sold"

typedef struct
{
	char *inventory_name;
	int quantity;
	double retail_price;
	double discount_amount;
	double extended_price;
	double cost_of_goods_sold;
	double sale_extended_price;
	char *primary_data_string;
	char *update_string;
	LIST *primary_key_list;
	char *update_system_string;
} INVENTORY_SALE;

/* Usage */
/* ----- */
LIST *inventory_sale_list(
		const char *inventory_sale_select,
		const char *inventory_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
INVENTORY_SALE *inventory_sale_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_SALE *inventory_sale_new(
		char *inventory_name );

/* Process */
/* ------- */
INVENTORY_SALE *inventory_sale_calloc(
		void );

/* Usage */
/* ----- */
INVENTORY_SALE *inventory_sale_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_primary_where(
		const char *sale_inventory_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double inventory_sale_total(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
double inventory_sale_CGS_total(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_update_string(
		char *inventory_sale_primary_data_string,
		double sale_extended_price );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_update_system_string(
		const char *inventory_sale_table,
		LIST *inventory_sale_primary_key_list );

/* Usage */
/* ----- */
LIST *inventory_sale_primary_key_list(
		const char *sale_inventory_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_join(
		const char *inventory_sale_table,
		const char *foreign_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *sale_date_time_column,
		boolean entity_contact_key_boolean );

