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
#include "sale_transaction.h"
#include "sale_loss_transaction.h"
#include "sale_fetch.h"

#define SALE_TABLE			"sale"

#define SALE_SELECT			"full_name,"			\
					"sale_date_time,"		\
					"gross_revenue,"		\
					"invoice_amount,"		\
					"payment_total,"		\
					"amount_due,"			\
					"completed_date_time,"		\
					"transaction_date_time"

#define SALE_DATE_TIME_COLUMN		"sale_date_time"
#define SALE_SERVICE_NAME_COLUMN	"service_name"
#define SALE_BEGIN_WORK_COLUMN		"begin_work_date_time"
#define SALE_MEMO			"Customer Sale"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	SALE_FETCH *sale_fetch;
	double inventory_sale_total;
	double inventory_sale_CGS_total;
	double specific_inventory_sale_total;
	double specific_inventory_sale_CGS_total;
	double fixed_service_sale_total;
	double hourly_service_sale_total;
	double gross_revenue;
	double sales_tax;
	double invoice_amount;
	double customer_payment_total;
	double amount_due;
	SALE_TRANSACTION *sale_transaction;
	SALE_LOSS_TRANSACTION *sale_loss_transaction;
	LIST *update_string_list;
	LIST *primary_key_list;
} SALE;

/* Usage */
/* ----- */
SALE *sale_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_uncollectible_date_time );

/* Process */
/* ------- */
SALE *sale_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *sale_primary_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

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
		specific_inventory_sale_total,			\
		fixed_service_sale_total,			\
		hourly_service_sale_total )			\
	( inventory_sale_total +				\
	  specific_inventory_sale_total +			\
	  fixed_service_sale_total +				\
	  hourly_service_sale_total )

/* Usage */
/* ----- */
#define SALE_SALES_TAX(						\
		inventory_sale_total,				\
		specific_inventory_sale_total,			\
		self_tax_state_sales_tax_rate )			\
	( ( inventory_sale_total +				\
	    specific_inventory_sale_total ) *			\
	    self_tax_state_sales_tax_rate )

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
LIST *sale_primary_key_list(
		const char *predictive_fund_column,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *sale_date_time_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
LIST *sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean shipping_charge_boolean,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean,
		boolean sales_tax_boolean,
		boolean payment_list_boolean,
		double shipping_charge,
		double inventory_sale_total,
		double specific_inventory_sale_total,
		double fixed_service_sale_total,
		double hourly_service_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount,
		double customer_payment_total,
		double sale_amount_due );

/* Usage */
/* ----- */

/* Returns heap memory or null (if not set_boolean) */
/* ------------------------------------------------ */
char *sale_update_string(
		const char sql_delimiter,
		char *sale_primary_data_string,
		const char *column_name,
		double money,
		boolean set_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Driver */
/* ------ */

/* Returns inserted sale_transaction->transaction_date_time */
/* -------------------------------------------------------- */
char *sale_update(
		const char *sale_table,
		char *application_name /* for transaction update */,
		LIST *update_string_list,
		LIST *primary_key_list,
		SALE_TRANSACTION *sale_transaction,
		SALE_LOSS_TRANSACTION *sale_loss_transaction );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *sale_update_system_string(
		const char *sale_table,
		LIST *primary_key_list );

#endif
