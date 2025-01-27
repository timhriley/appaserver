/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_inventory_purchase_return.c */
/* ----------------------------------------------------------------------- */
/* 									   */
/* Freely available software: see Appaserver.org			   */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "inventory_purchase_return.h"
#include "entity.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

void post_change_inventory_purchase_return_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time );

void post_change_inventory_purchase_return_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time,
				char *preupdate_inventory_name );

void post_change_inventory_purchase_return_predelete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *return_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	char *return_date_time;
	char *state;
	char *preupdate_inventory_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address purchase_date_time inventory_name return_date_time state preupdate_inventory_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	purchase_date_time = argv[ 3 ];
	inventory_name = argv[ 4 ];
	return_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_inventory_name = argv[ 7 ];

	/* If executed out of context. */
	/* --------------------------- */
	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	/* ----------------------------------------------------------- */
	/* INVENTORY_PURCHASE_RETURN.transaction_date_time DOES exist, */
	/* so execute predelete.				       */
	/* ----------------------------------------------------------- */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_inventory_purchase_return_predelete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
	"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 	 	application_name,
	 	 	inventory_name,
		 	return_date_time );

		if ( system( sys_string ) ) {};
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_purchase_return_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time );
	}
	else
	{
		post_change_inventory_purchase_return_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			return_date_time,
			preupdate_inventory_name );
	}

	return 0;

} /* main() */

void post_change_inventory_purchase_return_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *return_date_time )
{
	PURCHASE_ORDER *purchase_order;
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_PURCHASE_RETURN *inventory_purchase_return;
	TRANSACTION *transaction;
	char sys_string[ 1024 ];

	purchase_order =
		purchase_order_fetch_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_purchase = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_purchase_return =
			inventory_purchase_return_list_seek(
				inventory_purchase->
					inventory_purchase_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_purchase_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( inventory_purchase_return->transaction )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: transaction exists for [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
	}

	transaction =
	inventory_purchase_return->transaction =
		inventory_purchase_return_build_transaction(
			&inventory_purchase_return->transaction_date_time,
			application_name,
			(char *)0 /* fund_name */,
			inventory_purchase->full_name,
			inventory_purchase->street_address,
			inventory_purchase->unit_cost,
			inventory_purchase->inventory_account_name,
			inventory_purchase_return->return_date_time,
			inventory_purchase_return->returned_quantity,
			inventory_purchase_return->sales_tax,
			purchase_get_sum_vendor_payment_amount(
				purchase_order->vendor_payment_list ) );

	inventory_purchase_return->transaction_date_time =
		inventory_purchase_return_journal_ledger_refresh(
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->journal_ledger_list );

	/* Update INVENTORY_PURCHASE_RETURN.transaction_date_time */
	/* ------------------------------------------------------ */
	inventory_purchase_return_update(
		application_name,
		inventory_purchase->full_name,
		inventory_purchase->street_address,
		purchase_date_time,
		inventory_name,
		return_date_time,
		transaction->transaction_date_time,
		(char *)0 /* database_transaction_date_time */ );

	/* Update INVENTORY_PURCHASE.quantity_on_hand */
	/* ------------------------------------------ */
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->arrived_quantity,
			inventory_purchase->inventory_purchase_return_list ) -
		inventory_purchase->missing_quantity,

	inventory_purchase_list_update(
		application_name,
		purchase_order->inventory_purchase_list );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	if ( system( sys_string ) ) {};

} /* post_change_inventory_purchase_return_insert() */

void post_change_inventory_purchase_return_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *return_date_time,
			char *preupdate_inventory_name )
{
	PURCHASE_ORDER *purchase_order;
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_PURCHASE_RETURN *inventory_purchase_return;
	TRANSACTION *transaction;
	char sys_string[ 1024 ];
	enum preupdate_change_state inventory_name_change_state;

	inventory_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_inventory_name,
			inventory_name /* postupdate_data */,
			"preupdate_inventory_name" );


	purchase_order =
		purchase_order_fetch_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_purchase = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_purchase_return =
			inventory_purchase_return_list_seek(
				inventory_purchase->
					inventory_purchase_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_purchase_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( !inventory_purchase_return->transaction )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: NO transaction exists for [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
	}

	transaction =
	inventory_purchase_return->transaction =
		inventory_purchase_return_build_transaction(
			&inventory_purchase_return->transaction_date_time,
			application_name,
			(char *)0 /* fund_name */,
			inventory_purchase->full_name,
			inventory_purchase->street_address,
			inventory_purchase->unit_cost,
			inventory_purchase->inventory_account_name,
			inventory_purchase_return->return_date_time,
			inventory_purchase_return->returned_quantity,
			inventory_purchase_return->sales_tax,
			purchase_get_sum_vendor_payment_amount(
				purchase_order->vendor_payment_list ) );

	inventory_purchase_return->transaction_date_time =
		inventory_purchase_return_journal_ledger_refresh(
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->journal_ledger_list );

	inventory_purchase_return_update(
		application_name,
		inventory_purchase->full_name,
		inventory_purchase->street_address,
		purchase_date_time,
		inventory_name,
		return_date_time,
		transaction->transaction_date_time,
		(char *)0 /* database_transaction_date_time */ );

	/* Update INVENTORY_PURCHASE.quantity_on_hand */
	/* ------------------------------------------ */
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->arrived_quantity,
			inventory_purchase->inventory_purchase_return_list ) -
		inventory_purchase->missing_quantity,

	inventory_purchase_list_update(
		application_name,
		purchase_order->inventory_purchase_list );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	if ( system( sys_string ) ) {};

	if ( inventory_name_change_state == from_something_to_something_else )
	{
		sprintf(sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		preupdate_inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

		if ( system( sys_string ) ) {};
	}

} /* post_change_inventory_purchase_return_update() */

void post_change_inventory_purchase_return_predelete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *return_date_time )
{
	PURCHASE_ORDER *purchase_order;
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_PURCHASE_RETURN *inventory_purchase_return;

	purchase_order =
		purchase_order_fetch_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_purchase = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_purchase_return =
			inventory_purchase_return_list_seek(
				inventory_purchase->
					inventory_purchase_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_purchase_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( !inventory_purchase_return->transaction )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: no transaction for [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			inventory_purchase_return->
				transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			inventory_purchase_return->
				transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		inventory_purchase_return->
			transaction->
			journal_ledger_list,
		application_name );

	if ( !inventory_purchase_return_list_delete(
		inventory_purchase->
			inventory_purchase_return_list,
		inventory_purchase_return->return_date_time ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot delete return_date_time = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 return_date_time );
		exit( 1 );
	}

	/* Update INVENTORY_PURCHASE.quantity_on_hand */
	/* ------------------------------------------ */
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->arrived_quantity,
			inventory_purchase->inventory_purchase_return_list ) -
		inventory_purchase->missing_quantity;

	inventory_purchase_list_update(
		application_name,
		purchase_order->inventory_purchase_list );

} /* post_change_inventory_purchase_predelete() */

