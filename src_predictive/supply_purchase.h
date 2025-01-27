/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply_purchase.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SUPPLY_PURCHASE_H
#define SUPPLY_PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "supply.h"

/* Constants */
/* --------- */
#define SUPPLY_PURCHASE_TABLE	"supply_purchase"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */

typedef struct
{
	ENTITY *vendor_entity;
	SUPPLY *supply;
	char *purchase_date_time;
	int quantity;
	double unit_cost;
	double extended_cost;
} SUPPLY_PURCHASE;

/* Operations */
/* ---------- */
SUPPLY_PURCHASE *supply_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name );

LIST *supply_purchase_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

char *supply_purchase_system_string(
			char *where );

LIST *supply_purchase_system_list(
			char *system_string );

SUPPLY_PURCHASE *supply_purchase_parse(
			char *input );

double supply_purchase_total(
			LIST *supply_purchase_list );

#endif

