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
#include "sale_fetch.h"

#define SALE_TABLE		"sale"

#define SALE_SELECT		"full_name,"			\
				"street_address,"		\
				"sale_date_time,"		\
				"invoice_amount,"		\
				"payment_total,"		\
				"amount_due,"			\
				"completed_date_time,"		\
				"uncollectible_date_time,"	\
				"transaction_date_time"

#define SALE_MEMO		"Customer Sale"

typedef struct
{
	TRANSACTION *transaction;
} SALE_TRANSACTION;

/* Usage */
/* ----- */
SALE_TRANSACTION *sale_transaction_new(
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

/* Process */
/* ------- */
SALE_TRANSACTION *sale_transaction_calloc(
		void );

typedef struct
{
	TRANSACTION *transaction;
} SALE_LOSS_TRANSACTION;

/* Usage */
/* ----- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_new(
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

/* Process */
/* ------- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_calloc(
		void );

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	SALE_FETCH *sale_fetch;
	double inventory_sale_total;
	double specific_inventory_sale_total;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	double gross_revenue;
	double sales_tax;
	double invoice_amount;
	double customer_payment_total;
	double amount_due;
	SALE_TRANSACTION *sale_transaction;
	SALE_LOSS_TRANSACTION *sale_loss_transaction;
} SALE;

/* Safely returns */
/* -------------- */
SALE *sale_trigger_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_uncollectible_date_time,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean );

/* Process */
/* ------- */
SALE *sale_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
#define SALE_EXTENDED_PRICE(					\
		retail_price,					\
		quantity,					\
		discount_amount )				\
	( ( retail_price * (double)quantity ) - discount_amount )

/* Usage */
/* ----- */
#define SALE_GROSS_REVENUE(					\
		inventory_sale_total,				\
		fixed_service_sale_total,			\
		hourly_service_sale_total )			\
	( inventory_sale_total +				\
	  fixed_service_sale_total +				\
	  hourly_service_sale_total )

/* Usage */
/* ----- */
#define SALE_SALES_TAX(						\
		inventory_sale_total,				\
		self_tax_sales_tax_rate )			\
	( inventory_sale_total * self_tax_sales_tax_rate )

/* Usage */
/* ----- */
#define SALE_INVOICE_AMOUNT(					\
		sale_gross_revenue,				\
		sale_sales_tax,					\
		shipping_charge )				\
	( sale_gross_revenue + sale_sales_tax + shipping_charge )

/* Usage */
/* ----- */
#define SALE_AMOUNT_DUE(					\
		sale_invoice_amount,				\
		customer_payment_total )			\
	( sale_invoice_amount - customer_payment_total )

/* Usage */
/* ----- */
double sale_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time );

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
