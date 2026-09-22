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
			/* Mutually exclusive */ );

/* Process */
/* ------- */
INVENTORY_AVERAGE *inventory_average_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *inventory_average_cost_date_time(
		char *purchase_date_time,
		char *sale_date_time );

LIST *inventory_average_purchase_list(
		LIST *inventory_purchase_list );

LIST *inventory_average_sale_list(
		LIST *inventory_sale_list );

