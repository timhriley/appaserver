/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_balance.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "inventory_purchase.h"
#include "inventory_sale.h"

typedef struct
{
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_SALE *inventory_sale;
} INVENTORY_BALANCE;

/* Usage */
/* ----- */
LIST *inventory_balance_list(
		LIST *inventory_purchase_list,
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
INVENTORY_BALANCE *inventory_balance_calloc(
		void );

