/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DEPRECIATION_H
#define DEPRECIATION_H

#include "entity_self.h"
#include "entity.h"
#include "transaction.h"

#define DEPRECIATION_TABLE		"depreciation"
#define DEPRECIATION_MEMO		"Depreciation"
#define STRAIGHT_LINE			"straight_line"
#define DOUBLE_DECLINING_BALANCE	"double_declining_balance"
#define N_DECLINING_BALANCE		"n_declining_balance"
#define SUM_OF_YEARS_DIGITS		"sum_of_years_digits"
#define UNITS_OF_PRODUCTION		"units_of_production"
#define NOT_DEPRECIATED			"not_depreciated"

#define DEPRECIATION_INSERT_COLUMNS	"asset_name,"		\
					"serial_label,"		\
					"depreciation_date,"	\
					"units_produced,"	\
					"depreciation_amount,"	\
					"full_name,"		\
					"street_address,"	\
					"transaction_date_time"

#define DEPRECIATION_PRIMARY_KEY	"asset_name,"		\
					"serial_label,"		\
					"depreciation_date"

enum depreciation_method {	straight_line,
				double_declining_balance,
				n_declining_balance,
				sum_of_years_digits,
				units_of_production,
				not_depreciated };

typedef struct
{
	char *asset_name;
	char *serial_label;
	enum depreciation_method depreciation_method;
	char *depreciation_date;
	int units_produced;
	double amount;
	double accumulated_depreciation;
	ENTITY_SELF *entity_self;
	char *transaction_date_time;
} DEPRECIATION;

/* Usage */
/* ----- */
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

/* Process */
/* ------- */
double depreciation_accumulated(
		double prior_accumulated_depreciation,
		double depreciation_amount );

/* Usage */
/* ----- */
DEPRECIATION *depreciation_new(
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

DEPRECIATION *depreciation_calloc(
		void );

/* Usage */
/* ----- */
double depreciation_amount(
		enum depreciation_method,
		char *service_placement_date,
		char *prior_depreciation_date,
		char *depreciation_date,
		double cost_basis,
		int depreciation_units_produced,
		int estimated_residual_value,
		int estimated_useful_life_years,
		int estimated_useful_life_units,
		int declining_balance_n,
		double prior_accumulated_depreciation );

/* Process */
/* ------- */
double depreciation_straight_line(
		char *service_placement_date,
		char *prior_depreciation_date,
		char *depreciation_date,
		double cost_basis,
		int estimated_residual_value,
		int estimated_useful_life_years,
		double prior_accumulated_depreciation );

double depreciation_sum_of_years_digits(
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

/* Usage */
/* ----- */
int depreciation_units_produced(
		char *asset_name,
		char *serial_label,
		enum depreciation_method depreciation_method,
		int units_produced_so_far );

/* Process */
/* ------- */
int depreciation_units_produced_total(
		char *depreciation_table,
		char *asset_name,
		char *serial_label );

int depreciation_units_produced_current(
		int units_produced_so_far,
		int units_produced_total );

/* Usage */
/* ----- */
DEPRECIATION *depreciation_fetch(
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *depreciation_primary_where(
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

/* Returns heap memory */
/* ------------------- */
char *depreciation_system_string(
		char *depreciation_table,
		char *where );

DEPRECIATION *depreciation_parse(
		char *input );

/* Usage */
/* ----- */
void depreciation_list_insert(
		LIST *depreciation_list );

/* Process */
/* ------- */
FILE *depreciation_insert_pipe_open(
		char *depreciation_table,
		char *depreciation_insert_columns );

void depreciation_insert_pipe(
		char *asset_name,
		char *serial_label,
		char *depreciation_date,
		int depreciation_units_produced,
		double depreciation_amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		FILE *insert_pipe_open );

/* Usage */
/* ----- */
void depreciation_update(
		int units_produced,
		double depreciation_amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

/* Process */
/* ------- */
FILE *depreciation_update_pipe_open(
		char *depreciation_table,
		char *depreciation_primary_key );

void depreciation_update_pipe(
		int units_produced,
		double depreciation_amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *fixed_asset_name_escape,
		char *serial_label,
		char *depreciation_date,
		FILE *update_pipe_open );

/* Usage */
/* ----- */
LIST *depreciation_list_fetch(
		char *depreciation_table,
		char *asset_name,
		char *serial_label );

/* Usage */
/* ----- */
LIST *depreciation_system_list(
		char *depreciation_system_string );

/* Process */
/* ------- */
FILE *depreciation_input_pipe_open(
		char *depreciation_system_string );

/* Usage */
/* ----- */
TRANSACTION *depreciation_transaction(
		char *full_name,
		char *street_address,
		char *fixed_asset_purchase_depreciation_date,
		double depreciation_amount,
		char *account_depreciation_expense,
		char *account_accumulated_depreciation );

/* Public */
/* ------ */
enum depreciation_method depreciation_method_evaluate(
		char *depreciation_method_string );

char *depreciation_method_string(
 		enum depreciation_method depreciation_method );

double depreciation_fraction_of_year(
		char *service_placement_date,
		char *prior_depreciation_date,
		char *depreciation_date_string );

char *depreciation_prior_depreciation_date(
		char *depreciation_table,
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

/* Returns heap memory */
/* ------------------- */
char *depreciation_subquery_exists_where(
		char *depreciation_table,
		char *fixed_asset_purchase_table,
		char *depreciation_date );

FILE *depreciation_delete_open(
		char *depreciation_primary_key,
		char *depreciation_table );

#endif
