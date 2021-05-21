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
#include "tax_recovery.h"
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
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *service_placement_date;
	double fixed_asset_cost;
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
	double tax_accumulated_depreciation;
	TAX_RECOVERY *tax_recovery;
	LIST *depreciation_list;
	DEPRECIATION *depreciation;
} FIXED_ASSET_PURCHASE;

/* Operations */
/* ---------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_label,
			boolean fetch_depreciation_list );

LIST *fixed_asset_purchase_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			boolean fetch_depreciation_list );

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
			boolean fetch_depreciation_list );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input,
			boolean fetch_depreciation_list );

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
			double cost_basis,
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
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

void fixed_asset_purchase_depreciation_table(
			LIST *fixed_asset_purchase_list );

#endif

