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
#include "entity.h"
#include "customer_payment.h"
#include "journal.h"
#include "inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "entity.h"
#include "entity_self.h"
#include "account.h"
#include "work.h"
#include "customer_payment.h"
#include "sale.h"

SALE *sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	SALE *sale;

	if ( ! ( sale = calloc( 1, sizeof( SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sale->customer_entity =
		entity_new(
			full_name,
			street_address );

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

char *sale_select( void )
{
	return
	"full_name,"
	"street_address,"
	"sale_date_time,"
	"gross_revenue,"
	"sales_tax,"
	"shipping_charge,"
	"invoice_amount,"
	"total_payment,"
	"amount_due,"
	"completed_date_time,"
	"transaction_date_time";
}

/* Returns sale_steady_state() */
/* --------------------------- */
SALE *sale_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char gross_revenue[ 128 ];
	char sales_tax[ 128 ];
	char shipping_charge[ 128 ];
	char invoice_amount[ 128 ];
	char total_payment[ 128 ];
	char amount_due[ 128 ];
	char completed_date_time[ 128 ];
	char transaction_date_time[ 128 ];

	if ( !input ) return (SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( gross_revenue, SQL_DELIMITER, input, 3 );
	piece( sales_tax, SQL_DELIMITER, input, 4 );
	piece( shipping_charge, SQL_DELIMITER, input, 5 );
	piece( invoice_amount, SQL_DELIMITER, input, 6 );
	piece( total_payment, SQL_DELIMITER, input, 7 );
	piece( amount_due, SQL_DELIMITER, input, 8 );
	piece( completed_date_time, SQL_DELIMITER, input, 9 );
	piece( transaction_date_time, SQL_DELIMITER, input, 10 );

	return sale_steady_state(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			inventory_sale_list(
				full_name,
				street_address,
				sale_date_time ),
			fixed_service_sale_list(
				full_name,
				street_address,
				sale_date_time ),
			hourly_service_sale_list(
				full_name,
				street_address,
				sale_date_time ),
			customer_payment_list(
				full_name,
				street_address,
				sale_date_time ),
			atof( shipping_charge ),
			entity_self_state_sales_tax_rate(),
			atof( sales_tax )
				/* sales_tax_database */,
			strdup( completed_date_time ),
			strdup( transaction_date_time )
				/* transaction_date_time_database */,
			account_receivable( (char *)0 ),
			account_revenue( (char *)0 ),
			account_shipping_revenue( (char *)0 ),
			account_sales_tax_payable( (char *)0 ) );
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

double sale_sales_tax(
			double inventory_sale_total,
			double entity_state_sales_tax_rate )
{
	return	inventory_sale_total *
		entity_state_sales_tax_rate;
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
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total,
			double gross_revenue,
			double sales_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	FILE *update_pipe;

	update_pipe = sale_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^inventory_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		inventory_sale_total );

	fprintf(update_pipe,
	 	"%s^%s^%s^fixed_service_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		fixed_service_sale_total );

	fprintf(update_pipe,
	 	"%s^%s^%s^hourly_service_sale_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		hourly_service_sale_total );

	fprintf(update_pipe,
	 	"%s^%s^%s^gross_revenue^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		gross_revenue );

	fprintf(update_pipe,
	 	"%s^%s^%s^sales_tax^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		sales_tax );

	fprintf(update_pipe,
	 	"%s^%s^%s^invoice_amount^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		invoice_amount );

	fprintf(update_pipe,
	 	"%s^%s^%s^payment_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		payment_total );

	fprintf(update_pipe,
	 	"%s^%s^%s^amount_due^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		amount_due );

	fprintf(update_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

double sale_gross_revenue(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total )
{
	return 	inventory_sale_total +
		fixed_service_sale_total +
		hourly_service_sale_total;
}

double sale_invoice_amount(
			double gross_revenue,
			double sales_tax,
			double shipping_charge )
{
	return	gross_revenue +
		sales_tax +
		shipping_charge;
}

SALE *sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list,
			LIST *hourly_service_sale_list,
			LIST *customer_payment_list,
			double shipping_charge,
			double entity_self_sales_tax_rate,
			double sales_tax_database,
			char *completed_date_time,
			char *transaction_date_time_database,
			char *account_receivable,
			char *account_revenue,
			char *account_shipping_revenue,
			char *account_sales_tax_payable )
{
	/* Instantiate a new sale */
	/* ---------------------- */
	SALE *sale =
		sale_new(
			full_name,
			street_address,
			sale_date_time );

	/* Set the input parameters */
	/* ------------------------ */
	sale->inventory_sale_list = inventory_sale_list;
	sale->fixed_service_sale_list = fixed_service_sale_list;
	sale->hourly_service_sale_list = hourly_service_sale_list;
	sale->customer_payment_list = customer_payment_list;
	sale->shipping_charge = shipping_charge;
	sale->entity_self_sales_tax_rate = entity_self_sales_tax_rate;
	sale->sales_tax_database = sales_tax_database;
	sale->completed_date_time = completed_date_time;

	sale->transaction_date_time_database =
		transaction_date_time_database;

	sale->account_receivable = account_receivable;
	sale->account_revenue = account_revenue;
	sale->account_shipping_revenue = account_shipping_revenue;
	sale->account_sales_tax_payable = account_sales_tax_payable;

	/* Calculate the totals */
	/* -------------------- */
	sale->inventory_sale_total =
		inventory_sale_total(
			sale->inventory_sale_list );

	sale->fixed_service_sale_total =
		fixed_service_sale_total(
			sale->fixed_service_sale_list );

	sale->hourly_service_sale_total =
		hourly_service_sale_total(
			sale->hourly_service_sale_list );

	sale->customer_payment_total =
		customer_payment_total(
			sale->customer_payment_list );

	/* Calculate the new values */
	/* ------------------------ */
	sale->sale_gross_revenue =
		sale_gross_revenue(
			sale->inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total );

	sale->sales_tax =
		sale_sales_tax(
			sale->inventory_sale_total,
			sale->entity_self_sales_tax_rate );

	sale->sale_invoice_amount =
		sale_invoice_amount(
			sale->sale_gross_revenue,
			sale->sales_tax,
			sale->shipping_charge );

	sale->sale_amount_due =
		Sale_amount_due(
			sale->sale_invoice_amount,
			sale->customer_payment_total );

	/* Generate the transaction */
	/* ------------------------ */
	if ( completed_date_time && *completed_date_time )
	{
		if ( *transaction_date_time_database
		&&   strcmp(	completed_date_time,
				transaction_date_time_database ) != 0 )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: completed_date_time change is not yet implemented (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				sale->
					customer_entity->
					full_name,
				sale->sale_date_time );
			exit( 1 );
		}

		sale->sale_transaction =
			sale_transaction(
				sale->
					customer_entity->
					full_name,
				sale->
					customer_entity->
					street_address,
				sale->sale_date_time,
				sale->sale_invoice_amount,
				sale->sale_gross_revenue,
				sale->sales_tax,
				sale->shipping_charge,
				sale->account_receivable,
				sale->account_revenue,
				sale->account_shipping_revenue,
				sale->account_sales_tax_payable );
	}
	return sale;
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

