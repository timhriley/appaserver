/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "purchase_fetch.h"

#define PURCHASE_TABLE			"purchase"

#define PURCHASE_SELECT			"full_name,"			\
					"purchasee_date_time,"		\
					"sales_tax,"			\
					"freight_in,"			\
					"fixed_asset_total,"		\
					"supply_total,"			\
					"invoice_amount,"		\
					"transaction_date_time"

#define PURCHASE_DATE_TIME_COLUMN	"purchase_date_time"
#define PURCHASE_MEMO			"Purchase Order"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *purchase_date_time;
	PURCHASE_FETCH *purchase_fetch;
} PURCHASE;

PURCHASE *purchase_trigger_new(
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key );

/* --------------------------------- */
/* Allocates purchase->vendor_entity */
/* --------------------------------- */
PURCHASE *purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double purchase_equipment_total(
			LIST *equipment_purchase_list );

void purchase_update(	double fixed_asset_purchase_total,
			double purchase_invoice_amount,
			double vendor_payment_total,
			double purchase_amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

PURCHASE *purchase_calloc(
			void );

PURCHASE *purchase_seek(
			LIST *purchase_list,
			char *purchase_date_time );

LIST *purchase_vendor_payment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

LIST *purchase_equipment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Returns program memory */
/* ---------------------- */
char *purchase_select(	void );

PURCHASE *purchase_parse(
			char *input );

char *purchase_system_string(
			char *where,
			char *order );

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

double purchase_fetch_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Includes transaction->journal_list */
/* ---------------------------------- */
TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable );

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *purchase_transaction_memo,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

LIST *purchase_journal_list(
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable );

char *purchase_asset_account_name(
			LIST *purchase_equipment_list );

LIST *purchase_system_list(
			char *system_string );

LIST *purchase_amount_due_purchase_list(
			void );

FILE *purchase_update_open(
			void );

PURCHASE *purchase_steady_state(
			double sales_tax,
			double freight_in,
			LIST *fixed_asset_purchase_list,
			LIST *inventory_purchase_list,
			LIST *specific_inventory_purchase_list,
			LIST *supply_purchase_list,
			LIST *prepaid_asset_purchase_list,
			LIST *vendor_payment_list,
			PURCHASE *purchase );

double purchase_cost_basis(
		double purchase_cost,
		double sales_tax,
		double freight_in,
		double fixed_asset_purchase_total,
		double inventory_purchase_total,
		double specific_inventory_purchase_total,
		double supply_purchase_total,
		double prepaid_asset_purchase_total );

double purchase_total(
		double fixed_asset_purchase_total,
		double inventory_purchase_total,
		double specific_inventory_purchase_total,
		double supply_purchase_total,
		double prepaid_asset_purchase_total );

double purchase_invoice_amount(
		double sales_tax,
		double freight_in,
		double fixed_asset_purchase_total,
		double inventory_purchase_total,
		double specific_inventory_purchase_total,
		double supply_purchase_total,
		double prepaid_asset_purchase_total );

boolean purchase_is_participating(
			void );

