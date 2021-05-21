/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply_purchase.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "sql.h"
#include "environ.h"
#include "purchase.h"
#include "supply_purchase.h"

SUPPLY_PURCHASE *supply_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *supply_name )
{
	SUPPLY_PURCHASE *supply_purchase;

	if ( ! ( supply_purchase =
			calloc( 1, sizeof( SUPPLY_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	supply_purchase->vendor_entity =
		entity_new(
			full_name,
			street_address );

	supply_purchase->supply =
		supply_fetch(
			supply_name );

	supply_purchase->purchase_date_time = purchase_date_time;

	return supply_purchase;
}

LIST *supply_purchase_system_list(
			char *system_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			supply_purchase_parse( input ) );
	}
	pclose( input_pipe );
	return list;
}

char *supply_purchase_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" supply_name",
		 SUPPLY_PURCHASE_TABLE,
		 where );

	return strdup( system_string );
}

LIST *supply_purchase_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	return supply_purchase_system_list(
		supply_purchase_system_string(
			purchase_primary_where(
				full_name,
				street_address,
				purchase_date_time ) ) );
}

SUPPLY_PURCHASE *supply_purchase_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char supply_name[ 128 ];
	char piece_buffer[ 1024 ];
	SUPPLY_PURCHASE *supply_purchase;

	if ( !input || !*input ) return (SUPPLY_PURCHASE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( supply_name, SQL_DELIMITER, input, 2 );
	piece( purchase_date_time, SQL_DELIMITER, input, 3 );

	supply_purchase =
		supply_purchase_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ),
			strdup( supply_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	supply_purchase->quantity = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	supply_purchase->unit_cost = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	supply_purchase->extended_cost = atof( piece_buffer );

	return supply_purchase;
}

double supply_purchase_total(
			LIST *supply_purchase_list )
{
	SUPPLY_PURCHASE *supply_purchase;
	double total;

	if ( !list_rewind( supply_purchase_list ) ) return 0.0;

	total = 0.0;

	do {
		supply_purchase = list_get( supply_purchase_list );

		total += supply_purchase->extended_cost;

	} while ( list_next( supply_purchase_list ) );

	return total;
}

