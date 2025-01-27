/* ---------------------------------------------------------------	*/
/* src_predictive/propagate_inventory_purchase_layers.c			*/
/* ---------------------------------------------------------------	*/
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
#include "purchase.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
/*
void propagate_inventory_purchase_layers_not_latest(
				INVENTORY *purchase_inventory,
				LIST *customer_sale_list,
				char *application_name,
				enum inventory_cost_method );
*/

void propagate_inventory_purchase_layers_latest(
				INVENTORY_PURCHASE *inventory_purchase,
				INVENTORY *inventory,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *inventory_name;
	boolean is_latest;
	PURCHASE_ORDER *purchase_order;
	INVENTORY *inventory;
	INVENTORY_PURCHASE *inventory_purchase;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address purchase_date_time inventory_name is_latest_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	is_latest = (*argv[ 6 ] == 'y');

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot load purchase order for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 purchase_date_time );
		exit( 1 );
	}

/*
	inventory_purchase =
		inventory_purchase_fetch(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			inventory_name );
*/

	if ( ! ( inventory_purchase =
			inventory_purchase_list_seek(
				purchase_order->inventory_purchase_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek inventory purchase for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	inventory = inventory_load_new(
			application_name,
			inventory_name );

	propagate_inventory_purchase_layers_latest(
		inventory_purchase,
		inventory,
		application_name );

	if ( !is_latest )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		"propagate_inventory_sale_layers %s '' '' '' \"%s\" '' n",
			 application_name,
			 inventory_name );

		system( sys_string );
	}

	return 0;

} /* main() */

void propagate_inventory_purchase_layers_latest(
				INVENTORY_PURCHASE *inventory_purchase,
				INVENTORY *inventory,
				char *application_name )
{
	if ( !inventory_purchase )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: inventory_purchase is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !inventory_purchase->arrived_quantity )
	{
		inventory_purchase->arrived_quantity =
			inventory_purchase->ordered_quantity;
	}

	inventory_purchase->quantity_on_hand =
		inventory_purchase->arrived_quantity;

	inventory_purchase->extension =
		inventory_purchase_get_extension(
				inventory_purchase->ordered_quantity,
				inventory_purchase->unit_cost );

	inventory->last_inventory_balance->total_cost_balance =
		inventory_purchase_get_total_cost_balance(
			&inventory->last_inventory_balance->quantity_on_hand,
			&inventory->last_inventory_balance->average_unit_cost,
			inventory->last_inventory_balance->total_cost_balance,
			inventory_purchase->capitalized_unit_cost,
			inventory_purchase->ordered_quantity );

	inventory_last_inventory_balance_update(
		inventory->last_inventory_balance->quantity_on_hand,
		inventory->last_inventory_balance->average_unit_cost,
		inventory->last_inventory_balance->total_cost_balance,
		inventory->inventory_name,
		application_name );

	inventory_purchase_update(
		application_name,
		inventory->inventory_name,
		inventory_purchase->full_name,
		inventory_purchase->street_address,
		inventory_purchase->purchase_date_time,
		inventory_purchase->arrived_quantity,
		inventory_purchase->database_arrived_quantity,
		inventory_purchase->quantity_on_hand,
		inventory_purchase->database_quantity_on_hand,
		inventory_purchase->extension,
		inventory_purchase->database_extension,
		inventory_purchase->capitalized_unit_cost,
		inventory_purchase->database_capitalized_unit_cost,
		inventory_purchase->average_unit_cost,
		inventory_purchase->database_average_unit_cost );

} /* propagate_inventory_purchase_layers_latest() */

