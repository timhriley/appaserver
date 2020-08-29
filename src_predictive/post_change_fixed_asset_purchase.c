/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_fixed_asset_purchase.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "purchase.h"
#include "vendor_payment.h"
#include "account.h"
#include "equipment_purchase.h"
#include "transaction.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_equipment_purchase(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *serial_number;
	char *state;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address purchase_date_time asset_name serial_number state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	if ( ( purchase_date_time = argv[ 3 ] ) ){};
	if ( ( asset_name = argv[ 4 ] ) ){};
	if ( ( serial_number = argv[ 5 ] ) ){};
	if ( ( state = argv[ 6 ] ) ){};

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	post_change_equipment_purchase(
		full_name,
		street_address,
		purchase_date_time );

	return 0;
}

void post_change_equipment_purchase(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE *purchase;
	TRANSACTION *transaction;
	char *transaction_date_time = {0};

	if ( ! ( purchase =
			purchase_fetch(
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		return;
	}

	purchase->purchase_amount_due =
		Purchase_amount_due(
			purchase->purchase_invoice_amount,
			( purchase->purchase_vendor_payment_total =
				vendor_payment_total(
					purchase->
					     purchase_vendor_payment_list ) ) );

	if ( purchase->purchase_transaction )
	{
		/* Refresh the TRANSACTION */
		/* ----------------------- */
		purchase->purchase_transaction =
			/* ---------------------------------- */
			/* Includes transaction->journal_list */
			/* ---------------------------------- */
			purchase_transaction(
				purchase->vendor_entity->full_name,
				purchase->vendor_entity->street_address,
				purchase->arrived_date_time,
				purchase->purchase_invoice_amount,
				purchase_asset_account_name(
					purchase->
					     purchase_equipment_list ),
				account_payable( (char *)0 ) );

		transaction = purchase->purchase_transaction;

		transaction_date_time =
		transaction->transaction_date_time =
			transaction_journal_refresh(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->journal_list );
	}

	purchase_update(
		purchase->purchase_equipment_total,
		purchase->purchase_invoice_amount,
		purchase->purchase_vendor_payment_total,
		purchase->purchase_amount_due,
		transaction_date_time,
		purchase->vendor_entity->full_name,
		purchase->vendor_entity->street_address,
		purchase->purchase_date_time );
}

