/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset_purchase.c		*/
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
#include "prepaid_asset_purchase.h"

PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_new( char *asset_name )
{
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;

	prepaid_asset_purchase = prepaid_asset_purchase_calloc();
	prepaid_asset_purchase->asset_name = asset_name;

	return prepaid_asset_purchase;
}

PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_calloc( void )
{
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;

	if ( ! ( prepaid_asset_purchase =
			calloc( 1, sizeof( PREPAID_ASSET_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return prepaid_asset_purchase;
}

LIST *prepaid_asset_purchase_list(
		const char *prepaid_asset_purchase_select,
		const char *prepaid_asset_purchase_table,
		char *purchase_primary_where )
{
	char *system_string;
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list = list_new();
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)prepaid_asset_purchase_select,
			(char *)prepaid_asset_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		prepaid_asset_purchase = prepaid_asset_purchase_parse( input );

		if ( !prepaid_asset_purchase )
		{
			char message[ 2048 ];

			pclose( input_pipe );

			snprintf(
				message,
				sizeof ( message ),
			"prepaid_asset_purchase_parse(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, prepaid_asset_purchase );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_parse( char *input )
{
	char asset_name[ 128 ];
	char buffer[ 1024 ];
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;

	if ( !input || !*input ) return NULL;

	piece( asset_name, SQL_DELIMITER, input, 0 );

	prepaid_asset_purchase =
		prepaid_asset_purchase_new(
			strdup( asset_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) prepaid_asset_purchase->extended_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		prepaid_asset_purchase->accrual_period_years =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		prepaid_asset_purchase->accumulated_accrual =
			atof( buffer );

	return prepaid_asset_purchase;
}

double prepaid_asset_purchase_list_total( LIST *prepaid_asset_purchase_list )
{
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;
	double total = 0.0;

	if ( list_rewind( prepaid_asset_purchase_list ) )
	do {
		prepaid_asset_purchase =
			list_get(
				prepaid_asset_purchase_list );

		total += prepaid_asset_purchase->extended_cost;

	} while ( list_next( prepaid_asset_purchase_list ) );

	return total;
}
