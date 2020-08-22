/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equipment_purchase.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EQUIPMENT_PURCHASE_H
#define EQUIPMENT_PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "tax_recovery.h"
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
	char *service_placement_date;
	double equipment_cost;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method;
	double tax_cost_basis;
	char *tax_recovery_period;
	char *disposal_date;
	double finance_accumulated_depreciation;
	double tax_accumulated_depreciation;
	TAX_RECOVERY *tax_recovery;
	char *asset_account_name;
	LIST *depreciation_list;
} EQUIPMENT_PURCHASE;

/* Operations */
/* ---------- */
EQUIPMENT_PURCHASE *equipment_purchase_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

EQUIPMENT_PURCHASE *equipment_purchase_parse(
			char *input );

EQUIPMENT_PURCHASE *equipment_purchase_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Returns program memory */
/* ---------------------- */
char *equipment_purchase_select(
			void );

LIST *equipment_purchase_list_fetch(
			char *where );

LIST *equipment_purchase_list(
			char *purchase_primary_where );

FILE *equipment_purchase_update_open(
			void );

void equipment_purchase_update(
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

 /* --------------------- */
 /* Returns static memory */
 /* --------------------- */
char *equipment_purchase_escape_asset_name(
			char *asset_name );

/* Safely returns heap memory */
/* -------------------------- */
char *equipment_purchase_primary_where(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double equipment_purchase_total(
			LIST *equipment_purchase_list );

LIST *equipment_system_list(
			char *sys_string );

#endif

