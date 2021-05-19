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
#include "account.h"
#include "transaction.h"
#include "fixed_asset_purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_fixed_asset_purchase_insert(
			PURCHASE *purchase );

/*
void post_change_fixed_asset_purchase_update(
			PURCHASE *purchase,
			char *preupdate_asset_name,
			char *preupdate_serial_label );

void post_change_fixed_asset_purchase_delete(
			char *asset_name,
			char *serial_label );
*/

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_label;
	char *state;
	char *preupdate_asset_name;
	char *preupdate_serial_label;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s asset_name serial_label state preupdate_asset_name preupdate_serial_label\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	serial_label = argv[ 2 ];
	state = argv[ 3 ];
	preupdate_asset_name = argv[ 4 ];
	preupdate_serial_label = argv[ 5 ];

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		if ( ( fixed_asset_purchase =
			fixed_asset_purchase_fetch(
				asset_name,
				serial_label ) ) )
		{
			if ( ( purchase =
				purchase_fetch(
					fixed_asset_purchase->
						full_name,
					fixed_asset_purchase->
						street_address,
					fixed_asset_purchase->
						purchase_date_time ) ) )
			{
				post_change_fixed_asset_purchase_insert(
					purchase );
			}
		}
	}
/*
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_fixed_asset_purchase_update(
			asset_name,
			serial_label,
			preupdate_asset_name,
			preupdate_serial_label );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_fixed_asset_purchase_delete(
			asset_name,
			serial_label );
	}
*/

	return 0;
}

void post_change_fixed_asset_purchase_insert(
			PURCHASE *purchase )
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
				transaction->lock_transaction,
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

