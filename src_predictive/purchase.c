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
#include "list.h"
#include "boolean.h"
#include "journal.h"
#include "transaction.h"
#include "entity.h"
#include "vendor_payment.h"
#include "equipment_purchase.h"
#include "purchase.h"

PURCHASE *purchase_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 /* ---------------------- */
		 /* returns program memory */
		 /* ---------------------- */
		 purchase_select(),
		 PURCHASE_TABLE_NAME,
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time ) );

	return purchase_parse( pipe2string( sys_string ) );
}

PURCHASE *purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE *purchase;

	purchase = purchase_calloc();

	purchase->vendor_entity =
		entity_new(
			full_name,
			street_address ),

	purchase->purchase_date_time = purchase_date_time;

	return purchase;
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

PURCHASE *purchase_calloc( void )
{
	PURCHASE *purchase;

	if ( ! ( purchase = calloc( 1, sizeof( PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return purchase;
}

PURCHASE *purchase_seek(
			LIST *purchase_list,
			char *purchase_date_time )
{
	PURCHASE *purchase;

	if ( !list_rewind( purchase_list ) ) return (PURCHASE *)0;

	do {
		purchase = list_get( purchase_list );

		if ( strcmp(	purchase->purchase_date_time,
				purchase_date_time ) == 0 )
		{
			return purchase;
		}

	} while( list_next( purchase_list ) );
	return (PURCHASE *)0;
}

LIST *purchase_vendor_payment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return vendor_payment_fetch_list(
			purchase_primary_where(
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
			purchase_primary_where(
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
	transaction->memo = PURCHASE_MEMO;

	transaction->journal_list =
		purchase_journal_list(
			purchase->purchase_invoice_amount,
			asset_account_name,
			account_payable );

	return transaction;
}

void purchase_update(
			double purchase_price_total,
			double invoice_amount,
			double vendor_payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return purchase_update(
			purchase_price_total,
			invoice_amount,
			vendor_payment_total,
			amount_due,
			transaction_date_time,
			full_name,
			street_address,
			purchase_date_time );
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

char *purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *purchase_transaction_memo,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	return transaction_journal_refresh(
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		purchase_transaction_memo,
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

PURCHASE *purchase_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	PURCHASE *purchase;

	if ( !input ) return (PURCHASE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( purchase_date_time, SQL_DELIMITER, input, 2 );

	purchase =
		purchase_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	purchase->sales_tax = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	purchase->freight_in = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	purchase->arrived_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	purchase->purchase_equipment_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	purchase->purchase_invoice_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	purchase->purchase_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );

	if ( *piece_buffer )
	{
		purchase->purchase_transaction =
			transaction_fetch(
				purchase->vendor_entity->full_name,
				purchase->vendor_entity->street_address,
				piece_buffer /* transaction_date_time */ );
	}

	purchase->purchase_equipment_list =
		purchase_equipment_list(
			purchase->vendor_entity->full_name,
			purchase->vendor_entity->street_address,
			purchase->purchase_date_time );

	purchase->purchase_vendor_payment_list =
		purchase_vendor_payment_list(
			purchase->vendor_entity->full_name,
			purchase->vendor_entity->street_address,
			purchase->purchase_date_time );

	return purchase;
}

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_primary_where(
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

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_primary_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time );
}

double purchase_fetch_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "amount_due",
		 PURCHASE_TABLE_NAME,
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

LIST *purchase_journal_list(
			double purchase_invoice_amount,
			char *purchase_asset_account_name(
			char *account_payable )
{
	JOURNAL *journal;
	LIST *journal_list = list_new();

	/* Set debit */
	/* --------- */
	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			purchase_asset_account_name );

	journal->debit_amount = purchase_invoice_amount;

	list_set( journal_list, journal );

	/* Set credit */
	/* ---------- */
	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			account_payable );

	journal->credit_amount = purchase_invoice_amount;

	list_set( journal_list, journal );

	return journal_list;
}

TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable )
{
	if ( ! ( transaction =
			transaction_new(
				full_name,
				street_address,
				arrived_date_time
					/* transaction_date_time */ ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: transaction_new(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 arrived_date_time );
		exit( 1 );
	}

	transaction->transaction_amount = purchase_invoice_amount;

	transaction->journal_list =
		purchase_journal_list(
			purchase_invoice_amount,
			purchase_asset_account_name,
			account_payable );

	return transaction;
}

