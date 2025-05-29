/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
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
		char *preupdate_uncollectible_date_time,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean )
{
	SALE *sale;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_uncollectible_date_time )
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
	
	return sale;
}

char *sale_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "select.sh \"%s\" %s \%s\" select",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 sale_select(),
		 SALE_TABLE_NAME,
		 where );

	return strdup( sys_string );
}

/* Returns sale_steady_state() */
/* --------------------------- */
SALE *sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	return sale_parse(
		pipe2string(
			sale_sys_string(
		 		/* --------------------- */
		 		/* Returns static memory */
		 		/* --------------------- */
		 		sale_primary_where(
					full_name,
					street_address,
					sale_date_time ) ) ) );
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

TRANSACTION *sale_transaction(
			char *full_name,
			char *street_address,
			char *completed_date_time,
			double invoice_amount,
			double gross_revenue,
			double sales_tax,
			double shipping_charge,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !invoice_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			full_name,
			street_address,
			completed_date_time
				/* transaction_date_time */ );

	transaction->transaction_amount = invoice_amount;

	transaction->journal_list = list_new();

	/* Debit receivable */
	/* ---------------- */
	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			account_receivable );

	journal->debit_amount = invoice_amount;
	list_set( transaction->journal_list, journal );

	/* Credit revenue */
	/* ------------- */
	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			account_revenue );

	journal->credit_amount = gross_revenue;
	list_set( transaction->journal_list, journal );

	if ( shipping_charge )
	{
		/* Credit shipping revenue */
		/* ----------------------- */
		journal =
			journal_new(
				full_name,
				street_address,
				transaction->transaction_date_time,
				account_shipping_revenue );
	
		journal->credit_amount = shipping_charge;
		list_set( transaction->journal_list, journal );
	}

	if ( sales_tax )
	{
		/* Credit sales tax payable */
		/* ------------------------ */
		journal =
			journal_new(
				full_name,
				street_address,
				transaction->transaction_date_time,
				account_sales_tax_payable );
	
		journal->credit_amount = sales_tax;
		list_set( transaction->journal_list, journal );
	}

	return transaction;
}

FILE *sale_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key = "full_name,street_address,sale_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 SALE_TABLE_NAME,
		 key );

	return fopen( sys_string, "w" );
}

void sale_update(
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		double hourly_service_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax,
		double sale_invoice_amount,
		double customer_payment_total,
		double sale_amount_due )
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


	fprintf(pipe,
	 	"%s^%s^%s^inventory_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		inventory_sale_total );

	fprintf(pipe,
	 	"%s^%s^%s^fixed_service_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		fixed_service_sale_total );

	fprintf(pipe,
	 	"%s^%s^%s^hourly_service_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		hourly_service_sale_total );

	fprintf(pipe,
	 	"%s^%s^%s^gross_revenue^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		gross_revenue );

	fprintf(pipe,
	 	"%s^%s^%s^sales_tax^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		sales_tax );

	fprintf(pipe,
	 	"%s^%s^%s^invoice_amount^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		invoice_amount );

	fprintf(pipe,
	 	"%s^%s^%s^payment_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		payment_total );

	fprintf(pipe,
	 	"%s^%s^%s^amount_due^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		amount_due );

	fprintf(pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

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

