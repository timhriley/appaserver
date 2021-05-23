/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SALE_H
#define SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"
#include "predictive.h"

/* Constants */
/* --------- */
#define SALE_TABLE_NAME		"sale"
#define SALE_MEMO		"Customer Sale"

#define Sale_amount_due(				\
			invoice_amount,			\
			payment_total )			\
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
	enum title_passage_rule title_passage_rule;
	TRANSACTION *sale_transaction;
} SALE;

/* Operations */
/* ---------- */
SALE *sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns sale_steady_state() */
/* --------------------------- */
SALE *sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns program memory */
/* ---------------------- */
char *sale_select(
			void );

char *sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time );

char *sale_sys_string(	char *where );

SALE *sale_parse(	char *input );

double sale_sales_tax(
			double inventory_sale_total,
			double entity_self_sales_tax_rate );

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

void sale_update(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total,
			double gross_revenue,
			double sales_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time );

FILE *sale_update_open(
			void );

double sale_gross_revenue(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total );

double sale_invoice_amount(
			double gross_revenue,
			double sales_tax,
			double shipping_charge );

SALE *sale_steady_state(
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
