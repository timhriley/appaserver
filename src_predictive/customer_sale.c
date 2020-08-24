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
#include "entity.h"
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
	"extended_price_total,"
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
	char extended_price_total[ 128 ];
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
	piece( extended_price_total, SQL_DELIMITER, input, 3 );
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
			strdup( completed_date_time ),
			atof( shipping_charge ),
			atof( sales_tax ),
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
			account_receivable( (char *)0 ),
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
		 "sale_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_escape_full_name( full_name ),
		 street_address,
		 sale_date_time );

	return strdup( where );
}

double customer_sale_sales_tax(
			LIST *inventory_sale_list,
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
			double sales_tax_amount,
			char *account_receivable,
			char *account_revenue,
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
	/* -------------- */
	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			account_revenue );

	journal->debit_amount = invoice_amount - sales_tax_amount;
	list_set( transaction->journal_list, journal );

	/* Credit sales tax payable */
	/* ------------------------ */
	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			account_sales_tax_payable );

	journal->debit_amount = sales_tax_amount;
	list_set( transaction->journal_list, journal );

	return transaction;
}

char *customer_sale_completed_date_time(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char entity_buffer[ 128 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "sale_date_time = '%s'",
		 escape_character(	entity_buffer,
					full_name,
					'\'' ),
		 street_address,
		 sale_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "completed_date_time",
		 "customer_sale",
		 where );

	return pipe2string( sys_string );
}

double customer_sale_fetch_sales_tax(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *results;

	sprintf( where,
		 "%s and sale_date_time = '%s'",
		 entity_primary_where(
			full_name,
			street_address ),
		 sale_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"| sql",
		 "sales_tax",
		 "customer_sale",
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	return atof( results );
}

double customer_sale_payment_total(
			LIST *customer_payment_list )
{
	CUSTOMER_PAYMENT *customer_payment;
	double payment_total;

	if ( !list_rewind( customer_payment_list ) ) return 0.0;

	payment_total = 0.0;

	do {
		customer_payment =
			list_get(
				customer_payment_list );

		payment_total += customer_payment->payment_amount;

	} while ( list_next( customer_payment_list ) );

	return payment_total;
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
			double extended_price_total,
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

	update_pipe = customer_sale_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^extended_price_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		sale_date_time,
		extended_price_total );

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

double customer_sale_extended_price_total(
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list )
{
	return	customer_sale_inventory_sale_total(
			inventory_sale_list );
		customer_sale_fixed_service_sale_total(
			fixed_service_sale_list );
}

double customer_sale_invoice_amount(
			double extended_price_total,
			double sales_tax,
			double shipping_charge )
{
	return	extended_price_total +
		sales_tax +
		shipping_charge;
}

CUSTOMER_SALE *customer_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *completed_date_time,
			double shipping_charge,
			double sales_tax_amount,
			double entity_self_sales_tax_rate,
			LIST *inventory_sale_list,
			LIST *fixed_service_sale_list,
			LIST *hourly_service_sale_list,
			LIST *customer_payment_list,
			char *account_receivable,
			char *account_shipping_revenue,
			char *account_sales_tax_payable )
{
	CUSTOMER_SALE *customer_sale =
		customer_sale_new(
			full_name,
			street_address,
			sale_date_time );

	customer_sale->inventory_sale_list = inventory_sale_list;
	customer_sale->fixed_service_sale_list = fixed_service_sale_list;
	customer_sale->hourly_service_sale_list = hourly_service_sale_list;
	customer_sale->customer_payment_list = customer_payment_list;

	customer_sale->customer_sale_extended_price_total =
		customer_sale_extended_price_total(
			customer_sale->inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list );

	customer_sale->customer_sale_sales_tax =
		customer_sale_sales_tax(
			customer_sale->inventory_sale_list,
			entity_self_sales_tax_rate );

	customer_sale->customer_sale_invoice_amount =
		customer_sale_invoice_amount(
			customer_sale->customer_sale_extended_price_total,
			customer_sale->customer_sale_sales_tax,
			shipping_charge );

	customer_sale->customer_sale_amount_due =
		Customer_sale_amount_due(
			customer_sale->customer_sale_invoice_amount,
			customer_payment_total(
				customer_payment_list ) );


	customer_sale->transaction_date_time_database =
		transaction_date_time;

	customer_sale->sales_tax_amount_database =
		sales_tax_amount;

	if ( completed_date_time && *completed_date_time )
	{
		customer_sale->customer_sale_transaction =
			customer_sale_transaction(
				full_name,
				street_address,
				sale_date_time,
				customer_sale->customer_sale_invoice_amount,
				customer_sale->customer_sale_sales_tax,
				shipping_charge,
				account_receivable,
				account_shipping_revenue,
				account_sales_tax_payable );
	}

	return customer_sale;
}

