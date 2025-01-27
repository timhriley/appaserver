/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory.c				*/
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
#include "appaserver_library.h"
#include "inventory.h"
#include "inventory_purchase_return.h"
#include "inventory_sale_return.h"
#include "purchase.h"
#include "customer.h"
#include "entity.h"

INVENTORY *inventory_new( char *inventory_name )
{
	INVENTORY *h = (INVENTORY *)calloc( 1, sizeof( INVENTORY ) );

	if ( !h )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	h->inventory_name = inventory_name;
	h->last_inventory_balance = inventory_balance_new();

	return h;

}

INVENTORY_SALE *inventory_sale_new( void )
{
	INVENTORY_SALE *h =
		(INVENTORY_SALE *)
			calloc( 1, sizeof( INVENTORY_SALE ) );
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

INVENTORY_PURCHASE *inventory_purchase_new( void )
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

	return h;
}

void inventory_load(			char **inventory_account_name,
					char **cost_of_goods_sold_account_name,
					double *retail_price,
					int *reorder_quantity,
					int *quantity_on_hand,
					double *average_unit_cost,
					double *total_cost_balance,
					char *application_name,
					char *inventory_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	char piece_buffer[ 128 ];

	select =
"inventory_account,cost_of_goods_sold_account,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance";

/*
	sprintf(	where,
			"inventory_name = '%s'",
		 	escape_character(
				escape_buffer,
				inventory_name,
				'\'' ) );
*/

	where = inventory_get_where( inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 "inventory",
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot fetch from inventory: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name );
		exit( 1 );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*inventory_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*cost_of_goods_sold_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*retail_price = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*reorder_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	*quantity_on_hand = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	*average_unit_cost = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	*total_cost_balance = atof( piece_buffer );

	free( results );

}

void inventory_sale_list_extension_update(
				char *application_name,
				LIST *sale_list,
				char *inventory_name )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( sale_list ) ) return;

	table_name = get_table_name( application_name, "inventory_sale" );
	key_column_list =
	"full_name,street_address,inventory_name,sale_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 table_name,
		 key_column_list );

	output_pipe = popen( sys_string, "w" );

	do {
		inventory_sale = list_get( sale_list );

		if ( !timlib_dollar_virtually_same(
				inventory_sale->extension,
				inventory_sale->database_extension ) )
		{
			fprintf(output_pipe,
				"%s^%s^%s^%s^extension^%.2lf\n",
		 		inventory_sale->full_name,
		 		inventory_sale->street_address,
		 		inventory_name,
		 		inventory_sale->sale_date_time,
		 		inventory_sale->extension );
		}

	} while ( list_next( sale_list ) );

	pclose( output_pipe );

}

void inventory_sale_list_cost_of_goods_sold_update(
				char *application_name,
				LIST *inventory_sale_list,
				char *inventory_name )
{
	FILE *update_pipe;
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	update_pipe = inventory_sale_get_update_pipe( application_name );

	do {
		inventory_sale = list_get( inventory_sale_list );

		inventory_sale_pipe_update(
				update_pipe,
				inventory_name,
				inventory_sale->full_name,
				inventory_sale->street_address,
				inventory_sale->sale_date_time,
				0.0 /* extension */,
				0.0 /* database_extension */,
				inventory_sale->cost_of_goods_sold,
				inventory_sale->database_cost_of_goods_sold );

	} while ( list_next( inventory_sale_list ) );

	pclose( update_pipe );

}

void inventory_sale_update(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				double extension,
				double database_extension,
				double cost_of_goods_sold,
				double database_cost_of_goods_sold )
{
	FILE *update_pipe;

	update_pipe = inventory_sale_get_update_pipe( application_name );

	inventory_sale_pipe_update(
				update_pipe,
				inventory_name,
				full_name,
				street_address,
				sale_date_time,
				extension,
				database_extension,
				cost_of_goods_sold,
				database_cost_of_goods_sold );

	pclose( update_pipe );

}

FILE *inventory_sale_get_update_pipe(
				char *application_name )
{
	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	table_name = get_table_name( application_name, "inventory_sale" );
	key_column_list =
	"full_name,street_address,sale_date_time,inventory_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list );

	update_pipe = popen( sys_string, "w" );

	return update_pipe;

}

FILE *inventory_purchase_get_update_pipe(
				char *application_name )
{
	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	table_name = get_table_name( application_name, "inventory_purchase" );
	key_column_list =
	"full_name,street_address,purchase_date_time,inventory_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list );

	update_pipe = popen( sys_string, "w" );

	return update_pipe;

}

/* Update everything with a database_ */
/* ---------------------------------- */
void inventory_purchase_list_update(
				char *application_name,
				LIST *inventory_purchase_list )
{
	FILE *update_pipe;
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	update_pipe = inventory_purchase_get_update_pipe( application_name );

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		inventory_purchase_pipe_update(
			update_pipe,
			inventory_purchase->inventory_name,
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

	} while ( list_next( inventory_purchase_list ) );

	pclose( update_pipe );

}

void inventory_purchase_arrived_quantity_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name,
				int arrived_quantity,
				int database_arrived_quantity )
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
			0 /* quantity_on_hand */,
			0 /* database_quantity_on_hand */,
			0.0 /* extension */,
			0.0 /* database_extension */,
			0.0 /* capitalized_unit_cost */,
			0.0 /* database_capitalized_unit_cost */,
			0.0 /* average_unit_cost * */,
			0.0 /* database_average_unit_cost */ );

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

void inventory_purchase_pipe_update(
				FILE *update_pipe,
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

	if (	arrived_quantity !=
		database_arrived_quantity )
	{
		if ( arrived_quantity )
		{
			fprintf(update_pipe,
	 			"%s^%s^%s^%s^arrived_quantity^%d\n",
	 			full_name,
	 			street_address,
	 			purchase_date_time,
	 			inventory_name,
	 			arrived_quantity );
		}
		else
		{
			fprintf(update_pipe,
	 			"%s^%s^%s^%s^arrived_quantity^0\n",
	 			full_name,
	 			street_address,
	 			purchase_date_time,
	 			inventory_name );
		}
	}

	if (	quantity_on_hand !=
		database_quantity_on_hand )
	{
		fprintf(update_pipe,
	 		"%s^%s^%s^%s^quantity_on_hand^%d\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		inventory_name,
	 		quantity_on_hand );
	}

	if ( !timlib_dollar_virtually_same(
			extension,
			database_extension ) )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^extension^%.2lf\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		inventory_name,
	 		extension );
	}

	if ( !timlib_dollar_virtually_same(
			capitalized_unit_cost,
			database_capitalized_unit_cost ) )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^capitalized_unit_cost^%.4lf\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		inventory_name,
	 		capitalized_unit_cost );
	}

	if ( !timlib_double_virtually_same_places(
			average_unit_cost,
			database_average_unit_cost,
			4 ) )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^average_unit_cost^%.4lf\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		inventory_name,
	 		average_unit_cost );
	}

}

char *inventory_get_inventory_sale_join_where( void )
{
	char *join_where;

	join_where =	"inventory_sale.full_name =			"
			"	customer_sale.full_name and		"
			"inventory_sale.street_address =		"
			"	customer_sale.street_address and	"
			"inventory_sale.sale_date_time =		"
			"	customer_sale.sale_date_time and	"
			"inventory_sale.inventory_name =		"
			"	inventory.inventory_name		";

	return join_where;

}

char *inventory_get_inventory_purchase_join_where( void )
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

LIST *inventory_purchase_entity_get_list(
				char *application_name,
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

	select = inventory_purchase_get_select();

	join_where = inventory_get_inventory_purchase_join_where();

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
		 application_name,
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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

/* Not here. See inventory_purchase_arrived_date_get_list()

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_fetch_return_list(
				application_name,
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );
*/

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

	select = inventory_purchase_get_select();

	join_where = inventory_get_inventory_purchase_join_where();

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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
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

	select = inventory_purchase_get_select();

	join_where = inventory_get_inventory_purchase_join_where();

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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_fetch_return_list(
				application_name,
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		list_append_pointer( purchase_list, inventory_purchase );
	}

	pclose( input_pipe );

	return purchase_list;

}

LIST *inventory_sale_completed_date_get_list(
				char *application_name,
				char *inventory_name,
				char *earliest_completed_date_time,
				char *latest_completed_date_time )
{
	char sys_string[ 2048 ];
	char earliest_completed_where[ 128 ];
	char latest_completed_where[ 128 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_SALE *inventory_sale;
	LIST *sale_list;
	char escape_buffer[ 128 ];

	select = inventory_sale_get_select();

	folder = "customer_sale,inventory_sale,inventory";

	if ( earliest_completed_date_time && *earliest_completed_date_time )
	{
		sprintf( earliest_completed_where,
			 "purchase_order.completed_date_time >= '%s'",
			 earliest_completed_date_time );
	}
	else
	{
		strcpy( earliest_completed_where, "1 = 1" );
	}

	if ( latest_completed_date_time && *latest_completed_date_time )
	{
		sprintf( latest_completed_where,
			 "purchase_order.completed_date_time <= '%s'",
			 latest_completed_date_time );
	}
	else
	{
		strcpy( latest_completed_where, "1 = 1" );
	}

	join_where = inventory_get_inventory_sale_join_where();

	sprintf( where,
	 	 "inventory_sale.inventory_name = '%s' and	"
		 "completed_date_time is not null and		"
		 "%s						",
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
		 "			order=completed_date_time	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	sale_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_sale = inventory_sale_new();

		inventory_sale_parse(
			&inventory_sale->full_name,
			&inventory_sale->street_address,
			&inventory_sale->sale_date_time,
			&inventory_sale->inventory_name,
			&inventory_sale->sold_quantity,
			&inventory_sale->retail_price,
			&inventory_sale->discount_amount,
			&inventory_sale->extension,
			&inventory_sale->database_extension,
			&inventory_sale->cost_of_goods_sold,
			&inventory_sale->database_cost_of_goods_sold,
			&inventory_sale->completed_date_time,
			&inventory_sale->inventory_account_name,
			&inventory_sale->cost_of_goods_sold_account_name,
			input_buffer );

		inventory_sale->inventory_sale_return_list =
			inventory_sale_fetch_return_list(
				application_name,
				inventory_sale->full_name,
				inventory_sale->street_address,
				inventory_sale->sale_date_time,
				inventory_sale->inventory_name );

		list_append_pointer( sale_list, inventory_sale );
	}

	pclose( input_pipe );

	return sale_list;

}

LIST *inventory_sale_get_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	INVENTORY_SALE *inventory_sale;
	LIST *sale_list;
	char escape_buffer[ 128 ];

	select = inventory_sale_get_select();

	folder = "customer_sale,inventory_sale,inventory";

	join_where = inventory_get_inventory_sale_join_where();

	if ( sale_date_time && *sale_date_time )
	{
		sprintf( where,
			 "%s and inventory_sale.inventory_name = '%s' and %s",
			 ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					"inventory_sale" /* folder_name */,
					"sale_date_time" ),
		 	 escape_character(
				escape_buffer,
				inventory_name,
				'\'' ),
			 join_where );
	}
	else
	{
		sprintf( where,
		 	 "inventory_sale.inventory_name = '%s' and	"
			 "completed_date_time is not null and		"
			 "%s						",
		 	 escape_character(
				escape_buffer,
				inventory_name,
				'\'' ),
			 join_where );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=sale_date_time		",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	sale_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		inventory_sale = inventory_sale_new();

		inventory_sale_parse(
			&inventory_sale->full_name,
			&inventory_sale->street_address,
			&inventory_sale->sale_date_time,
			&inventory_sale->inventory_name,
			&inventory_sale->sold_quantity,
			&inventory_sale->retail_price,
			&inventory_sale->discount_amount,
			&inventory_sale->extension,
			&inventory_sale->database_extension,
			&inventory_sale->cost_of_goods_sold,
			&inventory_sale->database_cost_of_goods_sold,
			&inventory_sale->completed_date_time,
			&inventory_sale->inventory_account_name,
			&inventory_sale->cost_of_goods_sold_account_name,
			input_buffer );

		list_append_pointer( sale_list, inventory_sale );
	}

	pclose( input_pipe );

	return sale_list;

}

char *inventory_sale_get_latest_date_time(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *select = "max( sale_date_time )";

	sprintf( where,
		 "inventory_name = '%s'",
		 inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=inventory_sale		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	return pipe2string( sys_string );

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

	join_where = inventory_get_inventory_purchase_join_where();

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

	join_where = inventory_get_inventory_purchase_join_where();

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

char *inventory_sale_list_display( LIST *sale_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	INVENTORY_SALE *inventory_sale;

	*ptr = '\0';

	if ( list_rewind( sale_list ) )
	{
		do {
			inventory_sale = list_get( sale_list );

			ptr += sprintf( ptr,
					"sale_date_time = %s, "
					"quantity = %d, "
					"retail_price = %.2lf, "
					"discount_amount = %.2lf, "
					"extension = %.2lf, "
					"database_extension = %.2lf, "
					"cost_of_goods_sold = %.2lf, "
					"database_cost_of_goods_sold = %.2lf\n",
					inventory_sale->sale_date_time,
					inventory_sale->sold_quantity,
					inventory_sale->retail_price,
					inventory_sale->discount_amount,
					inventory_sale->extension,
					inventory_sale->database_extension,
					inventory_sale->cost_of_goods_sold,
					inventory_sale->
						database_cost_of_goods_sold );

		} while ( list_next( sale_list ) );
	}

	return strdup( buffer );

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
					"database_quantity_on_hand = %d, "
					"capitalized_unit_cost = %.4lf, "
					"extension = %.2lf, "
					"database_extension = %.2lf\n",
					inventory_purchase->purchase_date_time,
					inventory_purchase->arrived_quantity,
					inventory_purchase->missing_quantity,
					inventory_purchase->quantity_on_hand,
					inventory_purchase->
						database_quantity_on_hand,
					inventory_purchase->
						capitalized_unit_cost,
					inventory_purchase->extension,
					inventory_purchase->
						database_extension );

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

void inventory_sale_list_update(
			LIST *inventory_sale_list,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	FILE *update_pipe;

	if ( !list_rewind( inventory_sale_list ) ) return;

	update_pipe = inventory_sale_get_update_pipe( application_name );

	do {
		inventory_sale = list_get( inventory_sale_list );

		inventory_sale_pipe_update(
			update_pipe,
			inventory_sale->inventory_name,
			inventory_sale->full_name,
			inventory_sale->street_address,
			inventory_sale->sale_date_time,
			inventory_sale->extension,
			inventory_sale->database_extension,
			inventory_sale->cost_of_goods_sold,
			inventory_sale->database_cost_of_goods_sold );

	} while( list_next( inventory_sale_list ) );

	pclose( update_pipe );

}

void inventory_list_update(
			LIST *inventory_list,
			char *application_name )
{
	INVENTORY *inventory;

	if ( !list_rewind( inventory_list ) ) return;

	do {
		inventory = list_get( inventory_list );

		inventory_purchase_list_update(
			application_name,
			inventory->inventory_purchase_list );

		inventory_sale_list_extension_update(
			application_name,
			inventory->inventory_sale_list,
			inventory->inventory_name );

	} while( list_next( inventory_list ) );

}

void inventory_list_cost_of_goods_sold_update(
			LIST *inventory_list,
			char *application_name )
{
	INVENTORY *inventory;

	if ( !list_rewind( inventory_list ) ) return;

	do {
		inventory = list_get( inventory_list );

		inventory_sale_list_cost_of_goods_sold_update(
			application_name,
			inventory->inventory_sale_list,
			inventory->inventory_name );

	} while( list_next( inventory_list ) );

}

LIST *inventory_get_fifo_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list )
{
	LIST *inventory_balance_list;
	INVENTORY_BALANCE *inventory_balance = {0};
	INVENTORY_BALANCE *prior_inventory_balance = {0};
	double purchase_total_cost;
	int total_missing_quantity = 0;
	int quantity_minus_returned;
	boolean first_time = 1;

	inventory_balance_list =
		inventory_get_balance_list(
			inventory_purchase_list,
			inventory_sale_list );

	if ( !list_rewind( inventory_balance_list ) ) return (LIST *)0;

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( first_time )
		{
			if ( !inventory_balance->inventory_purchase )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: inventory balance must start with a purchase.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				return (LIST *)0;
			}

/*
			inventory_balance->quantity_on_hand =
				inventory_balance->
					inventory_purchase->
					ordered_quantity;
*/

			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				quantity_minus_returned;

			inventory_balance->
				average_unit_cost =
			inventory_balance->
				inventory_purchase->
				average_unit_cost =
					inventory_balance->
						inventory_purchase->
						capitalized_unit_cost;

			inventory_balance->total_cost_balance =
				(double)inventory_balance->
						quantity_on_hand *
				inventory_balance->
						average_unit_cost;

			total_missing_quantity =
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			prior_inventory_balance = inventory_balance;
			first_time = 0;
			continue;
		}

		if ( inventory_balance->inventory_purchase )
		{
/*
			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand +
				inventory_balance->
					inventory_purchase->
					ordered_quantity;
*/

			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand +
					quantity_minus_returned;

			purchase_total_cost =
				(double)quantity_minus_returned *
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance +
				purchase_total_cost;

			if ( inventory_balance->
				quantity_on_hand )
			{
				inventory_balance->
				inventory_purchase->
				average_unit_cost =
					  inventory_balance->
					    total_cost_balance /
					    (double)inventory_balance->
							quantity_on_hand;
			}
			else
			{
				inventory_balance->
					inventory_purchase->
					average_unit_cost = 0.0;
			}

			total_missing_quantity +=
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			inventory_balance->average_unit_cost =
				inventory_balance->
					inventory_purchase->
					average_unit_cost;
		}
		else
		/* ----------------- */
		/* If inventory sale */
		/* ----------------- */
		{
			inventory_balance->average_unit_cost =
				prior_inventory_balance->
					average_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance -
				( inventory_balance->average_unit_cost *
				  (double)inventory_balance->
					inventory_sale->
					sold_quantity );

			inventory_balance->
			inventory_sale->
			cost_of_goods_sold =
				inventory_balance->
					average_unit_cost *
				(double)inventory_balance->
					inventory_sale->
					sold_quantity;

/*
			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand -
				inventory_balance->
				inventory_sale->
					sold_quantity;
*/

			quantity_minus_returned =
				inventory_sale_get_quantity_minus_returned(
					inventory_balance->
						inventory_sale->
						sold_quantity,
					inventory_balance->
						inventory_sale->
						inventory_sale_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand -
					quantity_minus_returned;
		}

		prior_inventory_balance = inventory_balance;

	} while ( list_next( inventory_balance_list ) );

	/* Set the last balance attributes. */
	/* -------------------------------- */
	inventory_balance->quantity_on_hand -= total_missing_quantity;

	inventory_balance->total_cost_balance =
		(double)inventory_balance->quantity_on_hand *
		inventory_balance->average_unit_cost;

	return inventory_balance_list;

}

LIST *inventory_sort_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list )
{
	LIST *inventory_balance_list;
	INVENTORY_BALANCE *inventory_balance = {0};
	INVENTORY_BALANCE *prior_inventory_balance = {0};
	double purchase_total_cost;
	int total_missing_quantity = 0;
	int quantity_minus_returned;
	boolean first_time = 1;

	inventory_balance_list =
		inventory_get_balance_list(
			inventory_purchase_list,
			inventory_sale_list );

	if ( !list_rewind( inventory_balance_list ) ) return (LIST *)0;

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( first_time )
		{
			if ( !inventory_balance->inventory_purchase )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: inventory balance must start with a purchase.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				return (LIST *)0;
			}

			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				quantity_minus_returned;

			inventory_balance->
				average_unit_cost =
			inventory_balance->
				inventory_purchase->
				average_unit_cost =
					inventory_balance->
						inventory_purchase->
						capitalized_unit_cost;

			inventory_balance->total_cost_balance =
				(double)inventory_balance->
						quantity_on_hand *
				inventory_balance->
						average_unit_cost;

			total_missing_quantity =
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			prior_inventory_balance = inventory_balance;
			first_time = 0;
			continue;
		}

		if ( inventory_balance->inventory_purchase )
		{
			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand +
					quantity_minus_returned;

			purchase_total_cost =
				(double)quantity_minus_returned *
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance +
				purchase_total_cost;

			if ( inventory_balance->
				quantity_on_hand )
			{
				inventory_balance->
				inventory_purchase->
				average_unit_cost =
					  inventory_balance->
					    total_cost_balance /
					    (double)inventory_balance->
							quantity_on_hand;
			}
			else
			{
				inventory_balance->
					inventory_purchase->
					average_unit_cost = 0.0;
			}

			total_missing_quantity +=
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			inventory_balance->average_unit_cost =
				inventory_balance->
					inventory_purchase->
					average_unit_cost;
		}
		else
		/* ----------------- */
		/* If inventory sale */
		/* ----------------- */
		{
			inventory_balance->average_unit_cost =
				prior_inventory_balance->
					average_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance -
				( inventory_balance->average_unit_cost *
				  (double)inventory_balance->
					inventory_sale->
					sold_quantity );

			quantity_minus_returned =
				inventory_sale_get_quantity_minus_returned(
					inventory_balance->
						inventory_sale->
						sold_quantity,
					inventory_balance->
						inventory_sale->
						inventory_sale_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand -
					quantity_minus_returned;
		}

		prior_inventory_balance = inventory_balance;

	} while ( list_next( inventory_balance_list ) );

	/* Set the last balance attributes. */
	/* -------------------------------- */
	inventory_balance->quantity_on_hand -= total_missing_quantity;

	inventory_balance->total_cost_balance =
		(double)inventory_balance->quantity_on_hand *
		inventory_balance->average_unit_cost;

	return inventory_balance_list;

}

LIST *inventory_get_average_cost_inventory_balance_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list )
{
	LIST *inventory_balance_list;
	INVENTORY_BALANCE *inventory_balance = {0};
	INVENTORY_BALANCE *prior_inventory_balance = {0};
	double purchase_total_cost;
	int total_missing_quantity = 0;
	int quantity_minus_returned;
	boolean first_time = 1;

	inventory_balance_list =
		inventory_get_balance_list(
			inventory_purchase_list,
			inventory_sale_list );

	if ( !list_rewind( inventory_balance_list ) ) return (LIST *)0;

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( first_time )
		{
			if ( !inventory_balance->inventory_purchase )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: inventory balance must start with a purchase.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				return (LIST *)0;
			}

/*
			inventory_balance->quantity_on_hand =
				inventory_balance->
					inventory_purchase->
					ordered_quantity;
*/

			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				quantity_minus_returned;

			inventory_balance->
				average_unit_cost =
			inventory_balance->
				inventory_purchase->
				average_unit_cost =
					inventory_balance->
						inventory_purchase->
						capitalized_unit_cost;

			inventory_balance->total_cost_balance =
				(double)inventory_balance->
						quantity_on_hand *
				inventory_balance->
						average_unit_cost;

			total_missing_quantity =
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			prior_inventory_balance = inventory_balance;
			first_time = 0;
			continue;
		}

		if ( inventory_balance->inventory_purchase )
		{
/*
			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand +
				inventory_balance->
					inventory_purchase->
					ordered_quantity;
*/

			quantity_minus_returned =
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
					       inventory_purchase->
					       ordered_quantity,
					inventory_balance->
					       inventory_purchase->
					       inventory_purchase_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand +
					quantity_minus_returned;

			purchase_total_cost =
				(double)quantity_minus_returned *
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance +
				purchase_total_cost;

			if ( inventory_balance->
				quantity_on_hand )
			{
				inventory_balance->
				inventory_purchase->
				average_unit_cost =
					  inventory_balance->
					    total_cost_balance /
					    (double)inventory_balance->
							quantity_on_hand;
			}
			else
			{
				inventory_balance->
					inventory_purchase->
					average_unit_cost = 0.0;
			}

			total_missing_quantity +=
				inventory_balance->
					inventory_purchase->
					missing_quantity;

			inventory_balance->average_unit_cost =
				inventory_balance->
					inventory_purchase->
					average_unit_cost;
		}
		else
		/* ----------------- */
		/* If inventory sale */
		/* ----------------- */
		{
			inventory_balance->average_unit_cost =
				prior_inventory_balance->
					average_unit_cost;

			inventory_balance->
			total_cost_balance =
				prior_inventory_balance->
					total_cost_balance -
				( inventory_balance->average_unit_cost *
				  (double)inventory_balance->
					inventory_sale->
					sold_quantity );

			inventory_balance->
			inventory_sale->
			cost_of_goods_sold =
				inventory_balance->
					average_unit_cost *
				(double)inventory_balance->
					inventory_sale->
					sold_quantity;

/*
			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand -
				inventory_balance->
				inventory_sale->
					sold_quantity;
*/

			quantity_minus_returned =
				inventory_sale_get_quantity_minus_returned(
					inventory_balance->
						inventory_sale->
						sold_quantity,
					inventory_balance->
						inventory_sale->
						inventory_sale_return_list );

			inventory_balance->quantity_on_hand =
				prior_inventory_balance->
					quantity_on_hand -
					quantity_minus_returned;
		}

		prior_inventory_balance = inventory_balance;

	} while ( list_next( inventory_balance_list ) );

	/* Set the last balance attributes. */
	/* -------------------------------- */
	inventory_balance->quantity_on_hand -= total_missing_quantity;

	inventory_balance->total_cost_balance =
		(double)inventory_balance->quantity_on_hand *
		inventory_balance->average_unit_cost;

	return inventory_balance_list;

}

INVENTORY *inventory_load_new(	char *application_name,
				char *inventory_name )
{
	INVENTORY *inventory;

	inventory = inventory_new( inventory_name );

	inventory_load(	&inventory->inventory_account_name,
			&inventory->cost_of_goods_sold_account_name,
			&inventory->retail_price,
			&inventory->reorder_quantity,
			&inventory->
				last_inventory_balance->
				quantity_on_hand,
			&inventory->
				last_inventory_balance->
				average_unit_cost,
			&inventory->
				last_inventory_balance->
				total_cost_balance,
			application_name,
			inventory->inventory_name );

	return inventory;

}

int inventory_get_sale_quantity(
			char *application_name,
			char *inventory_name,
			char *sale_full_name,
			char *sale_street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *entity_where;
	char where[ 512 ];
	char *results;
	int quantity;

	entity_where = ledger_get_transaction_where(
					sale_full_name,
					sale_street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf(	where,
			"%s and inventory_name = '%s'",
			entity_where,
			inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=quantity		"
		 "			folder=inventory_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
		quantity = 0;
	else
		quantity = atoi( results );

	return quantity;

}

char *inventory_get_max_arrived_purchase_date_time(
						char *application_name,
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
		 inventory_get_inventory_purchase_join_where(),
		 inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

INVENTORY_PURCHASE *inventory_get_last_inventory_purchase(
				char *application_name,
				char *inventory_name )
{
	char *max_arrived_date_time;
	char *purchase_date_time;

	max_arrived_date_time =
		inventory_get_max_arrived_date_time(
			application_name,
			inventory_name );

	if ( !max_arrived_date_time || !*max_arrived_date_time )
		return (INVENTORY_PURCHASE *)0;

	if ( ! ( purchase_date_time =
			purchase_get_arrived_purchase_date_time(
				application_name,
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
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				purchase_date_time,
				inventory_name );

}

INVENTORY_SALE *inventory_get_last_inventory_sale(
				char *application_name,
				char *inventory_name )
{
	char *max_completed_date_time;
	char *sale_date_time;

	max_completed_date_time =
		inventory_get_max_completed_date_time(
			application_name,
			inventory_name );

	if ( !max_completed_date_time || !*max_completed_date_time )
		return (INVENTORY_SALE *)0;

	if ( ! ( sale_date_time =
			customer_get_completed_sale_date_time(
				application_name,
				max_completed_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get completed_sale_date_time with (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 inventory_name,
			 max_completed_date_time );
		exit( 1 );
	}

	return inventory_sale_fetch(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				sale_date_time,
				inventory_name );

}

INVENTORY_SALE *inventory_sale_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name )
{
	LIST *sale_list;

	sale_list =
		inventory_sale_get_list(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				inventory_name );

	if ( list_length( sale_list ) != 1 )
		return (INVENTORY_SALE *)0;
	else
		return (INVENTORY_SALE *)
				list_get_first_pointer(
					sale_list );

}

INVENTORY_PURCHASE *inventory_purchase_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *inventory_name )
{
	LIST *purchase_list;

	purchase_list =
		inventory_purchase_entity_get_list(
				application_name,
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

char *inventory_get_max_arrived_date_time(
				char *application_name,
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
		 inventory_get_inventory_purchase_join_where(),
		 inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

char *inventory_get_max_completed_date_time(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder;
	char where[ 1024 ];
	char escape_buffer[ 128 ];

	select = "max( customer_sale.completed_date_time )";
	folder = "customer_sale,inventory_sale,inventory";

	sprintf( where,
		 "%s and					"
		 "inventory_sale.inventory_name = '%s' and	"
		 "completed_date_time is not null		",
		 inventory_get_inventory_sale_join_where(),
		 escape_character(
			escape_buffer,
			inventory_name,
			'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

}

boolean inventory_is_latest_purchase(
			char *last_inventory_purchase_date_time,
			INVENTORY_SALE *last_inventory_sale,
			char *last_inventory_arrived_date_time,
			char *purchase_date_time )
{
	if ( !last_inventory_sale )
	{
		if ( timlib_strcmp(
			last_inventory_purchase_date_time,
			purchase_date_time ) != 0 )
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	if ( timlib_strcmp(
		last_inventory_purchase_date_time,
		purchase_date_time ) != 0 )
	{
		return 0;
	}
	else
	if ( timlib_strcmp(
		last_inventory_arrived_date_time,
		last_inventory_sale->completed_date_time ) <= 0 )
	{
		return 0;
	}

	return 1;

}

boolean inventory_is_latest_sale(
			char *last_inventory_sale_date_time,
			char *last_inventory_completed_date_time,
			char *last_inventory_arrived_date_time,
			char *sale_date_time )
{
	if ( timlib_strcmp(
		last_inventory_sale_date_time,
		sale_date_time ) != 0 )
	{
		return 0;
	}
	else
	if ( timlib_strcmp(
		last_inventory_completed_date_time,
		last_inventory_arrived_date_time ) <= 0 )
	{
		return 0;
	}

	return 1;

}

double inventory_purchase_get_total_cost_balance(
			int *quantity_on_hand,
			double *average_unit_cost,
			double total_cost_balance,
			double capitalized_unit_cost,
			int ordered_minus_returned_quantity )
{
	double capitalized_extension;

	if ( double_virtually_same( *average_unit_cost, 0.0 ) )
	{
		*quantity_on_hand = ordered_minus_returned_quantity;

		*average_unit_cost = capitalized_unit_cost;

		total_cost_balance =
			(double)ordered_minus_returned_quantity *
			capitalized_unit_cost;

		return total_cost_balance;
	}

	capitalized_extension =
		inventory_purchase_get_extension(
			ordered_minus_returned_quantity,
			capitalized_unit_cost );

	*quantity_on_hand =
		*quantity_on_hand +
		ordered_minus_returned_quantity;

	total_cost_balance += capitalized_extension;

	*average_unit_cost = total_cost_balance / (double)*quantity_on_hand;

	return total_cost_balance;

}


double inventory_purchase_get_extension(int ordered_quantity,
					double unit_cost )
{
	return unit_cost * (double)ordered_quantity;

}

double inventory_sale_get_average_cost_of_goods_sold(
				double *total_cost_balance,
				int *quantity_on_hand,
				double average_unit_cost,
				int quantity_sold )
{
	*quantity_on_hand = *quantity_on_hand - quantity_sold;
	*total_cost_balance = (double)*quantity_on_hand * average_unit_cost;
	return (double)quantity_sold * average_unit_cost;

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

void inventory_sale_list_delete(
			LIST *inventory_sale_list,
			char *sale_date_time,
			char *inventory_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	do {
		inventory_sale = list_get( inventory_sale_list );

		if ( strcmp(	inventory_sale->sale_date_time,
				sale_date_time ) == 0
		&&   strcmp(	inventory_sale->inventory_name,
				inventory_name ) == 0 )
		{
			list_delete_current( inventory_sale_list );
			return;
		}

	} while( list_next( inventory_sale_list ) );

}

char *inventory_get_subquery(	char *inventory_name,
				char *one2m_folder_name,
				char *mto1_folder_name,
				char *foreign_attribute_name )
{
	char where[ 2048 ];

	sprintf( where,
		 "exists (	select 1				    "
		 "		from %s					    "
		 "		where %s.full_name = %s.full_name	    "
		 "		  and %s.street_address = %s.street_address "
		 "		  and %s.%s = %s.%s			    "
		 "		  and %s.inventory_name = '%s' )	    ",
		 mto1_folder_name,
		 one2m_folder_name,
		 mto1_folder_name,
		 one2m_folder_name,
		 mto1_folder_name,
		 one2m_folder_name,
		 foreign_attribute_name,
		 mto1_folder_name,
		 foreign_attribute_name,
		 mto1_folder_name,
		 inventory_name );

	return strdup( where );

}

void inventory_list_delete(	LIST *inventory_list,
				char *inventory_name )
{
	INVENTORY *inventory;

	if ( ( inventory =
			inventory_list_seek(
				inventory_list,
				inventory_name ) ) )
	{
		list_delete_current( inventory_list );
	}

}

char *inventory_sale_get_select( void )
{
	char *select =
"inventory_sale.full_name,inventory_sale.street_address,inventory_sale.sale_date_time,inventory_sale.inventory_name,quantity,inventory_sale.retail_price,discount_amount,extension,cost_of_goods_sold,customer_sale.completed_date_time,inventory.inventory_account,inventory.cost_of_goods_sold_account";

	return select;

}

char *inventory_purchase_get_select( void )
{
	char *select =
"inventory_purchase.full_name,inventory_purchase.street_address,inventory_purchase.purchase_date_time,inventory_name,ordered_quantity,unit_cost,extension,capitalized_unit_cost,arrived_quantity,missing_quantity,quantity_on_hand,average_unit_cost,purchase_order.arrived_date_time";

	return select;

}

char *inventory_purchase_get_hash_table_key(
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

void inventory_sale_parse(
				char **full_name,
				char **street_address,
				char **sale_date_time,
				char **inventory_name,
				int *sold_quantity,
				double *retail_price,
				double *discount_amount,
				double *extension,
				double *database_extension,
				double *cost_of_goods_sold,
				double *database_cost_of_goods_sold,
				char **completed_date_time,
				char **inventory_account_name,
				char **cost_of_goods_sold_account_name,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *piece_buffer )
		*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *piece_buffer )
		*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		*sale_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		*inventory_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		*sold_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		*retail_price = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		*discount_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		*extension =
		*database_extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		*cost_of_goods_sold =
		*database_cost_of_goods_sold = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		*completed_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		*inventory_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		*cost_of_goods_sold_account_name = strdup( piece_buffer );

}

LIST *inventory_get_inventory_sale_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char where[ 1024 ];
	char buffer[ 128 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	INVENTORY_SALE *inventory_sale;
	LIST *inventory_sale_list;

	inventory_sale_list = list_new();

	select = inventory_sale_get_select();

	folder_list_string = "inventory_sale,customer_sale,inventory";

	sprintf( where,
		 "inventory_sale.full_name = '%s' and			"
		 "inventory_sale.street_address = '%s' and		"
		 "inventory_sale.sale_date_time = '%s' and		"
		 "%s							",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 sale_date_time,
		 customer_sale_get_inventory_sale_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		inventory_sale = inventory_sale_new();

		inventory_sale_parse(
			&inventory_sale->full_name,
			&inventory_sale->street_address,
			&inventory_sale->sale_date_time,
			&inventory_sale->inventory_name,
			&inventory_sale->sold_quantity,
			&inventory_sale->retail_price,
			&inventory_sale->discount_amount,
			&inventory_sale->extension,
			&inventory_sale->database_extension,
			&inventory_sale->cost_of_goods_sold,
			&inventory_sale->database_cost_of_goods_sold,
			&inventory_sale->completed_date_time,
			&inventory_sale->inventory_account_name,
			&inventory_sale->cost_of_goods_sold_account_name,
			input_buffer );

		list_append_pointer(
			inventory_sale_list,
			inventory_sale );
	}

	pclose( input_pipe );
	return inventory_sale_list;

}

LIST *inventory_get_inventory_purchase_list(
				char *application_name,
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

	select = inventory_purchase_get_select();

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
		 purchase_order_get_inventory_purchase_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		inventory_purchase->inventory_account_name =
			inventory_get_inventory_account_name(
				application_name,
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

LIST *inventory_get_specific_inventory_purchase_list(
				char *application_name,
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

	select = inventory_purchase_get_select();

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
		 purchase_order_get_inventory_purchase_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

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
				double *extension,
				double *database_extension,
				double *capitalized_unit_cost,
				double *database_capitalized_unit_cost,
				int *arrived_quantity,
				int *database_arrived_quantity,
				int *missing_quantity,
				int *quantity_on_hand,
				int *database_quantity_on_hand,
				double *average_unit_cost,
				double *database_average_unit_cost,
				char **arrived_date_time,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];


	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *piece_buffer )
		*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *piece_buffer )
		*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		*purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		*inventory_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		*ordered_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		*unit_cost = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		*extension =
		*database_extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		*capitalized_unit_cost =
		*database_capitalized_unit_cost = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		*arrived_quantity =
		*database_arrived_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		*missing_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		*quantity_on_hand =
		*database_quantity_on_hand = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		*average_unit_cost =
		*database_average_unit_cost = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		*arrived_date_time = strdup( piece_buffer );

}

HASH_TABLE *inventory_get_arrived_inventory_purchase_hash_table(
				LIST **inventory_purchase_list,
				LIST **inventory_purchase_name_list,
				char *application_name,
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

	select = inventory_purchase_get_select();

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
		 purchase_order_get_inventory_purchase_join_where() );

	order = "purchase_order.arrived_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
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
			&inventory_purchase->extension,
			&inventory_purchase->database_extension,
			&inventory_purchase->capitalized_unit_cost,
			&inventory_purchase->
				database_capitalized_unit_cost,
			&inventory_purchase->arrived_quantity,
			&inventory_purchase->database_arrived_quantity,
			&inventory_purchase->missing_quantity,
			&inventory_purchase->quantity_on_hand,
			&inventory_purchase->database_quantity_on_hand,
			&inventory_purchase->average_unit_cost,
			&inventory_purchase->database_average_unit_cost,
			&inventory_purchase->arrived_date_time,
			input_buffer );

		inventory_purchase->inventory_purchase_return_list =
			inventory_purchase_fetch_return_list(
				application_name,
				inventory_purchase->full_name,
				inventory_purchase->street_address,
				inventory_purchase->purchase_date_time,
				inventory_purchase->inventory_name );

		key = inventory_purchase_get_hash_table_key(
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

HASH_TABLE *inventory_get_completed_inventory_sale_hash_table(
				LIST **inventory_sale_list,
				LIST **inventory_sale_name_list,
				char *application_name,
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
	INVENTORY_SALE *inventory_sale;
	HASH_TABLE *inventory_sale_hash_table;
	char *key;

	*inventory_sale_list = list_new();
	*inventory_sale_name_list = list_new();
	inventory_sale_hash_table = hash_table_new( hash_table_huge );

	select = inventory_sale_get_select();

	folder_list_string = "inventory_sale,customer_sale,inventory";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale" /* one2m_folder_name */,
			"inventory_sale" /* mto1_folder_name */,
			"sale_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "customer_sale.completed_date_time is not null",
		 inventory_subquery,
		 customer_sale_get_inventory_sale_join_where() );

	order = "customer_sale.completed_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
		 select,
		 folder_list_string,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		inventory_sale = inventory_sale_new();

		inventory_sale_parse(
			&inventory_sale->full_name,
			&inventory_sale->street_address,
			&inventory_sale->sale_date_time,
			&inventory_sale->inventory_name,
			&inventory_sale->sold_quantity,
			&inventory_sale->retail_price,
			&inventory_sale->discount_amount,
			&inventory_sale->extension,
			&inventory_sale->database_extension,
			&inventory_sale->cost_of_goods_sold,
			&inventory_sale->database_cost_of_goods_sold,
			&inventory_sale->completed_date_time,
			&inventory_sale->inventory_account_name,
			&inventory_sale->cost_of_goods_sold_account_name,
			input_buffer );

		inventory_sale->inventory_sale_return_list =
			inventory_sale_fetch_return_list(
				application_name,
				inventory_sale->full_name,
				inventory_sale->street_address,
				inventory_sale->sale_date_time,
				inventory_sale->inventory_name );

		key = inventory_sale_get_hash_table_key(
				inventory_sale->full_name,
				inventory_sale->street_address,
				inventory_sale->sale_date_time,
				inventory_sale->inventory_name );

		hash_table_set_pointer(
			inventory_sale_hash_table,
			strdup( key ),
			inventory_sale );

		if ( strcmp(	inventory_name,
				inventory_sale->inventory_name ) == 0 )
		{
			list_append_pointer(
				*inventory_sale_list,
				inventory_sale );
		}

		if ( !list_exists_string(
			inventory_sale->inventory_name,
			*inventory_sale_name_list ) )
		{
			list_append_pointer(
				*inventory_sale_name_list,
				inventory_sale->inventory_name );
		}
	}

	pclose( input_pipe );
	return inventory_sale_hash_table;

}

char *inventory_sale_get_hash_table_key(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s^%s^%s^%s",
			full_name,
			street_address,
			sale_date_time,
			inventory_name );

	return key;

}

double inventory_purchase_list_set_extension(
			LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double sum_inventory_extension;

	if ( !list_rewind( inventory_purchase_list ) ) return 0.0;

	sum_inventory_extension = 0.0;

	do {
		inventory_purchase = list_get( inventory_purchase_list );

		inventory_purchase->extension =
			inventory_purchase_get_extension(
				inventory_purchase->ordered_quantity,
				inventory_purchase->unit_cost );

		sum_inventory_extension += inventory_purchase->extension;

	} while( list_next( inventory_purchase_list ) );

	return sum_inventory_extension;

}

void inventory_sale_list_set_extension(
				LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	do {
		inventory_sale = list_get( inventory_sale_list );

		inventory_sale->extension =
			inventory_sale_get_extension(
				inventory_sale->retail_price,
				inventory_sale->sold_quantity,
				inventory_sale->discount_amount );

	} while( list_next( inventory_sale_list ) );

}

double inventory_sale_get_extension(
				double retail_price,
				int quantity,
				double discount_amount )
{
	return ( retail_price * (double)quantity ) - discount_amount;
}

INVENTORY_PURCHASE *inventory_get_inventory_purchase(
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
			char *application_name,
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
					application_name,
					inventory_purchase->inventory_name,
					inventory_purchase->
						purchase_date_time ) )
		 				? 'y' : 'n';
		}

		sprintf( sys_string,
	"propagate_inventory_purchase_layers %s \"%s\" \"%s\" \"%s\" \"%s\" %c",
			application_name,
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
			char *application_name,
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
				application_name,
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
		application_name,
		inventory_purchase_list,
		force_not_latest );

}

/* -------------------------------------------------------------------- */
/* This function alters:						*/
/* 1) inventory_sale->cost_of_goods_sold				*/
/* 2) inventory->inventory_purchase_list->quantity_on_hand 		*/
/* 3) inventory->last_inventory_balance.				*/
/* -------------------------------------------------------------------- */
INVENTORY *inventory_sale_set_cost_of_goods_sold(
			INVENTORY_SALE *inventory_sale,
			char *application_name,
			char *inventory_name,
			enum inventory_cost_method inventory_cost_method )
{
	char *earliest_arrived_date_time;
	char *latest_arrived_date_time;
	INVENTORY *inventory;

	inventory =
		inventory_load_new(
			application_name,
			inventory_name );

	earliest_arrived_date_time =
	     inventory_get_non_zero_quantity_on_hand_arrived_date_time(
			application_name,
			inventory->inventory_name,
			"min" /* function */ );

	latest_arrived_date_time =
	     inventory_get_non_zero_quantity_on_hand_arrived_date_time(
			application_name,
			inventory->inventory_name,
			"max" /* function */ );

	inventory->inventory_purchase_list =
		inventory_purchase_arrived_date_get_list(
			application_name,
			inventory->inventory_name,
			earliest_arrived_date_time,
			latest_arrived_date_time );

	if ( inventory_sale->completed_date_time )
	{
		inventory_sale->cost_of_goods_sold =
		   inventory_sale_get_cost_of_goods_sold(
			inventory->inventory_purchase_list,
			&inventory->last_inventory_balance->total_cost_balance,
			&inventory->last_inventory_balance->quantity_on_hand,
			inventory->last_inventory_balance->average_unit_cost,
			inventory_sale->sold_quantity,
			inventory_cost_method,
			inventory_sale->completed_date_time );
	}

	return inventory;

}

double inventory_sale_get_cost_of_goods_sold(
			LIST *inventory_purchase_list,
			double *total_cost_balance,
			int *quantity_on_hand,
			double average_unit_cost,
			int sale_quantity,
			enum inventory_cost_method inventory_cost_method,
			char *completed_date_time )
{
	double cost_of_goods_sold;

	cost_of_goods_sold =
		inventory_sale_get_average_cost_of_goods_sold(
			total_cost_balance,
			quantity_on_hand,
			average_unit_cost,
			sale_quantity );

	if ( inventory_cost_method == inventory_fifo )
	{
		cost_of_goods_sold =
			inventory_decrement_quantity_on_hand_get_CGS_fifo(
				inventory_purchase_list,
				sale_quantity );
	}
	else
	if ( inventory_cost_method == inventory_average )
	{
		inventory_purchase_fifo_decrement_quantity_on_hand(
			inventory_purchase_list,
			sale_quantity );
	}
	else
	{
		cost_of_goods_sold =
			inventory_decrement_quantity_on_hand_get_CGS_lifo(
				inventory_purchase_list,
				sale_quantity,
				completed_date_time );
	}

	return cost_of_goods_sold;

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

/*
			capitalized_extra =
				(extra_cost * percent_of_total) /
				(double)inventory_purchase->ordered_quantity;
*/

			capitalized_extra =
				(extra_cost * percent_of_total) /
				(double)
				inventory_purchase_get_quantity_minus_returned(
					inventory_purchase->ordered_quantity,
					inventory_purchase->
						inventory_purchase_return_list);

			inventory_purchase->capitalized_unit_cost =
				inventory_purchase->unit_cost +
				capitalized_extra;

		}

	} while ( list_next( inventory_purchase_list ) );

}

char *inventory_balance_list_display(LIST *inventory_balance_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	INVENTORY_BALANCE *inventory_balance;

	if ( !list_rewind( inventory_balance_list ) ) return strdup( "" );

	*ptr = '\0';

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( inventory_balance->inventory_purchase )
		{
			ptr += sprintf( ptr,
					"purchase_date_time = %s, "
					"ordered_quantity = %d, "
					"capitalized_unit_cost = %4lf, ",
					inventory_balance->
						inventory_purchase->
						purchase_date_time,
					inventory_balance->
						inventory_purchase->
						ordered_quantity,
					inventory_balance->
						inventory_purchase->
						capitalized_unit_cost );
		}
		else
		if ( inventory_balance->inventory_sale )
		{
			ptr += sprintf( ptr,
					"sale_date_time = %s, "
					"quantity = %d, "
					"extension = %.2lf, "
					"cost_of_goods_sold = %.2lf, ",
					inventory_balance->
						inventory_sale->
						sale_date_time,
					inventory_balance->
						inventory_sale->
						sold_quantity,
					inventory_balance->
						inventory_sale->
						extension,
					inventory_balance->
						inventory_sale->
						cost_of_goods_sold );
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: both inventory_purchase and inventory_sale are missing.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		ptr += sprintf( ptr,
				"quantity_on_hand = %d, "
				"average_unit_cost = %.4lf, "
				"total_cost_balance = %.2lf\n",
				inventory_balance->quantity_on_hand,
				inventory_balance->average_unit_cost,
				inventory_balance->total_cost_balance );

	} while ( list_next( inventory_balance_list ) );

	return strdup( buffer );

}

LIST *inventory_get_latest_inventory_purchase_list(
				char *application_name,
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
		inventory_get_latest_zero_quantity_on_hand_arrived_date_time(
			application_name,
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

	select = inventory_purchase_get_select();

	join_where = inventory_get_inventory_purchase_join_where();

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
		 application_name,
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
				&inventory_purchase->extension,
				&inventory_purchase->database_extension,
				&inventory_purchase->capitalized_unit_cost,
				&inventory_purchase->
					database_capitalized_unit_cost,
				&inventory_purchase->arrived_quantity,
				&inventory_purchase->database_arrived_quantity,
				&inventory_purchase->missing_quantity,
				&inventory_purchase->quantity_on_hand,
				&inventory_purchase->database_quantity_on_hand,
				&inventory_purchase->average_unit_cost,
				&inventory_purchase->database_average_unit_cost,
				&inventory_purchase->arrived_date_time,
				input_buffer );

		list_append_pointer(
			inventory_purchase_list,
			inventory_purchase );
	}

	pclose( input_pipe );

	return inventory_purchase_list;

}

INVENTORY_COST_ACCOUNT *inventory_cost_account_new(
				char *account_name )
{
	INVENTORY_COST_ACCOUNT *h =
		(INVENTORY_COST_ACCOUNT *)
			calloc(	1,
				sizeof( INVENTORY_COST_ACCOUNT ) );

	if ( !h )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	h->account_name = account_name;
	return h;

}

INVENTORY_COST_ACCOUNT *inventory_get_or_set_cost_account(
				LIST *inventory_cost_account_list,
				char *account_name )
{
	INVENTORY_COST_ACCOUNT *inventory_cost_account;

	if ( !inventory_cost_account_list )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: null inventory_cost_account_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( inventory_cost_account_list ) )
	{
		inventory_cost_account =
			inventory_cost_account_new(
				account_name );

		list_append_pointer(	inventory_cost_account_list,
					inventory_cost_account );

		return inventory_cost_account;
	}

	do {
		inventory_cost_account =
			list_get_pointer(
				inventory_cost_account_list );

		if ( strcmp(	inventory_cost_account->account_name,
				account_name ) == 0 )
		{
			return inventory_cost_account;
		}

	} while( list_next( inventory_cost_account_list ) );

	list_append_pointer(	inventory_cost_account_list,
				inventory_cost_account );

	return inventory_cost_account;

}

char *inventory_get_inventory_account_name(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 256 ];
	char where[ 128 ];
	char escape_buffer[ 128 ];

	sprintf(	where,
			"inventory_name = '%s'",
		 	escape_character(
				escape_buffer,
				inventory_name,
				'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=inventory_account	"
		 "			folder=inventory		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

}

void inventory_balance_list_average_table_display(
				FILE *output_pipe,
				LIST *inventory_balance_list )
{
	INVENTORY_BALANCE *inventory_balance;

	if ( !list_rewind( inventory_balance_list ) ) return;

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( inventory_balance->inventory_purchase )
		{
			fprintf(output_pipe,
				"%s^Purchase^%s^%d^%.4lf^",
				inventory_balance->
					inventory_purchase->
					arrived_date_time,
				inventory_balance->
					inventory_purchase->
					full_name,
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
						inventory_purchase->
						ordered_quantity,
					inventory_balance->
						inventory_purchase->
						inventory_purchase_return_list),
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost );
		}
		else
		if ( inventory_balance->inventory_sale )
		{
			fprintf(output_pipe,
				"%s^Sale^%s^%d^^%.4lf",
				inventory_balance->
					inventory_sale->
					completed_date_time,
				inventory_balance->
					inventory_sale->
					full_name,
				inventory_balance->
					inventory_sale->
					sold_quantity,
				inventory_balance->
					inventory_sale->
					cost_of_goods_sold );
		}
		else
		{
			fprintf( output_pipe,
	"ERROR both inventory_purchase and inventory_sale are missing.\n" );
		}

		fprintf(output_pipe,
			"^%d^%.4lf^%.2lf\n",
			inventory_balance->quantity_on_hand,
			inventory_balance->average_unit_cost,
			inventory_balance->total_cost_balance );

	} while ( list_next( inventory_balance_list ) );

}

void inventory_folder_table_display(
			FILE *output_pipe,
			char *application_name,
			char *inventory_name,
			char *heading )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char where[ 256 ];

	if ( heading && *heading )
	{
		fprintf( output_pipe, "%s\n", heading );
	}

	select = "quantity_on_hand, average_unit_cost, total_cost_balance";

	sprintf( where, "inventory_name = '%s'", inventory_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=inventory	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
	{
		fprintf( output_pipe, "Error: cannot fetch\n" );
	}
	else
	{
		fprintf( output_pipe, "%s\n", results );
	}

}

char *inventory_get_where( char *inventory_name )
{
	static char where[ 256 ];
	char escape_buffer[ 256 ];

	sprintf(	where,
			"inventory_name = '%s'",
		 	escape_character(
				escape_buffer,
				inventory_name,
				'\'' ) );

	return where;

}

int inventory_purchase_get_quantity_minus_returned(
				int quantity,
				LIST *inventory_purchase_return_list )
{
	return	quantity -
		inventory_purchase_get_returned_quantity(
			inventory_purchase_return_list );

}

int inventory_sale_get_quantity_minus_returned(
				int quantity,
				LIST *inventory_sale_return_list )
{
	return	quantity -
		inventory_sale_get_returned_quantity(
			inventory_sale_return_list );

}

int inventory_purchase_get_returned_quantity(
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

int inventory_sale_get_returned_quantity(
				LIST *inventory_sale_return_list )
{
	INVENTORY_SALE_RETURN *i;
	int returned_quantity;

	if ( !list_rewind( inventory_sale_return_list ) ) return 0;

	returned_quantity = 0;

	do {
		i = list_get( inventory_sale_return_list );

		returned_quantity += i->returned_quantity;

	} while( list_next( inventory_sale_return_list ) );

	return returned_quantity;

}

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity,	*/
/*      inventory_sale.layer_inventory_purchase_list,	*/
/*      inventory_sale.cost_of_goods_sold.		*/
/* ---------------------------------------------------- */
void inventory_set_layer_inventory_purchase_list(
				LIST *inventory_purchase_list,
				LIST *inventory_sale_list,
				boolean is_fifo )
{
	INVENTORY_SALE *inventory_sale;

	if ( !list_rewind( inventory_sale_list ) ) return;

	do {
		inventory_sale = list_get( inventory_sale_list );

		/* ---------------------------------------------------- */
		/* Sets inventory_purchase.quantity_on_hand,		*/
		/*      inventory_purchase.layer_consumed_quantity	*/
		/* ---------------------------------------------------- */
		inventory_sale->layer_inventory_purchase_list =
			inventory_get_layer_inventory_purchase_list(
				&inventory_sale->cost_of_goods_sold,
				inventory_purchase_list,
				inventory_sale->sold_quantity,
				is_fifo );

	} while( list_next( inventory_sale_list ) );

}

/* ---------------------------------------------------- */
/* Sets inventory_purchase.quantity_on_hand,		*/
/*      inventory_purchase.layer_consumed_quantity	*/
/* ---------------------------------------------------- */
LIST *inventory_get_layer_inventory_purchase_list(
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

/* Use this for fifo and lifo */
/* -------------------------- */
void inventory_balance_list_table_display(
				FILE *output_pipe,
				LIST *inventory_balance_list )
{
	INVENTORY_BALANCE *inventory_balance;
	INVENTORY_PURCHASE *layer_inventory_purchase;
	LIST *layer_inventory_purchase_list;

	if ( !list_rewind( inventory_balance_list ) ) return;

	do {
		inventory_balance = list_get( inventory_balance_list );

		if ( inventory_balance->inventory_purchase )
		{
			if ( !inventory_balance->
					inventory_purchase->
					arrived_date_time )
			{
				fprintf( stderr,
			"Error in %s/%s()/%d: empty arrived_date_time.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
"\nPurchase: Arrived=%s, Vendor=%s, Net Ordered=%d, Capitalized Unit Cost=%.4lf\n",
				inventory_balance->
					inventory_purchase->
					arrived_date_time,
				inventory_balance->
					inventory_purchase->
					full_name,
				inventory_purchase_get_quantity_minus_returned(
					inventory_balance->
						inventory_purchase->
						ordered_quantity,
					inventory_balance->
						inventory_purchase->
						inventory_purchase_return_list),
				inventory_balance->
					inventory_purchase->
					capitalized_unit_cost );
		}
		else
		if ( inventory_balance->inventory_sale )
		{
			if ( !inventory_balance->
					inventory_sale->
					completed_date_time )
			{
				fprintf( stderr,
			"Error in %s/%s()/%d: empty completed_date_time.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
"\nSale: Completed=%s, Customer=%s, Sold=%d, Cost of Goods Sold=%.2lf\n",
				inventory_balance->
					inventory_sale->
					completed_date_time,
				inventory_balance->
					inventory_sale->
					full_name,
				inventory_balance->
					inventory_sale->
					sold_quantity,
				inventory_balance->
					inventory_sale->
					cost_of_goods_sold );

			layer_inventory_purchase_list =
				inventory_balance->
					inventory_sale->
					layer_inventory_purchase_list;

			if ( !list_rewind( layer_inventory_purchase_list ) )
			{
				fprintf( stderr,
"Error in %s/%s()/%d: empty layer_inventory_purchase_list.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			do {
				layer_inventory_purchase =
					list_get(
						layer_inventory_purchase_list );

				fprintf( output_pipe,
"\tPurchase arrived_date_time = %s, layer_consumed_quantity = %d, capitalized_unit_cost = %.4lf, layer_extension = %.2lf\n",
				layer_inventory_purchase->
					arrived_date_time,
				layer_inventory_purchase->
					layer_consumed_quantity,
				layer_inventory_purchase->
					capitalized_unit_cost,
				(double)layer_inventory_purchase->
					layer_consumed_quantity *
				layer_inventory_purchase->
					capitalized_unit_cost );

			} while( list_next( layer_inventory_purchase_list ) );
		}
		else
		{
			fprintf( output_pipe,
	"ERROR both inventory_purchase and inventory_sale are missing.\n" );
		}

		fprintf(output_pipe,
"\tQuantity on Hand=%d\n",
			inventory_balance->quantity_on_hand );

	} while ( list_next( inventory_balance_list ) );

}

enum inventory_cost_method inventory_cost_method_resolve(
				char *inventory_cost_method_string )
{
	if ( strcasecmp( inventory_cost_method_string, "fifo" ) == 0 )
		return inventory_fifo;
	else
	if ( strcasecmp( inventory_cost_method_string, "lifo" ) == 0 )
		return inventory_lifo;
	else
	if ( strcasecmp( inventory_cost_method_string, "average" ) == 0 )
		return inventory_average;
	else
	if ( strcasecmp(	inventory_cost_method_string,
				"moving_average" ) == 0 )
		return inventory_average;
	else
		return inventory_not_set;
}

char *inventory_select( void )
{
	return
		"inventory_name,"
		"inventory_account,"
		"cost_of_goods_sold_account";
}

INVENTORY *inventory_parse( char *input )
{
	INVENTORY *inventory;
	char inventory_name[ 128 ];
	char piece_buffer[ 128 ];

	piece( inventory_name, SQL_DELIMITER, input, 0 );

	inventory = inventory_new( strdup( inventory_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	inventory->inventory_account_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	inventory->cost_of_goods_sold_account_name = strdup( piece_buffer );

	return inventory;
}

LIST *inventory_list( void )
{
	INVENTORY *inventory;
	LIST *inventory_list;
	char sys_string[ 512 ];
	char input[ 512 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "echo \"select %s from %s order by %s;\" | sql",
		 inventory_select(),
		 "inventory",
		 inventory_select() );

	input_pipe = popen( sys_string, "r" );

	inventory_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		list_set( inventory_list, inventory_parse( input ) );
	}

	pclose( input_pipe );
	return inventory_list;
}

