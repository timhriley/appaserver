/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictbooks_self.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define PREDICTBOOKS_SELF_SELECT				\
	"full_name,"						\
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

#define PREDICTBOOKS_SELF_TABLE		"predictbooks_self"

enum payroll_pay_period
{
	payroll_pay_period_unknown,
	weekly,
	biweekly,
	semimonthly,
	monthly
};

enum inventory_cost_method
{
	inventory_cost_method_unknown,
	average,
	fifo,
	lifo
};

typedef struct
{
	char *full_name;
	char *contact_key;
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
	enum payroll_pay_period payroll_pay_period;
	enum inventory_cost_method inventory_cost_method;
} PREDICTBOOKS_SELF;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PREDICTBOOKS_SELF *predictbooks_self_fetch(
		const char *predictbooks_self_select,
		const char *predictbooks_self_table,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
PREDICTBOOKS_SELF *predictbooks_self_parse(
		boolean entity_contact_key_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PREDICTBOOKS_SELF *predictbooks_self_new(
		char *full_name );

/* Process */
/* ------- */
PREDICTBOOKS_SELF *predictbooks_self_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *predictbooks_self_select_string(
		const char *predictbooks_self_select,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *predictbooks_self_paypal_cash_account_name(
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double predictbooks_self_state_sales_tax_rate(
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *predictbooks_self_payroll_pay_period_string(
		enum payroll_pay_period payroll_pay_period );

/* Usage */
/* ----- */
enum payroll_pay_period predictbooks_self_payroll_pay_period(
		char *payoll_pay_period_string );

/* Usage */
/* ----- */
enum inventory_cost_method predictbooks_self_inventory_cost_method(
		char *inventory_cost_method_string );
