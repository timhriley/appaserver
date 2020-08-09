/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.h			*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks fixed_asset ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_ASSET_H
#define FIXED_ASSET_H

#include "tax_recovery.h"
#include "depreciation.h"
#include "list.h"
#include "boolean.h"
#include "inventory.h"
#include "ledger.h"

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
	char *debit_account_name;
	char *service_placement_date;
	double extension;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int estimated_residual_value;
	int declining_balance_n;
	char *depreciation_method;
	double tax_cost_basis;
	char *tax_recovery_period;
	char *disposal_date;
	double finance_accumulated_depreciation;
	double database_finance_accumulated_depreciation;
	double tax_accumulated_depreciation;
	double database_tax_accumulated_depreciation;
	DEPRECIATION *depreciation;
	TAX_RECOVERY *tax_recovery;
} FIXED_ASSET;

/* Operations */
/* ---------- */
FIXED_ASSET *fixed_asset_new(
					void );

FIXED_ASSET *fixed_asset_parse(
					char *input_buffer );

FIXED_ASSET *fixed_asset_purchase_fetch(
					char *application_name,
					char *asset_name,
					char *serial_number );

FIXED_ASSET *fixed_asset_prior_fetch(
					char *application_name,
					char *asset_name,
					char *serial_number );

FIXED_ASSET *fixed_asset_list_seek(
				LIST *fixed_asset_list,
				char *asset_name,
				char *serial_number );

char *fixed_property_get_select(
				char *application_name,
				char *folder_name,
				boolean with_account );

char *fixed_asset_get_select(
				char *application_name,
				char *folder_name,
				boolean with_account );

void fixed_asset_update_stream(
				FILE *update_pipe,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *asset_name,
				char *serial_number,
				double finance_accumulated_depreciation );

FILE *fixed_asset_get_update_pipe(
				char *application_name );

void fixed_asset_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation,
			double database_finance_accumulated_depreciation );

void fixed_asset_depreciation_update_and_transaction_propagate(
				FIXED_ASSET *fixed_asset,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_depreciation_delete(
				FIXED_ASSET *fixed_asset,
				char *application_name,
				char *fund_name );

void fixed_asset_list_depreciation_method_update(
				LIST *fixed_asset_purchase_list,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_list_depreciation_delete(
				LIST *fixed_asset_list,
				char *application_name,
				char *fund_name );

LIST *fixed_asset_get_account_name_list(
				char *application_name );

LIST *fixed_asset_account_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *purchase_asset_account_list );

void fixed_asset_depreciation_update_and_transaction_propagate(
				FIXED_ASSET *fixed_asset,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

char *fixed_asset_get_account_name(
				char *application_name,
				char *asset_name );

LIST *fixed_asset_purchase_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time );

LIST *fixed_property_depreciation_prior_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_property_depreciation_purchase_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_asset_depreciation_prior_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_asset_depreciation_purchase_fetch_list(
				char *application_name,
				char *fund_name );

char *fixed_asset_purchase_get_fund_where(
				char **folder_from,
				char *folder_name,
				char *fund_name );

char *fixed_asset_depreciation_get_where(
				char *fund_where,
				char *cost_attribute_name );

/*
void fixed_asset_depreciation_fund_list_update(
				LIST *depreciation_fund_list );

void fixed_asset_accumulated_depreciation_update(
				FILE *output_pipe,
				LIST *fixed_asset_list );

char *fixed_asset_accumulated_depreciation_get_update_sys_string(
				char *folder_name );
*/

LIST *fixed_asset_fetch_tax_list(
				char *application_name,
				int tax_year,
				char *folder_name );

char *fixed_asset_tax_get_where(
				int tax_year );

#endif

