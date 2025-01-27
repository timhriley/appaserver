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
#include "account.h"
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
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	vendor_payment->vendor_entity =
		entity_fetch(
			full_name,
			street_address );

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
	if ( !full_name
	||   !street_address
	||   !purchase_date_time
	||   !payment_date_time )
	{
		return (VENDOR_PAYMENT *)0;
	}

	return vendor_payment_parse(
			pipe2string(
				vendor_payment_system_string(
		 			vendor_payment_purchase_where(
						full_name,
						street_address,
						purchase_date_time,
						payment_date_time ) ) ) );
}

LIST *vendor_payment_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	if ( !full_name
	||   !street_address
	||   !purchase_date_time )
	{
		return (LIST *)0;
	}

	return vendor_payment_system_list(
			vendor_payment_system_string(
		 	/* -------------------------- */
		 	/* Safely returns heap memory */
		 	/* -------------------------- */
		 	purchase_primary_where(
				full_name,
				street_address,
				purchase_date_time ) ) );
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
			vendor_payment->
				vendor_entity->
				full_name,
			vendor_payment->
				vendor_entity->
				street_address,
			piece_buffer /* transaction_date_time */ );

	return vendor_payment;
}

LIST *vendor_payment_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *vendor_payment_list;

	vendor_payment_list = list_new();
	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	vendor_payment_list, 
				vendor_payment_parse( input ) );
	}

	pclose( input_pipe );
	return vendor_payment_list;
}

char *vendor_payment_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( system_string,
		 "select.sh '%s' %s \"%s\" %s",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 vendor_payment_select(),
		 "vendor_payment",
		 where,
		 "payment_date_time" );

	return strdup( system_string );
}

double vendor_payment_total( LIST *vendor_payment_list )
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
	char system_string[ 1024 ];
	char *field_list_string;
	FILE *output_pipe;

	field_list_string =
"full_name,street_address,purchase_date_time,payment_date_time,payment_amount,check_number,transaction_date_time";

	sprintf( system_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 "vendor_payment",
		 field_list_string );

	output_pipe = popen( system_string, "w" );

	fprintf( output_pipe,
		 "%s|%s|%s|%s|%.2lf",
		 entity_escape_full_name( full_name ),
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

	fprintf( output_pipe,
		 "|%s\n",
		 (transaction_date_time)
			? transaction_date_time
			: "" );
		
	pclose( output_pipe );
}

VENDOR_PAYMENT *vendor_payment_seek(
			LIST *vendor_payment_list,
			char *payment_date_time )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return (VENDOR_PAYMENT *)0;

	do {
		vendor_payment = list_get( vendor_payment_list );

		if ( timlib_strcmp(	vendor_payment->payment_date_time,
					payment_date_time ) == 0 )
		{
			return vendor_payment;
		}

	} while( list_next( vendor_payment_list ) );

	return (VENDOR_PAYMENT *)0;
}

TRANSACTION *vendor_payment_transaction(
			char *full_name,
			char *street_address,
			char *payment_date_time,
			double payment_amount,
			int check_number,
			char *account_payable,
			char *account_cash,
			char *account_uncleared_checks )
{
	TRANSACTION *transaction;

	if ( !payment_amount ) return (TRANSACTION *)0;

	if ( ! ( transaction =
			transaction_new(
				full_name,
				street_address,
				payment_date_time
					/* transaction_date_time */ ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: transaction_new(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 payment_date_time );
		exit( 1 );
	}

	transaction->transaction_amount = payment_amount;

	transaction->journal_list =
		vendor_payment_journal_list(
			transaction->transaction_amount,
			check_number,
			account_payable,
			account_cash,
			account_uncleared_checks );

	return transaction;
}

LIST *vendor_payment_journal_list(
			double transaction_amount,
			int check_number,
			char *account_payable,
			char *account_cash,
			char *account_uncleared_checks )
{
	JOURNAL *journal;
	char *credit_account;
	LIST *journal_list = list_new();

	/* Set debit */
	/* --------- */
	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			account_payable );

	journal->debit_amount = transaction_amount;

	list_set( journal_list, journal );

	/* Set credit */
	/* ---------- */
	if ( check_number )
		credit_account = account_uncleared_checks;
	else
		credit_account = account_cash;

	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			credit_account );

	journal->credit_amount = transaction_amount;

	list_set( journal_list, journal );

	return journal_list;
}

FILE *vendor_payment_update_open( void )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"purchase_date_time,"
		"payment_date_time";

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "vendor_payment",
		 key );

	return popen( system_string, "w" );
}

void vendor_payment_update(
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time )
{
	FILE *update_pipe;

	update_pipe = vendor_payment_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

/* Safely returns heap memory */
/* -------------------------- */
char *vendor_payment_purchase_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "purchase_date_time = '%s' and	"
		 "payment_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time,
		 payment_date_time );

	return strdup( where );
}

