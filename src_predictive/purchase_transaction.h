/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_transaction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "account.h"
#include "subsidiary_transaction_state.h"
#include "subsidiary_transaction.h"

typedef struct
{
	char *transaction_date_time;
	LIST *journal_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} PURCHASE_TRANSACTION;

/* Usage */
/* ----- */
PURCHASE_TRANSACTION *purchase_transaction_new(
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		boolean arrived_date_time_boolean,
		char *arrived_date_time,
		char *prior_transaction_date_time,
		double sales_tax,
		double freight_in,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_total,
		double service_purchase_total,
		double prepaid_asset_purchase_total,
		double purchase_invoice_amount );

/* Process */
/* ------- */
PURCHASE_TRANSACTION *purchase_transaction_calloc(
	void );

/* Returns parameter, static memory, or null */
/* ----------------------------------------- */
char *purchase_transaction_date_time(
		char *purchase_date_time,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		boolean arrived_date_time_boolean,
		char *arrived_date_time );

/* Usage */
/* ----- */
LIST *purchase_transaction_journal_list(
		double sales_tax,
		double freight_in,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_total,
		double service_purchase_total,
		double prepaid_asset_purchase_total,
		double purchase_invoice_amount );

/* Process */
/* ------- */
double purchase_transaction_debit_sum(
		double sales_tax,
		double freight_in,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_total,
		double service_purchase_total,
		double prepaid_asset_purchase_total );

double purchase_transaction_difference(
		double purchase_transaction_debit_sum,
		double purchase_invoice_amount );

double purchase_transaction_inventory_total(
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total );
