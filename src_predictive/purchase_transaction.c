/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_transaction.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "float.h"
#include "date.h"
#include "journal.h"
#include "account.h"
#include "purchase.h"
#include "purchase_transaction.h"

PURCHASE_TRANSACTION *purchase_transaction_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		boolean fund_boolean,
		boolean contact_key_boolean,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		char *arrived_date_time,
		char *prior_transaction_date_time,
		double sales_tax,
		double freight_in,
		double fixed_asset_total,
		double inventory_total,
		double specific_inventory_total,
		double supply_total,
		double prepaid_asset_total,
		double invoice_amount )
{
	PURCHASE_TRANSACTION *purchase_transaction;

	if ( !full_name
	||   !state )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( invoice_amount < 0.0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"invalid invoice_amount=%.2lf.",
			invoice_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( float_money_virtually_same( invoice_amount, 0.0 ) ) return NULL;

	purchase_transaction = purchase_transaction_calloc();

	purchase_transaction->transaction_date_time =
		/* ------------------------------------------------- */
		/* Returns arrived_date_time, static memory, or null */
		/* ------------------------------------------------- */
		purchase_transaction_date_time(
			predictive_title_passage_rule,
			shipped_date,
			arrived_date_time );

	if ( purchase_transaction->transaction_date_time )
	{
		purchase_transaction->journal_list =
			purchase_transaction_journal_list(
				sales_tax,
				freight_in,
				fixed_asset_total,
				inventory_total,
				specific_inventory_total,
				supply_total,
				prepaid_asset_total,
				invoice_amount );
	}

	purchase_transaction->subsidiary_transaction_state =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_state_new(
			"preupdate_fund_name"
				/* preupdate_fund_name_placeholder */,
			"preupdate_full_name"
				/* preupdate_full_name_placeholder */,
			"preupdate_contact_key"
				/* preupdate_contact_key_placeholder */,
			"preupdate_transaction_date_time"
				/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			prior_transaction_date_time
				/* preupdate_foreign_date_time */,
			fund_name,
			full_name,
			contact_key,
			purchase_transaction->transaction_date_time
				/* foreign_date_time */,
			fund_boolean,
			contact_key_boolean,
			purchase_transaction->journal_list
				/* insert_journal_list */ );

	purchase_transaction->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			PURCHASE_TABLE
				/* foreign_table_name */,
			"fund_name"
				/* foreign_fund_name_column */,
			"full_name"
				/* foreign_full_name_column */,
			"contact_key"
				/* foreign_contact_key_column */,
			"transaction_date_time"
				/* foreign_date_time_column */,
			"transaction_date_time"
				/* update_date_time_column */,
			purchase_transaction->journal_list
				/* insert_journal_list */,
			invoice_amount
				/* foreign_amount */,
			PURCHASE_MEMO /* transaction_memo */,
			purchase_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_insert,
			purchase_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_delete,
			fund_boolean,
			contact_key_boolean );

	return purchase_transaction;
}

PURCHASE_TRANSACTION *purchase_transaction_calloc( void )
{
	PURCHASE_TRANSACTION *purchase_transaction;

	if ( ! ( purchase_transaction =
			calloc( 1,
				sizeof ( PURCHASE_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return purchase_transaction;
}

char *purchase_transaction_date_time(
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		char *arrived_date_time )
{
	static char transaction_date_time[ 32 ];

	if ( predictive_title_passage_rule == title_passage_rule_null )
		return arrived_date_time;

	if ( predictive_title_passage_rule == FOB_shipping )
		return arrived_date_time;

	if ( !shipped_date || !*shipped_date )
	{
		return NULL;
	}

	snprintf(
		transaction_date_time,
		sizeof ( transaction_date_time ),
		"%s %s",
		shipped_date,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_colon_hhmmss( date_utc_offset() ) );

	return transaction_date_time;
}

LIST *purchase_transaction_journal_list(
		double sales_tax,
		double freight_in,
		double fixed_asset_total,
		double inventory_total,
		double specific_inventory_total,
		double supply_total,
		double prepaid_asset_total,
		double invoice_amount )
{
	LIST *list = list_new();
	double debit_sum;
	double difference;
	JOURNAL *journal;
	ACCOUNT *account;
	double transaction_inventory_total;

	debit_sum =
		purchase_transaction_debit_sum(
			sales_tax,
			freight_in,
			fixed_asset_total,
			inventory_total,
			specific_inventory_total,
			supply_total,
			prepaid_asset_total );

	difference =
		purchase_transaction_difference(
			debit_sum,
			invoice_amount );

	if ( !float_money_virtually_same( difference, 0.0 ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
		"debit_sum=%.2lf != credit_sum=%.2lf",
			debit_sum,
			invoice_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( sales_tax )
	{
		account =
			account_sales_tax_expense(
				ACCOUNT_SALES_TAX_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				sales_tax /* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	if ( freight_in )
	{
		account =
			account_freight_in_expense(
				ACCOUNT_FREIGHT_IN_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				freight_in /* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	if ( fixed_asset_total )
	{
		account =
			account_freight_in_expense(
				ACCOUNT_FIXED_ASSET_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				fixed_asset_total /* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	transaction_inventory_total =
		purchase_transaction_inventory_total(
			inventory_total,
			specific_inventory_total );

	if ( transaction_inventory_total )
	{
		account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_inventory(
				ACCOUNT_INVENTORY_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				transaction_inventory_total
					/* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	if ( supply_total )
	{
		account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_inventory(
				ACCOUNT_SUPPLY_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				supply_total /* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	if ( prepaid_asset_total )
	{
		account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_inventory(
				ACCOUNT_PREPAID_ASSET_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				prepaid_asset_total /* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_payable(
			ACCOUNT_PAYABLE_KEY,
			__FUNCTION__ );

	journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_account_new(
			invoice_amount,
			(ACCOUNT *)0 /* debit_account */,
			account /* credit_account */ );

	list_set( list, journal );

	return list;
}

double purchase_transaction_debit_sum(
		double sales_tax,
		double freight_in,
		double fixed_asset_total,
		double inventory_total,
		double specific_inventory_total,
		double supply_total,
		double prepaid_asset_total )
{
	return
	sales_tax +
	freight_in +
	fixed_asset_total +
	inventory_total +
	specific_inventory_total +
	supply_total +
	prepaid_asset_total;
}

double purchase_transaction_difference(
		double purchase_transaction_debit_sum,
		double invoice_amount )
{
	return
	purchase_transaction_debit_sum -
	invoice_amount;
}

double purchase_transaction_inventory_total(
		double inventory_total,
		double specific_inventory_total )
{
	return
	inventory_total +
	specific_inventory_total;
}
