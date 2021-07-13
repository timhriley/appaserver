/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_inventory_purchase.c			*/
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
void post_change_inventory_purchase_inventory_name_update(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *preupdate_inventory_name,
				char *application_name );

void post_change_inventory_purchase_missing_quantity_update(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_arrived_quantity_update(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				enum preupdate_change_state
					arrived_quantity_change_state,
				char *application_name );

void post_change_inventory_purchase_ordered_quantity_update(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_unit_cost_update(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				char *preupdate_ordered_quantity,
				char *preupdate_arrived_quantity,
				char *preupdate_missing_quantity,
				char *preupdate_unit_cost,
				char *preupdate_inventory_name  );

void post_change_inventory_purchase_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name );

void post_change_inventory_purchase_insert_FOB_shipping(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_insert_FOB_destination(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_insert_title_passage_rule_null(
				PURCHASE_ORDER *purchase_order,
				char *inventory_name,
				char *application_name );

void post_change_inventory_purchase_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	char *state;
	char *preupdate_ordered_quantity;
	char *preupdate_arrived_quantity;
	char *preupdate_missing_quantity;
	char *preupdate_unit_cost;
	char *preupdate_inventory_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address purchase_date_time inventory_name state preupdate_ordered_quantity preupdate_arrived_quantity preupdate_missing_quantity preupdate_unit_cost preupdate_inventory_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_ordered_quantity = argv[ 7 ];
	preupdate_arrived_quantity = argv[ 8 ];
	preupdate_missing_quantity = argv[ 9 ];
	preupdate_unit_cost = argv[ 10 ];
	preupdate_inventory_name = argv[ 11 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	/* ------------------------------------------------------------ */
	/* INVENTORY_PURCHASE.transaction_date_time doesn't exist,	*/
	/* so execute after delete.					*/
	/* ------------------------------------------------------------ */
	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_inventory_purchase_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_purchase_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name );
	}
	else
	{
		post_change_inventory_purchase_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name,
			preupdate_ordered_quantity,
			preupdate_arrived_quantity,
			preupdate_missing_quantity,
			preupdate_unit_cost,
			preupdate_inventory_name );
	}

	return 0;

} /* main() */

void post_change_inventory_purchase_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name )
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

	if ( purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_inventory_purchase_insert_FOB_shipping(
			purchase_order,
			inventory_name,
			application_name );
	}
	else
	if ( purchase_order->title_passage_rule == FOB_destination )
	{
		post_change_inventory_purchase_insert_FOB_destination(
			purchase_order,
			inventory_name,
			application_name );
	}
	else
	{
		post_change_inventory_purchase_insert_title_passage_rule_null(
			purchase_order,
			inventory_name,
			application_name );
	}

} /* post_change_inventory_purchase_insert() */

void post_change_inventory_purchase_insert_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	if ( purchase_order->arrived_date_time )
	{
		INVENTORY_PURCHASE *inventory_purchase;

		inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name );

		if ( !inventory_purchase->arrived_quantity )
		{
			inventory_purchase->arrived_quantity =
				inventory_purchase->ordered_quantity;

/*
			inventory_purchase->quantity_on_hand =
				inventory_purchase_get_quantity_on_hand(
				   inventory_purchase->arrived_quantity,
				   inventory_purchase->missing_quantity,
				   inventory_purchase_get_returned_quantity(
					inventory_purchase->
					     inventory_purchase_return_list ) );
*/

			inventory_purchase->quantity_on_hand =
				inventory_purchase_get_quantity_minus_returned(
					inventory_purchase->arrived_quantity,
					inventory_purchase->
					     inventory_purchase_return_list  ) -
				inventory_purchase->missing_quantity;

			/* Update everything with a database_ */
			/* ---------------------------------- */
			inventory_purchase_list_update(
					application_name,
					purchase_order->
						inventory_purchase_list );
		}
	}

	/* ---------------------------------------------------- */
	/* For setting shipped_date = arrived_date:		*/
	/* post_change_purchase_order_insert_FOB_shipping()	*/
	/* ---------------------------------------------------- */

	if ( purchase_order->arrived_date_time )
	{
		char sys_string[ 1024 ];
		boolean is_latest;

		is_latest =
			 purchase_inventory_is_latest(
				application_name,
				inventory_name,
				purchase_order->purchase_date_time );

		if ( is_latest )
		{
			sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
			 	application_name,
			 	purchase_order->full_name,
			 	purchase_order->street_address,
			 	purchase_order->purchase_date_time,
			 	inventory_name );
		}
		else
		{
			sprintf( sys_string,
	"propagate_inventory_sale_layers %s '' '' '' \"%s\" \"%s\" n",
		 		application_name,
		 		inventory_name,
				(purchase_order->transaction_date_time)
					? purchase_order->transaction_date_time
					: "" );
		}

		system( sys_string );
	}

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

} /* post_change_inventory_purchase_insert_FOB_shipping() */

void post_change_inventory_purchase_insert_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	if ( purchase_order->arrived_date_time )
	{
		INVENTORY_PURCHASE *inventory_purchase;
		char sys_string[ 1024 ];
		boolean is_latest;

		is_latest =
			 purchase_inventory_is_latest(
				application_name,
				inventory_name,
				purchase_order->purchase_date_time );

		inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name );

		if ( !inventory_purchase->arrived_quantity )
		{
			inventory_purchase->arrived_quantity =
				inventory_purchase->ordered_quantity;

/*
			inventory_purchase->quantity_on_hand =
				inventory_purchase_get_quantity_on_hand(
				   inventory_purchase->arrived_quantity,
				   inventory_purchase->missing_quantity,
				   inventory_purchase_get_returned_quantity(
					inventory_purchase->
					     inventory_purchase_return_list ) );
*/

			inventory_purchase->quantity_on_hand =
				inventory_purchase_get_quantity_minus_returned(
					inventory_purchase->arrived_quantity,
					inventory_purchase->
					     inventory_purchase_return_list  ) -
				inventory_purchase->missing_quantity;

			/* Update everything with a database_ */
			/* ---------------------------------- */
			inventory_purchase_list_update(
					application_name,
					purchase_order->
						inventory_purchase_list );
		}

		if ( is_latest )
		{
			sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
			 	application_name,
			 	purchase_order->full_name,
			 	purchase_order->street_address,
			 	purchase_order->purchase_date_time,
			 	inventory_name );
		}
		else
		{
			sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
		 		application_name,
		 		inventory_name,
				(purchase_order->transaction_date_time)
					? purchase_order->transaction_date_time
					: "" );
		}

		system( sys_string );
	}

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

} /* post_change_inventory_purchase_insert_FOB_destination() */

void post_change_inventory_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *preupdate_ordered_quantity,
			char *preupdate_arrived_quantity,
			char *preupdate_missing_quantity,
			char *preupdate_unit_cost,
			char *preupdate_inventory_name  )
{
	PURCHASE_ORDER *purchase_order;
	enum preupdate_change_state ordered_quantity_change_state;
	enum preupdate_change_state arrived_quantity_change_state;
	enum preupdate_change_state missing_quantity_change_state;
	enum preupdate_change_state unit_cost_change_state;
	enum preupdate_change_state inventory_name_change_state;

	ordered_quantity_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_ordered_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_ordered_quantity" );

	arrived_quantity_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_arrived_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_arrived_quantity" );

	missing_quantity_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_missing_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_missing_quantity" );

	unit_cost_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_unit_cost,
			(char *)0 /* postupdate_data */,
			"preupdate_unit_cost" );

	inventory_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_inventory_name,
			(char *)0 /* postupdate_data */,
			"preupdate_inventory_name" );

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

	if ( ordered_quantity_change_state == from_something_to_something_else
	||   ordered_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_ordered_quantity_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( arrived_quantity_change_state == from_something_to_something_else
	||   arrived_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_arrived_quantity_update(
			purchase_order,
			inventory_name,
			arrived_quantity_change_state,
			application_name );
	}

	if ( missing_quantity_change_state == from_something_to_something_else
	||   missing_quantity_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_missing_quantity_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( unit_cost_change_state == from_something_to_something_else
	||   unit_cost_change_state == from_null_to_something )
	{
		post_change_inventory_purchase_unit_cost_update(
			purchase_order,
			inventory_name,
			application_name );
	}

	if ( inventory_name_change_state == from_something_to_something_else )
	{
		post_change_inventory_purchase_inventory_name_update(
			purchase_order,
			inventory_name,
			preupdate_inventory_name,
			application_name );
	}

} /* post_change_inventory_purchase_update() */

void post_change_inventory_purchase_inventory_name_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *preupdate_inventory_name,
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		preupdate_inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	if ( purchase_order->transaction )
	{
		strcpy(sys_string,
"propagate_purchase_order_accounts ignored fund transaction_date_time" );

		system( sys_string );
	}

} /* post_change_inventory_purchase_inventory_name_update() */

void post_change_inventory_purchase_missing_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	char sys_string[ 1024 ];
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

/*
	inventory_purchase->quantity_on_hand =
			inventory_purchase_get_quantity_on_hand(
				   inventory_purchase->arrived_quantity,
				   inventory_purchase->missing_quantity,
				   inventory_purchase_get_returned_quantity(
					inventory_purchase->
					     inventory_purchase_return_list ) );
*/

	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->arrived_quantity,
			inventory_purchase->
				inventory_purchase_return_list  ) -
		inventory_purchase->missing_quantity;


	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
		application_name,
		purchase_order->
			inventory_purchase_list );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

	strcpy(sys_string,
"propagate_purchase_order_accounts ignored fund transaction_date_time" );

	system( sys_string );

} /* post_change_inventory_purchase_missing_quantity_update() */

void post_change_inventory_purchase_arrived_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			enum preupdate_change_state
				arrived_quantity_change_state,
			char *application_name )
{
	char sys_string[ 1024 ];

	if ( arrived_quantity_change_state == from_null_to_something
	&&   purchase_inventory_is_latest(
			application_name,
			inventory_name,
			purchase_order->purchase_date_time ) )
	{
		return;
	}

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );

	system( sys_string );

} /* post_change_inventory_purchase_arrived_quantity_update() */

void post_change_inventory_purchase_ordered_quantity_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

	inventory_purchase->extension =
		inventory_purchase_get_extension(
			inventory_purchase->ordered_quantity,
			inventory_purchase->unit_cost );

/*
	inventory_purchase->quantity_on_hand =
			inventory_purchase_get_quantity_on_hand(
				   inventory_purchase->arrived_quantity,
				   inventory_purchase->missing_quantity,
				   inventory_purchase_get_returned_quantity(
					inventory_purchase->
					     inventory_purchase_return_list ) );
*/
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->
				arrived_quantity,
			inventory_purchase->
				inventory_purchase_return_list  ) -
		inventory_purchase->missing_quantity;

	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
		application_name,
		purchase_order->
			inventory_purchase_list );

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_prepaid_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->prepaid_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );

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

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

} /* post_change_inventory_purchase_ordered_quantity_update() */

void post_change_inventory_purchase_unit_cost_update(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	inventory_purchase =
		inventory_purchase_list_seek(
			purchase_order->inventory_purchase_list,
			inventory_name );

	inventory_purchase->extension =
		inventory_purchase_get_extension(
			inventory_purchase->ordered_quantity,
			inventory_purchase->unit_cost );

	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
			application_name,
			purchase_order->
				inventory_purchase_list );

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_prepaid_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->prepaid_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );

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

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

} /* post_change_inventory_purchase_unit_cost_update() */

void post_change_inventory_purchase_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name )
{
	PURCHASE_ORDER *purchase_order;
	char sys_string[ 1024 ];

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" '' n",
	 		application_name,
	 		inventory_name );

		system( sys_string );

		exit( 0 );
	}

	purchase_order->purchase_amount =
		purchase_order_get_purchase_amount(
			&purchase_order->sum_inventory_extension,
			&purchase_order->sum_specific_inventory_extension,
			&purchase_order->sum_supply_extension,
			&purchase_order->sum_service_extension,
			&purchase_order->sum_fixed_asset_extension,
			&purchase_order->sum_prepaid_asset_extension,
			&purchase_order->sum_extension,
			purchase_order->inventory_purchase_list,
			purchase_order->specific_inventory_purchase_list,
			purchase_order->supply_purchase_list,
			purchase_order->service_purchase_list,
			purchase_order->fixed_asset_purchase_list,
			purchase_order->prepaid_asset_purchase_list,
			purchase_order->sales_tax,
			purchase_order->freight_in );

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
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( purchase_order->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->transaction_amount,
				purchase_order->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				purchase_order->
					transaction->
					journal_ledger_list );
		}
	}

	if ( purchase_order->arrived_date_time )
	{
		boolean is_latest;

		is_latest =
		 	purchase_inventory_is_latest(
				application_name,
				inventory_name,
				purchase_order->purchase_date_time );

		if ( is_latest )
		{
			sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
		 		application_name,
		 		purchase_order->full_name,
		 		purchase_order->street_address,
		 		purchase_order->purchase_date_time,
		 		inventory_name );
		}
		else
		{
			sprintf( sys_string,
	"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 			application_name,
	 			inventory_name,
				(purchase_order->transaction_date_time)
					? purchase_order->transaction_date_time
					: "" );
		}

		system( sys_string );
	}

} /* post_change_inventory_purchase_delete() */

void post_change_inventory_purchase_insert_title_passage_rule_null(
			PURCHASE_ORDER *purchase_order,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;
	char sys_string[ 1024 ];
	boolean is_latest;

	is_latest =
		 purchase_inventory_is_latest(
			application_name,
			inventory_name,
			purchase_order->purchase_date_time );

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek inventory_name = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( !inventory_purchase->arrived_quantity )
	{
		inventory_purchase->arrived_quantity =
			inventory_purchase->ordered_quantity;
	}

/*
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_on_hand(
				   inventory_purchase->arrived_quantity,
				   inventory_purchase->missing_quantity,
				   inventory_purchase_get_returned_quantity(
					inventory_purchase->
					     inventory_purchase_return_list ) );
*/
	inventory_purchase->quantity_on_hand =
		inventory_purchase_get_quantity_minus_returned(
			inventory_purchase->
				arrived_quantity,
			inventory_purchase->
				inventory_purchase_return_list  ) -
		inventory_purchase->missing_quantity;

	inventory_purchase->average_unit_cost =
		inventory_purchase->capitalized_unit_cost;

	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
		application_name,
		purchase_order->inventory_purchase_list );

	if ( purchase_order->transaction_date_time )
	{
		purchase_order->transaction =
			purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

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

	if ( is_latest )
	{
		sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" y",
	 		application_name,
	 		purchase_order->full_name,
	 		purchase_order->street_address,
	 		purchase_order->purchase_date_time,
	 		inventory_name );
	}
	else
	{
		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(purchase_order->transaction_date_time)
				? purchase_order->transaction_date_time
				: "" );
	}

	system( sys_string );

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

} /* post_change_inventory_purchase_insert_title_passage_rule_null() */

