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
					"quantity,"		\
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
	int quantity;
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

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *purchase_date_time
			/* Mutually exclusive */,
		char *sale_date_time
			/* Mutually exclusive */,
		double current_purchase_cost_basis );

/* Process */
/* ------- */
INVENTORY_AVERAGE *inventory_average_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *inventory_average_cost_date_time(
		char *purchase_date_time,
		char *sale_date_time );

/* Returns parameter (used to set the name) */
/* ---------------------------------------- */
LIST *inventory_average_purchase_list(
		LIST *inventory_purchase_list );

/* Returns parameter (used to set the name) */
/* ---------------------------------------- */
LIST *inventory_average_sale_list(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
void inventory_average_set_current_purchase_cost_basis(
	char *purchase_date_time,
	double current_purchase_cost_basis,
	LIST *inventory_purchase_list );

/* Usage */
/* ----- */
boolean inventory_average_first_purchase_boolean(
		const char *inventory_purchase_table,
		const char *purchase_date_time_column,
		const char *sale_inventory_column,
		char *inventory_name,
		char *purchase_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_first_purchase_system_string(
		const char *inventory_purchase_table,
		const char *purchase_date_time_column,
		const char *sale_inventory_column,
		char *inventory_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *inventory_average_first_purchase_where(
		const char *sale_inventory_column,
		char *inventory_name );
