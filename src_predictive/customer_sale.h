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
	char *completed_date_time;
	enum title_passage_rule title_passage_rule;
	LIST *customer_sale_payment_list;
	double customer_sale_sales_tax;
	double customer_sale_extended_price_total;
	double shipping_revenue;
	double customer_sale_invoice_amount;
	double customer_sale_payment_total;
	double customer_sale_amount_due;
	TRANSACTION *customer_sale_transaction;
	char *database_transaction_date_time;
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

double customer_sale_payment_total(
			LIST *customer_sale_payment_list );

double customer_sale_sales_tax(
			LIST *inventory_sale_list,
			double entity_state_sales_tax_rate );

TRANSACTION *customer_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double invoice_amount,
			double sales_tax_amount,
			double entity_self_sales_tax_rate,
			double shipping_charge,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable );

void customer_sale_update(
			double extended_price_total,
			double sales_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time );

FILE *customer_sale_update_open(
			void );

double customer_sale_extended_price_total(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total );

double customer_sale_invoice_amount(
			double extended_price_total,
			double sales_tax,
			double shipping_charge );

CUSTOMER_SALE *customer_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *completed_date_time,
			double shipping_charge,
			double sales_tax_rate,
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list,
			LIST *hourly_service_sale_list,
			LIST *customer_payment_list,
			char *account_receivable,
			char *account_shipping_revenue,
			char *account_sales_tax_payable );

#endif
