/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "entity.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPRECIATION_TABLE		"depreciation"
#define DEPRECIATION_MEMO		"Depreciation"
#define STRAIGHT_LINE			"straight_line"
#define DOUBLE_DECLINING_BALANCE	"double_declining_balance"
#define N_DECLINING_BALANCE		"n_declining_balance"
#define SUM_OF_YEARS_DIGITS		"sum_of_years_digits"
#define UNITS_OF_PRODUCTION		"units_of_production"
#define NOT_DEPRECIATED			"not_depreciated"

/* Enumerated types */
/* ---------------- */
enum depreciation_method {	straignt_line,
				double_declining_balance,
				n_declining_balance,
				sum_of_years_digits,
				units_of_production,
				not_depreciated };

/* Structures */
/* ---------- */
typedef struct
{
	char *asset_name;
	char *serial_label;
	char *depreciation_date;
	char *depreciation_prior_period_date;
	double fixed_asset_cost;
	int estimated_residual_value;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int declining_balance_n;
	double prior_accumulated_depreciation;
	int units_produced;
	enum depreciation_method depreciation_method;
	double depreciation_amount;
	char *transaction_date_time;
	TRANSACTION *transaction;
	ENTITY *self_entity;
} DEPRECIATION;

/* Operations */
/* ---------- */
DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_label,
			char *depreciation_date );

DEPRECIATION *depreciation_parse(
			char *input );

DEPRECIATION *depreciation_fetch(
			char *asset_name,
			char *serial_label,
			char *depreciation_date );

char *depreciation_system_string(
			char *where );

LIST *depreciation_system_list(
			char *system_string );

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

char *depreciation_primary_where(
			char *asset_name,
			char *serial_label,
			char *depreciation_date );

LIST *depreciation_list_fetch(
			char *asset_name,
			char *serial_label );

FILE *depreciation_update_pipe_open(
			void );

double depreciation_accumulated_depreciation(
			double prior_accumulated_depreciation,
			double depreciation_amount );

TRANSACTION *depreciation_transaction(
			char *full_name,
			char *street_address,
			char *depreciation_date,
			double depreciation_amount,
			char *account_depreciation_expense,
			char *account_accumulated_depreciation );

LIST *depreciation_transaction_list(
			LIST *depreciation_list );

void depreciation_list_insert(
			LIST *depreciation_list );

void depreciation_insert(
			FILE *insert_pipe,
			char *asset_name,
			char *serial_label,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date,
			double depreciation_amount,
			char *transaction_date_time );

char *depreciation_prior_period_date(
			char *asset_name,
			char *serial_label );

DEPRECIATION *depreciation_seek(
			LIST *depreciation_list,
			char *depreciation_date );

FILE *depreciation_delete_open(
			void );

char *depreciation_max_date(
			char *asset_name,
			char *serial_label );

char *depreciation_method_string(
	 		enum depreciation_method depreciation_method );

enum depreciation_method depreciation_method_resolve(
			char *depreciation_method_string );

#endif
