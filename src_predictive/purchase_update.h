/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_update.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "fixed_asset_purchase.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"
#include "supply_purchase.h"
#include "purchase_transaction.h"

typedef struct
{
	FIXED_ASSET_PURCHASE_LIST *fixed_asset_purchase_list;
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list;
	SUPPLY_PURCHASE_LIST *supply_purchase_list;
	char *system_string;
	LIST *string_list;
} PURCHASE_UPDATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PURCHASE_UPDATE *purchase_update_new(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		FIXED_ASSET_PURCHASE_LIST *
			fixed_asset_purchase_list
				/* Sets each update_string_list */,
		INVENTORY_PURCHASE_LIST *
			inventory_purchase_list
				/* Sets each update_string_list */,
		SPECIFIC_INVENTORY_PURCHASE_LIST *
			specific_inventory_purchase_list
				/* Sets each update_string_list */,
		SUPPLY_PURCHASE_LIST *
			supply_purchase_list
				/* Sets each update_string_list */,
		LIST *primary_key_list,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double purchase_return_list_total,
		double purchase_calculate_invoice_amount );

/* Process */
/* ------- */
PURCHASE_UPDATE *purchase_update_calloc(
		void );

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
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double purchase_return_list_total,
		double purchase_calculate_invoice_amount );

/* Usage */
/* ----- */
/* Returns transaction_date_time */
/* ----------------------------- */
char *purchase_update_execute(
		char *application_name,
		PURCHASE_TRANSACTION *purchase_transaction,
		PURCHASE_UPDATE *purchase_update );

/* Usage */
/* ----- */
void purchase_update_table_execute(
		LIST *update_string_list,
		char *update_system_string );

