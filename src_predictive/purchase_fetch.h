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
	boolean prepaid_asset_total_boolean;
	boolean program_boolean;
	boolean property_address_boolean;
	char *select;
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
	double prepaid_asset_total;
	double invoice_amount;
	char *transaction_date_time;
	char *program_name;
	char *property_street_address;
	LIST *fixed_asset_purchase_list;
	LIST *inventory_purchase_list;
	LIST *specific_inventory_purchase_list;
	LIST *supply_purchase_list;
	LIST *prepaid_asset_purchase_list;
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

