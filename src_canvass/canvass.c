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
		int maximum_weight,
		int utm_zone )
{
	CANVASS *canvass;

	if ( !start_street_name
	||   !city
	||   !state_code
	||   !canvass_name
	||   !maximum_weight
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

	canvass->canvass_street_list = list_new();

	canvass_street_fetch_list(
		canvass_name,
		1 /* include_boolean */,
		canvass->street_list,
		canvass->canvass_street_list /* out */ );

	canvass_street_fetch_list(
		canvass_name,
		0 /* not include_boolean */,
		canvass->street_list,
		canvass->canvass_street_list /* out */ );

	canvass->canvass_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_list );

	canvass->canvass_waypoint =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		canvass_waypoint_new(
			canvass->start_street->longitude_string,
			canvass->start_street->latitude_string,
			utm_zone,
			maximum_weight,
			canvass->canvass_waypoint_lonlat_list );

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

void canvass_output( LIST *waypoint_utm_list )
{
	WAYPOINT_UTM *waypoint_utm;
	CANVASS_STREET *canvass_street;

	if ( list_rewind( waypoint_utm_list ) )
	do {
		waypoint_utm = list_get( waypoint_utm_list );

		canvass_street = waypoint_utm->record;

		printf(	"%s^%d^%d^%d\n",
			canvass_street->
				street->
				street_name,
			canvass_street->
				street->
				apartment_count,
			canvass_street->
				street->
				house_count,
			canvass_street->
				street->
				total_count );

	} while ( list_next( waypoint_utm_list ) );
}

