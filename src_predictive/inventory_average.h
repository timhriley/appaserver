/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"

typedef struct
{
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	char *cost_date_time;
	char *inventory_purchase_cost_where;
	LIST *purchase_list;
	char *inventory_sale_cost_where;
	LIST *sale_list;
	LIST *inventory_balance_list;
	LIST *inventory_average_cost_list;
	double cost_of_goods_sold;
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
