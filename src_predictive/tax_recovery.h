/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_recovery.h			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TAX_RECOVERY_H
#define TAX_RECOVERY_H

#include "list.h"
#include "boolean.h"
#include "String.h"
#include "html_table.h"
#include "date.h"

/* Constants */
/* --------- */
#define TAX_RECOVERY_TABLE		"tax_recovery"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *asset_name;
	char *serial_label;
	int tax_year;
	char *service_placement_date;
	char *disposal_date;
	double cost_basis;
	char *tax_recovery_period;
	double prior_accumulated_recovery;

	/* Process */
	/* ------- */
	double tax_recovery_period_years;
	int tax_recovery_prior_recovery_year;
	int service_month;
	int service_year;
	int disposal_month;
	int disposal_year;
	double recovery_percent;
	double tax_recovery_amount;
} TAX_RECOVERY;

/* Operations */
/* ---------- */
TAX_RECOVERY *tax_recovery_evaluate(
			char *asset_name,
			char *serial_label,
			int tax_year,
			char *service_placement_date,
			char *disposal_date,
			double cost_basis,
			char *tax_recovery_period,
			double prior_accumulated_recovery );

TAX_RECOVERY *tax_recovery_new(
			char *asset_name,
			char *serial_label,
			int tax_year );

double tax_recovery_calculate_recovery_amount(
			double *recovery_percent,
			double cost_basis,
			char *service_placement_date,
			char *sale_date_string,
			double recovery_period_years,
			int tax_year );

int tax_recovery_fetch_max_tax_year(
			char *application_name,
			char *folder_name );

void tax_recovery_fixed_asset_list_set(
			LIST *fixed_asset_list,
			int tax_year );

void tax_recovery_depreciation_fund_list_set(
			LIST *depreciation_fund_list,
			int tax_year );

boolean tax_recovery_fixed_assets_insert(
			LIST *deprecation_fund_list );

void tax_recovery_fixed_asset_list_insert(
			char *folder_name,
			LIST *fixed_asset_list );

void tax_recovery_fixed_assets_display(
			LIST *depreciation_fund_list );

void tax_recovery_fixed_asset_list_display(
			FILE *output_pipe,
			LIST *fixed_asset_list );

int tax_recovery_now_year(
			void );

int tax_recovery_prior_recovery_year(
			char *asset_name,
			char *serial_label );

double tax_recovery_period_years(
			char *recovery_period_years );

TAX_RECOVERY *tax_recovery_parse(
			char *input );

double tax_recovery_amount(
			double *recovery_percent,
			int tax_year,
			int service_month,
			int service_year,
			int disposal_month,
			int disposal_year,
			double cost_basis,
			double tax_recovery_period_years,
			double prior_accumulated_recovery );

#endif
