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
#include "entity.h"
#include "fixed_asset.h"
#include "recovery.h"
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
	/* Attributes */
	/* ---------- */
	FIXED_ASSET *fixed_asset;
	char *serial_label;
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *service_placement_date;
	double fixed_asset_cost;
	int units_produced_so_far;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	enum depreciation_method depreciation_method;
	char *disposal_date;

	/* Process */
	/* ------- */
	double cost_basis;
	double fixed_asset_purchase_list_total;
	double finance_accumulated_depreciation;
	double tax_adjusted_basis;
	DEPRECIATION *depreciation;
	RECOVERY *recovery;
} FIXED_ASSET_PURCHASE;

/* Operations */
/* ---------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_label,
			boolean fetch_last_depreciation,
			boolean fetch_last_recovery );

LIST *fixed_asset_purchase_list_fetch(
			char *where,
			boolean fetch_last_depreciation,
			boolean fetch_last_recovery );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
			char *asset_name,
			char *serial_label );

char *fixed_asset_purchase_primary_where(
			char *asset_name,
			char *serial_label );

char *fixed_asset_purchase_system_string(
			char *where,
			char *order );

LIST *fixed_asset_purchase_system_list(
			char *system_string,
			boolean fetch_last_depreciation,
			boolean fetch_last_recovery );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input,
			boolean fetch_last_depreciation,
			boolean fetch_last_recovery );

double fixed_asset_purchase_total(
			LIST *fixed_asset_purchase_list );

double fixed_asset_purchase_cost_basis(
			double fixed_asset_cost,
			double sales_tax,
			double freight_in,
			double fixed_asset_purchase_list_total );

FILE *fixed_asset_purchase_update_open(
			void );

void fixed_asset_purchase_update(
			FILE *update_pipe,
			double cost_basis,
			double finance_accumulated_depreciation,
			double tax_adjusted_basis,
			char *asset_name,
			char *serial_label );

 /* --------------------- */
 /* Returns static memory */
 /* --------------------- */
char *fixed_asset_purchase_escape_asset_name(
			char *asset_name );

/* Returns static memory */
/* --------------------- */
char *fixed_asset_purchase_primary_where(
			char *asset_name,
			char *serial_label );

double fixed_asset_purchase_total(
			LIST *fixed_asset_purchase_list );

/* -------------------------------------------- */
/* Returns fixed_asset_purchase_list with	*/
/* fixed_asset_purchase->depreciation set.	*/
/* -------------------------------------------- */
LIST *fixed_asset_purchase_list_depreciate(
			LIST *fixed_asset_purchase_list,
			char *depreciation_date );

LIST *fixed_asset_purchase_depreciation_list(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_update(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_depreciation_display(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_recovery_display(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_finance_fetch_update(
			char *asset_name,
			char *serial_label );

void fixed_asset_purchase_cost_fetch_update(
			char *asset_name,
			char *serial_label );

LIST *fixed_asset_purchase_list_cost_recover(
			LIST *fixed_asset_purchase_list,
			int tax_year );

LIST *fixed_asset_purchase_cost_recovery_list(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_add_depreciation_amount(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_subtract_recovery_amount(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_subtract_recovery_amount(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase );

void fixed_asset_purchase_list_negate_depreciation_amount(
			LIST *fixed_asset_purchase_list );

void fixed_asset_purchase_list_negate_recovery_amount(
			LIST *fixed_asset_purchase_list );

#endif

