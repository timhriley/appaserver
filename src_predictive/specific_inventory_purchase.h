/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_purchase.h	*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SPECIFIC_INVENTORY_PURCHASE_H
#define SPECIFIC_INVENTORY_PURCHASE_H

#include "list.h"
#include "specific_inventory.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define SPECIFIC_INVENTORY_PURCHASE_TABLE	"specific_inventory_purchase"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *purchase_date_time;
	SPECIFIC_INVENTORY *specific_inventory;
	char *serial_label;
	char *arrived_date_time;
	double inventory_cost;
	double cost_basis;
	boolean arrived;
	boolean missing;
	boolean returned;
} SPECIFIC_INVENTORY_PURCHASE;

/* Operations */
/* ---------- */
SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *serial_label );

double specific_inventory_purchase_total(
			LIST *specific_inventory_purchase_list );

#endif
