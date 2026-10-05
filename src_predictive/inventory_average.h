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
	double inventory_average_total_cost_balance;
	double inventory_average_unit_cost;
	double inventory_average_cost_of_goods_sold;
} INVENTORY_AVERAGE;

/* Usage */
/* ----- */
INVENTORY_AVERAGE *inventory_average_fetch(
		const char *inventory_average_select,
		const char *inventory_average_table,
		char *inventory_average_primary_where );

/* Usage */
/* ----- */
LIST *inventory_average_list(
		char *inventory_name,
		char *prior_date_time_key );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_list_where(
		char *inventory_name,
		char *prior_date_time_key );

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
		char *purchase_date_time_column,
		char *inventory_name,
		char *purchase_date_time );

/* Usage */
/* ----- */
void inventory_average_list_set(
		LIST *inventory_average_list
			/* Sets each inventory_average_quantity_on_hand */
			/* Sets each inventory_average_total_cost_balance */
			/* Sets each inventory_average_unit_cost */
			/* Sets each cost_of_goods_sold */ );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *inventory_average_date_time_key(
		char *purchase_date_time,
		char *sale_date_time );

typedef struct
{
	INVENTORY_AVERAGE *inventory_average;
	boolean update_boolean;
	char *inventory_average_prior_purchase_date_time;
	LIST *inventory_average_list;
}

