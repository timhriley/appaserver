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
	char *date_time;
	char *inventory_purchase_cost_where;
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	char *inventory_sale_cost_where;
	INVENTORY_SALE_LIST *inventory_sale_list;
	LIST *inventory_balance_list;
	LIST *inventory_average_cost_list;
} INVENTORY_AVERAGE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *arrived_date_time
			/* Mutually exclusive */,
		char *completed_date_time
			/* Mutually exclusive */ );

/* Process */
/* ------- */
INVENTORY_AVERAGE *inventory_average_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *inventory_average_cost_date_time(
		char *arrived_date_time,
		char *completed_date_time );

