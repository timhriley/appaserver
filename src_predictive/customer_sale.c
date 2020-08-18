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
	"shipping_revenue,"
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
	char piece_buffer[ 1024 ];
	CUSTOMER_SALE *customer_sale;

	if ( !input ) return (CUSTOMER_SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );

	customer_sale =
		customer_sale_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	customer_sale->customer_sale_extended_price_total =
		atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	customer_sale->customer_sale_sales_tax = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	customer_sale->shipping_revenue = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	customer_sale->customer_sale_invoice_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	customer_sale->customer_sale_payment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	customer_sale->customer_sale_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	customer_sale->completed_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	customer_sale->customer_sale_transaction =
		transaction_fetch(
			customer_sale->customer_entity->full_name,
			customer_sale->customer_entity->street_address,
			piece_buffer /* transaction_date_time */ );

	return customer_sale;
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

double customer_sale_calculate_sales_tax(
			LIST *customer_inventory_sale_list,
			double entity_state_sales_tax_rate )
{
	return customer_inventory_sale_total(
			customer_inventory_sale_list ) *
		entity_state_sales_tax_rate;
}

double customer_inventory_sale_total(
			LIST *customer_inventory_sale_list )
{
	return 0.0;
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

double customer_sale_payment_total(
			LIST *customer_payment_list )
{
	return customer_payment_total( customer_payment_list );
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

double customer_sale_total_payment(
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
		 "echo \"select %s from %s where %s;\" | sql",
		 "sum( payment_amount )",
		 "customer_payment",
		 where );

	results = pipe2string( sys_string );

	if ( !results )
		return 0.0;
	else
		return atof( results );
}

