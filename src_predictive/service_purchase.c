/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/service_purchase.c			*/
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
#include "service_purchase.h"

SERVICE_PURCHASE *service_purchase_new( char *service_name )
{
	SERVICE_PURCHASE *service_purchase;

	service_purchase = service_purchase_calloc();
	service_purchase->service_name = service_name;

	return service_purchase;
}

SERVICE_PURCHASE *service_purchase_calloc( void )
{
	SERVICE_PURCHASE *service_purchase;

	if ( ! ( service_purchase =
			calloc( 1, sizeof( SERVICE_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return service_purchase;
}

LIST *service_purchase_list(
		const char *service_purchase_select,
		const char *service_purchase_table,
		char *purchase_primary_where )
{
	char *system_string;
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list = list_new();
	SERVICE_PURCHASE *service_purchase;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)service_purchase_select,
			(char *)service_purchase_table,
			purchase_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		service_purchase = service_purchase_parse( input );

		if ( !service_purchase )
		{
			char message[ 2048 ];

			pclose( input_pipe );

			snprintf(
				message,
				sizeof ( message ),
			"service_purchase_parse(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, service_purchase );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

SERVICE_PURCHASE *service_purchase_parse( char *input )
{
	char service_name[ 128 ];
	char buffer[ 1024 ];
	SERVICE_PURCHASE *service_purchase;

	if ( !input || !*input ) return NULL;

	piece( service_name, SQL_DELIMITER, input, 0 );

	service_purchase =
		service_purchase_new(
			strdup( service_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) service_purchase->account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		service_purchase->service_cost =
			atof( buffer );

	return service_purchase;
}

double service_purchase_list_total( LIST *service_purchase_list )
{
	SERVICE_PURCHASE *service_purchase;
	double total = 0.0;

	if ( list_rewind( service_purchase_list ) )
	do {
		service_purchase =
			list_get(
				service_purchase_list );

		total += service_purchase->service_cost;

	} while ( list_next( service_purchase_list ) );

	return total;
}
