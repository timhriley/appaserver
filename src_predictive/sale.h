/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SALE_H
#define SALE_H

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "transaction.h"
#include "entity.h"

#define SALE_TABLE		"sale"
#define SALE_MEMO		"Customer Sale"

#define SALE_GROSS_REVENUE(					\
			inventory_sale_total,			\
			fixed_service_sale_total,		\
			hourly_service_sale_total )		\
	( inventory_sale_total +				\
	  fixed_service_sale_total +				\
	  hourly_service_sale_total )

#define SALE_SALES_TAX(						\
			inventory_sale_total,			\
			self_tax_sales_tax_rate )		\
	( inventory_sale_total * self_tax_sales_tax_rate )

#define SALE_INVOICE_AMOUNT(					\
			sale_gross_revenue,			\
			sale_sales_tax,				\
			shipping_charge )			\
	( sale_gross_revenue + sale_sales_tax + shipping_charge )

#define SALE_AMOUNT_DUE(					\
			sale_invoice_amount,			\
			customer_payment_total )		\
	( sale_invoice_amount - customer_payment_total )

typedef struct
{
	TRANSACTION *transaction;
} SALE_TRANSACTION;

TRANSACTION *sale_transaction(
		char *full_name,
		char *street_address,
		char *completed_date_time,
		double invoice_amount,
		double gross_revenue,
		double sales_tax,
		double shipping_charge,
		char *account_receivable,
		char *account_revenue,
		char *account_shipping_revenue,
		char *account_sales_tax_payable );

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
	double sale_gross_revenue;
	double entity_self_sales_tax_rate;
	double sales_tax;
	double sale_invoice_amount;
	double customer_payment_total;
	double sale_amount_due;
	double shipping_charge;
	char *completed_date_time;
	char *transaction_date_time_database;
	char *account_receivable;
	char *account_revenue;
	char *account_shipping_revenue;
	char *account_sales_tax_payable;
	double sales_tax_database;
	enum predictive_title_passage_rule predictive_title_passage_rule;
	TRANSACTION *sale_transaction;
} SALE;

SALE *sale_fetch(
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
SALE *sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *string_pipe_fetch );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SALE *sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
void sale_update(
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		double hourly_service_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount,
		double customer_payment_total,
		double sale_amount_due );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *sale_update_system_string(
		const char *sale_table );

#endif
