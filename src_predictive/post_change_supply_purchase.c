/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_supply_purchase.c				*/
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
void post_change_supply_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_supply_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name );

void post_change_supply_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name,
			char *preupdate_supply_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *supply_name;
	char *state;
	char *preupdate_supply_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address purchase_date_time supply_name state preupdate_supply_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	supply_name = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_supply_name = argv[ 7 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_supply_purchase_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_supply_purchase_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			supply_name );
	}

	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_supply_purchase_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			supply_name,
			preupdate_supply_name);
	}

	return 0;

} /* main() */

void post_change_supply_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name,
			char *preupdate_supply_name )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_SUPPLY *purchase_supply;
	enum preupdate_change_state supply_name_change_state;

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

	if ( ( purchase_supply =
		purchase_supply_list_seek(
			purchase_order->supply_purchase_list,
			supply_name ) ) )
	{
		purchase_supply_update(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				supply_name,
				purchase_supply->extension
					/* set in purchase_supply_get_list() */,
				purchase_supply->database_extension );
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
		ledger_purchase_order_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->memo,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->
				fixed_asset_purchase_list,
			purchase_order->
				prepaid_asset_purchase_list,
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

	supply_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_supply_name,
			supply_name /* postupdate_data */,
			"preupdate_supply_name" );

	if ( supply_name_change_state == from_something_to_something_else )
	{
		char *supply_expense_account;

		if ( ! ( supply_expense_account =
				ledger_get_supply_expense_account(
					application_name,
					preupdate_supply_name ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get supply_expense_account for (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 preupdate_supply_name );
			exit( 1 );
		}

		ledger_propagate(
			application_name,
			purchase_order->
				transaction->
				transaction_date_time,
			supply_expense_account );
	}

} /* post_change_supply_purchase_update() */

void post_change_supply_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_SUPPLY *purchase_supply;

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

	if ( ( purchase_supply =
		purchase_supply_list_seek(
			purchase_order->supply_purchase_list,
			supply_name ) ) )
	{
		purchase_supply_update(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				supply_name,
				purchase_supply->extension
					/* set in purchase_supply_get_list() */,
				purchase_supply->database_extension );
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
		ledger_purchase_order_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->memo,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->
				fixed_asset_purchase_list,
			purchase_order->
				prepaid_asset_purchase_list,
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

} /* post_change_supply_purchase_insert() */

void post_change_supply_purchase_delete(
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
			 "ERROR In %s/%s()/%d: cannot find purchase_order.\n",
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
			ledger_purchase_order_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->sales_tax,
				purchase_order->freight_in,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->
					fixed_asset_purchase_list,
				purchase_order->
					prepaid_asset_purchase_list,
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
	}

} /* post_change_supply_purchase_delete() */

