/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "purchase.h"
#include "fixed_asset.h"

#define FIXED_ASSET_PURCHASE_TABLE	"fixed_asset_purchase"

#define FIXED_ASSET_PURCHASE_SELECT	"asset_name,"			    \
					"serial_key,"			    \
					"full_name,"			    \
					"purchase_date_time,"		    \
					"service_placement_date,"	    \
					"fixed_asset_cost,"		    \
					"units_produced_so_far,"	    \
					"disposal_date,"		    \
					"recovery_class_year,"		    \
					"recovery_method,"		    \
					"recovery_convention,"		    \
					"recovery_system,"		    \
					"depreciation_method,"		    \
					"estimated_useful_life_years,"	    \
					"estimated_useful_life_units,"	    \
					"estimated_residual_value,"	    \
					"declining_balance_n,"		    \
					"cost_basis,"			    \
					"finance_accumulated_depreciation," \
					"tax_adjusted_basis"

typedef struct
{
	char *asset_name;
	char *serial_key;
	char *purchase_date_time;
	char *service_placement_date;
	double fixed_asset_cost;
	int units_produced_so_far;
	char *disposal_date;
	char *recovery_class_year_string;
	char *recovery_method;
	char *recovery_convention;
	char *recovery_system;
	/* enum depreciation_method depreciation_method_resolve; */
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	double declining_balance_n;
	double cost_basis;
	double finance_accumulated_depreciation;
	double tax_adjusted_basis;
	FIXED_ASSET *fixed_asset;
} FIXED_ASSET_PURCHASE;

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_list(
		const char *fixed_asset_purchase_select,
		const char *fixed_asset_purchase_table,
		char *purchase_primary_where,
		boolean entity_contact_key_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_purchase_system_string(
		char *entity_select_string,
		const char *fixed_asset_purchase_table,
		char *purchase_primary_where,
		char *order );

/* Usage */
/* ----- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *asset_name,
		char *serial_key,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
		char *asset_name,
		char *serial_key,
		boolean fetch_fixed_asset_boolean );

/* Process */
/* ------- */
FIXED_ASSET_PURCHASE *fixed_asset_purchase_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_asset_purchase_primary_where(
		char *asset_name,
		char *serial_key,
		char *purchase_primary_where );

/* Usage */
/* ----- */
double fixed_asset_purchase_cost_basis(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		double fixed_asset_cost );

/* Usage */
/* ----- */
double fixed_asset_purchase_total(
		LIST *fixed_asset_purchase_list );

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_primary_key_list(
		const char *purchase_asset_column,
		const char *sale_serial_key_column,
		LIST *purchase_primary_key_list /* out */ );

/* Usage */
/* ----- */
LIST *fixed_asset_purchase_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double fixed_asset_purchase_cost_basis );
