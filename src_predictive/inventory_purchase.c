/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "entity.h"
#include "inventory_purchase.h"

INVENTORY_PURCHASE *inventory_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name )
{
	INVENTORY_PURCHASE *h =
		(INVENTORY_PURCHASE *)
			calloc( 1, sizeof( INVENTORY_PURCHASE ) );
	if ( !h )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	h->vendor_entity =
		entity_new(
			full_name,
			street_address );

	h->purchase_date_time = purchase_date_time;
	h->inventory_name = inventory_name;
	return h;
}

double inventory_purchase_total(
			LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double total;

	if ( !list_rewind( inventory_purchase_list ) ) return 0.0;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		total += inventory_purchase->extended_cost;

	} while ( list_next( inventory_purchase_list ) );

	return total;
}

#ifdef NOT_DEFINED
FILE *inventory_purchase_update_pipe(
			void )
{
	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *key_column_list;

	key_column_list =
	"full_name,street_address,purchase_date_time,inventory_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 INVENTORY_PURCHASE_TABLE,
		 key_column_list );

	update_pipe = popen( sys_string, "w" );

	return update_pipe;
}

void inventory_purchase_list_update(
			LIST *inventory_purchase_list )
{
	FILE *update_pipe;
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	update_pipe = inventory_purchase_update_pipe();

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		inventory_purchase_pipe_update(
			update_pipe,
			inventory_purchase->inventory_name,
			inventory_purchase->full_name,
			inventory_purchase->street_address,
			inventory_purchase->purchase_date_time,
			inventory_purchase->arrived_quantity,
			inventory_purchase->quantity_on_hand,
			inventory_purchase->extended_cost,
			inventory_purchase->capitalized_unit_cost,
			inventory_purchase->average_unit_cost );

	} while ( list_next( inventory_purchase_list ) );

	pclose( update_pipe );
}

void inventory_purchase_arrived_quantity_update(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			int arrived_quantity )
{
	FILE *update_pipe;

	update_pipe = inventory_purchase_update_pipe();

	inventory_purchase_arrived_pipe_update(
		update_pipe,
		inventory_name,
		full_name,
		street_address,
		purchase_date_time,
		arrived_quantity );

	pclose( update_pipe );
}

void inventory_purchase_update(
				char *application_name,
				char *inventory_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				int arrived_quantity,
				int database_arrived_quantity,
				int quantity_on_hand,
				int database_quantity_on_hand,
				double extension,
				double database_extension,
				double capitalized_unit_cost,
				double database_capitalized_unit_cost,
				double average_unit_cost,
				double database_average_unit_cost )
{
	FILE *update_pipe;

	update_pipe = inventory_purchase_get_update_pipe( application_name );

	inventory_purchase_pipe_update(
			update_pipe,
			inventory_name,
			full_name,
			street_address,
			purchase_date_time,
			arrived_quantity,
			database_arrived_quantity,
			quantity_on_hand,
			database_quantity_on_hand,
			extension,
			database_extension,
			capitalized_unit_cost,
			database_capitalized_unit_cost,
			average_unit_cost,
			database_average_unit_cost );

	pclose( update_pipe );

}

void inventory_sale_pipe_update(
				FILE *update_pipe,
				char *inventory_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				double extension,
				double database_extension,
				double cost_of_goods_sold,
				double database_cost_of_goods_sold )
{
	if ( !timlib_dollar_virtually_same(
			extension,
			database_extension ) )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^extension^%.2lf\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		inventory_name,
	 		extension );
	}

	if ( !timlib_dollar_virtually_same(
			cost_of_goods_sold,
			database_cost_of_goods_sold ) )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^cost_of_goods_sold^%.2lf\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		inventory_name,
	 		cost_of_goods_sold );
	}

}

void inventory_purchase_arrived_pipe_update(
			FILE *update_pipe,
			char *inventory_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			int arrived_quantity )
{

	fprintf(update_pipe,
		"%s^%s^%s^%s^arrived_quantity^%d\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	inventory_name,
	 	arrived_quantity );
}

void inventory_purchase_pipe_update(
			FILE *update_pipe,
			char *inventory_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			int arrived_quantity,
			int quantity_on_hand,
			double extended_cost,
			double capitalized_unit_cost,
			double average_unit_cost )
{
	fprintf(update_pipe,
		"%s^%s^%s^%s^arrived_quantity^%d\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	inventory_name,
	 	arrived_quantity );

	fprintf(update_pipe,
 		"%s^%s^%s^%s^quantity_on_hand^%d\n",
 		full_name,
 		street_address,
 		purchase_date_time,
 		inventory_name,
 		quantity_on_hand );

	fprintf(update_pipe,
		"%s^%s^%s^%s^extended_cost^%.2lf\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	inventory_name,
	 	extended_cost );

	fprintf(update_pipe,
		"%s^%s^%s^%s^capitalized_unit_cost^%.4lf\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	inventory_name,
	 	capitalized_unit_cost );

	fprintf(update_pipe,
		"%s^%s^%s^%s^average_unit_cost^%.4lf\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	inventory_name,
	 	average_unit_cost );
}

char *inventory_purchase_join_where( void )
{
	char *join_where;

	join_where =	"inventory_purchase.full_name =		"
			"	purchase_order.full_name and		"
			"inventory_purchase.street_address =		"
			"	purchase_order.street_address and	"
			"inventory_purchase.purchase_date_time =	"
			"	purchase_order.purchase_date_time	";

	return join_where;

}

LIST *inventory_purchase_entity_list(
			char *full_name,
			char *street_address,
			char *inventory_name,
			char *purchase_date_time )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder = "purchase_order,inventory_purchase";
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *inventory_purchase_list;

	if ( !inventory_name || !*inventory_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty inventory_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	select = inventory_purchase_select();

	join_where = inventory_purchase_join_where();

	sprintf( where,
		 "%s and inventory_name = '%s' and %s",
		 ledger_get_transaction_where(
				full_name,
				street_address,
				purchase_date_time,
				"inventory_purchase" /* folder_name */,
				"purchase_date_time" ),
		 inventory_name,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	inventory_purchase_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
				&inventory_purchase->full_name,
				&inventory_purchase->street_address,
				&inventory_purchase->purchase_date_time,
				&inventory_purchase->inventory_name,
				&inventory_purchase->ordered_quantity,
				&inventory_purchase->unit_cost,
				&inventory_purchase->extended_cost,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		list_append_pointer(
			inventory_purchase_list,
			inventory_purchase );
	}

	pclose( input_pipe );

	return inventory_purchase_list;
}

LIST *inventory_purchase_date_get_list(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder = "purchase_order,inventory_purchase";
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *purchase_list;

	if ( !inventory_name || !*inventory_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty inventory_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	select = inventory_purchase_select();

	join_where = inventory_purchase_join_where();

	sprintf( where,
		"inventory_name = '%s' and				"
		"inventory_purchase.purchase_date_time = '%s' and	"
		"%s							",
		 	 inventory_name,
			 purchase_date_time,
			 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	purchase_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
				&inventory_purchase->full_name,
				&inventory_purchase->street_address,
				&inventory_purchase->purchase_date_time,
				&inventory_purchase->inventory_name,
				&inventory_purchase->ordered_quantity,
				&inventory_purchase->unit_cost,
				&inventory_purchase->extended_cost,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		list_append_pointer( purchase_list, inventory_purchase );
	}

	pclose( input_pipe );

	return purchase_list;

}

LIST *inventory_purchase_arrived_date_get_list(
				char *application_name,
				char *inventory_name,
				char *earliest_arrived_date_time,
				char *latest_arrived_date_time )
{
	char sys_string[ 2048 ];
	char earliest_arrived_where[ 128 ];
	char latest_arrived_where[ 128 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder = "purchase_order,inventory_purchase";
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *purchase_list;

	if ( !inventory_name || !*inventory_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty inventory_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	select = inventory_purchase_select();

	join_where = inventory_purchase_join_where();

	if ( earliest_arrived_date_time && *earliest_arrived_date_time )
	{
		sprintf( earliest_arrived_where,
			 "purchase_order.arrived_date_time >= '%s'",
			 earliest_arrived_date_time );
	}
	else
	{
		strcpy( earliest_arrived_where, "1 = 1" );
	}

	if ( latest_arrived_date_time && *latest_arrived_date_time )
	{
		sprintf( latest_arrived_where,
			 "purchase_order.arrived_date_time <= '%s'",
			 latest_arrived_date_time );
	}
	else
	{
		strcpy( latest_arrived_where, "1 = 1" );
	}

	sprintf(where,
		"inventory_name = '%s' and				"
		"arrived_date_time is not null and			"
		"%s and %s and %s					",
		 	 inventory_name,
			 earliest_arrived_where,
			 latest_arrived_where,
			 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	purchase_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
				&inventory_purchase->full_name,
				&inventory_purchase->street_address,
				&inventory_purchase->purchase_date_time,
				&inventory_purchase->inventory_name,
				&inventory_purchase->ordered_quantity,
				&inventory_purchase->unit_cost,
				&inventory_purchase->extended_cost,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_fetch_return_list(
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		list_append_pointer( purchase_list, inventory_purchase );
	}

	pclose( input_pipe );

	return purchase_list;

}

char *inventory_get_non_zero_quantity_on_hand_arrived_date_time(
				char *application_name,
				char *inventory_name,
				char *function )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char select[ 128 ];
	char *folder;
	char *join_where;

	join_where = inventory_purchase_join_where();

	sprintf( select, "%s( purchase_order.arrived_date_time )", function );
	folder = "inventory_purchase,purchase_order";

	sprintf( where,
		 "inventory_name = '%s' and				"
		 "arrived_date_time is not null and			"
		 "ifnull( quantity_on_hand, 0 ) != 0 and		"
		 "%s							",
		 inventory_name,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

char *inventory_get_latest_zero_quantity_on_hand_arrived_date_time(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char *select;
	char *folder;
	char *join_where;

	join_where = inventory_purchase_join_where();

	select = "max( purchase_order.arrived_date_time )";
	folder = "inventory_purchase,purchase_order";

	sprintf( where,
		 "inventory_name = '%s' and				"
		 "arrived_date_time is not null and			"
		 "ifnull( quantity_on_hand, 0 ) = 0 and			"
		 "%s							",
		 inventory_name,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

void inventory_purchase_fifo_decrement_quantity_on_hand(
				LIST *purchase_list,
				int quantity )
{
	INVENTORY_PURCHASE *inventory_purchase;
	int total_quantity_remaining = quantity;

	if ( !list_rewind( purchase_list ) || !quantity ) return;

	do {
		inventory_purchase = list_get( purchase_list );

		if ( !inventory_purchase->quantity_on_hand )
		{
			continue;
		}
		else
		if ( inventory_purchase->quantity_on_hand <
		     total_quantity_remaining )
		{
			total_quantity_remaining -= 
				inventory_purchase->quantity_on_hand;

			inventory_purchase->quantity_on_hand = 0;
		}
		else
		{
			inventory_purchase->quantity_on_hand -=
				total_quantity_remaining;

			break;
		}

	} while ( list_next( purchase_list ) );

}

void inventory_purchase_lifo_decrement_quantity_on_hand(
				LIST *purchase_list,
				int quantity )
{
	INVENTORY_PURCHASE *inventory_purchase;
	int total_quantity_remaining = quantity;

	if ( !list_go_tail( purchase_list ) || !quantity ) return;

	do {
		inventory_purchase = list_get( purchase_list );

		if ( !inventory_purchase->quantity_on_hand )
		{
			continue;
		}
		else
		if ( inventory_purchase->quantity_on_hand <
		     total_quantity_remaining )
		{
			total_quantity_remaining -= 
				inventory_purchase->quantity_on_hand;

			inventory_purchase->quantity_on_hand = 0;
		}
		else
		{
			inventory_purchase->quantity_on_hand -=
				total_quantity_remaining;

			break;
		}

	} while ( list_previous( purchase_list ) );

}

double inventory_sale_get_sum_cost_of_goods_sold(
			LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double sum_cost_of_goods_sold;

	if ( !list_rewind( inventory_sale_list ) ) return 0.0;

	sum_cost_of_goods_sold = 0.0;

	do {
		inventory_sale = list_get( inventory_sale_list );

		sum_cost_of_goods_sold +=
			inventory_sale->cost_of_goods_sold;

	} while ( list_next( inventory_sale_list ) );

	return sum_cost_of_goods_sold;

}

void inventory_purchase_list_reset_quantity_on_hand(
			LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		inventory_purchase->quantity_on_hand =
			inventory_purchase_get_quantity_minus_returned(
				inventory_purchase->
					arrived_quantity,
				inventory_purchase->
					inventory_purchase_return_list ) -
			inventory_purchase->missing_quantity;

	} while( list_next( inventory_purchase_list ) );

}

void inventory_set_quantity_on_hand_fifo(
			LIST *inventory_sale_list,
			LIST *inventory_purchase_list )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	inventory_purchase_list_reset_quantity_on_hand(
		inventory_purchase_list );

	do {
		inventory_sale = list_get( inventory_sale_list );

		inventory_decrement_quantity_on_hand_get_CGS_fifo(
			inventory_purchase_list,
			inventory_sale->sold_quantity );

	} while ( list_next( inventory_sale_list ) );

}

void inventory_set_quantity_on_hand_CGS_fifo(
			LIST *inventory_sale_list,
			LIST *inventory_purchase_list )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	inventory_purchase_list_reset_quantity_on_hand(
		inventory_purchase_list );

	do {
		inventory_sale = list_get( inventory_sale_list );

		inventory_sale->cost_of_goods_sold =
			inventory_decrement_quantity_on_hand_get_CGS_fifo(
				inventory_purchase_list,
				inventory_sale->sold_quantity );

	} while ( list_next( inventory_sale_list ) );

}

double inventory_decrement_quantity_on_hand_get_CGS_fifo(
			LIST *inventory_purchase_list,
			int quantity )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double cost_of_goods_sold = 0.0;
	int total_quantity_remaining = quantity;

	if ( !list_rewind( inventory_purchase_list ) || !quantity )
	{
		return 0.0;
	}

	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		if ( !inventory_purchase->quantity_on_hand )
		{
			continue;
		}
		else
		if ( inventory_purchase->quantity_on_hand <
		     total_quantity_remaining )
		{
			cost_of_goods_sold +=
				inventory_purchase->capitalized_unit_cost *
				(double)inventory_purchase->quantity_on_hand;

			total_quantity_remaining -= 
				inventory_purchase->quantity_on_hand;

			inventory_purchase->quantity_on_hand = 0;
		}
		else
		{
			cost_of_goods_sold +=
				( inventory_purchase->capitalized_unit_cost *
				  (double)
				      total_quantity_remaining );

			inventory_purchase->quantity_on_hand -=
				total_quantity_remaining;

			return cost_of_goods_sold;
		}

	} while ( list_next( inventory_purchase_list ) );

	return 0.0;

}

void inventory_set_quantity_on_hand_CGS_lifo(
			LIST *inventory_sale_list,
			LIST *inventory_purchase_list )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	inventory_purchase_list_reset_quantity_on_hand(
		inventory_purchase_list );

	do {
		inventory_sale = list_get( inventory_sale_list );

		if ( !inventory_sale->completed_date_time ) continue;

		inventory_sale->cost_of_goods_sold =
			inventory_decrement_quantity_on_hand_get_CGS_lifo(
				inventory_purchase_list,
				inventory_sale->sold_quantity,
				inventory_sale->completed_date_time );

	} while ( list_next( inventory_sale_list ) );

}

double inventory_decrement_quantity_on_hand_get_CGS_lifo(
			LIST *inventory_purchase_list,
			int quantity,
			char *completed_date_time )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double cost_of_goods_sold = 0.0;
	int total_quantity_remaining = quantity;

	if ( !list_go_tail( inventory_purchase_list ) || !quantity )
	{
		return 0.0;
	}

	do {
		inventory_purchase =
			list_get( inventory_purchase_list );

		if ( !inventory_purchase->quantity_on_hand ) continue;

		if ( !inventory_purchase->arrived_date_time ) continue;

		if ( strcmp(	inventory_purchase->arrived_date_time,
				completed_date_time ) > 0 )
		{
			continue;
		}

		if ( inventory_purchase->quantity_on_hand <
		     total_quantity_remaining )
		{
			cost_of_goods_sold +=
				inventory_purchase->capitalized_unit_cost *
				(double)inventory_purchase->quantity_on_hand;

			total_quantity_remaining -= 
				inventory_purchase->quantity_on_hand;

			inventory_purchase->quantity_on_hand = 0;
		}
		else
		{
			cost_of_goods_sold +=
				( inventory_purchase->capitalized_unit_cost *
				  (double)
				      total_quantity_remaining );

			inventory_purchase->quantity_on_hand -=
				total_quantity_remaining;

			return cost_of_goods_sold;
		}

	} while ( list_previous( inventory_purchase_list ) );

	return 0.0;

}

char *inventory_purchase_list_display( LIST *purchase_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	INVENTORY_PURCHASE *inventory_purchase;

	*ptr = '\0';

	if ( list_rewind( purchase_list ) )
	{
		do {
			inventory_purchase = list_get( purchase_list );

			ptr += sprintf( ptr,
					"purchase_date_time = %s, "
					"arrived_quantity = %d, "
					"missing_quantity = %d, "
					"quantity_on_hand = %d, "
					"capitalized_unit_cost = %.4lf, "
					"extended_cost = %.2lf, "
					inventory_purchase->purchase_date_time,
					inventory_purchase->arrived_quantity,
					inventory_purchase->missing_quantity,
					inventory_purchase->quantity_on_hand,
					inventory_purchase->
						capitalized_unit_cost,
					inventory_purchase->extended_cost );

		} while ( list_next( purchase_list ) );
	}

	return strdup( buffer );

}

void inventory_last_inventory_balance_update(
				int quantity_on_hand,
				double average_unit_cost,
				double total_cost_balance,
				char *inventory_name,
				char *application_name )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	table_name = get_table_name( application_name, "inventory" );
	key_column_list = "inventory_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s^quantity_on_hand^%d\n",
		inventory_name,
		quantity_on_hand );

	fprintf(output_pipe,
		"%s^average_unit_cost^%.4lf\n",
		inventory_name,
		average_unit_cost );

	fprintf(output_pipe,
		"%s^total_cost_balance^%.2lf\n",
		inventory_name,
		total_cost_balance );

	pclose( output_pipe );

}

enum inventory_cost_method inventory_get_cost_method(
					char *application_name )
{
	ENTITY_SELF *self;

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return self->inventory_cost_method;

}

INVENTORY_BALANCE *inventory_balance_new( void )
{
	INVENTORY_BALANCE *h =
		(INVENTORY_BALANCE *)
			calloc( 1, sizeof( INVENTORY_BALANCE ) );
	if ( !h )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return h;

}

LIST *inventory_get_balance_list(
			LIST *inventory_purchase_list,
			LIST *inventory_sale_list )
{
	INVENTORY_BALANCE *inventory_balance;
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_SALE *inventory_sale;
	LIST *balance_list;

	if (	!list_length( inventory_purchase_list )
	&& 	!list_length( inventory_sale_list ) )
	{
		return (LIST *)0;
	}

	list_rewind( inventory_purchase_list );
	list_rewind( inventory_sale_list );

	balance_list = list_new();

	while( 1 )
	{
		if ( !list_past_end( inventory_purchase_list ) )
		{
			inventory_purchase =
				list_get(
					inventory_purchase_list );
		}
		else
		{
			inventory_purchase = (INVENTORY_PURCHASE *)0;
		}

		if ( !list_past_end( inventory_sale_list ) )
		{
			inventory_sale =
				list_get(
					inventory_sale_list );
		}
		else
		{
			inventory_sale = (INVENTORY_SALE *)0;
		}

		/* All done. */
		/* --------- */
		if ( !inventory_purchase && !inventory_sale ) break;

		inventory_balance = inventory_balance_new();

		/* If out of sales */
		/* --------------- */
		if ( !inventory_sale )
		{
			inventory_balance->inventory_purchase =
				inventory_purchase;

			list_next( inventory_purchase_list );
		}
		else
		/* ------------------- */
		/* If out of purchases */
		/* ------------------- */
		if ( !inventory_purchase )
		{
			inventory_balance->inventory_sale =
				inventory_sale;

			list_next( inventory_sale_list );
		}
		else
		if ( timlib_strcmp(
				inventory_purchase->arrived_date_time,
		     		inventory_sale->completed_date_time ) <= 0 )
		{
			inventory_balance->inventory_purchase =
				inventory_purchase;

			list_next( inventory_purchase_list );
		}
		else
		{
			inventory_balance->inventory_sale =
				inventory_sale;

			list_next( inventory_sale_list );
		}

		list_append_pointer( balance_list, inventory_balance );
	}

	return balance_list;

}
 
INVENTORY_PURCHASE *inventory_purchase_list_seek(
				LIST *inventory_purchase_list,
				char *inventory_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) )
		return (INVENTORY_PURCHASE *)0;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( strcmp(	inventory_purchase->inventory_name,
				inventory_name ) == 0 )
		{
			return inventory_purchase;
		}

	} while( list_next( inventory_purchase_list ) );

	return (INVENTORY_PURCHASE *)0;

}

INVENTORY_SALE *inventory_sale_list_seek(
				LIST *inventory_sale_list,
				char *inventory_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) )
		return (INVENTORY_SALE *)0;

	do {
		inventory_sale = list_get( inventory_sale_list );

		if ( strcmp(	inventory_sale->inventory_name,
				inventory_name ) == 0 )
		{
			return inventory_sale;
		}

	} while( list_next( inventory_sale_list ) );

	return (INVENTORY_SALE *)0;
}

INVENTORY *inventory_list_seek(
				LIST *inventory_list,
				char *inventory_name )
{
	INVENTORY *inventory;

	if ( !list_rewind( inventory_list ) ) return (INVENTORY *)0;

	do {
		inventory = list_get( inventory_list );

		if ( timlib_strcmp(
			inventory->inventory_name,
			inventory_name ) == 0 )
		{
			return inventory;
		}

	} while( list_next( inventory_list ) );

	return (INVENTORY *)0;

}

char *inventory_max_arrived_purchase_date_time(
			char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder;
	char where[ 1024 ];

	select = "max( purchase_order.purchase_date_time )";
	folder = "purchase_order,inventory_purchase";

	sprintf( where,
		 "%s and			"
		 "inventory_name = '%s' and	"
		 "arrived_date_time is not null",
		 inventory_purchase_join_where(),
		 inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

INVENTORY_PURCHASE *inventory_last_inventory_purchase(
				char *inventory_name )
{
	char *max_arrived_date_time;
	char *purchase_date_time;

	max_arrived_date_time =
		inventory_max_arrived_date_time(
			inventory_name );

	if ( !max_arrived_date_time || !*max_arrived_date_time )
		return (INVENTORY_PURCHASE *)0;

	if ( ! ( purchase_date_time =
			purchase_arrived_purchase_date_time(
				max_arrived_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get arrived_purchase_date_time with (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 max_arrived_date_time );
		exit( 1 );
	}

	return inventory_purchase_fetch(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			purchase_date_time,
			inventory_name );

}

INVENTORY_PURCHASE *inventory_purchase_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name )
{
	LIST *purchase_list;

	purchase_list =
		inventory_purchase_entity_list(
			full_name,
			street_address,
			inventory_name,
			purchase_date_time );

	if ( list_length( purchase_list ) != 1 )
		return (INVENTORY_PURCHASE *)0;
	else
		return (INVENTORY_PURCHASE *)
				list_get_first_pointer(
					purchase_list );

}

char *inventory_max_arrived_date_time(
			char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder;
	char where[ 1024 ];

	select = "max( purchase_order.arrived_date_time )";
	folder = "purchase_order,inventory_purchase";

	sprintf( where,
		 "%s and			"
		 "inventory_name = '%s' and	"
		 "arrived_date_time is not null",
		 inventory_purchase_join_where(),
		 inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );
}

double inventory_purchase_total_cost_balance(
			int *quantity_on_hand,
			double *average_unit_cost,
			double total_cost_balance,
			double capitalized_unit_cost,
			int ordered_minus_returned_quantity )
{
	double capitalized_extended_cost;

	if ( double_virtually_same( *average_unit_cost, 0.0 ) )
	{
		*quantity_on_hand = ordered_minus_returned_quantity;

		*average_unit_cost = capitalized_unit_cost;

		total_cost_balance =
			(double)ordered_minus_returned_quantity *
			capitalized_unit_cost;

		return total_cost_balance;
	}

	capitalized_extended_cost =
		inventory_purchase_extended_cost(
			ordered_minus_returned_quantity,
			capitalized_unit_cost );

	*quantity_on_hand =
		*quantity_on_hand +
		ordered_minus_returned_quantity;

	total_cost_balance += capitalized_extension;

	*average_unit_cost = total_cost_balance / (double)*quantity_on_hand;

	return total_cost_balance;
}


double inventory_purchase_extended_cost(
			int ordered_quantity,
			double unit_cost )
{
	return unit_cost * (double)ordered_quantity;
}

void inventory_purchase_list_delete(
			LIST *inventory_purchase_list,
			char *purchase_date_time,
			char *inventory_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( strcmp(	inventory_purchase->purchase_date_time,
				purchase_date_time ) == 0
		&&   strcmp(	inventory_purchase->inventory_name,
				inventory_name ) == 0 )
		{
			list_delete_current( inventory_purchase_list );
			return;
		}

	} while( list_next( inventory_purchase_list ) );
}

char *inventory_purchase_select( void )
{
	char *select =
"inventory_purchase.full_name,inventory_purchase.street_address,inventory_purchase.purchase_date_time,inventory_name,ordered_quantity,unit_cost,extension,capitalized_unit_cost,arrived_quantity,missing_quantity,quantity_on_hand,average_unit_cost,purchase_order.arrived_date_time";

	return select;

}

char *inventory_purchase_hash_table_key(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s^%s^%s^%s",
			full_name,
			street_address,
			purchase_date_time,
			inventory_name );

	return key;

}

LIST *inventory_purchase_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char where[ 1024 ];
	char buffer[ 128 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *inventory_purchase_list;

	inventory_purchase_list = list_new();

	select = inventory_purchase_select();

	folder_list_string = "inventory_purchase,purchase_order";

	sprintf( where,
		 "inventory_purchase.full_name = '%s' and		"
		 "inventory_purchase.street_address = '%s' and		"
		 "inventory_purchase.purchase_date_time = '%s' and	"
		 "%s							",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 purchase_date_time,
		 inventory_purchase_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
				&inventory_purchase->full_name,
				&inventory_purchase->street_address,
				&inventory_purchase->purchase_date_time,
				&inventory_purchase->inventory_name,
				&inventory_purchase->ordered_quantity,
				&inventory_purchase->unit_cost,
				&inventory_purchase->extended_cost,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		inventory_purchase->inventory_account_name =
			inventory_purchase_account_name(
				inventory_purchase->inventory_name );

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_return_fetch_list(
				application_name,
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		list_append_pointer(
			inventory_purchase_list,
			inventory_purchase );
	}

	pclose( input_pipe );
	return inventory_purchase_list;
}

void inventory_purchase_parse(
			char **full_name,
			char **street_address,
			char **purchase_date_time,
			char **inventory_name,
			int *ordered_quantity,
			double *unit_cost,
			double *extended_cost,
			double *capitalized_unit_cost,
			int *arrived_quantity,
			int *missing_quantity,
			int *quantity_on_hand,
			double *average_unit_cost,
			char **arrived_date_time,
			char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *piece_buffer )
		*full_name = strdup( piece_buffer );
	else
		*full_name = "";

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *piece_buffer )
		*street_address = strdup( piece_buffer );
	else
		*street_address = "";

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		*purchase_date_time = strdup( piece_buffer );
	else
		*purchase_date_time = "";

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		*inventory_name = strdup( piece_buffer );
	else
		*inventory_name = "";

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		*ordered_quantity = atoi( piece_buffer );
	else
		*ordered_quantity = 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		*unit_cost = atof( piece_buffer );
	else
		*unit_cost = 0.0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		*extended_cost = atof( piece_buffer );
	else
		*extended_cost = 0.0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		*capitalized_unit_cost = atof( piece_buffer );
	else
		*capitalized_unit_cost = 0.0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		*arrived_quantity = atoi( piece_buffer );
	else
		*arrived_quantity = 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		*missing_quantity = atoi( piece_buffer );
	else
		*missing_quantity = 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		*quantity_on_hand = atoi( piece_buffer );
	else
		*quantity_on_hand = 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		*average_unit_cost = atof( piece_buffer );
	else
		*average_unit_cost = 0.0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		*arrived_date_time = strdup( piece_buffer );
	else
		*arrived_date_time = "";
}

HASH_TABLE *inventory_purchase_arrived_hash_table(
				LIST **inventory_purchase_list,
				LIST **inventory_purchase_name_list,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char *inventory_subquery;
	char where[ 1024 ];
	char *order;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	INVENTORY_PURCHASE *inventory_purchase;
	HASH_TABLE *inventory_purchase_hash_table;
	char *key;

	*inventory_purchase_list = list_new();
	*inventory_purchase_name_list = list_new();
	inventory_purchase_hash_table = hash_table_new( hash_table_huge );

	select = inventory_purchase_select();

	folder_list_string = "inventory_purchase,purchase_order";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "purchase_order.arrived_date_time is not null",
		 inventory_subquery,
		 inventory_purchase_join_where() );

	order = "purchase_order.arrived_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 environment_application_name(),
		 select,
		 folder_list_string,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
			&inventory_purchase->full_name,
			&inventory_purchase->street_address,
			&inventory_purchase->purchase_date_time,
			&inventory_purchase->inventory_name,
			&inventory_purchase->ordered_quantity,
			&inventory_purchase->unit_cost,
			&inventory_purchase->extended_cost,
			&inventory_purchase->capitalized_unit_cost,
			&inventory_purchase->arrived_quantity,
			&inventory_purchase->missing_quantity,
			&inventory_purchase->quantity_on_hand,
			&inventory_purchase->average_unit_cost,
			&inventory_purchase->arrived_date_time,
			input_buffer );

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_fetch_return_list(
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		key = inventory_purchase_hash_table_key(
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		hash_table_set_pointer(
			inventory_purchase_hash_table,
			strdup( key ),
			inventory_purchase );

		if ( strcmp(	inventory_name,
				inventory_purchase->inventory_name ) == 0 )
		{
			list_append_pointer(
				*inventory_purchase_list,
				inventory_purchase );
		}

		if ( !list_exists_string(
			inventory_purchase->inventory_name,
			*inventory_purchase_name_list ) )
		{
			list_append_pointer(
				*inventory_purchase_name_list,
				inventory_purchase->inventory_name );
		}
	}

	pclose( input_pipe );
	return inventory_purchase_hash_table;

}

double inventory_purchase_list_set_extended_cost(
			LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double sum_inventory_extension;

	if ( !list_rewind( inventory_purchase_list ) ) return 0.0;

	sum_inventory_extension = 0.0;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		inventory_purchase->extended_cost =
			inventory_purchase_extended_cost(
				inventory_purchase->ordered_quantity,
				inventory_purchase->unit_cost );

		sum_inventory_extension += inventory_purchase->extension;

	} while( list_next( inventory_purchase_list ) );

	return sum_inventory_extension;
}

INVENTORY_PURCHASE *inventory_purchase_seek(
			LIST *inventory_purchase_list,
			char *inventory_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) )
		return (INVENTORY_PURCHASE *)0;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( strcmp(	inventory_purchase->inventory_name,
				inventory_name ) == 0 )
		{
			return inventory_purchase;
		}

	} while( list_next( inventory_purchase_list ) );

	return (INVENTORY_PURCHASE *)0;
}

void inventory_purchase_layers_propagate(
			LIST *inventory_purchase_list,
			boolean force_not_latest )
{
	INVENTORY_PURCHASE *inventory_purchase;
	char sys_string[ 1024 ];
	char is_latest_yn;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( force_not_latest )
		{
			is_latest_yn = 'n';
		}
		else
		{
			is_latest_yn =
		 		( purchase_inventory_is_latest(
					environment_application_name(),
					inventory_purchase->inventory_name,
					inventory_purchase->
						purchase_date_time ) )
		 				? 'y' : 'n';
		}

		sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" %c",
			environment_application_name(),
		 	inventory_purchase->full_name,
		 	inventory_purchase->street_address,
		 	inventory_purchase->purchase_date_time,
		 	inventory_purchase->inventory_name,
			is_latest_yn );

		if ( system( sys_string ) ) {};

	} while( list_next( inventory_purchase_list ) );
}

void inventory_purchase_arrived_quantity_update_with_propagate(
			LIST *inventory_purchase_list,
			boolean force_not_latest )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		inventory_purchase->arrived_quantity =
			inventory_purchase->ordered_quantity -
			inventory_purchase->missing_quantity;

		if (	inventory_purchase->arrived_quantity !=
			inventory_purchase->database_arrived_quantity )
		{
			inventory_purchase_arrived_quantity_update(
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name,
				inventory_purchase->arrived_quantity,
				inventory_purchase->
					database_arrived_quantity );

			inventory_purchase->database_arrived_quantity =
				inventory_purchase->arrived_quantity;
		}
	} while( list_next( inventory_purchase_list ) );

	inventory_purchase_layers_propagate(
		inventory_purchase_list,
		force_not_latest );
}

void inventory_purchase_list_set_capitalized_unit_cost(
			LIST *inventory_purchase_list,
			double sum_inventory_extension,
			double sales_tax,
			double freight_in )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double extra_cost;
	double percent_of_total;
	double capitalized_extra;

	extra_cost = sales_tax + freight_in;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( !inventory_purchase->ordered_quantity ) continue;

		if ( timlib_dollar_virtually_same(
			inventory_purchase->extension,
			0.0 ) )
		{
			continue;
		}

		if ( timlib_dollar_virtually_same( extra_cost, 0.0 ) )
		{
			inventory_purchase->capitalized_unit_cost =
				inventory_purchase->unit_cost;
		}
		else
		{
			percent_of_total =
				inventory_purchase->extension /
				sum_inventory_extension;

			capitalized_extra =
				(extra_cost * percent_of_total) /
				(double)
				inventory_purchase_quantity_minus_returned(
					inventory_purchase->ordered_quantity,
					inventory_purchase->
						inventory_purchase_return_list);

			inventory_purchase->capitalized_unit_cost =
				inventory_purchase->unit_cost +
				capitalized_extra;

		}

	} while ( list_next( inventory_purchase_list ) );
}

LIST *inventory_purchase_list_latest(
				char *inventory_name )
{
	char *latest_zero_quantity_on_hand_arrived_date_time;
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char arrived_where[ 128 ];
	char *join_where;
	char *select;
	char *folder = "purchase_order,inventory_purchase";
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *inventory_purchase_list;
	char escape_buffer[ 128 ];

	if ( !inventory_name || !*inventory_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty inventory_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	latest_zero_quantity_on_hand_arrived_date_time =
		inventory_latest_zero_quantity_on_hand_arrived_date_time(
			inventory_name );

	if ( latest_zero_quantity_on_hand_arrived_date_time
	&&   *latest_zero_quantity_on_hand_arrived_date_time )
	{
		sprintf( arrived_where,
			 "arrived_date_time > '%s'",
			 latest_zero_quantity_on_hand_arrived_date_time );
	}
	else
	{
		strcpy( arrived_where, "1 = 1" );
	}

	select = inventory_purchase_select();

	join_where = inventory_purchase_join_where();

	sprintf( where,
		 "%s and inventory_name = '%s' and %s",
		 arrived_where,
		 escape_character(
			escape_buffer,
			inventory_name,
			'\'' ),
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	inventory_purchase_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_purchase = inventory_purchase_new();

		inventory_purchase_parse(
				&inventory_purchase->full_name,
				&inventory_purchase->street_address,
				&inventory_purchase->purchase_date_time,
				&inventory_purchase->inventory_name,
				&inventory_purchase->ordered_quantity,
				&inventory_purchase->unit_cost,
				&inventory_purchase->extended_cost,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		list_append_pointer(
			inventory_purchase_list,
			inventory_purchase );
	}

	pclose( input_pipe );

	return inventory_purchase_list;

}

int inventory_purchase_quantity_minus_returned(
			int quantity,
			LIST *inventory_purchase_return_list )
{
	return	quantity -
		inventory_purchase_returned_quantity(
			inventory_purchase_return_list );
}

int inventory_purchase_returned_quantity(
			LIST *inventory_purchase_return_list )
{
	INVENTORY_PURCHASE_RETURN *i;
	int returned_quantity;

	if ( !list_rewind( inventory_purchase_return_list ) ) return 0;

	returned_quantity = 0;

	do {
		i = list_get( inventory_purchase_return_list );

		returned_quantity += i->returned_quantity;

	} while( list_next( inventory_purchase_return_list ) );

	return returned_quantity;

}

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity	*/
/* ---------------------------------------------------- */
LIST *inventory_layer_inventory_purchase_list(
			double *cost_of_goods_sold,
			LIST *inventory_purchase_list,
			int sold_quantity,
			boolean is_fifo )
{
	INVENTORY_PURCHASE *inventory_purchase;
	int total_quantity_remaining;
	LIST *layer_inventory_purchase_list;
	boolean (*rewind_fn)();
	boolean (*next_fn)();

	if ( is_fifo )
	{
		rewind_fn = list_rewind;
		next_fn = list_next;
	}
	else
	{
		rewind_fn = list_go_tail;
		next_fn = list_previous;
	}

	if ( !(*rewind_fn)( inventory_purchase_list ) ) return (LIST *)0;

	*cost_of_goods_sold = 0.0;
	total_quantity_remaining = sold_quantity;
	layer_inventory_purchase_list = list_new();

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		if ( !inventory_purchase->quantity_on_hand )
		{
			continue;
		}
		else
		if ( inventory_purchase->quantity_on_hand <
		     total_quantity_remaining )
		{
			inventory_purchase->layer_consumed_quantity =
				inventory_purchase->quantity_on_hand;

			*cost_of_goods_sold +=
				inventory_purchase->capitalized_unit_cost *
				(double)inventory_purchase->
						layer_consumed_quantity;

			total_quantity_remaining -=
				inventory_purchase->
					layer_consumed_quantity;

			inventory_purchase->quantity_on_hand = 0;

			list_append_pointer(
				layer_inventory_purchase_list,
				inventory_purchase );
		}
		else
		{
			inventory_purchase->layer_consumed_quantity =
				total_quantity_remaining;

			*cost_of_goods_sold +=
				inventory_purchase->capitalized_unit_cost *
				(double)inventory_purchase->
						layer_consumed_quantity;

			inventory_purchase->quantity_on_hand -=
				inventory_purchase->layer_consumed_quantity;

			list_append_pointer(
				layer_inventory_purchase_list,
				inventory_purchase );

			return layer_inventory_purchase_list;
		}

	} while ( (*next_fn)( inventory_purchase_list ) );

	/* This will happen if the inventory runs out. */
	/* ------------------------------------------- */
	return layer_inventory_purchase_list;
}
#endif
