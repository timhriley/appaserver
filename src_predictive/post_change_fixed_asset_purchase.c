/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_fixed_asset_purchase.c	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_error.h"
#include "fixed_asset_purchase.h"

void post_change_fixed_asset_purchase_insert_update(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase );

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_label;
	char *state;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s asset_name serial_label state\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	serial_label = argv[ 2 ];
	state = argv[ 3 ];

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( ! ( fixed_asset_purchase =
			fixed_asset_purchase_fetch(
				asset_name,
				serial_label,
				0 /* not fetch_last_depreciation */,
				0 /* not fetch_last_recovery */ ) ) )
	{
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_fixed_asset_purchase_insert_update(
			fixed_asset_purchase );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_fixed_asset_purchase_insert_update(
			fixed_asset_purchase );
	}

	return 0;
}

#ifdef NOT_DEFINED
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
#endif

void post_change_fixed_asset_purchase_insert_update(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase )
{
	if ( !fixed_asset_purchase )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fixed_asset_purchase is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fixed_asset_purchase->cost_basis =
		fixed_asset_purchase_cost_basis(
			fixed_asset_purchase->fixed_asset_cost );

	fixed_asset_purchase->tax_adjusted_basis =
		fixed_asset_purchase_tax_adjusted_basis(
			fixed_asset_purchase->fixed_asset_cost );

	fixed_asset_purchase_update( fixed_asset_purchase );
}

