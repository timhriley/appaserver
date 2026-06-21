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

#define DEPRECIATION_SELECT		"asset_name,"		\
					"serial_label,"		\
					"depreciation_date,"	\
					"units_produced,"	\
					"depreciation_amount,"	\
					"full_name,"		\
					"transaction_date_time"

#define DEPRECIATION_INSERT		"asset_name,"		\
					"serial_label,"		\
					"depreciation_date,"	\
					"units_produced,"	\
					"depreciation_amount,"	\
					"full_name,"		\
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
	enum depreciation_method depreciation_method_resolve;
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
DEPRECIATION *depreciation_parse(
		boolean entity_contact_key_boolean,
		char *input );

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

/* Usage */
/* ----- */
void depreciation_list_insert(
		LIST *depreciation_list );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *depreciation_insert_string(
		const char sql_delimiter,
		char *asset_name,
		char *serial_label,
		char *depreciation_date,
		int depreciation_units_produced,
		double depreciation_amount,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *depreciation_insert_system_string(
		const char *depreciation_select,
		const char *depreciation_table,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
TRANSACTION *depreciation_transaction(
		char *full_name,
		char *contact_key,
		char *fixed_asset_purchase_depreciation_date,
		double depreciation_amount,
		char *account_depreciation_expense,
		char *account_accumulated_depreciation );

/* Usage */
/* ----- */

enum depreciation_method depreciation_method_resolve(
		char *depreciation_method_string );

/* Usage */
/* ----- */

char *depreciation_method_string(
 		enum depreciation_method depreciation_method );

/* Usage */
/* ----- */

double depreciation_fraction_of_year(
		char *service_placement_date,
		char *prior_depreciation_date,
		char *depreciation_date_string );

/* Usage */
/* ----- */

char *depreciation_prior_depreciation_date(
		char *depreciation_table,
		char *asset_name,
		char *serial_label,
		char *depreciation_date );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *depreciation_subquery_exists_where(
		char *depreciation_table,
		char *fixed_asset_purchase_table,
		char *depreciation_date );

#endif
