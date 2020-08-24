/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_sale.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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
#include "customer_payment.h"
#include "customer_sale.h"

CUSTOMER_SALE *customer_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	CUSTOMER_SALE *customer_sale;

	if ( ! ( customer_sale = calloc( 1, sizeof( CUSTOMER_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	customer_sale->customer_entity =
		entity_new(
			full_name,
			street_address );

	customer_sale->sale_date_time = sale_date_time;
	
	return customer_sale;
}

CUSTOMER_SALE *customer_sale_fetch(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];

	if ( !full_name || !street_address )
	{
		return (CUSTOMER_SALE *)0;
	}

	if ( !sale_date_time || !*sale_date_time )
	{
		return (CUSTOMER_SALE *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 customer_sale_select(),
		 "customer_sale",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 customer_sale_primary_where(
			full_name,
			street_address,
			sale_date_time ) );

	return customer_sale_parse( pipe2string( sys_string ) );
}

char *customer_sale_select( void )
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

CUSTOMER_SALE *customer_sale_parse( char *input )
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

	if ( !input ) return (CUSTOMER_SALE *)0;

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

	return customer_sale_steady_state(
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
			entity_self_sales_tax_rate(),
			atof( sales_tax ) /* sales_tax_database */,
			strdup( completed_date_time ),
			account_receivable( (char *)0 ),
			account_revenue( (char *)0 ),
			account_shipping_revenue( (char *)0 ),
			account_sales_tax_payable( (char *)0 ) );
}

char *customer_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "sale_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 sale_date_time );

	return strdup( where );
}

double customer_sale_sales_tax(
			double 
			double entity_state_sales_tax_rate )
{
	return customer_inventory_sale_total(
			customer_inventory_sale_list ) *
		entity_state_sales_tax_rate;
}

double customer_inventory_sale_total(
			LIST *customer_inventory_sale_list )
{
	double sale_total;
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( customer_inventory_sale_list ) ) return 0.0;

	sale_total = 0.0;

	do {
		inventory_sale =
			list_get(
				customer_inventory_sale_list );

		sale_total +=
			inventory_sale->
				inventory_sale_extended_price;

	} while ( list_next( customer_inventory_sale_list ) );

	return sale_total;
}

TRANSACTION *customer_sale_transaction(
			char *full_name,
			char *street_address,
			char *sale_date_time,
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

	transaction =
		transaction_new(
			full_name,
			street_address,
			sale_date_time
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

	if ( shipping_revenue )
	{
		/* Credit shipping revenue */
		/* ----------------------- */
		journal =
			journal_new(
				full_name,
				street_address,
				transaction->transaction_date_time,
				account_shipping_revenue );
	
		journal->credit_amount = shipping_revenue;
		list_set( transaction->journal_list, journal );
	}

	if ( sales_tax_amount )
	{
		/* Credit sales tax payable */
		/* ------------------------ */
		journal =
			journal_new(
				full_name,
				street_address,
				transaction->transaction_date_time,
				account_sales_tax_payable );
	
		journal->credit_amount = sales_tax_amount;
		list_set( transaction->journal_list, journal );
	}

	return transaction;
}

FILE *customer_sale_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key = "full_name,street_address,sale_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 CUSTOMER_SALE_TABLE,
		 key );

	return fopen( sys_string, "w" );
}

void customer_sale_update(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total,
			double gross_revenue,
			double sale_tax,
			double invoice_amount,
			double payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	FILE *update_pipe;

	update_pipe = customer_sale_update_open();

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

LIST *customer_sale_payment_list(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	return customer_payment_list(
			full_name,
			street_address,
			sale_date_time );
}

double customer_sale_gross_revenue(
			double inventory_sale_total,
			double fixed_service_sale_total,
			double hourly_service_sale_total )
{
	return 	inventory_sale_total +
		fixed_service_sale_total +
		hourly_service_sale_total;
}

double customer_sale_invoice_amount(
			double gross_revenue,
			double sales_tax,
			double shipping_charge )
{
	return	gross_revenue +
		sales_tax +
		shipping_charge;
}

CUSTOMER_SALE *customer_sale_steady_state(
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
			char *account_sales_tax_payable );
{
	/* Instantiate a new customer_sale */
	/* ------------------------------- */
	CUSTOMER_SALE *customer_sale =
		customer_sale_new(
			full_name,
			street_address,
			sale_date_time );

	/* Set the input parameters */
	/* ------------------------ */
	customer_sale->inventory_sale_list = inventory_sale_list;
	customer_sale->fixed_service_sale_list = fixed_service_sale_list;
	customer_sale->hourly_service_sale_list = hourly_service_sale_list;
	customer_sale->customer_payment_list = customer_payment_list;
	customer_sale->shipping_charge = shipping_charge;
	customer_sale->entity_self_sales_tax_rate = entity_self_sales_tax_rate;
	customer_sale->sales_tax_database = sales_tax_database;
	customer_sale->completed_date_time = completed_date_time;

	customer_sale->transaction_date_time_database =
		transaction_date_time_database;

	customer_sale->account_receivable = account_receivable;
	customer_sale->account_revenue = account_revenue;
	customer_sale->account_shipping_revenue = account_shipping_revenue;
	customer_sale->account_sales_tax_payable = account_sales_tax_payable;

	/* Calculate the totals */
	/* -------------------- */
	customer_sale->inventory_sale_total =
		inventory_sale_total(
			customer_sale->inventory_sale_list );

	customer_sale->fixed_service_sale_total =
		fixed_service_sale_total(
			customer_sale->fixed_service_sale_list );

	customer_sale->hourly_service_sale_total =
		hourly_service_sale_total(
			customer_sale->hourly_service_sale_list );

	customer_sale->customer_payment_total =
		customer_payment_total(
			customer_sale->customer_payment_list );

	/* Calculate the new values */
	/* ------------------------ */
	customer_sale->customer_sale_gross_revenue =
		customer_sale_gross_revenue(
			customer_sale->inventory_sale_total,
			customer_sale->fixed_service_sale_total,
			customer_sale->hourly_service_sale_total );

	customer_sale->customer_sale_sales_tax =
		customer_sale_sales_tax(
			customer_sale->inventory_sale_total,
			customer_sale->entity_self_sales_tax_rate );

	customer_sale->customer_sale_invoice_amount =
		customer_sale_invoice_amount(
			customer_sale->customer_sale_gross_revenue,
			customer_sale->customer_sale_sales_tax,
			customer_sale->shipping_charge );

	customer_sale->customer_sale_amount_due =
		Customer_sale_amount_due(
			customer_sale->customer_sale_invoice_amount,
			customer_sale->customer_payment_total );

	/* Generate the transaction */
	/* ------------------------ */
	if ( completed_date_time && *completed_date_time )
	{
		customer_sale->customer_sale_transaction =
			customer_sale_transaction(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time,
				customer_sale->customer_sale_invoice_amount,
				customer_sale->customer_sale_gross_revenue,
				customer_sale->customer_sale_sales_tax,
				customer_sale->shipping_charge,
				customer_sale->account_receivable,
				customer_sale->account_revenue,
				customer_sale->account_shipping_revenue,
				customer_sale->account_sales_tax_payable );
	}
	return customer_sale;
}

