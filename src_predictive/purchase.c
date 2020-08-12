/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "folder.h"
#include "environ.h"
#include "list.h"
#include "boolean.h"
#include "vendor_payment.h"
#include "equipment_purchase.h"
#include "purchase.h"

PURCHASE_ORDER *purchase_order_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and 			"
		 "purchase_date_time = '%s'			",
		 transaction_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 purchase_select(),
		 "purchase_order",
		 where );

	return purchase_parse( pipe2string( sys_string ) );
}

PURCHASE_ORDER *purchase_order_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	purchase_order = purchase_order_calloc();

	purchase_order->vendor_entity =
		entity_new(
			full_name,
			street_address ),

	purchase_order->purchase_date_time = purchase_date_time;

	return purchase_order;
}

double purchase_equipment_total(
			LIST *purchase_equipment_list )
{
	return equipment_purchase_total( purchase_equipment_list );
}

double purchase_vendor_payment_total(
			LIST *vendor_payment_list )
{
	return vendor_payment_total( vendor_payment_list );
}

PURCHASE_ORDER *purchase_order_calloc( void )
{
	PURCHASE_ORDER *purchase_order;

	if ( ! ( purchase_order = calloc( 1, sizeof( PURCHASE_ORDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return purchase_order;
}

PURCHASE_ORDER *purchase_order_seek(
			LIST *purchase_order_list,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	if ( !list_rewind( purchase_order_list ) ) return (PURCHASE_ORDER *)0;

	do {
		purchase_order = list_get( purchase_order_list );

		if ( strcmp(	purchase_order->purchase_date_time,
				purchase_date_time ) == 0 )
		{
			return purchase_order;
		}

	} while( list_next( purchase_order_list ) );
	return (PURCHASE_ORDER *)0;
}

LIST *purchase_vendor_payment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return vendor_payment_list(
			purchase_order_primary_where(
				full_name,
				street_address,
				purchase_date_time ) );
}

LIST *purchase_equipment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return equipment_purchase_list(
			purchase_order_primary_where(
				full_name,
				street_address,
				purchase_date_time ) );
}

TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double invoice_amount,
			char *asset_account_name,
			char *account_payable )
{
	TRANSACTION *transaction;

	transaction =
		transaction_new(
			full_name,
			street_address,
			arrived_date_time
				/* transaction_date_time */ );

	transaction->transaction_amount = invoice_amount;

	transaction->journal_list =
		transaction_binary_journal_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			asset_account_name
				/* debit_account */,
			account_payable
				/* credit_account */ );

	return transaction;
}

void purchase_update(
			double equipment_purchase_total,
			double invoice_amount,
			double vendor_payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;
	char *key;

	key = "full_name,street_address,purchase_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "transaction",
		 key );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
		"%s^%s^%s^equipment_purchase_total^%.2lf\n",
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		equipment_purchase_total );

	fprintf(update_pipe,
		"%s^%s^%s^invoice_amount^%.2lf\n",
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		invoice_amount );

	fprintf(update_pipe,
		"%s^%s^%s^vendor_payment_total^%.2lf\n",
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		vendor_payment_total );

	fprintf(update_pipe,
		"%s^%s^%s^amount_due^%.2lf\n",
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		amount_due );

	fprintf(update_pipe,
		"%s^%s^%s^transaction_date_time^%s\n",
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		transaction_date_time );

	pclose( update_pipe );
}

void purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	transaction_refresh(
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		PURCHASE_ORDER_MEMO,
		0 /* check_number */,
		1 /* lock_transaction */,
		journal_list );
}

/* Returns program memory */
/* ---------------------- */
char *purchase_select( void )
{
	return
		"full_name,"
		"street_address,"
		"purchase_date_time,"
		"sales_tax,"
		"freight_in,"
		"arrived_date_time,"
		"purchase_equipment_total,"
		"invoice_amount,"
		"vendor_payment_total,"
		"amount_due,"
		"transaction_date_time";
}

PURCHASE_ORDER *purchase_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	PURCHASE_ORDER *purchase_order;

	if ( !input ) return (PURCHASE_ORDER *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( purchase_date_time, SQL_DELIMITER, input, 2 );

	purchase_order =
		purchase_order_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	purchase_order->sales_tax = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	purchase_order->freight_in = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	purchase_order->arrived_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	purchase_order->purchase_equipment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	purchase_order->invoice_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	purchase_order->purchase_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );

	if ( *piece_buffer )
	{
		purchase_order->purchase_transaction =
			transaction_fetch(
				purchase_order->vendor_entity->full_name,
				purchase_order->vendor_entity->street_address,
				piece_buffer /* transaction_date_time */ );
	}

	purchase_order->purchase_equipment_list =
		purchase_equipment_list(
			purchase_order->vendor_entity->full_name,
			purchase_order->vendor_entity->street_address,
			purchase_order->purchase_date_time );

	purchase_order->purchase_vendor_payment_list =
		purchase_vendor_payment_list(
			purchase_order->vendor_entity->full_name,
			purchase_order->vendor_entity->street_address,
			purchase_order->purchase_date_time );

	return purchase_order;
}

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_order_primary_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "purchase_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time );

	return strdup( where );
}

