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

LIST *customer_sale_payment_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

double customer_inventory_sale_total(
			LIST *customer_inventory_sale_list );

double customer_sale_sales_tax(
			LIST *customer_inventory_sale_list,
			double entity_state_sales_tax_rate );

TRANSACTION *customer_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double invoice_amount,
			double sales_tax_amount,
			char *account_receivable,
			char *account_revenue,
			char *account_sales_tax_payable );


char *customer_sale_completed_date_time(
			char *full_name,
			char *street_address,
			char *sale_date_time );

double customer_sale_fetch_sales_tax(
			char *full_name,
			char *street_address,
			char *sale_date_time );

double customer_sale_payment_total(
			LIST *customer_payment_list );

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

char *customer_sale_update_sys_string(
			void );

double customer_sale_extended_price_total(
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list );

LIST *customer_fixed_service_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

#endif
