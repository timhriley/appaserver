/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity_self.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_SELF_H
#define ENTITY_SELF_H

#include "boolean.h"
#include "entity.h"

typedef struct
{
	/* ENTITY * must stay on top. */
	/* -------------------------- */
	ENTITY *entity;

	char *payroll_beginning_day;
	double social_security_combined_tax_rate;
	int social_security_payroll_ceiling;
	double medicare_combined_tax_rate;
	double medicare_additional_withholding_rate;
	int medicare_additional_gross_pay_floor;
	int federal_unemployment_wage_base;
	double federal_nonresident_withholding_income_premium;
	double federal_unemployment_tax_standard_rate;
	double federal_unemployment_threshold_rate;
	double federal_unemployment_tax_minimum_rate;
	double federal_withholding_allowance_period_value;
	int state_unemployment_wage_base;
	double state_unemployment_tax_rate;
	double state_withholding_allowance_period_value;
	double state_itemized_allowance_period_value;
	double entity_self_sales_tax_rate;
	char *entity_self_paypal_cash_account_name;
/*
	enum inventory_cost_method inventory_cost_method;
	enum payroll_pay_period payroll_pay_period;
	LIST *purchase_order_list;
	LIST *customer_sale_list;
	INVENTORY *purchase_inventory;
	INVENTORY *sale_inventory;
	HASH_TABLE *inventory_purchase_hash_table;
	LIST *inventory_purchase_name_list;
	HASH_TABLE *inventory_sale_hash_table;
	LIST *inventory_sale_name_list;
	HASH_TABLE *transaction_hash_table;
	HASH_TABLE *journal_ledger_hash_table;
	LIST *inventory_list;
	LIST *employee_list;
*/
} ENTITY_SELF;

/* Usage */
/* ----- */
ENTITY_SELF *entity_self_fetch(
			void );

/* Process */
/* ------- */
ENTITY_SELF *entity_self_new(		char *full_name,
					char *street_address );


/* Public */
/* ------ */
LIST *entity_get_inventory_list(
					char *application_name );

LIST *entity_get_purchase_order_list(
					char *application_name,
					char *inventory_name );

LIST *entity_get_customer_sale_list(
					char *application_name,
					char *inventory_name );

ENTITY *entity_calloc(			void );

ENTITY *entity_new(			char *full_name,
					char *street_address );

boolean entity_load(			char **city,
					char **state_code,
					char **zip_code,
					char **phone_number,
					char **email_address,
					char *application_name,
					char *full_name,
					char *street_address );

ENTITY_SELF *entity_self_load(
			void );

LIST *entity_get_inventory_purchase_order_list(
					char *application_name,
					char *inventory_name );

LIST *entity_get_inventory_customer_sale_list(
					char *application_name,
					char *inventory_name );

enum inventory_cost_method entity_get_inventory_cost_method(
					char *inventory_cost_method_string );

void entity_propagate_purchase_order_ledger_accounts(
				char *application_name,
				char *fund_name,
				char *purchase_order_transaction_date_time );

/*
ENTITY_SELF *entity_self_inventory_load(
				char *application_name,
				char *inventory_name );
*/

enum title_passage_rule
	entity_get_title_passage_rule(
				char *title_passage_rule_string );

char *entity_get_title_passage_rule_string(
				enum title_passage_rule title_passage_rule );

ENTITY *entity_get_sales_tax_payable_entity(
				char *application_name );

char *entity_get_payroll_pay_period_string(
				enum payroll_pay_period );

enum payroll_pay_period entity_get_payroll_pay_period(
				char *payroll_pay_period_string );

LIST *print_checks_get_current_liability_account_list(
				char *application_name );

ENTITY *entity_get_or_set(	LIST *entity_list,
				char *full_name,
				char *street_address,
				boolean with_strdup );

char *entity_self_select(	void );

char *entity_list_display(
			LIST *entity_list );

boolean entity_location_fetch(
			char **city,
			char **state_code,
			char **zip_code,
			char *application_name,
			char *full_name,
			char *street_address );

double entity_self_state_sales_tax_rate(
			void );

char *entity_self_paypal_cash_account_name(
			void );

#endif
