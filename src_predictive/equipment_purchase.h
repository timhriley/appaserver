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
#include "depreciation.h"

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
	DEPRECIATION *equipment_purchase_depreciation;
} EQUIPMENT_PURCHASE;

/* Operations */
/* ---------- */
EQUIPMENT_PURCHASE *equipment_purchase_new(
			char *asset_name,
			char *serial_number );

/* ------------------------------------------------------- */
/* Returns with equipment_purchase->depreciation_list set. */
/* ------------------------------------------------------- */
EQUIPMENT_PURCHASE *equipment_purchase_parse(
			char *input,
			char *depreciate_folder_name );

EQUIPMENT_PURCHASE *equipment_purchase_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *service_placement_date );

/* Returns program memory */
/* ---------------------- */
char *equipment_purchase_select(
			void );

/* -------------------------------------------- */
/* Returns equipment_purchase_list with		*/
/* equipment_purchase->depreciation_list set.	*/
/* -------------------------------------------- */
LIST *equipment_purchase_list_fetch(
			char *asset_folder_name,
			char *depreciate_folder_name,
			char *where );

FILE *equipment_purchase_update_open(
			void );

void equipment_purchase_update(
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *service_placement_date );

 /* --------------------- */
 /* Returns static memory */
 /* --------------------- */
char *equipment_purchase_escape_asset_name(
			char *asset_name );

/* Safely returns heap memory */
/* -------------------------- */
char *equipment_purchase_primary_where(
			char *asset_name,
			char *serial_number );

double equipment_purchase_total(
			LIST *equipment_purchase_list );

LIST *equipment_system_list(
			char *sys_string );

DEPRECIATION *equipment_purchase_depreciation(
			EQUIPMENT_PURCHASE *equipment_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			/* ---------------------------- */
			/* Null value omits transaction */
			/* ---------------------------- */
			char *transaction_date_time,
			int units_produced );

LIST *equipment_purchase_depreciation_list(
			LIST *equipment_purchase_list );

/* -------------------------------------------- */
/* Returns equipment_purchase_list with		*/
/* equipment_purchase->depreciation_list set.	*/
/* -------------------------------------------- */
LIST *equipment_purchase_list_depreciate(
			LIST *equipment_purchase_list,
			char *depreciation_date,
			boolean set_depreciation_transaction );

void equipment_purchase_depreciation_table(
			LIST *equipment_purchase_list );

char *equipment_purchase_sys_string(
			char *asset_folder_name,
			char *where,
			char *order );

#endif

