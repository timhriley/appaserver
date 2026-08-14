/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.c			*/
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
#include "security.h"
#include "fixed_asset.h"

FIXED_ASSET *fixed_asset_calloc( void )
{
	FIXED_ASSET *fixed_asset;

	if ( ! ( fixed_asset =
			calloc( 1, sizeof ( FIXED_ASSET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_new( char *asset_name )
{
	FIXED_ASSET *fixed_asset;

	if ( !asset_name )
	{
		char message[ 128 ];

		sprintf(message, "asset_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	fixed_asset = fixed_asset_calloc();
	fixed_asset->asset_name = asset_name;

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_parse( char *input )
{
	char asset_name[ 128 ];
	char buffer[ 128 ];
	FIXED_ASSET *fixed_asset;

	if ( !input || !*input ) return NULL;

	/* See FIXED_ASSET_SELECT */
	/* ---------------------- */
	piece( asset_name, SQL_DELIMITER, input, 0 );

	fixed_asset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_asset_new(
			strdup( asset_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) fixed_asset->asset_account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) fixed_asset->credit_account_name = strdup( buffer );

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_fetch( char *asset_name )
{
	return
	fixed_asset_parse(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				FIXED_ASSET_SELECT,
				FIXED_ASSET_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_primary_where(
					asset_name ) ) ) );
}

char *fixed_asset_primary_where( char *asset_name )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"asset_name = '%s'",
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		security_escape( asset_name ) );

	return where;
}

