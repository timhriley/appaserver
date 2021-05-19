/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.c			*/
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
#include "fixed_asset.h"

FIXED_ASSET *fixed_asset_new(
			char *asset_name,
			char *serial_number )
{
	FIXED_ASSET *fixed_asset;

	if ( ! ( fixed_asset =
			calloc( 1, sizeof( FIXED_ASSET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fixed_asset->asset_name = asset_name;
	fixed_asset->serial_number = serial_number;

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_parse(
			char *input )
{
	char asset_name[ 128 ];
	char serial_number[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_ASSET *fixed_asset;

	if ( !input ) return (FIXED_ASSET *)0;

	/* See attribute_list fixed_asset */
	/* ------------------------------ */
	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_number, SQL_DELIMITER, input, 1 );

	fixed_asset =
		fixed_asset_new(
			strdup( asset_name ),
			strdup( serial_number ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	fixed_asset->account_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	fixed_asset->activity_energy_kilowatt_draw = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	fixed_asset->activity_depreciation_per_hour = atof( piece_buffer );

	return fixed_asset;
}

FIXED_ASSET *fixed_asset_fetch(
			char *asset_name,
			char *serial_number )
{
	return fixed_asset_parse(
		pipe2string(
			fixed_asset_system_string(
				fixed_asset_primary_where(
					asset_name,
					serial_number ) ) ) );
}

char *fixed_asset_primary_where(
			char *asset_name,
			char *serial_number )
{
	static char where[ 256 ];

	sprintf(where,
		"asset_name = '%s' and "
		"serial_number = '%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_name_escape(
			asset_name ),
		serial_number );

	return where;
}

char *fixed_asset_system_string(
			char *where )
{
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" asset_name",
		where,
		FIXED_ASSET_TABLE );

	return system_string;
}

char *fixed_asset_name_escape(
			char *asset_name )
{
	static char asset_name_escape[ 128 ];

	return string_escape_quote(
		asset_name_escape,
		asset_name );
}

