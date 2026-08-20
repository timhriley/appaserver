/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply_purchase.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "piece.h"
#include "sql.h"
#include "purchase.h"
#include "supply_purchase.h"

SUPPLY_PURCHASE *supply_purchase_new( char *supply_name )
{
	SUPPLY_PURCHASE *supply_purchase;

	supply_purchase = supply_purchase_calloc();
	supply_purchase->supply_name = supply_name;

	return supply_purchase;
}

SUPPLY_PURCHASE *supply_purchase_calloc( void )
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

	return supply_purchase;
}

SUPPLY_PURCHASE_LIST *supply_purchase_list_new(
		const char *supply_purchase_select,
		const char *supply_purchase_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *purchase_primary_where )
{
	char *system_string;
	char input[ 1024 ];
	FILE *input_pipe;
	SUPPLY_PURCHASE_LIST *supply_purchase_list;
	SUPPLY_PURCHASE *supply_purchase;

	supply_purchase_list = supply_purchase_list_calloc();
	supply_purchase_list->list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)supply_purchase_select,
			(char *)supply_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		supply_purchase = supply_purchase_parse( input );

		if ( !supply_purchase )
		{
			char message[ 2048 ];

			pclose( input_pipe );

			snprintf(
				message,
				sizeof ( message ),
				"supply_purchase_parse(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		supply_purchase->extended_cost =
			SUPPLY_PURCHASE_EXTENDED_COST(
				supply_purchase->quantity,
				supply_purchase->unit_cost );

		supply_purchase->update_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			supply_purchase_update_string(
				SQL_DELIMITER,
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				supply_purchase->supply_name,
				fund_boolean,
				contact_key_boolean,
				supply_purchase->extended_cost );

		list_set( supply_purchase_list->list, supply_purchase );
	}

	pclose( input_pipe );

	if ( !list_length( supply_purchase_list->list ) )
	{
		list_free( supply_purchase_list->list );
		supply_purchase_list->list = NULL;

		return supply_purchase_list;
	}

	supply_purchase_list->primary_key_list =
		supply_purchase_list_primary_key_list(
			PURCHASE_SUPPLY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	supply_purchase_list->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		supply_purchase_list_update_system_string(
			supply_purchase_table,
			supply_purchase_list->primary_key_list );

	supply_purchase_list->update_string_list =
		supply_purchase_list_update_string_list(
			supply_purchase_list->list
				/* supply_purchase_list */ );

	return supply_purchase_list;
}

SUPPLY_PURCHASE *supply_purchase_parse( char *input )
{
	char supply_name[ 128 ];
	char buffer[ 1024 ];
	SUPPLY_PURCHASE *supply_purchase;

	if ( !input || !*input ) return NULL;

	piece( supply_name, SQL_DELIMITER, input, 0 );

	supply_purchase = supply_purchase_new( strdup( supply_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) supply_purchase->quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) supply_purchase->unit_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) supply_purchase->extended_cost = atof( buffer );

	return supply_purchase;
}

double supply_purchase_list_total( LIST *supply_purchase_list )
{
	SUPPLY_PURCHASE *supply_purchase;
	double total = 0.0;

	if ( list_rewind( supply_purchase_list ) )
	do {
		supply_purchase = list_get( supply_purchase_list );

		total += supply_purchase->extended_cost;

	} while ( list_next( supply_purchase_list ) );

	return total;
}

LIST *supply_purchase_list_update_string_list( LIST *supply_purchase_list )
{
	SUPPLY_PURCHASE *supply_purchase;
	LIST *list = list_new();

	if ( list_rewind( supply_purchase_list ) )
	do {
		supply_purchase = list_get( supply_purchase_list );
		list_set( list, supply_purchase->update_string );

	} while ( list_next( supply_purchase_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}
