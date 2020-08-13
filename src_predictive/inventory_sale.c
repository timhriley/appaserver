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

char *inventory_sale_select( void )
{
	return
	"full_name,"
	"street_address,"
	"sale_date_time,"
	"inventory_name,"
	"quantity,"
	"retail_price,"
	"discount_amount,"
	"extended_price,"
	"cost_of_goods_sold";
}

INVENTORY_SALE *inventory_sale_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char inventory_name[ 128 ];
	char piece_buffer[ 1024 ];
	INVENTORY_SALE *inventory_sale;

	if ( !input ) return (INVENTORY_SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( inventory_name, SQL_DELIMITER, input, 3 );

	inventory_sale =
		inventory_sale_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			strdup( inventory_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	inventory_sale->quantity = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	inventory_sale->retail_price = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	inventory_sale->discount_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	inventory_sale->inventory_sale_extended_price = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	inventory_sale->inventory_sale_cost_of_goods_sold =
		atof( piece_buffer );

	return inventory_sale;
}

double inventory_sale_extended_price_total(
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
				inventory_sale->retail_price );

	} while ( list_next( inventory_sale_list ) );

	return total;
}

double inventory_sale_sales_tax(
			double extended_price_total,
			double entity_state_sales_tax_rate )
{
	return extended_price_total * entity_state_sales_tax_rate;
}

double inventory_sale_extended_price(
			int quantity,
			double retail_price )
{
	return (double)quantity * retail_price;
}
