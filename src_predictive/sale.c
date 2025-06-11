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
#include "sql.h"
#include "transaction.h"
#include "journal.h"
#include "specific_inventory_sale.h"
#include "inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "customer_payment.h"
#include "entity_self.h"
#include "self_tax.h"
#include "account.h"
#include "sale.h"

SALE *sale_trigger_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_uncollectible_date_time )
{
	SALE *sale;

	if ( !full_name
	||   !street_address
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

	sale->full_name = full_name;
	sale->street_address = street_address;
	sale->sale_date_time = sale_date_time;

	sale->sale_fetch =
		sale_fetch_new(
			SALE_SELECT,
			SALE_TABLE,
			full_name,
			street_address,
			sale_date_time );

	if ( !sale->sale_fetch )
	{
		free( sale );
		return NULL;
	}

	if ( list_length( sale->sale_fetch->inventory_sale_list ) )
	{
		sale->inventory_sale_total =
			inventory_sale_total(
				sale->sale_fetch->inventory_sale_list );

		sale->inventory_sale_CGS_total =
			inventory_sale_CGS_total(
				sale->sale_fetch->inventory_sale_list );
	}

	if ( list_length( sale->sale_fetch->specific_inventory_sale_list ) )
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

	if ( list_length( sale->sale_fetch->fixed_service_sale_list ) )
	{
		sale->fixed_service_sale_total =
			fixed_service_sale_total(
				sale->sale_fetch->fixed_service_sale_list );
	}

	if ( list_length( sale->sale_fetch->hourly_service_sale_list ) )
	{
		sale->hourly_service_sale_total =
			hourly_service_sale_total(
				sale->sale_fetch->hourly_service_sale_list );
	}

	sale->gross_revenue =
		SALE_GROSS_REVENUE(
			sale->inventory_sale_total,
			sale->specific_inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total );

	sale->sales_tax =
		SALE_SALES_TAX(
			sale->inventory_sale_total,
			sale->specific_inventory_sale_total,
			sale->sale_fetch->self_tax_state_sales_tax_rate );

	sale->invoice_amount =
		SALE_INVOICE_AMOUNT(
			sale->gross_revenue,
			sale->sales_tax,
			sale->sale_fetch->shipping_charge );

	sale->customer_payment_total =
		customer_payment_total(
			sale->
				sale_fetch->
				customer_payment_list );

	sale->amount_due =
		SALE_AMOUNT_DUE(
			sale->invoice_amount,
			sale->customer_payment_total );

	sale->sale_transaction =
		sale_transaction_new(
			full_name,
			street_address,
			state,
			preupdate_full_name,
			preupdate_street_address,
			sale->sale_fetch->predictive_title_passage_rule,
			sale->sale_fetch->completed_date_time,
			sale->sale_fetch->transaction_date_time,
			sale->sale_fetch->shipped_date_time,
			sale->sale_fetch->arrived_date,
			sale->sale_fetch->shipping_charge,
			sale->inventory_sale_total,
			sale->inventory_sale_CGS_total,
			sale->specific_inventory_sale_total,
			sale->specific_inventory_sale_CGS_total,
			sale->gross_revenue,
			sale->sales_tax,
			sale->invoice_amount );

	if ( sale->sale_fetch->uncollectible_date_time )
	{
		sale->sale_loss_transaction =
			sale_loss_transaction_new(
				full_name,
				street_address,
				sale->
					sale_fetch->
					uncollectible_date_time,
				state,
				preupdate_full_name,
				preupdate_street_address,
				preupdate_uncollectible_date_time,
				sale->amount_due );
	}

	return sale;
}

char *sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	static char where[ 128 ];
	char *tmp1;
	char *tmp2;
	char *tmp3;

	if ( !full_name
	||   !street_address
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

	snprintf(
		where,
		sizeof ( where ),
		"full_name = '%s' and		"
		"street_address = '%s' and	"
		"sale_date_time = '%s'		",
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		( tmp1 = security_escape( full_name ) ),
		( tmp2 = security_escape( street_address ) ),
		( tmp3 = security_escape( sale_date_time ) ) );

	return where;
}

char *sale_update_system_string( const char *sale_table )
{
	char system_string[ 1024 ];
	char *key;

	key = "full_name,street_address,sale_date_time";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | sql",
		sale_table,
		key );

	return strdup( system_string );
}

void sale_update(
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean,
		double inventory_sale_total,
		double specific_inventory_sale_total,
		double fixed_service_sale_total,
		double hourly_service_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount,
		double customer_payment_total,
		double sale_amount_due,
		SALE_TRANSACTION *sale_transaction )
{
	char *system_string;
	FILE *pipe;

	if ( !full_name
	||   !street_address
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

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_update_system_string(
			sale_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	if ( inventory_sale_boolean )
	{
		fprintf(pipe,
	 		"%s^%s^%s^inventory_sale_total^%.2lf\n",
			entity_escape_full_name( full_name ),
			entity_escape_street_address( street_address ),
			sale_date_time,
			inventory_sale_total );
	}

	if ( specific_inventory_sale_boolean )
	{
		fprintf(pipe,
	 		"%s^%s^%s^specific_inventory_sale_total^%.2lf\n",
			entity_escape_full_name( full_name ),
			entity_escape_street_address( street_address ),
			sale_date_time,
			specific_inventory_sale_total );
	}

	if ( fixed_service_sale_boolean )
	{
		fprintf(pipe,
	 		"%s^%s^%s^fixed_service_sale_total^%.2lf\n",
			entity_escape_full_name( full_name ),
			entity_escape_street_address( street_address ),
			sale_date_time,
			fixed_service_sale_total );
	}

	if ( hourly_service_sale_boolean )
	{
		fprintf(pipe,
	 		"%s^%s^%s^hourly_service_sale_total^%.2lf\n",
			entity_escape_full_name( full_name ),
			entity_escape_street_address( street_address ),
			sale_date_time,
			hourly_service_sale_total );
	}

	fprintf(pipe,
	 	"%s^%s^%s^gross_revenue^%.2lf\n",
		entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		sale_date_time,
		sale_gross_revenue );

	if ( inventory_sale_boolean
	||   specific_inventory_sale_boolean )
	{
		fprintf(pipe,
	 		"%s^%s^%s^sales_tax^%.2lf\n",
			entity_escape_full_name( full_name ),
			entity_escape_street_address( street_address ),
			sale_date_time,
			sale_sales_tax );
	}

	fprintf(pipe,
	 	"%s^%s^%s^invoice_amount^%.2lf\n",
		entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		sale_date_time,
		sale_invoice_amount );

	fprintf(pipe,
	 	"%s^%s^%s^payment_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		sale_date_time,
		customer_payment_total );

	fprintf(pipe,
	 	"%s^%s^%s^amount_due^%.2lf\n",
		entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		sale_date_time,
		sale_amount_due );

	fprintf(pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
		entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		sale_date_time,
		/* ------------------------------------------- */
		/* Returns component of sale_transaction or "" */
		/* ------------------------------------------- */
		sale_update_transaction_date_time(
			sale_transaction ) );

	pclose( pipe );
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

	if ( !end_work_date_time ) return 0.0;

	if ( ! ( earlier_date =
			date_19new(
				begin_work_date_time ) ) )
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

	if ( ! ( later_date =
			date_19new(
				end_work_date_time ) ) )
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

