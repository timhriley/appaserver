/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "security.h"
#include "date.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "optional_column.h"
#include "sql.h"
#include "transaction.h"
#include "journal.h"
#include "specific_inventory_sale.h"
#include "inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "customer_payment.h"
#include "entity.h"
#include "entity_self.h"
#include "self_tax.h"
#include "account.h"
#include "sale.h"

SALE *sale_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_uncollectible_date_time )
{
	SALE *sale;

	if ( !full_name
	||   !sale_date_time
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

	sale = sale_calloc();

	sale->fund_name = fund_name;
	sale->full_name = full_name;
	sale->contact_key = contact_key;
	sale->sale_date_time = sale_date_time;

	sale->sale_fetch =
		sale_fetch_new(
			SALE_SELECT,
			SALE_TABLE,
			fund_name,
			full_name,
			contact_key,
			sale_date_time );

	/* May be deleted */
	/* -------------- */
	if ( !sale->sale_fetch )
	{
		free( sale );
		return NULL;
	}

/*
	if ( sale->sale_fetch->inventory_sale_boolean )
	{
		sale->inventory_sale_total =
			inventory_sale_total(
				sale->sale_fetch->inventory_sale_list );

		sale->inventory_sale_CGS_total =
			inventory_sale_CGS_total(
				sale->sale_fetch->inventory_sale_list );
	}

	if ( sale->sale_fetch->specific_inventory_sale_boolean )
	{
		sale->specific_inventory_sale_total =
			specific_inventory_sale_total(
				sale->
					sale_fetch->
					specific_inventory_sale_list );

		sale->specific_inventory_sale_CGS_total =
			specific_inventory_sale_CGS_total(
				sale->
					sale_fetch->
					specific_inventory_sale_list );
	}
*/

	if ( sale->sale_fetch->fixed_service_sale_boolean )
	{
		sale->fixed_service_sale_total =
			fixed_service_sale_total(
				sale->sale_fetch->fixed_service_sale_list );
	}

/*
	if ( sale->sale_fetch->hourly_service_sale_boolean )
	{
		sale->hourly_service_sale_total =
			hourly_service_sale_total(
				sale->sale_fetch->hourly_service_sale_list );
	}
*/

	sale->gross_revenue =
		SALE_GROSS_REVENUE(
			sale->inventory_sale_total,
			sale->specific_inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total );

	if ( sale->sale_fetch->sales_tax_boolean )
	{
		sale->sales_tax =
			SALE_SALES_TAX(
				sale->inventory_sale_total,
				sale->specific_inventory_sale_total,
				sale->
					sale_fetch->
					self_tax_state_sales_tax_rate );
	}

	sale->invoice_amount =
		SALE_INVOICE_AMOUNT(
			sale->gross_revenue,
			sale->sales_tax,
			sale->sale_fetch->shipping_charge );

	sale->customer_payment_total =
		customer_payment_total(
			sale->sale_fetch->cash_account,
			sale->sale_fetch->completed_date_time,
			sale->sale_fetch->invoice_amount,
			sale->sale_fetch->customer_payment_list );

	sale->amount_due =
		SALE_AMOUNT_DUE(
			sale->invoice_amount,
			sale->customer_payment_total );

	sale->sale_transaction =
		sale_transaction_new(
			fund_name,
			full_name,
			contact_key,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			sale->sale_fetch->predictive_fund_boolean,
			sale->sale_fetch->entity_contact_key_boolean,
			sale->sale_fetch->cash_account,
			sale->sale_fetch->predictive_title_passage_rule,
			sale->sale_fetch->completed_date_time,
			sale->sale_fetch->shipped_date_time,
			sale->sale_fetch->arrived_date,
			sale->sale_fetch->transaction_date_time
				/* prior_transaction_date_time */,
			sale->sale_fetch->shipping_charge,
			sale->inventory_sale_total,
			sale->inventory_sale_CGS_total,
			sale->specific_inventory_sale_total,
			sale->specific_inventory_sale_CGS_total,
			sale->gross_revenue,
			sale->sales_tax,
			sale->invoice_amount );

	sale->sale_loss_transaction =
		sale_loss_transaction_new(
			fund_name,
			full_name,
			contact_key,
			sale->
				sale_fetch->
				uncollectible_date_time,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_uncollectible_date_time,
			sale->
				sale_fetch->
				predictive_fund_boolean,
			sale->
				sale_fetch->
				entity_contact_key_boolean,
			sale->amount_due );

	sale->update_string_list =
		sale_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			sale->sale_fetch->predictive_fund_boolean,
			sale->sale_fetch->entity_contact_key_boolean,
			sale->sale_fetch->shipping_charge_boolean,
			sale->sale_fetch->inventory_sale_boolean,
			sale->sale_fetch->specific_inventory_sale_boolean,
			sale->sale_fetch->fixed_service_sale_boolean,
			sale->sale_fetch->hourly_service_sale_boolean,
			sale->sale_fetch->sales_tax_boolean,
			sale->shipping_charge,
			sale->inventory_sale_total,
			sale->specific_inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total,
			sale->gross_revenue,
			sale->sales_tax,
			sale->invoice_amount,
			sale->customer_payment_total,
			sale->amount_due );

	sale->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_update_system_string(
			SALE_TABLE,
			sale->sale_fetch->primary_key_list );

	return sale;
}

char *sale_primary_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_where;
	char *primary_where;
	char *escape_date_time;
	static char where[ 256 ];

	if ( !full_name
	||   !sale_date_time )
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

	fund_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_where(
			PREDICTIVE_FUND_COLUMN,
			fund_name,
			fund_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			(char *)0 /* table_name */,
			full_name,
			contact_key,
			contact_key_boolean );

	escape_date_time =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_escape_date_time(
			sale_date_time );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s and sale_date_time = '%s'",
		fund_where,
		primary_where,
		escape_date_time );

	return where;
}

char *sale_update_system_string(
		const char *sale_table,
		LIST *primary_key_list )
{
	char *delimited_string;
	char system_string[ 1024 ];

	if ( !list_length( primary_key_list ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delimited_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		list_delimited_string(
			primary_key_list,
			',' );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | sql.e",
		sale_table,
		delimited_string );

	free( delimited_string );

	return strdup( system_string );
}

char *sale_update(
		char *application_name,
		LIST *update_string_list,
		char *update_system_string,
		SALE_TRANSACTION *sale_transaction,
		SALE_LOSS_TRANSACTION *sale_loss_transaction )
{
	FILE *pipe;
	char *update_string;
	char *transaction_date_time = {0};

	if ( !update_system_string )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"update_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	pipe = appaserver_output_pipe( update_system_string );

	if ( list_rewind( update_string_list ) )
	do {
		update_string = list_get( update_string_list );
		fprintf( pipe, "%s\n", update_string );

	} while ( list_next( update_string_list ) );

	pclose( pipe );

	if ( sale_transaction )
	{
		/* ------------------------------------ */
		/* Updates the parent table.		*/
		/* Returns transaction_date_time.	*/
		/* ------------------------------------ */
		transaction_date_time =
			subsidiary_transaction_execute(
				application_name,
				sale_transaction->
					subsidiary_transaction->
					delete_transaction,
				sale_transaction->
					subsidiary_transaction->
					insert_transaction,
				sale_transaction->
					subsidiary_transaction->
					update_template,
				sale_transaction->
					subsidiary_transaction->
					predictive_fund_boolean,
				sale_transaction->
					subsidiary_transaction->
					entity_contact_key_boolean );
	}

	if ( sale_loss_transaction )
	{
		(void)subsidiary_transaction_execute(
			application_name,
			sale_loss_transaction->
				subsidiary_transaction->
				delete_transaction,
			sale_loss_transaction->
				subsidiary_transaction->
				insert_transaction,
			sale_loss_transaction->
				subsidiary_transaction->
				update_template,
			sale_loss_transaction->
				subsidiary_transaction->
				predictive_fund_boolean,
			sale_loss_transaction->
				subsidiary_transaction->
				entity_contact_key_boolean );
	}

	return transaction_date_time;
}

double sale_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time )
{
	double hours;
	DATE *earlier_date;
	DATE *later_date;
	int subtract_minutes;

	if ( !begin_work_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"begin_work_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !end_work_date_time || !*end_work_date_time ) return 0.0;

	if ( ! ( earlier_date = date_19new( begin_work_date_time ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_19new(%s) returned empty.",
			begin_work_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( later_date = date_19new( end_work_date_time ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_19new(%s) returned empty.",
			end_work_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	subtract_minutes =
		date_subtract_minutes(
			later_date,
			earlier_date );

	hours = (double)subtract_minutes / 60.0;

	date_free( earlier_date );
	date_free( later_date );

	return hours;
}

SALE *sale_calloc( void )
{
	SALE *sale;

	if ( ! ( sale = calloc( 1, sizeof ( SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return sale;
}

char *sale_update_transaction_date_time(
		SALE_TRANSACTION *sale_transaction )
{
	char *transaction_date_time = "";

	if ( sale_transaction
	&&   sale_transaction->subsidiary_transaction
	&&   sale_transaction->subsidiary_transaction->insert_transaction )
	{
		transaction_date_time =
			sale_transaction->
				subsidiary_transaction->
				insert_transaction->
				transaction_date_time;
	}

	if ( !transaction_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_time;
}

char *sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_string;
	char *primary_data_string;
	char data_string[ 1024 ];

	if ( !full_name
	||   !sale_date_time )
	{
		char message[ 1024 ];

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

	fund_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_string(
			sql_delimiter,
			fund_name,
			fund_boolean );

	primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_primary_data_string(
			sql_delimiter,
			contact_key_boolean,
			full_name,
			contact_key );

	snprintf(
		data_string,
		sizeof ( data_string ),
		"%s%s%c%s",
		fund_string,
		primary_data_string,
		sql_delimiter,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_escape_date_time(
			sale_date_time ) );

	free( primary_data_string );

	return strdup( data_string );
}


LIST *sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean shipping_charge_boolean,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean,
		boolean sales_tax_boolean,
		double shipping_charge,
		double inventory_sale_total,
		double specific_inventory_sale_total,
		double fixed_service_sale_total,
		double hourly_service_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount,
		double customer_payment_total,
		double sale_amount_due )
{
	LIST *list = list_new();
	char *primary_data_string;
	char *update_string;

	primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"shipping_charge" /* column_name */,
			shipping_charge /* money */,
			shipping_charge_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"inventory_sale_total" /* column_name */,
			inventory_sale_total /* money */,
			inventory_sale_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"specific_inventory_sale_total" /* column_name */,
			specific_inventory_sale_total /* money */,
			specific_inventory_sale_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"fixed_service_sale_total" /* column_name */,
			fixed_service_sale_total /* money */,
			fixed_service_sale_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"hourly_service_sale_total" /* column_name */,
			hourly_service_sale_total /* money */,
			hourly_service_sale_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"gross_revenue" /* column_name */,
			sale_gross_revenue /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"sales_tax" /* column_name */,
			sale_sales_tax /* money */,
			sales_tax_boolean /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"invoice_amount" /* column_name */,
			sale_invoice_amount /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"payment_total" /* column_name */,
			customer_payment_total /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"amount_due" /* column_name */,
			sale_amount_due /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	return list;
}

char *sale_update_string(
		const char sql_delimiter,
		char *primary_data_string,
		const char *column_name,
		double money,
		boolean set_boolean )
{
	OPTIONAL_COLUMN *optional_column;
	char *update_string = {0};

	if ( set_boolean )
	{
		optional_column =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			optional_column_new(
				sql_delimiter,
				primary_data_string /* base_string */,
				(char *)column_name /* component */,
				0 /* not escape_boolean */,
				1 /* set_boolean */ );

		optional_column =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			optional_column_money_new(
				sql_delimiter,
				optional_column->return_string
					/* base_string */,
				money,
				1 /* set_boolean */ );

		free( optional_column->prior_return_string );

		update_string = optional_column->return_string;
	}

	return update_string;
}

