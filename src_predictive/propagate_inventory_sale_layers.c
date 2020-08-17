/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/propagate_inventory_sale_layers.c	*/
/* -----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

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
void propagate_inventory_sale_layers_not_latest(
				INVENTORY *inventory,
				LIST *customer_sale_list,
				char *application_name,
				enum inventory_cost_method,
				LIST *inventory_list );

double propagate_inventory_sale_layers_latest(
				INVENTORY_SALE *inventory_sale,
				INVENTORY *inventory,
				char *application_name,
				enum inventory_cost_method );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *inventory_name;
	char *propagate_transaction_date_time;
	boolean is_latest;
	INVENTORY_SALE *inventory_sale;
	double cost_of_goods_sold = 0.0;
	ENTITY_SELF *entity_self;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address sale_date_time inventory_name propagate_transaction_date_time is_latest_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	if ( ( propagate_transaction_date_time = argv[ 6 ] ) );
	is_latest = (*argv[ 7 ] == 'y');

	if ( is_latest )
	{
		INVENTORY *inventory;

		entity_self = entity_self_load(	application_name );

		inventory_sale =
			inventory_sale_fetch(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				inventory_name );

		inventory =
			inventory_load_new(
				application_name,
				inventory_name );

		inventory->inventory_purchase_list =
			inventory_get_latest_inventory_purchase_list(
				application_name,
				inventory_name );

		cost_of_goods_sold =
			propagate_inventory_sale_layers_latest(
				inventory_sale,
				inventory,
				application_name,
				entity_self->inventory_cost_method );
	}
	else
	{
		entity_self =
			entity_self_inventory_load(
				application_name,
				inventory_name );

		propagate_inventory_sale_layers_not_latest(
			entity_self->sale_inventory,
			entity_self->customer_sale_list,
			application_name,
			entity_self->inventory_cost_method,
			entity_self->inventory_list );

		if ( *full_name )
		{
			if ( ! ( inventory_sale =
					inventory_sale_fetch(
						application_name,
						full_name,
						street_address,
						sale_date_time,
						inventory_name ) ) )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot inventory_sale_fetch(%s)\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	inventory_name );
				exit( 1 );
			}

			cost_of_goods_sold =
				inventory_sale->
					cost_of_goods_sold;
		}
	}

	if ( *full_name )
	{
		printf( "%.2lf\n", cost_of_goods_sold );
	}

	return 0;

} /* main() */

double propagate_inventory_sale_layers_latest(
				INVENTORY_SALE *inventory_sale,
				INVENTORY *inventory,
				char *application_name,
				enum inventory_cost_method
					inventory_cost_method )
{
	if ( !inventory_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: inventory_sale is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	inventory_sale->extension =
		inventory_sale_get_extension(
				inventory_sale->retail_price,
				inventory_sale->sold_quantity,
				inventory_sale->discount_amount );

	inventory_sale->
		cost_of_goods_sold =
		   inventory_sale_get_cost_of_goods_sold(
			/* -------------------------------------- */
			/* Decrements inventory->quantity_on_hand */
			/* -------------------------------------- */
			inventory->inventory_purchase_list,
			&inventory->last_inventory_balance->total_cost_balance,
			&inventory->last_inventory_balance->quantity_on_hand,
			inventory->last_inventory_balance->average_unit_cost,
			inventory_sale->sold_quantity,
			inventory_cost_method,
			inventory_sale->completed_date_time );
/*
	inventory_sale->
		cost_of_goods_sold =
		inventory_sale_get_average_cost_of_goods_sold(
			&inventory->last_inventory_balance->total_cost_balance,
			&inventory->last_inventory_balance->quantity_on_hand,
			inventory->last_inventory_balance->average_unit_cost,
			inventory_sale->sold_quantity );

	inventory_purchase_fifo_decrement_quantity_on_hand(
		inventory->inventory_purchase_list,
		inventory_sale->sold_quantity );
*/

	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
		application_name,
		inventory->inventory_purchase_list );

	inventory_last_inventory_balance_update(
		inventory->last_inventory_balance->quantity_on_hand,
		inventory->last_inventory_balance->average_unit_cost,
		inventory->last_inventory_balance->total_cost_balance,
		inventory->inventory_name,
		application_name );

	inventory_sale_update(
		application_name,
		inventory_sale->full_name,
		inventory_sale->street_address,
		inventory_sale->sale_date_time,
		inventory->inventory_name,
		inventory_sale->extension,
		inventory_sale->database_extension,
		inventory_sale->cost_of_goods_sold,
		inventory_sale->database_cost_of_goods_sold );

	return inventory_sale->cost_of_goods_sold;

} /* propagate_inventory_sale_layers_latest() */

void propagate_inventory_sale_layers_not_latest(
				INVENTORY *inventory,
				LIST *customer_sale_list,
				char *application_name,
				enum inventory_cost_method
					inventory_cost_method,
				LIST *inventory_list )
{
	inventory->inventory_balance_list =
		inventory_get_average_cost_inventory_balance_list(
			inventory->inventory_purchase_list,
			inventory->inventory_sale_list );

	if ( list_length( inventory->inventory_balance_list ) )
	{
		inventory->last_inventory_balance =
			list_get_last_pointer(
				inventory->inventory_balance_list );
	}
	else
	{
		inventory->last_inventory_balance =
			inventory_balance_new();
	}

	inventory_last_inventory_balance_update(
		inventory->last_inventory_balance->quantity_on_hand,
		inventory->last_inventory_balance->average_unit_cost,
		inventory->last_inventory_balance->total_cost_balance,
		inventory->inventory_name,
		application_name );

	if ( inventory_cost_method == inventory_fifo )
	{
		inventory_purchase_list_reset_quantity_on_hand(
			inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_CGS_fifo(
			inventory->inventory_sale_list,
			inventory->inventory_purchase_list );
	}
	else
	if ( inventory_cost_method == inventory_lifo )
	{
		inventory_purchase_list_reset_quantity_on_hand(
			inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_CGS_lifo(
			inventory->inventory_sale_list,
			inventory->inventory_purchase_list );
	}
	else
	{
		inventory_purchase_list_reset_quantity_on_hand(
			inventory->inventory_purchase_list );

		inventory_set_quantity_on_hand_fifo(
			inventory->inventory_sale_list,
			inventory->inventory_purchase_list );
	}

	/* Update everything with a database_ */
	/* ---------------------------------- */
	inventory_purchase_list_update(
		application_name,
		inventory->inventory_purchase_list );

	inventory_sale_list_cost_of_goods_sold_update(
		application_name,
		inventory->inventory_sale_list,
		inventory->inventory_name );

	/* ------------------------------------------------------------ */
	/* Set customer_sale->inventory_cost->cost_of_goods_sold =	*/
	/* sum(inventory_sale)						*/
	/* ------------------------------------------------------------ */
	customer_sale_list_cost_of_goods_sold_set(
		customer_sale_list,
		inventory_list );

	customer_sale_list_cost_of_goods_sold_transaction_update(
		application_name,
		customer_sale_list );

} /* propagate_inventory_sale_layers_not_latest() */

