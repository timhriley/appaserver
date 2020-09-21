/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"
#include "sale.h"
#include "journal.h"
#include "customer_payment.h"

CUSTOMER_PAYMENT *customer_payment_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( ! ( customer_payment = calloc( 1, sizeof( CUSTOMER_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	customer_payment->customer_entity =
		entity_new(
			full_name,
			street_address );

	customer_payment->sale_date_time = sale_date_time;
	customer_payment->payment_date_time = payment_date_time;
	
	return customer_payment;
}

LIST *customer_payment_list(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	return customer_payment_system_list(
		customer_payment_sys_string(
			sale_primary_where(
				full_name,
				street_address,
				sale_date_time ) ) );
}

char *customer_payment_select( void )
{
	return
		"first_name,"
		"street_address,"
		"sale_date_time,"
		"payment_date_time,"
		"payment_amount,"
		"check_number,"
		"transaction_date_time";
}

CUSTOMER_PAYMENT *customer_payment_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char payment_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	CUSTOMER_PAYMENT *customer_payment;

	if ( !input || !*input ) return (CUSTOMER_PAYMENT *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( payment_date_time, SQL_DELIMITER, input, 3 );

	customer_payment =
		customer_payment_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			strdup( payment_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	customer_payment->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	customer_payment->customer_payment_transaction =
		transaction_fetch(
			customer_payment->customer_entity->full_name,
			customer_payment->customer_entity->street_address,
			piece_buffer /* transaction_date_time */ );

	return customer_payment;
}

char *customer_payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "select.sh '%s' %s \"%s\" select",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 customer_payment_select(),
		 "customer_payment",
		 where );

	return strdup( sys_string );
}

LIST *customer_payment_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *payment_list;

	payment_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( payment_list, customer_payment_parse( input ) );
	}

	pclose( input_pipe );
	return payment_list;
}

double customer_payment_total(
			LIST *customer_payment_list )
{
	CUSTOMER_PAYMENT *customer_payment;
	double total;

	if ( !list_rewind( customer_payment_list ) ) return 0.0;

	total = 0.0;
	do {
		customer_payment = list_get( customer_payment_list );
		total += customer_payment->payment_amount;

	} while ( list_next( customer_payment_list ) );

	return total;
}

TRANSACTION *customer_payment_transaction(
			char *full_name,
			char *street_address,
			char *payment_date_time,
			double payment_amount,
			char *account_cash,
			char *account_receivable )
{
	TRANSACTION *transaction;

	if ( !payment_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			full_name,
			street_address,
			payment_date_time
				/* transaction_date_time */ );

	transaction->transaction_amount = payment_amount;

	transaction->journal_list =
		journal_binary_journal_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			account_cash
				/* debit_account */,
			account_receivable
				/* credit_account */ );

	return transaction;
}

