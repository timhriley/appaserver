/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_PURCHASE_H
#define FIXED_ASSET_PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "tax_recovery.h"
#include "entity.h"
#include "fixed_asset.h"
#include "depreciation.h"

/* Constants */
/* --------- */
#define FIXED_ASSET_PURCHASE_TABLE	"fixed_asset_purchase"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */

typedef struct
{
	FIXED_ASSET *fixed_asset;
	char *service_placement_date;
	double fixed_asset_cost;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method_string;
	double tax_cost_basis;
	char *tax_recovery_period;
	char *disposal_date;
	double finance_accumulated_depreciation;
	double tax_accumulated_depreciation;
	TAX_RECOVERY *tax_recovery;
	ENTITY *vendor_entity;
	DEPRECIATION *depreciation;
} FIXED_ASSET_PURCHASE;

/* Operations */
/* ---------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
			char *asset_name,
			char *serial_number );

/* ------------------------------------------------------- */
/* Returns with fixed_asset_purchase->depreciation_list set. */
/* ------------------------------------------------------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input,
			char *depreciate_folder_name );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *service_placement_date );

/* Returns program memory */
/* ---------------------- */
char *fixed_asset_purchase_select(
			void );

/* -------------------------------------------- */
/* Returns fixed_asset_purchase_list with		*/
/* fixed_asset_purchase->depreciation_list set.	*/
/* -------------------------------------------- */
LIST *fixed_asset_purchase_list_fetch(
			char *asset_folder_name,
			char *depreciate_folder_name,
			char *where );

FILE *fixed_asset_purchase_update_open(
			void );

void fixed_asset_purchase_update(
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
char *fixed_asset_purchase_escape_asset_name(
			char *asset_name );

/* Returns static memory */
/* --------------------- */
char *fixed_asset_purchase_primary_where(
			char *asset_name,
			char *serial_number );

double fixed_asset_purchase_total(
			LIST *fixed_asset_purchase_list );

LIST *fixed_asset_system_list(
			char *system_string );

DEPRECIATION *fixed_asset_purchase_depreciation(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			/* ---------------------------- */
			/* Null value omits transaction */
			/* ---------------------------- */
			char *transaction_date_time,
			int units_produced );

LIST *fixed_asset_purchase_list_depreciate(
			LIST *fixed_asset_purchase_list,
			char *depreciation_date );

/* -------------------------------------------- */
/* Returns fixed_asset_purchase_list with		*/
/* fixed_asset_purchase->depreciation_list set.	*/
/* -------------------------------------------- */
LIST *fixed_asset_purchase_list_depreciate(
			LIST *fixed_asset_purchase_list,
			char *depreciation_date,
			boolean set_depreciation_transaction );

void fixed_asset_purchase_depreciation_table(
			LIST *fixed_asset_purchase_list );

char *fixed_asset_purchase_sys_string(
			char *asset_folder_name,
			char *where,
			char *order );

#endif

