/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equipment_purchase.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EQUIPMENT_PURCHASE_H
#define EQUIPMENT_PURCHASE_H

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
	double equipment_purchase_cost;
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
	LIST *equipment_purchase_list;
} EQUIPMENT_PURCHASE;

/* Operations */
/* ---------- */
EQUIPMENT_PURCHASE *equipment_purchase_new(
			char *asset_name,
			char *serial_number );

EQUIPMENT_PURCHASE *equipment_purchase_parse(
			char *input );

EQUIPMENT_PURCHASE *equipment_purchase_fetch(
			char *asset_name,
			char *serial_number );

EQUIPMENT_PURCHASE *equipment_purchase_list_seek(
			LIST *equipment_purchase_list,
			char *asset_name,
			char *serial_number );

/* Returns program memory */
/* ---------------------- */
char *equipment_purchase_select(
			void );

LIST *equipment_purchase_list(
			char *where_clause );

FILE *equipment_purchase_update_open(
			void );

void equipment_purchase_update(
			double finance_accumulated_depreciation,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

#endif

