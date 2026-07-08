/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_fetch.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SALE_FETCH_H
#define SALE_FETCH_H

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "customer.h"
#include "entity_self.h"
#include "folder.h"

typedef struct
{
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	FOLDER *folder_fetch;
	boolean feeder_account_boolean;
	boolean shipping_charge_boolean;
	boolean instructions_boolean;
	boolean inventory_sale_boolean;
	boolean specific_inventory_sale_boolean;
	boolean fixed_service_sale_boolean;
	boolean hourly_service_sale_boolean;
	boolean sales_tax_boolean;
	boolean title_passage_rule_boolean;
	boolean shipped_date_time_boolean;
	boolean arrived_date_boolean;
	boolean uncollectible_date_time_boolean;
	char *feeder_account;
	double shipping_charge;
	char *instructions;
	double inventory_sale_total;
	double specific_inventory_sale_total;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	double gross_revenue;
	double sales_tax;
	double invoice_amount;
	char *title_passage_rule_string;
	enum predictive_title_passage_rule predictive_title_passage_rule;
	double payment_total;
	double amount_due;
	char *completed_date_time;
	char *shipped_date_time;
	char *uncollectible_date_time;
	char *arrived_date;
	char *transaction_date_time;
	LIST *inventory_sale_list;
	LIST *specific_inventory_sale_list;
	LIST *fixed_service_sale_list;
	LIST *hourly_service_sale_list;
	LIST *customer_payment_list;
	CUSTOMER *customer;
	ENTITY_SELF *entity_self;
	double self_tax_state_sales_tax_rate;
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
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Process */
/* ------- */
SALE_FETCH *sale_fetch_calloc(
		void );

boolean sale_fetch_feeder_account_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_shipping_charge_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_instructions_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_inventory_sale_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_specific_inventory_sale_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_fixed_service_sale_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_hourly_service_sale_boolean(
		LIST *folder_attribute_list );

boolean sale_fetch_sales_tax_boolean(
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
		boolean sale_fetch_feeder_account_boolean,
		boolean sale_fetch_shipping_charge_boolean,
		boolean sale_fetch_instructions_boolean,
		boolean sale_fetch_inventory_sale_boolean,
		boolean sale_fetch_specific_inventory_sale_boolean,
		boolean sale_fetch_fixed_service_sale_boolean,
		boolean sale_fetch_hourly_service_sale_boolean,
		boolean sale_fetch_sales_tax_boolean,
		boolean sale_fetch_title_passage_rule_boolean,
		boolean sale_fetch_shipped_date_time_boolean,
		boolean sale_fetch_arrived_date_boolean,
		boolean sale_fetch_uncollectible_date_time_boolean );

/* Usage */
/* ----- */
void sale_fetch_parse(
		SALE_FETCH *sale_fetch_calloc /* in/out */,
		boolean sale_fetch_feeder_account_boolean,
		boolean sale_fetch_shipping_charge_boolean,
		boolean sale_fetch_instructions_boolean,
		boolean sale_fetch_inventory_sale_boolean,
		boolean sale_fetch_specific_inventory_sale_boolean,
		boolean sale_fetch_fixed_service_sale_boolean,
		boolean sale_fetch_hourly_service_sale_boolean,
		boolean sale_fetch_sales_tax_boolean,
		boolean sale_fetch_title_passage_rule_boolean,
		boolean sale_fetch_shipped_date_time_boolean,
		boolean sale_fetch_arrived_date_boolean,
		boolean sale_fetch_uncollectible_date_time_boolean,
		char *input );

#endif

