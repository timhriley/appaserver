/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_fetch.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "folder.h"
#include "fixed_asset_purchase.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *purchase_date_time;
	FOLDER *folder_fetch;
	boolean title_passage_rule_boolean;
	boolean shipped_date_boolean;
	boolean arrived_date_time_boolean;
	boolean inventory_total_boolean;
	boolean specific_inventory_total_boolean;
	boolean prepaid_asset_total_boolean;
	boolean return_total_boolean;
	boolean program_name_boolean;
	boolean property_address_boolean;
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	char *purchase_primary_where;
	double sales_tax;
	double freight_in;
	char *title_passage_rule_string;
	enum predictive_title_passage_rule
		predictive_title_passage_rule;
	char *shipped_date;
	char *arrived_date_time;
	double fixed_asset_total;
	double inventory_total;
	double specific_inventory_total;
	double supply_total;
	double service_total;
	double prepaid_asset_total;
	double purchase_return_total;
	double invoice_amount;
	char *transaction_date_time;
	char *program_name;
	char *property_street_address;
	FIXED_ASSET_PURCHASE_LIST *fixed_asset_purchase_list;
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	SPECIFIC_INVENTORY_PURCHASE_LIST *specific_inventory_purchase_list;
	LIST *supply_purchase_list;
	LIST *service_purchase_list;
	LIST *prepaid_asset_purchase_list;
	LIST *purchase_return_list;
	LIST *primary_key_list;
} PURCHASE_FETCH;

/* Usage */
/* ----- */
PURCHASE_FETCH *purchase_fetch_new(
		const char *purchase_select,
		const char *purchase_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time );

/* Process */
/* ------- */
PURCHASE_FETCH *purchase_fetch_calloc(
		void );

boolean purchase_fetch_title_passage_rule_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_shipped_date_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_arrived_date_time_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_inventory_total_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_specific_inventory_total_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_prepaid_asset_total_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_return_total_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_program_name_boolean(
		LIST *folder_attribute_list );

boolean purchase_fetch_property_address_boolean(
		LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *purchase_fetch_select(
		const char *purchase_select,
		boolean purchase_fetch_title_passage_rule_boolean,
		boolean purchase_fetch_shipped_date_boolean,
		boolean purchase_fetch_arrived_date_time_boolean,
		boolean purchase_fetch_inventory_total_boolean,
		boolean purchase_fetch_specific_inventory_total_boolean,
		boolean purchase_fetch_prepaid_asset_total_boolean,
		boolean purchase_fetch_return_total_boolean,
		boolean purchase_fetch_program_name_boolean,
		boolean purchase_fetch_property_address_boolean );

/* Usage */
/* ----- */
void purchase_fetch_parse(
		PURCHASE_FETCH *purchase_fetch_calloc /* in/out */,
		boolean purchase_fetch_title_passage_rule_boolean,
		boolean purchase_fetch_shipped_date_boolean,
		boolean purchase_fetch_arrived_date_time_boolean,
		boolean purchase_fetch_inventory_total_boolean,
		boolean purchase_fetch_specific_inventory_total_boolean,
		boolean purchase_fetch_prepaid_asset_total_boolean,
		boolean purchase_fetch_return_total_boolean,
		boolean purchase_fetch_program_name_boolean,
		boolean purchase_fetch_property_address_boolean,
		char *string_system_input );

/* Usage */
/* ----- */
LIST *purchase_fetch_primary_key_list(
		const char *predictive_fund_column,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *purchase_date_time_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

