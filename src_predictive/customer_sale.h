/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_sale.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_SALE_H
#define CUSTOMER_SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define CUSTOMER_SALE_TABLE		"customer_sale"
#define CUSTOMER_SALE_MEMO		"Customer Sale"

#define Customer_sale_amount_due(					\
			invoice_amount,					\
			payment_total )					\
	( invoice_amount - payment_total )


/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	LIST *inventory_sale_list;
	LIST *fixed_service_sale_list;
	LIST *hourly_service_sale_list;
	LIST *customer_payment_list;
	double inventory_sale_total;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	double customer_sale_gross_revenue;
	double entity_self_sales_tax_rate;
	double customer_sale_sales_tax;
	double customer_sale_invoice_amount;
	double customer_payment_total;
	double customer_sale_amount_due;
	double shipping_revenue;
	char *completed_date_time;
	char *transaction_date_time_database;
	double sales_tax_database;
	enum title_passage_rule title_passage_rule;
	TRANSACTION *customer_sale_transaction;
} CUSTOMER_SALE;

/* Operations */
/* ---------- */
CUSTOMER_SALE *customer_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time );

CUSTOMER_SALE *customer_sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns program memory */
/* ---------------------- */
char *customer_sale_select(
			void );

char *customer_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time );

CUSTOMER_SALE *customer_sale_parse(
			char *input );

double customer_sale_sales_tax(
			double inventory_sale_total,
			double entity_self_sales_tax_rate );

TRANSACTION *customer_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double invoice_amount,
			double gross_revenue,
			double sales_tax,
			double shipping_charge,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable );

void customer_sale_update(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total,
			double gross_revenue,
			double sale_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time );

FILE *customer_sale_update_open(
			void );

double customer_sale_gross_revenue(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total );

double customer_sale_invoice_amount(
			double gross_revenue,
			double sales_tax,
			double shipping_charge );

CUSTOMER_SALE *customer_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list,
			LIST *hourly_service_sale_list,
			LIST *customer_payment_list,
			double shipping_charge,
			double entity_self_sales_tax_rate,
			double sales_tax_database,
			char *completed_date_time,
			char *transaction_date_time_database,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable );

#endif
