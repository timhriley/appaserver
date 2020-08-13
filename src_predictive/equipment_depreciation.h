/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equipment_depreciation.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EQUIPMENT_DEPRECIATIION_H
#define EQUIPMENT_DEPRECIATIION_H

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
	char *asset_name;
	char *serial_number;
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *depreciation_date;
} EQUIPMENT_DEPRECIATION;

/* Operations */
/* ---------- */
EQUIPMENT_DEPRECIATION *equipment_depreciation_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date );

EQUIPMENT_DEPRECIATION *equipment_depreciation_parse(
			char *input );

EQUIPMENT_DEPRECIATION *equipment_depreciation_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time );

/* Returns program memory */
/* ---------------------- */
char *equipment_depreciation_select(
			void );

LIST *equipment_depreciation_list_fetch(
			char *where );

LIST *equipment_depreciation_list(
			char *equipment_purchase_primary_where );

FILE *equipment_depreciation_update_open(
			void );

void equipment_depreciation_update(
			double depreciation_amount,
			char *transaction_date_time,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time );

double equipment_depreciation_amount(
			LIST *equipment_depreciation_list );

#endif

