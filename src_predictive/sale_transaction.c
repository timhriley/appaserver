/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_transaction.c			*/
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
#include "sale.h"
#include "sale_transaction.h"

SALE_TRANSACTION *sale_transaction_new(
		char *full_name,
		char *street_address,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *completed_date_time,
		char *shipped_date_time,
		char *arrived_date,
		char *transaction_date_time,
		double shipping_charge,
		double inventory_sale_total,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_total,
		double specific_inventory_sale_CGS_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount )
{
	SALE_TRANSACTION *sale_transaction;

	if ( !full_name
	||   !street_address
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

	sale_transaction = sale_transaction_calloc();

	sale_transaction->transaction_date_time =
/* ---------------------------------------------------------------------- */
/* Returns completed_date_time, shipped_date_time, static memory, or null */
/* ---------------------------------------------------------------------- */
		sale_transaction_date_time(
			predictive_title_passage_rule,
			completed_date_time,
			shipped_date_time,
			arrived_date );

	if ( sale_transaction->transaction_date_time )
	{
		sale_transaction->journal_list =
			sale_transaction_journal_list(
				shipping_charge,
				inventory_sale_total,
				inventory_sale_CGS_total,
				specific_inventory_sale_total,
				specific_inventory_sale_CGS_total,
				sale_invoice_amount,
				sale_gross_revenue,
				sale_sales_tax );
	}

	sale_transaction->subsidiary_transaction_state =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_state_new(
			"preupdate_full_name"
				/* preupdate_full_name_placeholder */,
			"preupdate_street_address"
				/* preupdate_street_address_placeholder */,
			"preupdate_uncollectible_date_time"
				/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_full_name,
			preupdate_street_address,
			transaction_date_time
				/* preupdate_foreign_date_time */,
			full_name,
			street_address,
			sale_transaction->transaction_date_time
				/* foreign_date_time */,
			sale_transaction->journal_list
				/* insert_journal_list */ );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: delete_boolean=%d\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	sale_transaction->subsidiary_transaction_state->delete_boolean );
msg( (char *)0, message );
}
	sale_transaction->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			SALE_TABLE
				/* foreign_table_name */,
			"full_name"
				/* foreign_full_name_column */,
			"street_address"
				/* foreign_street_address_column */,
			"transaction_date_time"
				/* foreign_date_time_column */,
			transaction_date_time,
			sale_transaction->journal_list
				/* insert_journal_list */,
			full_name,
			street_address,
			sale_transaction->transaction_date_time
				/* foreign_date_time */,
			sale_invoice_amount
				/* foreign_amount */,
			"Sale" /* transaction_memo */,
			sale_transaction->
				subsidiary_transaction_state->
				insert_boolean,
			sale_transaction->
				subsidiary_transaction_state->
				delete_boolean,
			sale_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_delete );

	return sale_transaction;
}

SALE_TRANSACTION *sale_transaction_calloc( void )
{
	SALE_TRANSACTION *sale_transaction;

	if ( ! ( sale_transaction =
			calloc( 1,
				sizeof ( SALE_TRANSACTION ) ) ) )
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

	return sale_transaction;
}

char *sale_transaction_date_time(
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *completed_date_time,
		char *shipped_date_time,
		char *arrived_date )
{
	static char transaction_date_time[ 32 ];

	if ( !completed_date_time
	||   !*completed_date_time )
	{
		return NULL;
	}

	if ( predictive_title_passage_rule == title_passage_rule_null )
		return completed_date_time;

	if ( predictive_title_passage_rule == FOB_shipping )
		return shipped_date_time;

	if ( !arrived_date
	||   !*arrived_date )
	{
		return NULL;
	}

	snprintf(
		transaction_date_time,
		sizeof ( transaction_date_time ),
		"%s %s",
		arrived_date,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_colon__hhmmss( date_utc_offset() ) );

	return transaction_date_time;
}

LIST *sale_transaction_journal_list(
		double shipping_charge,
		double inventory_sale_total,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_total,
		double specific_inventory_sale_CGS_total,
		double sale_invoice_amount,
		double sale_gross_revenue,
		double sale_sales_tax )
{
	LIST *list = list_new();
	double debit_sum;
	double credit_sum;
	double difference;
	JOURNAL *journal;
	ACCOUNT *receivable;
	ACCOUNT *revenue;

	debit_sum =
		sale_transaction_debit_sum(
			sale_invoice_amount,
			inventory_sale_CGS_total,
			specific_inventory_sale_CGS_total );

	credit_sum =
		sale_transaction_credit_sum(
			shipping_charge,
			inventory_sale_total,
			specific_inventory_sale_total,
			sale_gross_revenue,
			sale_sales_tax );

	difference =
		sale_transaction_difference(
			debit_sum,
			credit_sum );

	if ( !float_virtually_same( difference, 0.0 ) )
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

	receivable =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_receivable(
			ACCOUNT_RECEIVABLE_KEY,
			__FUNCTION__ );

	journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_account_new(
			sale_invoice_amount,
			receivable /* debit_account */,
			(ACCOUNT *)0 /* credit_account */ );

	list_set( list, journal );

	if (	inventory_sale_total
	||	specific_inventory_sale_total )
	{
		double CGS_total;
		double inventory_total;
		ACCOUNT *cost_of_goods_sold;
		ACCOUNT *inventory;

		CGS_total =
			sale_transaction_CGS_total(
				inventory_sale_CGS_total,
				specific_inventory_sale_CGS_total );

		cost_of_goods_sold =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_cost_of_goods_sold(
				ACCOUNT_CGS_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				CGS_total
					/* journal_amount */,
				cost_of_goods_sold
					/* debit_account */,
				(ACCOUNT *)0
					/* credit_account */ );

		list_set( list, journal );

		inventory_total =
			sale_transaction_inventory_total(
				inventory_sale_total,
				specific_inventory_sale_total );

		inventory =
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
				inventory_total
					/* journal_amount */,
				(ACCOUNT *)0
					/* debit_account */,
				inventory
					/* credit_account */ );

		list_set( list, journal );
	}

	revenue =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_revenue(
			ACCOUNT_REVENUE_KEY,
			__FUNCTION__ );

	journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_account_new(
			sale_gross_revenue /* journal_amount */,
			(ACCOUNT *)0 /* debit_account */,
			revenue /* credit_account */ );

	list_set( list, journal );

	if ( shipping_charge )
	{
		ACCOUNT *shipping_revenue;

		shipping_revenue =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			account_shipping_revenue(
				ACCOUNT_SHIPPING_REVENUE_KEY,
				__FUNCTION__ );

		journal =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			journal_account_new(
				shipping_charge /* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				shipping_revenue /* credit_account */ );

		list_set( list, journal );
	}

	if ( sale_sales_tax )
	{
		ACCOUNT *sales_tax_payable;

		sales_tax_payable =
			account_sales_tax_payable(
				ACCOUNT_SALES_TAX_PAYABLE_KEY,
				__FUNCTION__ );

		journal =
			journal_account_new(
				sale_sales_tax /* journal_amount */,
				(ACCOUNT *)0 /* debit_account */,
				sales_tax_payable /* credit_account */ );

		list_set( list, journal );
	}

	return list;
}

double sale_transaction_debit_sum(
		double sale_invoice_amount,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_CGS_total )
{
	return
	sale_invoice_amount +
	inventory_sale_CGS_total +
	specific_inventory_sale_CGS_total;
}

double sale_transaction_credit_sum(
		double shipping_charge,
		double inventory_sale_total,
		double specific_inventory_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax )
{
	return
	shipping_charge +
	inventory_sale_total +
	specific_inventory_sale_total +
	sale_gross_revenue +
	sale_sales_tax;
}

double sale_transaction_difference(
		double sale_transaction_debit_sum,
		double sale_transaction_credit_sum )
{
	return
	sale_transaction_debit_sum -
	sale_transaction_credit_sum;
}

double sale_transaction_CGS_total(
		double inventory_sale_CGS_total,
		double specific_inventory_sale_CGS_total )
{
	return
	inventory_sale_CGS_total +
	specific_inventory_sale_CGS_total;
}

double sale_transaction_inventory_total(
		double inventory_sale_total,
		double specific_inventory_sale_total )
{
	return
	inventory_sale_total +
	specific_inventory_sale_total;
}
