/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver depreciation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "ledger.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define FIXED_ASSET_DEPRECIATION_FOLDER		"fixed_asset_depreciation"
#define PRIOR_FIXED_ASSET_DEPRECIATION_FOLDER	"prior_fixed_asset_depreciation"
#define PROPERTY_DEPRECIATION_FOLDER		"property_depreciation"
#define PRIOR_PROPERTY_DEPRECIATION_FOLDER	"prior_property_depreciation"
#define DEPRECIATION_MEMO		"Depreciation"

/* Structures */
/* ---------- */
typedef struct
{
	int undo_fixed_asset_year;
	int undo_fixed_prior_year;
	int undo_property_year;
	int undo_property_prior_year;
	int max_undo_year;
} DEPRECIATION_TAX_YEAR;

typedef struct
{
	char *undo_fixed_asset_date;
	char *prior_fixed_asset_date;
	char *undo_fixed_prior_date;
	char *prior_fixed_prior_date;
	char *undo_property_date;
	char *prior_property_date;
	char *undo_property_prior_date;
	char *prior_property_prior_date;
	char *max_undo_date;
} DEPRECIATION_DATE;

typedef struct
{
	TRANSACTION *purchase_fixed_asset_transaction;
	TRANSACTION *prior_fixed_asset_transaction;
	TRANSACTION *purchase_property_transaction;
	TRANSACTION *prior_property_transaction;
} DEPRECIATION_TRANSACTION;

typedef struct
{
	double purchase_fixed_asset_depreciation_amount;
	double prior_fixed_asset_depreciation_amount;
	double purchase_property_depreciation_amount;
	double prior_property_depreciation_amount;
} DEPRECIATION_AMOUNT;

typedef struct
{
	LIST *purchase_fixed_asset_list;
	LIST *prior_fixed_asset_list;
	LIST *purchase_property_list;
	LIST *prior_property_list;
} DEPRECIATION_ASSET_LIST;

typedef struct
{
	char *fund_name;
	TRANSACTION *purchase_fixed_asset_transaction;
	TRANSACTION *prior_fixed_asset_transaction;
	TRANSACTION *purchase_property_transaction;
	TRANSACTION *prior_property_transaction;
	double purchase_fixed_asset_depreciation_amount;
	double prior_fixed_asset_depreciation_amount;
	double purchase_property_depreciation_amount;
	double prior_property_depreciation_amount;
	char *depreciation_expense_account;
	char *accumulated_depreciation_account;
	DEPRECIATION_AMOUNT *depreciation_amount;
	DEPRECIATION_TRANSACTION *depreciation_transaction;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
} DEPRECIATION_FUND;

typedef struct
{
	char *depreciation_date_string;
	int units_produced;
	double depreciation_amount;
	double database_depreciation_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} DEPRECIATION;

typedef struct
{
	DEPRECIATION_DATE *depreciation_date;
	DEPRECIATION_TAX_YEAR *depreciation_tax_year;
	LIST *depreciation_fund_list;
} DEPRECIATION_STRUCTURE;

/* Operations */
/* ---------- */
DEPRECIATION_TRANSACTION *depreciation_transaction_new(
			void );

DEPRECIATION_AMOUNT *depreciation_amount_new(
			void );

DEPRECIATION_STRUCTURE *depreciation_structure_new(
			char *application_name );

DEPRECIATION_ASSET_LIST *depreciation_asset_list_new(
			char *application_name,
			char *fund_name );

DEPRECIATION_DATE *depreciation_date_new(
			char *application_name );

DEPRECIATION_TAX_YEAR *depreciation_tax_year_new(
			char *application_name );

DEPRECIATION *depreciation_calloc(
			void );

DEPRECIATION_FUND *depreciation_fund_new(
			char *application_name,
			char *fund_name );

DEPRECIATION *depreciation_new(
			char *depreciation_date_string );

double depreciation_fixed_asset_calculate_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced );

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date );

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation );

LIST *depreciation_journal_ledger_refresh(
			char *application_name,
			char *fund_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double depreciation_amount );

double depreciation_get_fraction_of_year(
			char *prior_depreciation_date_string,
			char *depreciation_date_string );

double depreciation_n_declining_balance_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			int n );

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation );

int depreciation_fetch_max_tax_year(
			char *application_name,
			char *folder_name );

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *folder_name );

LIST *depreciation_fetch_fund_list(
			char *application_name );

LIST *fixed_asset_depreciation_purchase_fetch_list(
			char *application_name,
			char *fund_name );

void depreciation_fund_list_table_display(
			char *process_name,
			LIST *depreciation_fund_list );

void depreciation_fixed_asset_list_table_display(
			FILE *output_pipe,
			LIST *fixed_asset_list );

/* Sets the true depreciation_transaction->transaction_date_time */
/* ------------------------------------------------------------- */
void depreciation_fund_transaction_insert(
			LIST *depreciation_fund_list,
			char *application_name );

boolean depreciation_fund_asset_depreciation_insert(
			LIST *depreciation_fund_list,
			char *full_name,
			char *street_address );

void depreciation_asset_list_depreciation_insert(
			LIST *fixed_asset_list,
			char *folder_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

boolean depreciation_fund_list_transaction_set(
			LIST *depreciation_fund_list,
			char *full_name,
			char *street_address );

DEPRECIATION_AMOUNT *depreciation_asset_list_depreciation_amount_set(
			LIST *purchase_fixed_asset_list,
			LIST *prior_fixed_asset_list,
			LIST *purchase_property_list,
			LIST *prior_property_list );

double depreciation_asset_list_accumulate_depreciation_amount(
			LIST *fixed_asset_list );

DEPRECIATION_TRANSACTION *depreciation_fund_get_transaction(
			char *full_name,
			char *street_address,
			char *depreciation_expense_account,
			char *accumulated_depreciation_account,
			double purchase_fixed_asset_depreciation_amount,
			double prior_fixed_asset_depreciation_amount,
			double purchase_property_depreciation_amount,
			double prior_property_depreciation_amount );

void depreciation_fixed_asset_list_set(
			LIST *fixed_asset_list,
			char *depreciation_date_string,
			char *prior_depreciation_date );

void depreciation_fund_list_asset_list_set(
			LIST *depreciation_fund_list,
			char *depreciation_date_string,
			char *prior_fixed_asset_date,
			char *prior_fixed_prior_date,
			char *prior_property_date,
			char *prior_property_prior_date );

void depreciation_fund_list_asset_list_build(
			LIST *depreciation_fund_list,
			char *application_name );

void depreciation_fund_list_depreciation_amount_set(
			LIST *depreciation_fund_list );

#endif
