/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver entity ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_H
#define ENTITY_H

#include "boolean.h"
#include "transaction.h"
#include "hash_table.h"

/* Enumerated types */
/* ---------------- */
enum title_passage_rule{	title_passage_rule_null,
				FOB_shipping,
				FOB_destination };

enum inventory_cost_method{	inventory_not_set,
				inventory_fifo,
				inventory_average,
				inventory_lifo };

enum payroll_pay_period{	pay_period_not_set,
				pay_period_weekly,
				pay_period_biweekly,
				pay_period_semimonthly,
				pay_period_monthly };

/* Constants */
/* --------- */
/* #define REFRESH_COMPLETELY	1 */

#define TITLE_PASSAGE_RULE_SHIPPED_DATE "FOB_shipping"
#define TITLE_PASSAGE_RULE_ARRIVED_DATE "FOB_destination"
#define TITLE_PASSAGE_RULE_NULL		""

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *phone_number;
	char *email_address;
	LIST *depreciable_fixed_asset_purchase_list;
	double depreciation_amount;
	TRANSACTION *depreciation_transaction;
	LIST *liability_account_list;
	double sum_balance;
	double payment_amount;
	double loss_amount;
	int check_number;
	LIST *purchase_order_list;
} ENTITY;

/* Operations */
/* ---------- */
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

ENTITY *entity_seek(		LIST *entity_list,
				char *full_name,
				char *street_address );

char *entity_list_display(	LIST *entity_list );

boolean entity_list_exists(	LIST *entity_list,
				char *full_name,
				char *street_address );

boolean entity_location_fetch(	char **city,
				char **state_code,
				char **zip_code,
				char *application_name,
				char *full_name,
				char *street_address );

ENTITY *entity_fetch(		char *application_name,
				char *full_name,
				char *street_address );

#endif
