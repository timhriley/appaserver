/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/vendor_payment.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "purchase.h"
#include "vendor_payment.h"

VENDOR_PAYMENT *vendor_payment_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( ! ( vendor_payment = calloc( 1, sizeof( VENDOR_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	vendor_payment->vendor_entity =
		entity_new( full_name, street_address );

	vendor_payment->purchase_date_time = purchase_date_time;
	vendor_payment->payment_date_time = payment_date_time;
	return vendor_payment;
}

VENDOR_PAYMENT *vendor_payment_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time )
{
	char sys_string[ 1024 ];

	if ( !full_name
	||   !street_address
	||   !purchase_date_time
	||   !payment_date_time )
	{
		return (VENDOR_PAYMENT *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 vendor_payment_select(),
		 "vendor_payment",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 purchase_order_primary_where(
			full_name,
			street_address,
			purchase_date_time ) );

	return vendor_payment_parse( pipe2string( sys_string ) );
}

LIST *vendor_payment_list( char *purchase_order_where )
{
	return vendor_payment_list_fetch( purchase_order_where );
}

char *vendor_payment_select( void )
{
	return
	"full_name,"
	"street_address,"
	"purchase_date_time,"
	"payment_date_time,"
	"payment_amount,"
	"check_number,"
	"transaction_date_time";
}


VENDOR_PAYMENT *vendor_payment_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char payment_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	VENDOR_PAYMENT *vendor_payment;

	if ( !input ) return (VENDOR_PAYMENT *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( purchase_date_time, SQL_DELIMITER, input, 2 );
	piece( payment_date_time, SQL_DELIMITER, input, 3 );

	vendor_payment =
		vendor_payment_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ),
			strdup( payment_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	vendor_payment->payment_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	vendor_payment->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	vendor_payment->vendor_payment_transaction =
		transaction_fetch(
			vendor_payment->vendor_entity->full_name,
			vendor_payment->vendor_entity->street_address,
			piece_buffer /* transaction_date_time */ );

	return vendor_payment;
}

LIST *vendor_payment_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *vendor_payment_list;

	vendor_payment_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	vendor_payment_list, 
				vendor_payment_parse( input ) );
	}

	pclose( input_pipe );
	return vendor_payment_list;
}

LIST *vendor_payment_list_fetch( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (LIST *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 vendor_payment_select(),
		 "vendor_payment",
		 where,
		 vendor_payment_select() );

	return vendor_payment_system_list( sys_string );
}

double vendor_payment_total(
			LIST *vendor_payment_list )
{
	VENDOR_PAYMENT *vendor_payment;
	double total;

	if ( !list_rewind( vendor_payment_list ) ) return 0.0;

	total = 0.0;

	do {
		vendor_payment = list_get( vendor_payment_list );
		total += vendor_payment->payment_amount;

	} while ( list_next( vendor_payment_list ) );

	return total;
}

void vendor_payment_insert(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time,
			double payment_amount,
			int check_number,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field_list_string;
	FILE *output_pipe;
	char buffer[ 128 ];

	field_list_string =
"full_name,street_address,purchase_date_time,payment_date_time,payment_amount,check_number,transaction_date_time";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 "vendor_payment",
		 field_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s|%s|%s|%s|%.2lf",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 purchase_date_time,
		 payment_date_time,
		 payment_amount );

	if ( check_number )
	{
		fprintf( output_pipe,
			 "|%d",
			 check_number );
	}
	else
	{
		fprintf( output_pipe, "|" );
	}

	if ( transaction_date_time )
	{
		fprintf( output_pipe,
			 "|%s\n",
			 transaction_date_time );
	}
	else
	{
		fprintf( output_pipe, "|\n" );
	}

	pclose( output_pipe );
}

double vendor_payment_fetch_payment_amount_total(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "sum( payment_amount )",
		 "vendor_payment",
		 purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time ) );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
}

