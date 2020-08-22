/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPRECIATION_TABLE_NAME		"depreciation"
#define DEPRECIATION_MEMO		"Depreciation"

/* Structures */
/* ---------- */
typedef struct
{
	char *asset_name;
	char *serial_number;
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *depreciation_date;
	int units_produced;
	double depreciation_amount;
	double depreciation_accumulated_depreciation;
	TRANSACTION *depreciation_transaction;
} DEPRECIATION;

/* Operations */
/* ---------- */
DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date );

DEPRECIATION *depreciation_parse(
			char *input );

DEPRECIATION *depreciation_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date );

/* Returns program memory */
/* ---------------------- */
char *depreciation_select(
			void );

LIST *depreciation_list_fetch(
			char *where );

LIST *depreciation_list(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

FILE *depreciation_update_open(
			void );

void depreciation_update(
			double depreciation_amount,
			char *transaction_date_time,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time );

double depreciation_amount_total(
			LIST *depreciation_list );

double depreciation_amount(
			char *depreciation_method,
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced );

double depreciation_sum_of_years_digits(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			char *service_placement_date );

double depreciation_straight_line(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation );

double depreciation_double_declining_balance(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation );

double depreciation_double_declining_balance(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation );

double depreciation_n_declining_balance(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			int n );

double depreciation_units_of_production(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation );

double depreciation_fraction_of_year(
			char *prior_depreciation_date,
			char *depreciation_date );

char *depreciation_sys_string(
			char *where );

LIST *depreciation_system_list(
			char *sys_string );

char *depreciation_primary_where(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_string );

LIST *depreciation_fetch_list(
			char *where );

#endif
