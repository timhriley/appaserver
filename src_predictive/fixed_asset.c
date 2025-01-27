/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "sql.h"
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
	char piece_buffer[ 128 ];
	FIXED_ASSET *fixed_asset;

	if ( !input ) return (FIXED_ASSET *)0;

	/* See attribute_list fixed_asset */
	/* ------------------------------ */
	piece( asset_name, SQL_DELIMITER, input, 0 );

	fixed_asset =
		fixed_asset_new(
			strdup( asset_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	fixed_asset->account_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	fixed_asset->activity_energy_kilowatt_draw = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	fixed_asset->activity_depreciation_per_hour = atof( piece_buffer );

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_fetch( char *asset_name )
{
	return
	fixed_asset_parse(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			fixed_asset_system_string(
				FIXED_ASSET_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_primary_where(
					asset_name ) ) ) );
}

char *fixed_asset_primary_where( char *asset_name )
{
	static char where[ 256 ];

	sprintf(where,
		"asset_name = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_name_escape( asset_name ) );

	return where;
}

char *fixed_asset_system_string(
		char *fixed_asset_table,
		char *where )
{
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		fixed_asset_table,
		where );

	return system_string;
}

char *fixed_asset_name_escape( char *asset_name )
{
	static char escape[ 128 ];

	return string_escape_quote(
		escape,
		asset_name );
}

