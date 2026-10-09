/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"

#define INVENTORY_AVERAGE_SELECT	"inventory_name,"	\
					"date_time_key,"	\
					"purchase_date_time,"	\
					"sale_date_time,"	\
					"ordered_quantity,"	\
					"arrived_quantity,"	\
					"slippage_quantity,"	\
					"sold_quantity,"	\
					"quantity_on_hand,"	\
					"total_cost_balance,"	\
					"average_unit_cost"

#define INVENTORY_AVERAGE_TABLE		"inventory_average"
#define INVENTORY_AVERAGE_DATE_COLUMN	"date_time_key"

typedef struct
{
	char *inventory_name;
	char *date_time_key;
	char *purchase_date_time;
	char *sale_date_time;
	int ordered_quantity;
	int arrived_quantity;
	int slippage_quantity;
	int sold_quantity;
	int quantity_on_hand;
	double total_cost_balance;
	double average_unit_cost;

	/* Set externally */
	/* -------------- */
	int inventory_average_quantity_on_hand;
	double inventory_average_unit_cost;
	double inventory_average_total_cost_balance;
	double cost_of_goods_sold;
} INVENTORY_AVERAGE;

/* Usage */
/* ----- */
INVENTORY_AVERAGE *inventory_average_fetch(
		const char *inventory_average_select,
		const char *inventory_average_table,
		char *inventory_average_primary_where );

/* Usage */
/* ----- */
INVENTORY_AVERAGE *inventory_average_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *date_time_key );

/* Process */
/* ------- */
INVENTORY_AVERAGE *inventory_average_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_primary_where(
		const char *inventory_column,
		const char *inventory_average_date_column,
		char *inventory_name,
		char *date_time_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *inventory_average_prior_purchase_date_time(
		const char *inventory_column,
		const char *purchase_date_time_column,
		char *inventory_name,
		char *purchase_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *inventory_average_prior_purchase_date_system_string(
		const char *inventory_column,
		const char *purchase_date_time_column,
		const char *inventory_average_table,
		char *inventory_name,
		char *purchase_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_prior_purchase_where(
		const char *inventory_column,
		const char *purchase_date_time_column,
		char *inventory_name,
		char *purchase_date_time );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *inventory_average_date_time_key(
		char *purchase_date_time,
		char *sale_date_time );

/* Usage */
/* ----- */
LIST *inventory_average_primary_key_list(
		const char *inventory_name,
		const char *inventory_average_date_column );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_primary_data_string(
		const char sql_delimiter,
		char *inventory_name,
		char *inventory_average_date_time_key );

typedef struct
{
	LIST *list;
	char *where;
	LIST *average_primary_key_list;
	char *average_update_system_string;
	LIST average_update_string_list;
	LIST *purchase_primary_key_list;
	char *purchase_update_system_string;
	LIST *purchase_update_string_list;
	LIST *inventory_primary_key_list;
	char *inventory_update_system_string;
	LIST *inventory_update_string_list;
	LIST *sale_primary_key_list;
	char *sale_update_system_string;
	LIST *sale_update_string_list;
} INVENTORY_AVERAGE_LIST;

/* Usage */
/* ----- */
INVENTORY_AVERAGE_LIST *inventory_average_list_new(
		char *inventory_name,
		char *inventory_average_date_time_key,
		char *inventory_average_primary_data_string,
		char *inventory_average_prior_purchase_date_time );

/* Process */
/* ------- */
INVENTORY_AVERAGE_LIST *inventory_average_list_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *inventory_average_list_where(
	const char *inventory_column,
	const char *inventory_average_date_column,
	char *inventory_name,
	char *inventory_average_prior_purchase_date_time );

LIST *inventory_average_list_average_update_string_list(
		char *inventory_average_primary_data_string,
		LIST *inventory_average_list );

/* Usage */
/* ----- */
void inventory_average_list_set(
		LIST *inventory_average_list
			/* Sets each inventory_average_quantity_on_hand */
			/* Sets each inventory_average_total_cost_balance */
			/* Sets each inventory_average_unit_cost */
			/* Sets each cost_of_goods_sold */ );

/* Process */
/* ------- */
int inventory_average_list_purchase_quantity_on_hand(
		int prior_quantity_on_hand,
		int arrived_quantity,
		int slippage_quantity );

double inventory_average_list_purchase_total_cost_balance(
		double prior_total_cost_balance,
		int ordered_quantity,
		double average_unit_cost );

double inventory_average_list_purchase_unit_cost(
		int inventory_average_purchase_quantity_on_hand,
		double inventory_average_total_cost_balance );

int inventory_average_list_sale_quantity_on_hand(
		int prior_quantity_on_hand,
		int sold_quantity );

double inventory_average_list_sale_total_cost_balance(
		int inventory_average_list_sale_quantity_on_hand,
		double inventory_average_unit_cost );

double inventory_average_list_cost_of_goods_sold(
		int sold_quantity,
		double inventory_average_unit_cost );

/* Usage */
/* ----- */
void inventory_average_list_update(
		INVENTORY_AVERAGE_LIST *inventory_average_list );

typedef struct
{
	int quantity_on_hand;
	double unit_cost;
	char *inventory_average_date_time_key;
	char *inventory_average_primary_where;
	boolean insert_boolean;
	char *insert_sql;
	LIST *inventory_average_primary_key_list;
	char *predictive_update_system_string;
	char *inventory_average_primary_data_string;
	LIST *update_string_list;
	char *inventory_average_prior_purchase_date_time;

	/* Set externally */
	/* -------------- */
	INVENTORY_AVERAGE_LIST *inventory_average_list;
} INVENTORY_AVERAGE_PURCHASE;

/* Usage */
/* ----- */
INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase_fetch(
		char *inventory_name,
		char *purchase_date_time,
		int ordered_quantity,
		int arrived_quantity,
		int slippage_quantity,
		double inventory_purchase_cost_basis );

/* Process */
/* ------- */
INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase_calloc(
		void );

int inventory_average_purchase_quantity_on_hand(
		int arrived_quantity,
		int slippage_quantity );

double inventory_average_purchase_unit_cost(
		int ordered_quantity,
		double inventory_purchase_cost_basis );

boolean inventory_average_purchase_insert_boolean(
		INVENTORY_AVERAGE *inventory_average_fetch );

/* Returns heap memory */
/* ------------------- */
char *inventory_average_purchase_insert_sql(
		const char *inventory_average_table,
		const char *inventory_column,
		const char *inventory_average_date_column,
		char *inventory_name,
		char *inventory_average_date_time_key );

LIST *inventory_average_purchase_update_string_list(
		const char sql_delimiter,
		char *purchase_date_time,
		int ordered_quantity,
		int arrived_quantity,
		int slippage_quantity,
		double total_cost_balance,
		int inventory_average_purchase_quantity_on_hand,
		double average_unit_cost,
		char *inventory_average_primary_data_string );

/* Driver */
/* ------ */
void inventory_average_purchase_save(
		char *inventory_name,
		INVENTORY_AVERAGE_PURCHASE *inventory_average_purchase
			/* In/out sets inventory_average_list */ );

typedef struct
{
	char *inventory_average_date_time_key;
	char *inventory_average_primary_where;
	boolean insert_boolean;
	char *insert_sql;
	LIST *inventory_average_primary_key_list;
	char *predictive_update_system_string;
	char *inventory_average_primary_data_string;
	LIST *update_string_list;
	char *inventory_average_prior_purchase_date_time;

	/* Set externally */
	/* -------------- */
	INVENTORY_AVERAGE_LIST *inventory_average_list;
} INVENTORY_AVERAGE_SALE;

/* Usage */
/* ----- */
INVENTORY_AVERAGE_SALE *inventory_average_sale_fetch(
		char *inventory_name,
		char *sale_date_time,
		int sold_quantity );

/* Process */
/* ------- */
INVENTORY_AVERAGE_SALE *inventory_average_sale_calloc(
		void );

LIST *inventory_average_sale_update_string_list(
		const char sql_delimiter,
		char *sale_date_time,
		int sold_quantity,
		char *inventory_average_primary_data_string );

/* Driver */
/* ------ */
void inventory_average_sale_save(
		char *inventory_name,
		INVENTORY_AVERAGE_SALE *inventory_average_sale
			/* In/out sets inventory_average_list */ );

