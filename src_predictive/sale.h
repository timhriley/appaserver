/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "transaction.h"
#include "entity.h"
#include "inventory_sale.h"
#include "specific_inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "sale_transaction.h"
#include "sale_loss_transaction.h"
#include "sale_fetch.h"

#define SALE_SELECT			"full_name,"			\
					"sale_date_time,"		\
					"gross_revenue,"		\
					"invoice_amount,"		\
					"payment_total,"		\
					"amount_due,"			\
					"completed_date_time,"		\
					"transaction_date_time"

#define SALE_TABLE			"sale"
#define SALE_DATE_TIME_COLUMN		"sale_date_time"
#define SALE_SERVICE_NAME_COLUMN	"service_name"
#define SALE_SERVICE_DESCRIPTION_COLUMN	"service_description"
#define SALE_INVENTORY_COLUMN		"inventory_name"
#define SALE_SERIAL_KEY_COLUMN		"serial_key"
#define SALE_BEGIN_WORK_COLUMN		"begin_work_date_time"
#define SALE_PAYMENT_DATE_COLUMN	"payment_date_time"
#define SALE_COMPLETED_DATE_COLUMN	"completed_date_time"
#define SALE_MEMO			"Customer Sale"

typedef struct
{
	double shipping_revenue;
	double gross_revenue;
	double cost_of_goods_sold;
	int inventory_markup_percent;
	double sales_tax;
	double invoice_amount;
	double customer_payment_total;
	double amount_due;
} SALE_CALCULATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SALE_CALCULATE *sale_calculate_new(
		boolean shipping_revenue_boolean,
		CUSTOMER *customer,
		INVENTORY_SALE_LIST *inventory_sale_list,
		SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list,
		FIXED_SERVICE_SALE_LIST *fixed_service_sale_list,
		HOURLY_SERVICE_SALE_LIST *hourly_service_sale_list,
		boolean sales_tax_boolean,
		double predictbooks_self_state_sales_tax_rate,
		char *cash_account,
		char *completed_date_time,
		LIST *customer_payment_list );

/* Process */
/* ------- */
SALE_CALCULATE *sale_calculate_calloc(
		void );

/* Usage */
/* ----- */
#define SALE_CALCULATE_GROSS_REVENUE(				\
		inventory_sale_list_extended_total,		\
		specific_inventory_sale_list_extended_total,	\
		fixed_service_sale_list_revenue_total,		\
		hourly_service_sale_list_revenue_total )	\
	( inventory_sale_list_extended_total +			\
	  specific_inventory_sale_list_extended_total +		\
	  fixed_service_sale_list_revenue_total +		\
	  hourly_service_sale_list_revenue_total )

/* Usage */
/* ----- */
#define SALE_CALCULATE_COST_OF_GOODS_SOLD(			\
		inventory_sale_list_extended_total,		\
		specific_inventory_sale_list_extended_total )	\
	( inventory_sale_list_extended_total +			\
	  specific_inventory_sale_list_extended_total )

/* Usage */
/* ----- */
int sale_calculate_inventory_markup_percent(
		double inventory_sale_list_extended_total,
		double specific_inventory_sale_list_extended_total,
		int sale_calculate_cost_of_goods_sold );

/* Process */
/* ------- */
double sale_calculate_inventory_total(
		double inventory_sale_list_extended_total,
		double specific_inventory_sale_list_extended_total );

double sale_calculate_inventory_markup_ratio(
		double sale_calculate_cost_of_goods_sold,
		double sale_calculate_inventory_total );

/* Usage */
/* ----- */
#define SALE_CALCULATE_SALES_TAX(				\
		inventory_extended_total,			\
		specific_inventory_extended_total,		\
		predictbooks_self_state_sales_tax_rate )	\
	( ( inventory_extended_total +				\
	    specific_inventory_extended_total ) *		\
	    predictbooks_self_state_sales_tax_rate )

/* Usage */
/* ----- */
#define SALE_CALCULATE_INVOICE_AMOUNT(				\
		sale_gross_revenue,				\
		sale_calculate_sales_tax,			\
		sale_calculate_shipping_revenue )		\
		( sale_gross_revenue +				\
		  sale_calculate_sales_tax +			\
		  sale_calculate_shipping_revenue )

/* Usage */
/* ----- */
#define SALE_CALCULATE_AMOUNT_DUE(				\
		sale_calculate_invoice_amount,			\
		customer_payment_total )			\
	( sale_calculate_invoice_amount - customer_payment_total )

typedef struct
{
	INVENTORY_SALE_LIST *inventory_sale_list;
	SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list;
	FIXED_SERVICE_SALE_LIST *fixed_service_sale_list;
	HOURLY_SERVICE_SALE_LIST *hourly_service_sale_list;
	char *update_system_string;
	LIST *update_string_list;
} SALE_UPDATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SALE_UPDATE *sale_update_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		SALE_FETCH *sale_fetch,
		double shipping_revenue,
		double gross_revenue,
		double cost_of_goods_sold,
		int inventory_markup_percent,
		double sales_tax,
		double invoice_amount,
		double customer_payment_total,
		double amount_due );

/* Process */
/* ------- */
SALE_UPDATE *sale_update_calloc(
		void );

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
		double sale_fetch_inventory_total,
		double inventory_sale_list_extended_total,
		double sale_fetch_specific_inventory_total,
		double specific_inventory_sale_list_extended_total,
		double sale_fetch_fixed_service_total,
		double fixed_service_sale_list_revenue_total,
		double sale_fetch_hourly_service_total,
		double hourly_service_sale_list_revenue_total,
		double sale_fetch_shipping_revenue,
		double shipping_revenue,
		double sale_fetch_gross_revenue,
		double gross_revenue,
		double sale_fetch_cost_of_goods_otal,
		double cost_of_goods_sold,
		int sale_fetch_inventory_markup_percent,
		int inventory_markup_percent,
		double sale_fetch_sales_tax,
		double sales_tax,
		double sale_fetch_invoice_amount,
		double invoice_amount,
		double sale_fetch_payment_total,
		double customer_payment_total,
		double sale_fetch_amount_due,
		double amount_due );

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

/* Returns heap memory or null (if not set_boolean) */
/* ------------------------------------------------ */
char *sale_update_integer_string(
		const char sql_delimiter,
		char *sale_primary_data_string,
		const char *column_name,
		int integer,
		boolean set_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null (if not set_boolean) */
/* ------------------------------------------------ */
char *sale_update_text_string(
		const char sql_delimiter,
		char *sale_primary_data_string,
		const char *column_name,
		char *text,
		boolean set_boolean );

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	SALE_FETCH *sale_fetch;
	SALE_CALCULATE *sale_calculate;
	SALE_TRANSACTION *sale_transaction;
	SALE_LOSS_TRANSACTION *sale_loss_transaction;
	SALE_UPDATE *sale_update;
} SALE;

/* Usage */
/* ----- */
SALE *sale_trigger_new(
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_uncollectible_date_time,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *state );

/* Process */
/* ------- */
SALE *sale_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *sale_primary_where(
		const char *sale_date_time_column,
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
double sale_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time );

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
char *sale_update_execute(
		char *application_name /* for update_statement_execute */,
		SALE_UPDATE *sale_update,
		SALE_TRANSACTION *sale_transaction,
		SALE_LOSS_TRANSACTION *sale_loss_transaction );

/* Usage */
/* ----- */
void sale_update_purchase_quantity_on_hand_execute(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *sale_update_system_string(
		const char *sale_table,
		LIST *sale_fetch_primary_key_list );

