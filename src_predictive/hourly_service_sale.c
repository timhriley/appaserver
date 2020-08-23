/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_SERVICE_SALE_H
#define FIXED_SERVICE_SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define FIXED_SERVICE_SALE_TABLE		"fixed_service_sale"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	char *service_name;
	double fixed_price;
	int estimate_work_hours;
	char *fixed_service_sale_revenue_account_name;
} FIXED_SERVICE_SALE;

/* Operations */
/* ---------- */
FIXED_SERVICE_SALE *fixed_service_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time );

FIXED_SERVICE_SALE *fixed_service_sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns program memory */
/* ---------------------- */
char *fixed_service_sale_select(
			void );

char *fixed_service_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time );

FIXED_SERVICE_SALE *fixed_service_sale_parse(
			char *input );

double fixed_service_sale_payment_total(
			LIST *fixed_service_sale_payment_list );

LIST *fixed_service_sale_payment_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

double customer_inventory_sale_total(
			LIST *customer_inventory_sale_list );

double fixed_service_sale_sales_tax(
			LIST *customer_inventory_sale_list,
			double entity_state_sales_tax_rate );

TRANSACTION *fixed_service_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double invoice_amount,
			double sales_tax_amount,
			double shipping_charge,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable );

void fixed_service_sale_update(
			double extended_price_total,
			double sales_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time );

FILE *fixed_service_sale_update_open(
			void );

double fixed_service_sale_extended_price_total(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total );

double fixed_service_sale_invoice_amount(
			double extended_price_total,
			double sales_tax,
			double shipping_charge );

#endif
