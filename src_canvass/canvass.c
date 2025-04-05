/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "canvass.h"

CANVASS *canvass_new( char *canvass_name )
{
	CANVASS *canvass;
	char buffer[ 128 ];

	canvass = canvass_calloc();

	if ( ! ( canvass->start_record = canvass_start_record() ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: canvass_start_record() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	piece( buffer, SQL_DELIMITER, canvass->start_record, 0 );
	canvass->start_longitude_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, canvass->start_record, 1 );
	canvass->start_latitude_string = strdup( buffer );

	canvass->canvass_street_include_list =
		canvass_street_include_list(
			canvass_name );

	canvass->canvass_street_include_list_weight =
		canvass_street_include_list_weight(
			canvass->canvass_street_include_list );

	canvass->include_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_include_list );

	canvass->canvass_street_not_include_list =
		canvass_street_not_include_list(
			canvass_name );

	canvass->not_include_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_not_include_list );

	return canvass;
}

CANVASS *canvass_calloc( void )
{
	CANVASS *canvass;

	if ( ! ( canvass = calloc( 1, sizeof ( CANVASS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return canvass;
}

char *canvass_start_record( void )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe( "canvass_start.sh" );
}

