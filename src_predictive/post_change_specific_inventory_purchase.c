/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_specific_inventory_purchase.c		*/
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
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "inventory.h"
#include "entity.h"
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean get_delete_full_name(
			char **full_name,
			char **street_address,
			char **purchase_date_time,
			char *inventory_name,
			char *serial_number,
			char *application_name );

boolean delete_specific_inventory_purchase(
			LIST *specific_inventory_purchase_list,
			char *inventory_name,
			char *serial_number );

void post_change_specific_inventory_purchase_delete(
			char *application_name,
			char *inventory_name,
			char *serial_number );

void post_change_specific_inventory_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_specific_inventory_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	char *serial_number;
	char *state;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address purchase_date_time inventory_name serial_number state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	serial_number = argv[ 6 ];
	state = argv[ 7 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_specific_inventory_purchase_delete(
			application_name,
			inventory_name,
			serial_number );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_specific_inventory_purchase_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}

	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_specific_inventory_purchase_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}

	return 0;

} /* main() */

void post_change_specific_inventory_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	purchase_order =
		purchase_order_new(
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

	purchase_order_update(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->sum_extension,
			purchase_order->database_sum_extension,
			purchase_order->purchase_amount,
			purchase_order->database_purchase_amount,
			purchase_order->amount_due,
			purchase_order->database_amount_due,
			purchase_order->transaction_date_time,
			purchase_order->database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

	if ( !purchase_order->transaction_date_time ) return;

	purchase_order->transaction =
	     ledger_specific_inventory_build_transaction(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->memo,
		purchase_order->
			specific_inventory_purchase_list,
		purchase_order->fund_name );

	ledger_transaction_refresh(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->transaction_amount,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */,
		purchase_order->transaction->journal_ledger_list );

} /* post_change_specific_inventory_purchase_update() */

void post_change_specific_inventory_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	purchase_order =
		purchase_order_new(
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

	purchase_order_update(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->sum_extension,
			purchase_order->database_sum_extension,
			purchase_order->purchase_amount,
			purchase_order->database_purchase_amount,
			purchase_order->amount_due,
			purchase_order->database_amount_due,
			purchase_order->transaction_date_time,
			purchase_order->database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

	if ( !purchase_order->transaction_date_time ) return;

	purchase_order->transaction =
	     ledger_specific_inventory_build_transaction(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->memo,
		purchase_order->
			specific_inventory_purchase_list,
		purchase_order->fund_name );

	ledger_transaction_refresh(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->transaction_amount,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */,
		purchase_order->transaction->journal_ledger_list );

} /* post_change_specific_inventory_purchase_insert() */

void post_change_specific_inventory_purchase_delete(
			char *application_name,
			char *inventory_name,
			char *serial_number )
{
	PURCHASE_ORDER *purchase_order;
	char *full_name = {0};
	char *street_address = {0};
	char *purchase_date_time = {0};

	if ( !get_delete_full_name(
		&full_name,
		&street_address,
		&purchase_date_time,
		inventory_name,
		serial_number,
		application_name ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get_delete_full_name().\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot find purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !delete_specific_inventory_purchase(
			purchase_order->specific_inventory_purchase_list,
			inventory_name,
			serial_number ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot delete purchase of (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 serial_number );
		exit( 1 );
	}

	purchase_order_update(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->sum_extension,
			purchase_order->database_sum_extension,
			purchase_order->purchase_amount,
			purchase_order->database_purchase_amount,
			purchase_order->amount_due,
			purchase_order->database_amount_due,
			purchase_order->transaction_date_time,
			purchase_order->
				database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->
				database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
		     ledger_specific_inventory_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO,
			purchase_order->
				specific_inventory_purchase_list,
			purchase_order->fund_name );

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );

		/* Doesn't refresh all the accounts. */
		/* --------------------------------- */
		ledger_propagate(
			application_name,
			purchase_order->
				transaction->
				transaction_date_time,
			cash_account );

		ledger_propagate(
			application_name,
			purchase_order->
				transaction->
				transaction_date_time,
			uncleared_checks_account );

		ledger_propagate(
			application_name,
			purchase_order->
				transaction->
				transaction_date_time,
			account_payable_account );

	}

} /* post_change_specific_inventory_purchase_delete() */

boolean get_delete_full_name(
			char **full_name,
			char **street_address,
			char **purchase_date_time,
			char *inventory_name,
			char *serial_number,
			char *application_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;
	char piece_buffer[ 256 ];
	char inventory_name_buffer[ 128 ];
	char *results;

	select = "full_name,street_address,purchase_date_time";
	folder = "specific_inventory_purchase";

	sprintf( where,
		 "inventory_name = '%s' and serial_number = '%s'",
		 escape_character(
			inventory_name_buffer,
			inventory_name,
			'\'' ),
		 serial_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
		return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*purchase_date_time = strdup( piece_buffer );

	return 1;

} /* get_delete_full_name() */


boolean delete_specific_inventory_purchase(
			LIST *specific_inventory_purchase_list,
			char *inventory_name,
			char *serial_number )
{
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;

	if ( !list_rewind( specific_inventory_purchase_list ) ) return 0;

	do {
		purchase_specific_inventory =
			list_get( specific_inventory_purchase_list );

		if ( strcmp(	purchase_specific_inventory->inventory_name,
				inventory_name ) == 0
		&&   strcmp(	purchase_specific_inventory->serial_number,
				serial_number ) == 0 )
		{
			list_delete_current(
				specific_inventory_purchase_list );
			return 1;
		}

	} while( list_next( specific_inventory_purchase_list ) );

	return 0;

} /* delete_specific_inventory_purchase() */

