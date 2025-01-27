/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_inventory_sale_return.c	*/
/* -------------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "inventory_sale_return.h"
#include "entity.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

void post_change_inventory_sale_return_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time );

void post_change_inventory_sale_return_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time,
				char *preupdate_inventory_name );

void post_change_inventory_sale_return_predelete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
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
"Usage: %s full_name street_address sale_date_time inventory_name return_date_time state preupdate_inventory_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	inventory_name = argv[ 4 ];
	return_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_inventory_name = argv[ 7 ];

	/* If executed out of context. */
	/* --------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 )
		exit( 0 );

	/* ----------------------------------------------------------- */
	/* INVENTORY_SALE_RETURN.transaction_date_time DOES exist, */
	/* so execute predelete.				       */
	/* ----------------------------------------------------------- */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_inventory_sale_return_predelete(
			application_name,
			full_name,
			street_address,
			sale_date_time,
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
		post_change_inventory_sale_return_insert(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			return_date_time );
	}
	else
	{
		post_change_inventory_sale_return_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			return_date_time,
			preupdate_inventory_name );
	}

	return 0;

} /* main() */

void post_change_inventory_sale_return_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *return_date_time )
{
	CUSTOMER_SALE *customer_sale;
	INVENTORY_SALE *inventory_sale;
	INVENTORY_SALE_RETURN *inventory_sale_return;
	TRANSACTION *transaction;
	char sys_string[ 1024 ];

	customer_sale =
		customer_sale_fetch_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_sale = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_sale_return =
			inventory_sale_return_list_seek(
				inventory_sale->
					inventory_sale_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_sale_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( inventory_sale_return->transaction )
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
	inventory_sale_return->transaction =
		inventory_sale_return_build_transaction(
			&inventory_sale_return->transaction_date_time,
			application_name,
			(char *)0 /* fund_name */,
			inventory_sale->full_name,
			inventory_sale->street_address,
			inventory_sale->retail_price,
			inventory_sale->discount_amount,
			inventory_sale->inventory_account_name,
			inventory_sale_return->return_date_time,
			inventory_sale_return->returned_quantity,
			inventory_sale->sold_quantity,
			customer_get_sum_payment_amount(
				customer_sale->payment_list ) );

	inventory_sale_return->transaction_date_time =
		inventory_sale_return_journal_ledger_refresh(
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->journal_ledger_list );

	/* Update INVENTORY_SALE_RETURN.transaction_date_time */
	/* ------------------------------------------------------ */
	inventory_sale_return_update(
		application_name,
		inventory_sale->full_name,
		inventory_sale->street_address,
		sale_date_time,
		inventory_name,
		return_date_time,
		transaction->transaction_date_time,
		(char *)0 /* database_transaction_date_time */ );

	/* Update INVENTORY_SALE.cost_of_goods_sold */
	/* ---------------------------------------- */
/*
	inventory_sale->quantity_on_hand =
		inventory_sale_get_quantity_minus_returned(
			inventory_sale->arrived_quantity,
			inventory_sale->inventory_sale_return_list ) -
		inventory_sale->missing_quantity,
*/

	inventory_sale_list_update(
		customer_sale->inventory_sale_list,
		application_name );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(customer_sale->transaction_date_time)
				? customer_sale->transaction_date_time
				: "" );

	if ( system( sys_string ) ) {};

} /* post_change_inventory_sale_return_insert() */

void post_change_inventory_sale_return_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *return_date_time,
			char *preupdate_inventory_name )
{
	CUSTOMER_SALE *customer_sale;
	INVENTORY_SALE *inventory_sale;
	INVENTORY_SALE_RETURN *inventory_sale_return;
	TRANSACTION *transaction;
	char sys_string[ 1024 ];
	enum preupdate_change_state inventory_name_change_state;

	inventory_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_inventory_name,
			inventory_name /* postupdate_data */,
			"preupdate_inventory_name" );


	customer_sale =
		customer_sale_fetch_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_sale = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_sale_return =
			inventory_sale_return_list_seek(
				inventory_sale->
					inventory_sale_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_sale_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( !inventory_sale_return->transaction )
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
	inventory_sale_return->transaction =
		inventory_sale_return_build_transaction(
			&inventory_sale_return->transaction_date_time,
			application_name,
			(char *)0 /* fund_name */,
			inventory_sale->full_name,
			inventory_sale->street_address,
			inventory_sale->retail_price,
			inventory_sale->discount_amount,
			inventory_sale->inventory_account_name,
			inventory_sale_return->return_date_time,
			inventory_sale_return->returned_quantity,
			inventory_sale->sold_quantity,
			customer_get_sum_payment_amount(
				customer_sale->payment_list ) );

	inventory_sale_return->transaction_date_time =
		inventory_sale_return_journal_ledger_refresh(
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->journal_ledger_list );

	inventory_sale_return_update(
		application_name,
		inventory_sale->full_name,
		inventory_sale->street_address,
		sale_date_time,
		inventory_name,
		return_date_time,
		transaction->transaction_date_time,
		(char *)0 /* database_transaction_date_time */ );

	/* Update INVENTORY_SALE.quantity_on_hand */
	/* ------------------------------------------ */
/*
	inventory_sale->quantity_on_hand =
		inventory_sale_get_quantity_minus_returned(
			inventory_sale->arrived_quantity,
			inventory_sale->inventory_sale_return_list ) -
		inventory_sale->missing_quantity,
*/

	inventory_sale_list_update(
		customer_sale->inventory_sale_list,
		application_name );

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		inventory_name,
			(customer_sale->transaction_date_time)
				? customer_sale->transaction_date_time
				: "" );

	if ( system( sys_string ) ) {};

	if ( inventory_name_change_state == from_something_to_something_else )
	{
		sprintf(sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" \"%s\" n",
	 		application_name,
	 		preupdate_inventory_name,
			(customer_sale->transaction_date_time)
				? customer_sale->transaction_date_time
				: "" );

		if ( system( sys_string ) ) {};
	}

} /* post_change_inventory_sale_return_update() */

void post_change_inventory_sale_return_predelete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *return_date_time )
{
	CUSTOMER_SALE *customer_sale;
	INVENTORY_SALE *inventory_sale;
	INVENTORY_SALE_RETURN *inventory_sale_return;

	customer_sale =
		customer_sale_fetch_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot seek inventory_sale = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	if ( ! ( inventory_sale_return =
			inventory_sale_return_list_seek(
				inventory_sale->
					inventory_sale_return_list,
				return_date_time ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot seek inventory_sale_return = [%s/%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 return_date_time );
		exit( 1 );
	}

	if ( !inventory_sale_return->transaction )
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
			inventory_sale_return->
				transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			inventory_sale_return->
				transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		inventory_sale_return->
			transaction->
			journal_ledger_list,
		application_name );

	if ( !inventory_sale_return_list_delete(
		inventory_sale->
			inventory_sale_return_list,
		inventory_sale_return->return_date_time ) )
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
/*
	inventory_sale->quantity_on_hand =
		inventory_sale_get_quantity_minus_returned(
			inventory_sale->arrived_quantity,
			inventory_sale->inventory_sale_return_list ) -
		inventory_sale->missing_quantity;
*/

	inventory_sale_list_update(
		customer_sale->inventory_sale_list,
		application_name );

} /* post_change_inventory_sale_predelete() */

