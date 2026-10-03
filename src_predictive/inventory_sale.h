/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "inventory_average.h"
#include "inventory_purchase_update.h"

#define INVENTORY_SALE_SELECT		"full_name,"			\
					"sale_date_time,"		\
					"inventory_name,"		\
					"quantity,"			\
					"retail_price,"			\
					"discount_amount,"		\
					"extended_price,"		\
					"cost_of_goods_sold,"		\
					"inventory_markup_percent"

#define INVENTORY_SALE_TABLE		"inventory_sale"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *inventory_name;
	int quantity;
	double retail_price;
	double discount_amount;
	double extended_price;
	double cost_of_goods_sold;
	int markup_percent;
	double sale_extended_price;
	INVENTORY_AVERAGE *inventory_average;
	int inventory_sale_markup_percent;
	LIST *update_string_list;
	INVENTORY_PURCHASE_UPDATE *inventory_purchase_update;
} INVENTORY_SALE;

/* Usage */
/* ----- */
INVENTORY_SALE *inventory_sale_parse(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean inventory_average_boolean,
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
int inventory_sale_markup_percent(
		double sale_extended_price,
		double cost_of_goods_sold );

/* Process */
/* ------- */
double inventory_sale_markup_ratio(
		double extended_price,
		double cost_of_goods_sold );

/* Usage */
/* ----- */
LIST *inventory_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *inventory_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double extended_price,
		double sale_extended_price,
		LIST *inventory_average_cost_list,
		int markup_percent,
		int inventory_sale_markup_percent );

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

/* Returns heap memory or null */
/* --------------------------- */
char *inventory_sale_cost_where(
		const char *inventory_sale_table,
		const char *sale_inventory_column,
		const char *sale_date_time_column,
		char *inventory_name,
		char *sale_date_time );

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

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
	LIST *update_string_list;
	double extended_total;
	double CGS_total;
} INVENTORY_SALE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_SALE_LIST *inventory_sale_list_new(
		const char *inventory_sale_select,
		const char *inventory_sale_table,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *where,
		boolean inventory_average_boolean );

/* Process */
/* ------- */
INVENTORY_SALE_LIST *inventory_sale_list_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_list_select(
		const char *inventory_sale_select,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_list_system_string(
		char *inventory_sale_list_select,
		const char *inventory_sale_table,
		char *where,
		const char *sale_date_time_column
			/* For order clause */ );

/* Usage */
/* ----- */
LIST *inventory_sale_list_primary_key_list(
		const char *sale_inventory_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_list_update_system_string(
		const char *inventory_sale_table,
		LIST *inventory_sale_list_primary_key_list );

/* Usage */
/* ----- */
LIST *inventory_sale_list_update_string_list(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
double inventory_sale_list_extended_total(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
double inventory_sale_list_CGS_total(
		LIST *inventory_sale_list );

