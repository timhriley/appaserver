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
#include "depreciation.h"
#include "tax_recovery.h"
#include "purchase.h"

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
	char *asset_account_name;
	char *service_placement_date;
	double fixed_asset_purchase_cost;
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
	DEPRECIATION *depreciation;
	TAX_RECOVERY *tax_recovery;
	PURCHASE_ORDER *purchase_order;
	LIST *fixed_asset_purchase_list;
} FIXED_ASSET_PURCHASE;

/* Operations */
/* ---------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
			char *asset_name,
			char *serial_number );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_number );

FIXED_ASSET_PURCHASE *fixed_asset_purchase_list_seek(
			LIST *fixed_asset_purchase_list,
			char *asset_name,
			char *serial_number );

char *fixed_asset_purchase_select(
			void );

LIST *fixed_asset_purchase_list_fetch(
			char *where_clause );

LIST *fixed_asset_purchase_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

FILE *fixed_asset_purchase_update_open(
			void );

void fixed_asset_purchase_update(
			double finance_accumulated_depreciation,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

#endif

