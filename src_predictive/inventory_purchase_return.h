/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase_return.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_PURCHASE_RETURN_H
#define INVENTORY_PURCHASE_RETURN_H

#include "list.h"
#include "transaction.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *sale_date_time;
	char *purchase_date_time;
	char *inventory_name;
	char *return_date_time;
	int returned_quantity;
	double inventory_purchase_return_sales_tax;
	TRANSACTION *inventory_purchase_return_transaction;
} INVENTORY_PURCHASE_RETURN;

/* Operations */
/* ---------- */

#endif

