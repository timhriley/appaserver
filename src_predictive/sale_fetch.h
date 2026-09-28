/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_fetch.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "customer.h"
#include "inventory_sale.h"
#include "specific_inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "predictbooks_self.h"
#include "folder.h"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	FOLDER *folder_fetch;
	boolean cash_account_boolean;
	boolean shipping_revenue_boolean;
	boolean instructions_boolean;
	boolean inventory_total_boolean;
	boolean specific_inventory_total_boolean;
	boolean fixed_service_total_boolean;
	boolean hourly_service_total_boolean;
	boolean cost_of_goods_sold_total_boolean;
	boolean inventory_markup_percent_boolean;
	boolean sales_tax_boolean;
	boolean payment_list_boolean;
	boolean title_passage_rule_boolean;
	boolean shipped_date_time_boolean;
	boolean arrived_date_boolean;
	boolean uncollectible_date_time_boolean;
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	char *cash_account;
	double shipping_revenue;
	char *instructions;
	double inventory_sale_total;
	double specific_inventory_sale_total;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	double cost_of_goods_sold_total;
	int inventory_markup_percent;
	double gross_revenue;
	double sales_tax;
	double invoice_amount;
	enum predictive_title_passage_rule predictive_title_passage_rule;
	double payment_total;
	double amount_due;
	char *completed_date_time;
	char *shipped_date_time;
	char *arrived_date;
	char *uncollectible_date_time;
	char *transaction_date_time;
	CUSTOMER *customer;
	INVENTORY_SALE_LIST *inventory_sale_list;
	SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list;
	FIXED_SERVICE_SALE_LIST *fixed_service_sale_list;
	HOURLY_SERVICE_SALE_LIST *hourly_service_sale_list;
	LIST *customer_payment_list;
	PREDICTBOOKS_SELF *predictbooks_self;
	double predictbooks_self_state_sales_tax_rate;
	LIST *primary_key_list;
} SALE_FETCH;

/* Usage */
/* ----- */
SALE_FETCH *sale_fetch_new(
		const char *sale_select,
		const char *sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean customer_entity_boolean );

/* Process */
/* ------- */
SALE_FETCH *sale_fetch_calloc(
		void );

boolean sale_fetch_cash_account_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_shipping_revenue_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_instructions_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_inventory_total_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_specific_inventory_total_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_fixed_service_total_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_hourly_service_total_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_cost_of_goods_sold_total_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_inventory_markup_percent_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_sales_tax_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_payment_list_boolean(
		const char *customer_payment_table,
		const char *customer_payment_date_column,
		LIST *folder_attribute_list );

boolean sale_fetch_title_passage_rule_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_shipped_date_time_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_arrived_date_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_uncollectible_date_time_boolean(
		LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *sale_fetch_select(
		const char *sale_select,
		boolean sale_fetch_cash_account_boolean,
		boolean sale_fetch_shipping_revenue_boolean,
		boolean sale_fetch_instructions_boolean,
		boolean sale_fetch_inventory_total_boolean,
		boolean sale_fetch_specific_inventory_total_boolean,
		boolean sale_fetch_fixed_service_total_boolean,
		boolean sale_fetch_hourly_service_total_boolean,
		boolean sale_fetch_cost_of_goods_sold_total_boolean,
		boolean sale_fetch_inventory_markup_percent_boolean,
		boolean sale_fetch_sales_tax_boolean,
		boolean sale_fetch_title_passage_rule_boolean,
		boolean sale_fetch_shipped_date_time_boolean,
		boolean sale_fetch_arrived_date_boolean,
		boolean sale_fetch_uncollectible_date_time_boolean );

/* Usage */
/* ----- */
void sale_fetch_parse(
		SALE_FETCH *sale_fetch_calloc /* in/out */,
		boolean sale_fetch_cash_account_boolean,
		boolean sale_fetch_shipping_revenue_boolean,
		boolean sale_fetch_instructions_boolean,
		boolean sale_fetch_inventory_total_boolean,
		boolean sale_fetch_specific_inventory_total_boolean,
		boolean sale_fetch_fixed_service_total_boolean,
		boolean sale_fetch_hourly_service_total_boolean,
		boolean sale_fetch_cost_of_goods_sold_total_boolean,
		boolean sale_fetch_inventory_markup_percent_boolean,
		boolean sale_fetch_sales_tax_boolean,
		boolean sale_fetch_title_passage_rule_boolean,
		boolean sale_fetch_shipped_date_time_boolean,
		boolean sale_fetch_arrived_date_boolean,
		boolean sale_fetch_uncollectible_date_time_boolean,
		char *input );

/* Usage */
/* ----- */
LIST *sale_fetch_primary_key_list(
		const char *predictive_fund_column,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *sale_date_time_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

