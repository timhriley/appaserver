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

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	double gross_revenue;
	double invoice_amount;
	double payment_total;
	double amount_due;
	char *completed_date_time;
	char *uncollectible_date_time;
	char *transaction_date_time;
	double inventory_sale_total;
	double specific_inventory_sale_total;
	double sales_tax;
	char *title_passage_rule_string;
	char *shipped_date_time;
	char *arrived_date;
	double shipping_charge;
	char *instructions;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	enum predictive_title_passage_rule predictive_title_passage_rule;
	LIST *inventory_sale_list;
	LIST *specific_inventory_sale_list;
	CUSTOMER *customer;
	ENTITY_SELF *entity_self;
	double self_tax_state_sales_tax_rate;
	LIST *fixed_service_sale_list;
	LIST *hourly_service_sale_list;
	LIST *customer_payment_list;
} SALE_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SALE_FETCH *sale_fetch_new(
		const char *sale_select,
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean hourly_service_sale_boolean,
		boolean fixed_service_sale_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *sale_fetch_select(
		const char *sale_select,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean hourly_service_sale_boolean,
		boolean fixed_service_sale_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SALE_FETCH *sale_fetch_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean hourly_service_sale_boolean,
		boolean fixed_service_sale_boolean,
		char *string_pipe_fetch );

/* Process */
/* ------- */
SALE_FETCH *sale_fetch_calloc(
		void );

#endif

