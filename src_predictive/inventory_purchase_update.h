/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase_update.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *update_system_string;
	LIST *update_string_list;
} INVENTORY_PURCHASE_UPDATE;

/* Usage */
/* ----- */

/* Used to set INVENTORY_PURCHASE.quantity_on_hand */
/* Safely returns				   */
/* ----------------------------------------------- */
INVENTORY_PURCHASE_UPDATE *inventory_purchase_update_new(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *inventory_average_cost_list );

/* Process */
/* ------- */
INVENTORY_PURCHASE_UPDATE *inventory_purchase_update_calloc(
		void );

/* Usage */
/* ----- */
LIST *inventory_purchase_update_average_cost_list(
		LIST *inventory_average_cost_list );

