/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale_return.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_SALE_RETURN_H
#define INVENTORY_SALE_RETURN_H

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
	ENTITY *customer_entity;
	char *sale_date_time;
	char *inventory_name;
	char *return_date_time;
	int returned_quantity;
	double inventory_sale_return_cost_of_goods_sold;
	double inventory_sale_return_restocking_fee;
	TRANSACTION *inventory_sale_return_transaction;
} INVENTORY_SALE_RETURN;

/* Operations */
/* ---------- */

#endif

