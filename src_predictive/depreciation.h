/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "entity_self.h"
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
enum depreciation_method {	straight_line,
				double_declining_balance,
				n_declining_balance,
				sum_of_years_digits,
				units_of_production,
				not_depreciated };

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	char *asset_name;
	char *serial_label;
	enum depreciation_method depreciation_method;
	char *depreciation_date;
	char *depreciation_prior_period_date;
	double fixed_asset_cost;
	int estimated_residual_value;
	int estimated_useful_life_years;
	int estimated_useful_life_units;
	int declining_balance_n;
	double prior_accumulated_depreciation;

	/* Process */
	/* ------- */
	double depreciation_amount;
	double depreciation_accumulated_depreciation;
	int units_produced_current;
	char *transaction_date_time;
	TRANSACTION *depreciation_transaction;
	ENTITY_SELF *entity_self;
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
			enum depreciation_method,
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int units_produced_current,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			double prior_accumulated_depreciation );

double depreciation_sum_of_years_digits(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation );

double depreciation_straight_line(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation );

double depreciation_double_declining_balance(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation );

double depreciation_n_declining_balance(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int declining_balance_n,
			double prior_accumulated_depreciation );

double depreciation_units_of_production(
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double prior_accumulated_depreciation );

double depreciation_fraction_of_year(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date_string );

/* Returns static memory */
/* --------------------- */
char *depreciation_primary_where(
			char *asset_name,
			char *serial_label,
			char *depreciation_date );

LIST *depreciation_list_fetch(
			char *asset_name,
			char *serial_label );

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
			char *depreciation_date,
			int units_produced_current,
			double depreciation_amount,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

char *depreciation_prior_depreciation_date(
			char *asset_name,
			char *serial_label );

DEPRECIATION *depreciation_seek(
			LIST *depreciation_list,
			char *depreciation_date );

FILE *depreciation_delete_open(
			void );

char *depreciation_method_string(
	 		enum depreciation_method depreciation_method );

enum depreciation_method depreciation_method_evaluate(
			char *depreciation_method_string );

DEPRECIATION *depreciation_evaluate(
			char *asset_name,
			char *serial_label,
			enum depreciation_method depreciation_method,
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int units_produced_so_far,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			double prior_accumulated_depreciation );

int depreciation_units_produced_total(
			char *asset_name,
			char *serial_label );

int depreciation_units_produced_current(
			int units_produced_so_far,
			int units_produced_total );

void depreciation_list_negate_depreciation_amount(
			LIST *depreciation_list );

#endif
