/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_transaction.c		*/
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
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		boolean fund_boolean,
		boolean contact_key_boolean,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		boolean arrived_date_time_boolean,
		char *arrived_date_time,
		char *prior_transaction_date_time,
		double sales_tax_expense,
		double freight_in_expense,
		double cost_basis_fixed_asset_total,
		double cost_basis_inventory_total,
		double cost_basis_specific_inventory_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double return_list_total,
		double purchase_invoice_amount )
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

	if ( purchase_invoice_amount < 0.0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"invalid purchase_invoice_amount=%.2lf.",
			purchase_invoice_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( float_money_virtually_zero( purchase_invoice_amount ) )
		return NULL;

	purchase_transaction = purchase_transaction_calloc();

	purchase_transaction->transaction_date_time =
		/* ----------------------------------------- */
		/* Returns parameter, static memory, or null */
		/* ----------------------------------------- */
		purchase_transaction_date_time(
			purchase_date_time,
			predictive_title_passage_rule,
			shipped_date,
			arrived_date_time_boolean,
			arrived_date_time );

	if ( purchase_transaction->transaction_date_time )
	{
		purchase_transaction->journal_list =
			purchase_transaction_journal_list(
				sales_tax_expense,
				freight_in_expense,
				cost_basis_fixed_asset_total,
				cost_basis_inventory_total,
				cost_basis_specific_inventory_total,
				supply_purchase_list_total,
				service_purchase_list_total,
				prepaid_asset_purchase_list_total,
				return_list_total,
				purchase_invoice_amount );
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
			"purchase_date_time"
				/* foreign_date_time_column */,
			"transaction_date_time"
				/* update_date_time_column */,
			purchase_transaction->journal_list
				/* insert_journal_list */,
			purchase_invoice_amount
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
		char *purchase_date_time,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *shipped_date,
		boolean arrived_date_time_boolean,
		char *arrived_date_time )
{
	static char transaction_date_time[ 32 ];

	if ( predictive_title_passage_rule == title_passage_rule_null )
	{
		if ( arrived_date_time_boolean )
			return arrived_date_time;
		else
			return purchase_date_time;
	}

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
		double sales_tax_expense,
		double freight_in_expense,
		double cost_basis_fixed_asset_total,
		double cost_basis_inventory_total,
		double cost_basis_specific_inventory_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double return_list_total,
		double purchase_invoice_amount )
{
	LIST *list = list_new();
	double debit_sum;
	double credit_sum;
	double difference;
	JOURNAL *journal;
	ACCOUNT *account;
	double transaction_inventory_total;

	debit_sum =
		purchase_transaction_debit_sum(
			sales_tax_expense,
			freight_in_expense,
			cost_basis_fixed_asset_total,
			cost_basis_inventory_total,
			cost_basis_specific_inventory_total,
			supply_purchase_list_total,
			service_purchase_list_total,
			prepaid_asset_purchase_list_total );

	credit_sum =
		purchase_transaction_credit_sum(
			return_list_total,
			purchase_invoice_amount );

	difference =
		purchase_transaction_difference(
			debit_sum,
			credit_sum );

	if ( !float_money_virtually_zero( difference ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
		"debit_sum=%.2lf != credit_sum=%.2lf",
			debit_sum,
			credit_sum );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !float_money_virtually_zero( sales_tax_expense ) )
	{
		account =
			account_sales_tax_expense(
				ACCOUNT_SALES_TAX_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				sales_tax_expense /* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( freight_in_expense ) )
	{
		account =
			account_freight_in_expense(
				ACCOUNT_FREIGHT_IN_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				freight_in_expense /* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( cost_basis_fixed_asset_total ) )
	{
		account =
			account_freight_in_expense(
				ACCOUNT_FIXED_ASSET_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				cost_basis_fixed_asset_total
					/* journal_amount */,
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	transaction_inventory_total =
		purchase_transaction_inventory_total(
			cost_basis_inventory_total,
			cost_basis_specific_inventory_total );

	if ( !float_money_virtually_zero( transaction_inventory_total ) )
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
				account /* debit_account */,
				(ACCOUNT *)0 /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( supply_purchase_list_total ) )
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
				supply_purchase_list_total /* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( service_purchase_list_total ) )
	{
		account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_inventory(
				ACCOUNT_SERVICE_EXPENSE_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				service_purchase_list_total
					/* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( prepaid_asset_purchase_list_total ) )
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
				prepaid_asset_purchase_list_total
					/* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				account /* credit_account */ );

		list_set( list, journal );
	}

	if ( !float_money_virtually_zero( return_list_total ) )
	{
		account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_inventory_return(
				ACCOUNT_INVENTORY_RETURN_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				return_list_total /* journal_amount */,
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
			purchase_invoice_amount,
			(ACCOUNT *)0 /* debit_account */,
			account /* credit_account */ );

	list_set( list, journal );

	return list;
}

double purchase_transaction_debit_sum(
		double sales_tax_expense,
		double freight_in_expense,
		double cost_basis_fixed_asset_total,
		double cost_basis_inventory_total,
		double cost_basis_specific_inventory_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total )
{
	return
	sales_tax_expense +
	freight_in_expense +
	cost_basis_fixed_asset_total +
	cost_basis_inventory_total +
	cost_basis_specific_inventory_total +
	supply_purchase_list_total +
	service_purchase_list_total +
	prepaid_asset_purchase_list_total;
}

double purchase_transaction_credit_sum(
		double return_list_total,
		double invoice_amount )
{
	return
	return_list_total +
	invoice_amount;
}

double purchase_transaction_difference(
		double debit_sum,
		double credit_sum )
{
	return
	debit_sum -
	credit_sum;
}

double purchase_transaction_inventory_total(
		double cost_basis_inventory_total,
		double cost_basis_specific_inventory_total )
{
	return
	cost_basis_inventory_total +
	cost_basis_specific_inventory_total;
}
