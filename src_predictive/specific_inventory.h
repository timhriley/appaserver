/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SPECIFIC_INVENTORY_H
#define SPECIFIC_INVENTORY_H

#include "list.h"
#include "boolean.h"
#include "entity.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *inventory_name;
	char *inventory_account;
	char *cost_of_goods_sold_account;
} SPECIFIC_INVENTORY;

/* Operations */
/* ---------- */
SPECIFIC_INVENTORY *specific_inventory_new(
			char *inventory_name );

#endif
