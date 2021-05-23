/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "sale.h"
#include "inventory_sale.h"

INVENTORY_SALE *inventory_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( ! ( inventory_sale = calloc( 1, sizeof( INVENTORY_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	inventory_sale->customer_entity =
		entity_new(
			full_name,
			street_address );

	inventory_sale->sale_date_time = sale_date_time;
	inventory_sale->inventory_name = inventory_name;

	return inventory_sale;
}

INVENTORY_SALE *inventory_sale_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char inventory_name[ 128 ];
	char quantity[ 128 ];
	char retail_price[ 128 ];
	char discount_amount[ 128 ];
	char extended_price[ 128 ];
	char cost_of_goods_sold[ 128 ];

	if ( !input || !*input ) return (INVENTORY_SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( inventory_name, SQL_DELIMITER, input, 3 );
	piece( quantity, SQL_DELIMITER, input, 4 );
	piece( retail_price, SQL_DELIMITER, input, 5 );
	piece( discount_amount, SQL_DELIMITER, input, 6 );
	piece( extended_price, SQL_DELIMITER, input, 7 );
	piece( cost_of_goods_sold, SQL_DELIMITER, input, 8 );

	return inventory_sale_steady_state(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			strdup( inventory_name ),
			atoi( quantity ),
			atof( retail_price ),
			atof( discount_amount ),
			atof( extended_price ),
			atof( cost_of_goods_sold ),
			(LIST *)0,
			(LIST *)0 );
}

double inventory_sale_total(
			LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;
	double total;

	if ( !list_rewind( inventory_sale_list ) ) return 0.0;

	total = 0.0;
	do {
		inventory_sale = list_get( inventory_sale_list );

		total += inventory_sale_extended_price(
				inventory_sale->quantity,
				inventory_sale->retail_price,
				inventory_sale->discount_amount );

	} while ( list_next( inventory_sale_list ) );

	return total;
}

double inventory_sale_sales_tax(
			double extended_price_total,
			double entity_state_sales_tax_rate )
{
	return extended_price_total * entity_state_sales_tax_rate;
}

LIST *inventory_sale_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *inventory_sale_list;

	inventory_sale_list = list_new();
	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	inventory_sale_list, 
				inventory_sale_parse( input ) );
	}

	pclose( input_pipe );
	return inventory_sale_list;
}

char *inventory_sale_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf(system_string,
		"select.sh '*' %s \"%s\" inventory_name",
		 INVENTORY_SALE_TABLE,
		 where );

	return strdup( system_string );
}

LIST *inventory_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	return inventory_sale_system_list(
			inventory_sale_system_string(
				sale_primary_where(
					full_name,
					street_address,
					sale_date_time ) ) );
}

double inventory_sale_extended_price(
			double retail_price,
			double discount_amount,
			int quantity )
{
	return ( ( retail_price *
		   (double)quantity ) -
		   discount_amount );
}

double inventory_sale_cost_of_goods_sold(
			int quantity,
			LIST *inventory_purchase_list,
			LIST *inventory_sale_list )
{
if ( quantity ){}
if ( inventory_purchase_list ){}
if ( inventory_sale_list ){}

	return 0.0;

}

/* Everything is strdup() in. */
/* -------------------------- */
INVENTORY_SALE *inventory_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			int quantity,
			double retail_price,
			double discount_amount,
			double extended_price_database,
			double cost_of_goods_sold_database,
			LIST *inventory_purchase_list,
			LIST *inventory_sale_list )
{
	INVENTORY_SALE *inventory_sale;

	inventory_sale =
		inventory_sale_new(
			full_name,
			street_address,
			sale_date_time,
			inventory_name );

	inventory_sale->quantity = quantity;
	inventory_sale->discount_amount = discount_amount;

	inventory_sale->inventory_sale_extended_price =
		inventory_sale_extended_price(
			retail_price,
			discount_amount,
			quantity );

	inventory_sale->inventory_sale_cost_of_goods_sold =
		inventory_sale_cost_of_goods_sold(
			quantity,
			inventory_purchase_list,
			inventory_sale_list );

	inventory_sale->extended_price_database =
		extended_price_database;

	inventory_sale->cost_of_goods_sold_database =
		cost_of_goods_sold_database;

	return inventory_sale;
}

