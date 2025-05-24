/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/self_tax.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SELF_TAX_H
#define SELF_TAX_H

#include "list.h"
#include "boolean.h"

enum self_tax_payroll_pay_period{
	pay_period_unknown,
	pay_period_weekly,
	pay_period_biweekly,
	pay_period_semimonthly,
	pay_period_monthly };

#define SELF_TAX_TABLE		"self_tax"

#define SELF_TAX_SELECT						\
	"full_name,"						\
	"street_address,"					\
	"inventory_cost_method,"				\
	"payroll_pay_period,"					\
	"payroll_begin_day,"					\
	"social_security_combined_tax_rate,"			\
	"social_security_payroll_ceiling,"			\
	"medicare_combined_tax_rate,"				\
	"medicare_additional_withholding_rate,"			\
	"medicare_additional_gross_pay_floor,"			\
	"federal_withholding_allowance_period_value,"		\
	"federal_nonresident_withholding_income_premium,"	\
	"state_withholding_allowance_period_value,"		\
	"state_itemized_allowance_period_value,"		\
	"federal_unemployment_wage_base,"			\
	"federal_unemployment_tax_standard_rate,"		\
	"federal_unemployment_threshold_rate,"			\
	"federal_unemployment_tax_minimum_rate,"		\
	"state_unemployment_wage_base,"				\
	"state_unemployment_tax_rate,"				\
	"state_sales_tax_rate,"					\
	"energy_charge_kilowatts_per_hour,"			\
	"paypall_cash_account_name"

typedef struct
{
	char *full_name;
	char *street_address;
	char *inventory_cost_method;
	char *payroll_pay_period_string;
	char *payroll_begin_day;
	double social_security_combined_tax_rate;
	int social_security_payroll_ceiling;
	double medicare_combined_tax_rate;
	double medicare_additional_withholding_rate;
	int medicare_additional_gross_pay_floor;
	double federal_withholding_allowance_period_value;
	double federal_nonresident_withholding_income_premium;
	double state_withholding_allowance_period_value;
	double state_itemized_allowance_period_value;
	int federal_unemployment_wage_base;
	double federal_unemployment_tax_standard_rate;
	double federal_unemployment_threshold_rate;
	double federal_unemployment_tax_minimum_rate;
	int state_unemployment_wage_base;
	double state_unemployment_tax_rate;
	double state_sales_tax_rate;
	double energy_charge_kilowatts_per_hour;
	char *paypall_cash_account_name;
	enum self_tax_payroll_pay_period payroll_pay_period;
} SELF_TAX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SELF_TAX *self_tax_fetch(
		const char *self_tax_table,
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
enum self_tax_payroll_pay_period self_tax_resolve_payroll_pay_period(
		char *payroll_pay_period_string );

/* Usage */
/* ----- */
SELF_TAX *self_tax_parse(
		char *full_name,
		char *street_address,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SELF_TAX *self_tax_new(
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
SELF_TAX *self_tax_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *self_tax_paypal_cash_account_name(
		char *full_name,
		char *street_address );

/* Usage */
/* ----- */
double self_tax_state_sales_tax_rate(
		char *full_name,
		char *street_address );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *self_tax_resolve_payroll_pay_period_string(
		enum self_tax_payroll_pay_period
			self_tax_payroll_pay_period );

#endif

