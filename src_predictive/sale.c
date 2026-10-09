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
#include "float.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "update.h"
#include "sql.h"
#include "update.h"
#include "transaction.h"
#include "journal.h"
#include "customer_payment.h"
#include "entity.h"
#include "entity_self.h"
#include "predictbooks_self.h"
#include "account.h"
#include "sale.h"

SALE *sale_trigger_new(
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_uncollectible_date_time,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *state )
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

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) return NULL;

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
			sale_date_time,
			0 /* not customer_entity_boolean */ );

	if ( !sale->sale_fetch )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"sale_fetch_new(%s,%s) returned empty.",
			full_name,
			sale_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sale->sale_calculate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		sale_calculate_new(
			sale->sale_fetch->shipping_revenue_boolean,
			sale->sale_fetch->customer,
			sale->sale_fetch->inventory_sale_list,
			sale->sale_fetch->specific_inventory_sale_list,
			sale->sale_fetch->fixed_service_sale_list,
			sale->sale_fetch->hourly_service_sale_list,
			sale->sale_fetch->sales_tax_boolean,
			sale->sale_fetch->state_sales_tax_rate,
			sale->sale_fetch->cash_account,
			sale->sale_fetch->completed_date_time,
			sale->sale_fetch->customer_payment_list );

	sale->sale_transaction =
		sale_transaction_new(
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state,
			sale->sale_fetch->predictive_fund_boolean,
			sale->sale_fetch->entity_contact_key_boolean,
			sale->sale_fetch->cash_account,
			sale->sale_fetch->predictive_title_passage_rule,
			sale->sale_fetch->completed_date_time,
			sale->sale_fetch->shipped_date_time,
			sale->sale_fetch->arrived_date,
			sale->sale_fetch->transaction_date_time
				/* fetch_transaction_date_time */,
			sale->sale_calculate->shipping_revenue,
			sale->sale_fetch->inventory_sale_list->extended_total,
			sale->sale_fetch->inventory_sale_list->CGS_total,
			sale->
				sale_fetch->
				specific_inventory_sale_list->
				extended_total,
			sale->
				sale_fetch->
				specific_inventory_sale_list->
				CGS_total,
			sale->sale_calculate->gross_revenue,
			sale->sale_calculate->sales_tax,
			sale->sale_calculate->invoice_amount );

	sale->sale_loss_transaction =
		sale_loss_transaction_new(
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_uncollectible_date_time,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			sale->
				sale_fetch->
				uncollectible_date_time,
			state,
			sale->
				sale_fetch->
				predictive_fund_boolean,
			sale->
				sale_fetch->
				entity_contact_key_boolean,
			sale->sale_fetch->amount_due );

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) != 0 )
	{
		sale->sale_update =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			sale_update_new(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				sale->sale_fetch,
				sale->sale_calculate->shipping_revenue,
				sale->sale_calculate->gross_revenue,
				sale->sale_calculate->cost_of_goods_sold,
				sale->sale_calculate->inventory_markup_percent,
				sale->sale_calculate->sales_tax,
				sale->sale_calculate->invoice_amount,
				sale->sale_calculate->customer_payment_total,
				sale->sale_calculate->amount_due );
	}
	else
	{
		sale->sale_update = sale_update_calloc();
	}

	return sale;
}

char *sale_primary_where(
		const char *sale_date_time_column,
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
		"%s and %s and %s = '%s'",
		fund_where,
		primary_where,
		sale_date_time_column,
		escape_date_time );

	return where;
}

char *sale_update_system_string(
		const char *sale_table,
		LIST *primary_key_list )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	predictive_update_system_string(
		sale_table,
		primary_key_list );
}

char *sale_update_execute(
		char *application_name,
		SALE_UPDATE *sale_update,
		SALE_TRANSACTION *sale_transaction,
		SALE_LOSS_TRANSACTION *sale_loss_transaction )
{
	char *transaction_date_time = {0};

	if ( !sale_update
	||   !sale_update->inventory_sale_list
	||   !sale_update->specific_inventory_sale_list
	||   !sale_update->hourly_service_sale_list
	||   !sale_update->fixed_service_sale_list )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_string_list_execute(
		sale_update->update_system_string,
		sale_update->update_string_list );

	update_string_list_execute(
		sale_update->
			inventory_sale_list->
			update_system_string,
		sale_update->
			inventory_sale_list->
			update_string_list );

	sale_update_purchase_quantity_on_hand_execute(
		sale_update->
			inventory_sale_list->
			list
			/* inventory_sale_list */ );

	update_string_list_execute(
		sale_update->
			specific_inventory_sale_list->
			update_system_string,
		sale_update->
			specific_inventory_sale_list->
			update_string_list );

	update_string_list_execute(
		sale_update->
			hourly_service_sale_list->
			update_system_string,
		sale_update->
			hourly_service_sale_list->
			update_string_list );

	update_string_list_execute(
		sale_update->
			fixed_service_sale_list->
			update_system_string,
		sale_update->
			fixed_service_sale_list->
			update_string_list );

	if ( sale_transaction )
	{
		/* ------------------------------------ */
		/* Updates the many table.		*/
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
					update_null_sql,
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
				update_null_sql,
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

	if ( !begin_work_date_time || !*begin_work_date_time ) return 0.0;
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

char *sale_update_string(
		const char sql_delimiter,
		char *primary_data_string,
		const char *column_name,
		double money,
		boolean set_boolean )
{
	return
	/* ------------------------------------------------ */
	/* Returns heap memory or null (if not set_boolean) */
	/* ------------------------------------------------ */
	update_double_string(
		sql_delimiter,
		primary_data_string,
		column_name,
		money /* number */,
		set_boolean );
}

char *sale_update_integer_string(
		const char sql_delimiter,
		char *primary_data_string,
		const char *column_name,
		int integer,
		boolean set_boolean )
{
	return
	/* ------------------------------------------------ */
	/* Returns heap memory or null (if not set_boolean) */
	/* ------------------------------------------------ */
	update_integer_string(
		sql_delimiter,
		primary_data_string,
		column_name,
		integer /* number */,
		set_boolean );
}

char *sale_update_text_string(
		const char sql_delimiter,
		char *primary_data_string,
		const char *column_name,
		char *text,
		boolean set_boolean )
{
	return
	/* ------------------------------------------------ */
	/* Returns heap memory or null (if not set_boolean) */
	/* ------------------------------------------------ */
	update_text_string(
		sql_delimiter,
		primary_data_string,
		column_name,
		text,
		set_boolean );
}

SALE_CALCULATE *sale_calculate_new(
		boolean shipping_revenue_boolean,
		CUSTOMER *customer,
		INVENTORY_SALE_LIST *inventory_sale_list,
		SPECIFIC_INVENTORY_SALE_LIST *specific_inventory_sale_list,
		FIXED_SERVICE_SALE_LIST *fixed_service_sale_list,
		HOURLY_SERVICE_SALE_LIST *hourly_service_sale_list,
		boolean sales_tax_boolean,
		double predictbooks_self_state_sales_tax_rate,
		char *cash_account,
		char *completed_date_time,
		LIST *customer_payment_list )
{
	SALE_CALCULATE *sale_calculate;

	if ( !customer
	||   !inventory_sale_list
	||   !specific_inventory_sale_list
	||   !fixed_service_sale_list
	||   !hourly_service_sale_list )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty: %x,%x,%x,%x,%x",
			(unsigned int)(long)customer,
			(unsigned int)(long)inventory_sale_list,
			(unsigned int)(long)specific_inventory_sale_list,
			(unsigned int)(long)fixed_service_sale_list,
			(unsigned int)(long)hourly_service_sale_list );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sale_calculate = sale_calculate_calloc();

	if ( shipping_revenue_boolean )
	{
/*
		sale_calculate->shipping_revenue =
			sale_calculate_shipping_revenue(
				customer->zip_code );
*/
	}

	sale_calculate->gross_revenue =
		SALE_CALCULATE_GROSS_REVENUE(
			inventory_sale_list->extended_total,
			specific_inventory_sale_list->extended_total,
			fixed_service_sale_list->revenue_total,
			hourly_service_sale_list->revenue_total );

	sale_calculate->cost_of_goods_sold =
		SALE_CALCULATE_COST_OF_GOODS_SOLD(
			inventory_sale_list->CGS_total,
			specific_inventory_sale_list->CGS_total );

	sale_calculate->inventory_markup_percent =
		sale_calculate_inventory_markup_percent(	
			inventory_sale_list->extended_total,
			specific_inventory_sale_list->extended_total,
			sale_calculate->cost_of_goods_sold );

	if ( sales_tax_boolean )
	{
		sale_calculate->sales_tax =
			SALE_CALCULATE_SALES_TAX(
				inventory_sale_list->extended_total,
				specific_inventory_sale_list->extended_total,
				predictbooks_self_state_sales_tax_rate );
	}

	sale_calculate->invoice_amount =
		SALE_CALCULATE_INVOICE_AMOUNT(
			sale_calculate->gross_revenue,
			sale_calculate->sales_tax,
			sale_calculate->shipping_revenue );

	sale_calculate->customer_payment_total =
		customer_payment_total(
			cash_account,
			completed_date_time,
			customer_payment_list,
			sale_calculate->invoice_amount );

	sale_calculate->amount_due =
		SALE_CALCULATE_AMOUNT_DUE(
			sale_calculate->invoice_amount,
			sale_calculate->customer_payment_total );

	return sale_calculate;
}

SALE_CALCULATE *sale_calculate_calloc( void )
{
	SALE_CALCULATE *sale_calculate;

	if ( ! ( sale_calculate = calloc( 1, sizeof ( SALE_CALCULATE ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return sale_calculate;
}

int sale_calculate_inventory_markup_percent(
		double inventory_sale_list_extended_total,
		double specific_inventory_sale_list_extended_total,
		int cost_of_goods_sold )
{
	double inventory_total;
	double ratio;

	inventory_total =
		sale_calculate_inventory_total(
			inventory_sale_list_extended_total,
			specific_inventory_sale_list_extended_total );

	if ( !inventory_total ) return 0;

	ratio =
		sale_calculate_inventory_markup_ratio(
			cost_of_goods_sold,
			inventory_total );
	return
	float_round_integer( ratio * 100.0 );
}

double sale_calculate_inventory_total(
		double inventory_sale_list_extended_total,
		double specific_inventory_sale_list_extended_total )
{
	return
	inventory_sale_list_extended_total +
	specific_inventory_sale_list_extended_total;
}

double sale_calculate_inventory_markup_ratio(
		double cost_of_goods_sold,
		double inventory_total )
{
	if ( float_money_virtually_zero( inventory_total ) ) return 0.0;

	return
	(inventory_total - cost_of_goods_sold) / inventory_total;
}

SALE_UPDATE *sale_update_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		SALE_FETCH *sale_fetch,
		double shipping_revenue,
		double gross_revenue,
		double cost_of_goods_sold,
		int inventory_markup_percent,
		double sales_tax,
		double invoice_amount,
		double customer_payment_total,
		double amount_due )
{
	SALE_UPDATE *sale_update;

	if ( !full_name
	||   !sale_date_time
	||   !sale_fetch )
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


	sale_update = sale_update_calloc();

	sale_update->inventory_sale_list =
		sale_fetch->inventory_sale_list;

	sale_update->specific_inventory_sale_list =
		sale_fetch->specific_inventory_sale_list;

	sale_update->fixed_service_sale_list =
		sale_fetch->fixed_service_sale_list;

	sale_update->hourly_service_sale_list =
		sale_fetch->hourly_service_sale_list;

	sale_update->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_update_system_string(
			SALE_TABLE,
			sale_fetch->primary_key_list );

	sale_update->update_string_list =
		sale_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			sale_fetch->predictive_fund_boolean,
			sale_fetch->entity_contact_key_boolean,
			sale_fetch->inventory_total,
			sale_update->
				inventory_sale_list->
				extended_total,
			sale_fetch->specific_inventory_total,
			sale_update->
				specific_inventory_sale_list->
				extended_total,
			sale_fetch->fixed_service_total,
			sale_update->
				fixed_service_sale_list->
				revenue_total,
			sale_fetch->hourly_service_total,
			sale_update->
				hourly_service_sale_list->
				revenue_total,
			sale_fetch->shipping_revenue,
			shipping_revenue,
			sale_fetch->gross_revenue,
			gross_revenue,
			sale_fetch->cost_of_goods_sold,
			cost_of_goods_sold,
			sale_fetch->inventory_markup_percent,
			inventory_markup_percent,
			sale_fetch->sales_tax,
			sales_tax,
			sale_fetch->invoice_amount,
			invoice_amount,
			sale_fetch->payment_total,
			customer_payment_total,
			sale_fetch->amount_due,
			amount_due );

	return sale_update;
}

SALE_UPDATE *sale_update_calloc( void )
{
	SALE_UPDATE *sale_update;

	if ( ! ( sale_update = calloc( 1, sizeof ( SALE_UPDATE ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return sale_update;
}

LIST *sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double sale_fetch_inventory_total,
		double inventory_sale_list_extended_total,
		double sale_fetch_specific_inventory_total,
		double specific_inventory_sale_list_extended_total,
		double sale_fetch_fixed_service_total,
		double fixed_service_sale_list_revenue_total,
		double sale_fetch_hourly_service_total,
		double hourly_service_sale_list_revenue_total,
		double sale_fetch_shipping_revenue,
		double shipping_revenue,
		double sale_fetch_gross_revenue,
		double gross_revenue,
		double sale_fetch_cost_of_goods_sold,
		double cost_of_goods_sold,
		int sale_fetch_inventory_markup_percent,
		int inventory_markup_percent,
		double sale_fetch_sales_tax,
		double sales_tax,
		double sale_fetch_invoice_amount,
		double invoice_amount,
		double sale_fetch_payment_total,
		double customer_payment_total,
		double sale_fetch_amount_due,
		double amount_due )
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

	if ( !float_money_virtually_same(
		sale_fetch_inventory_total,
		inventory_sale_list_extended_total ) )
	{
		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"inventory_total",
				inventory_sale_list_extended_total,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_specific_inventory_total,
		specific_inventory_sale_list_extended_total ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"specific_inventory_total" /* column */,
				specific_inventory_sale_list_extended_total
					/* money */,
				1 /* set_boolean */ );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_fixed_service_total,
		fixed_service_sale_list_revenue_total ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"fixed_service_total",
				fixed_service_sale_list_revenue_total,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_hourly_service_total,
		hourly_service_sale_list_revenue_total ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"hourly_service_total",
				hourly_service_sale_list_revenue_total,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_shipping_revenue,
		shipping_revenue ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"shipping_revenue",
				shipping_revenue,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_gross_revenue,
		gross_revenue ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"gross_revenue",
				gross_revenue,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_cost_of_goods_sold,
		cost_of_goods_sold ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"cost_of_goods_sold",
				cost_of_goods_sold,
				1 );

		list_set( list, update_string );
	}

	if (	sale_fetch_inventory_markup_percent !=
		inventory_markup_percent )
	{
		update_string =
			sale_update_integer_string(
				sql_delimiter,
				primary_data_string,
				"inventory_markup_percent" /* column_name */,
				inventory_markup_percent /* integer */,
				1 /* set_boolean */ );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_sales_tax,
		sales_tax ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"sales_tax",
				sales_tax,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_invoice_amount,
		invoice_amount ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"invoice_amount",
				invoice_amount,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_payment_total,
		customer_payment_total ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"payment_total",
				customer_payment_total,
				1 );

		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		sale_fetch_amount_due,
		amount_due ) )
	{
		update_string =
			sale_update_string(
				sql_delimiter,
				primary_data_string,
				"amount_due",
				amount_due,
				1 );

		list_set( list, update_string );
	}

	free( primary_data_string );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

void sale_update_purchase_quantity_on_hand_execute( LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;

	if ( list_rewind( inventory_sale_list ) )
	do {
		inventory_sale = list_get( inventory_sale_list );

		if ( !inventory_sale->inventory_purchase_update )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"inventory_sale->inventory_purchase_update is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		update_string_list_execute(
			inventory_sale->
				inventory_purchase_update->
				update_system_string,
			inventory_sale->
				inventory_purchase_update->
				update_string_list );

	} while ( list_next( inventory_sale_list ) );
}
