/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_inventory_sale.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_INVENTORY_SALE_H
#define CUSTOMER_INVENTORY_SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define CUSTOMER_INVENTORY_SALE_MEMO		"Customer Sale"

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
	LIST *customer_payment_list;
	double customer_inventory_sale_sales_tax;
	double customer_inventory_sale_extended_price_total;
	double shipping_revenue;
	double customer_inventory_sale_invoice_amount;
	double customer_inventory_sale_payment_total;
	double customer_inventory_sale_amount_due;
	TRANSACTION *customer_inventory_sale_transaction;
} CUSTOMER_INVENTORY_SALE;

/* Operations */
/* ---------- */
CUSTOMER_INVENTORY_SALE *customer_inventory_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time );

CUSTOMER_INVENTORY_SALE *customer_inventory_sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns program memory */
/* ---------------------- */
char *customer_inventory_sale_select(
			void );

char *customer_inventory_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time );

CUSTOMER_INVENTORY_SALE *customer_inventory_sale_parse(
			char *input );

double customer_inventory_sale_payment_total(
			LIST *customer_payment_list );

double customer_inventory_sale_sales_tax(
			LIST *customer_inventory_sale_list,
			double entity_state_sales_tax_rate );

TRANSACTION *customer_inventory_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double invoice_amount,
			double sales_tax_amount,
			char *account_receivable,
			char *account_revenue,
			char *account_sales_tax_payable );


#endif
