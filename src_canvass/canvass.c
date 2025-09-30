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
#include "canvass_street.h"
#include "canvass_waypoint.h"
#include "canvass.h"

CANVASS *canvass_new(
		char *start_street_name,
		char *city,
		char *state_code,
		char *canvass_name,
		int radius_yards,
		int utm_zone )
{
	CANVASS *canvass;

	if ( !start_street_name
	||   !city
	||   !state_code
	||   !canvass_name
	||   !radius_yards
	||   !utm_zone )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	canvass = canvass_calloc();

	canvass->start_street =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		street_fetch(
			start_street_name,
			city,
			state_code );

	canvass->street_list =
		street_list(
			city,
			state_code );

	canvass->canvass_street_list =
		canvass_street_list(
			canvass_name,
			canvass->street_list );

	canvass->canvass_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_list );

	canvass->canvass_waypoint_utm_list =
		canvass_waypoint_utm_list(
			utm_zone,
			canvass->canvass_waypoint_lonlat_list );

	canvass->canvass_waypoint =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		canvass_waypoint_new(
			canvass->start_street->longitude_string,
			canvass->start_street->latitude_string,
			radius_yards,
			utm_zone,
			canvass->canvass_waypoint_utm_list );

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

void canvass_output( LIST *radius_utm_list )
{
	WAYPOINT_UTM *waypoint_utm;
	CANVASS_STREET *canvass_street;

	if ( list_rewind( radius_utm_list ) )
	do {
		waypoint_utm = list_get( radius_utm_list );

		canvass_street = waypoint_utm->record;

		canvass_street_output(
			canvass_street,
			waypoint_utm->distance_yards );

	} while ( list_next( radius_utm_list ) );
}

LIST *canvass_waypoint_lonlat_list( LIST *canvass_street_list )
{
	CANVASS_STREET *canvass_street;
	LIST *lonlat_list = list_new();
	WAYPOINT_LONLAT *waypoint_lonlat;

	if ( list_rewind( canvass_street_list ) )
	do {
		canvass_street = list_get( canvass_street_list );

		if ( !canvass_street->street )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: canvass_street->street is empty\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !canvass_street->street->total_count
		||   !canvass_street->street->longitude_string
		||   !canvass_street->street->latitude_string )
		{
			continue;
		}

		waypoint_lonlat =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			waypoint_lonlat_new(
				canvass_street /* record */,
				canvass_street->street->longitude_string,
				canvass_street->street->latitude_string );

		list_set( lonlat_list, waypoint_lonlat );

	} while ( list_next( canvass_street_list ) );

	if ( !list_length( lonlat_list ) )
	{
		list_free( lonlat_list );
		lonlat_list = NULL;
	}

	return lonlat_list;
}

LIST *canvass_waypoint_utm_list(
		int utm_zone,
		LIST *canvass_waypoint_lonlat_list )
{
	return
	waypoint_utm_list(
		utm_zone,
		canvass_waypoint_lonlat_list );
}
