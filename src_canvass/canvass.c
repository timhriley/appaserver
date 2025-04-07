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

	canvass->include_canvass_street_list =
		canvass_street_fetch_list(
			canvass_name,
			1 /* include_boolean */,
			canvass->street_list );

	canvass->not_include_canvass_street_list =
		canvass_street_fetch_list(
			canvass_name,
			0 /* not include_boolean */,
			canvass->street_list );

	canvass->include_canvass_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->include_canvass_street_list );

	canvass->not_include_canvass_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->not_include_canvass_street_list );

	canvass->include_canvass_waypoint =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		canvass_waypoint_new(
			canvass->start_street->longitude_string,
			canvass->start_street->latitude_string,
			utm_zone,
			maximum_weight,
			canvass->include_canvass_waypoint_lonlat_list );

	canvass->continue_street =
		/* -------------------------------------------------------- */
		/* Returns component of second parameter or first parameter */
		/* -------------------------------------------------------- */
		canvass_continue_street(
			canvass->start_street,
			canvass->include_canvass_street_list );

	canvass->not_include_canvass_waypoint =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		canvass_waypoint_new(
			canvass->continue_street->longitude_string
				/* start_longitude_string */,
			canvass->continue_street->latitude_string
				/* start_latitude_string */,
			utm_zone,
			maximum_weight -
			canvass->include_canvass_waypoint->total_weight
				/* maximum_weight */,
			canvass->not_include_canvass_waypoint_lonlat_list );

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

void canvass_output(
		LIST *include_waypoint_utm_list,
		LIST *not_include_waypoint_utm_list )
{
	WAYPOINT_UTM *waypoint_utm;
	CANVASS_STREET *canvass_street;

	if ( list_rewind( include_waypoint_utm_list ) )
	do {
		waypoint_utm = list_get( include_waypoint_utm_list );

		canvass_street = waypoint_utm->record;

		canvass_street_output( canvass_street );

	} while ( list_next( include_waypoint_utm_list ) );

	if ( list_rewind( not_include_waypoint_utm_list ) )
	do {
		waypoint_utm = list_get( not_include_waypoint_utm_list );

		canvass_street = waypoint_utm->record;
		canvass_street_output( canvass_street );

	} while ( list_next( not_include_waypoint_utm_list ) );

}

STREET *canvass_continue_street(
		STREET *start_street,
		LIST *include_canvass_street_list )
{
	CANVASS_STREET *canvass_street;

	if ( !list_length( include_canvass_street_list ) )
		return start_street;


	canvass_street =
		/* -------------------------------- */
		/* Returns the last element or null */
		/* -------------------------------- */
		list_last(
			include_canvass_street_list );

	return canvass_street->street;
}
