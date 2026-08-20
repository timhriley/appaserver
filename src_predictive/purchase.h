/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "purchase_transaction.h"
#include "purchase_calculate.h"
#include "purchase_fetch.h"

#define PURCHASE_TABLE			"purchase"

#define PURCHASE_SELECT			"full_name,"			\
					"purchasee_date_time,"		\
					"sales_tax,"			\
					"freight_in,"			\
					"fixed_asset_total,"		\
					"supply_total,"			\
					"service_total,"		\
					"invoice_amount,"		\
					"transaction_date_time"

#define PURCHASE_DATE_TIME_COLUMN	"purchase_date_time"
#define PURCHASE_ASSET_COLUMN		"asset_name"
#define PURCHASE_MEMO			"Purchase Order"

typedef struct
{
	PURCHASE_FETCH *purchase_fetch;
	double fixed_asset_purchase_list_total;
	double inventory_purchase_list_total;
	double specific_inventory_purchase_list_total;
	double supply_purchase_list_total;
	double service_purchase_list_total;
	double prepaid_asset_purchase_list_total;
	double return_list_total;
	double total;
	double invoice_amount;
	PURCHASE_CALCULATE *purchase_calculate;
	PURCHASE_TRANSACTION *purchase_transaction;
	char *update_system_string;
	LIST *update_string_list;
} PURCHASE;

/* Usage */
/* ----- */
PURCHASE *purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key );

/* Process */
/* ------- */
PURCHASE *purchase_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *purchase_primary_where(
		const char *purchase_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *purchase_update_system_string(
		const char *purchase_table,
		LIST *purchase_fetch_primary_key_list );

/* Usage */
/* ----- */
LIST *purchase_update_string_list(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double fixed_asset_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double purchase_return_list_total,
		double purchase_calculate_invoice_amount );
