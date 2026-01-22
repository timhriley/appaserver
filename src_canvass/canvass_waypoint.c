/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/canvass_waypoint.c	   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include "canvass_street.h"
#include "canvass_waypoint.h"

CANVASS_WAYPOINT *canvass_waypoint_new(
		char *home_longitude_string,
		char *home_latitude_string,
		int radius_yards,
		int utm_zone,
		LIST *canvass_waypoint_utm_list )
{
	CANVASS_WAYPOINT *canvass_waypoint;

	if ( !home_longitude_string
	||   !home_latitude_string
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

	canvass_waypoint = canvass_waypoint_calloc();

	canvass_waypoint->home_waypoint_utm =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		home_waypoint_utm(
			home_longitude_string,
			home_latitude_string,
			utm_zone );

	canvass_waypoint->start_waypoint_utm =
		canvass_waypoint_start_utm(
			canvass_waypoint_utm_list,
			canvass_waypoint->home_waypoint_utm );

	if ( !canvass_waypoint->start_waypoint_utm ) return canvass_waypoint;

	canvass_waypoint->radius_utm_list =
		canvass_waypoint_radius_utm_list(
			radius_yards,
			canvass_waypoint_utm_list,
			canvass_waypoint->start_waypoint_utm );

	canvass_waypoint->waypoint_utm_distance_sort_list =
		waypoint_utm_distance_sort_list(
			canvass_waypoint->radius_utm_list );

	canvass_waypoint->waypoint_utm_canvass_street_list =
		waypoint_utm_canvass_street_list(
			canvass_waypoint->
				waypoint_utm_distance_sort_list );

	return canvass_waypoint;
}

CANVASS_WAYPOINT *canvass_waypoint_calloc( void )
{
	CANVASS_WAYPOINT *canvass_waypoint;

	if ( ! ( canvass_waypoint =
			calloc( 1,
				sizeof ( CANVASS_WAYPOINT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return canvass_waypoint;
}

WAYPOINT_UTM *canvass_waypoint_start_utm(
		LIST *canvass_waypoint_utm_list,
		WAYPOINT_UTM *home_waypoint_utm )
{
	int minimum_distance = INT_MAX;
	WAYPOINT_UTM *start_waypoint_utm = {0};
	WAYPOINT_UTM *waypoint_utm;
	int distance_yards;
	CANVASS_STREET *canvass_street;

	if ( !home_waypoint_utm )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: home_waypoint_utm is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( canvass_waypoint_utm_list ) )
	do {
		waypoint_utm = list_get( canvass_waypoint_utm_list );

		canvass_street = waypoint_utm->record;

		if ( canvass_street->start_boolean )
		{
			start_waypoint_utm = waypoint_utm;
			break;
		}

		distance_yards =
			waypoint_utm_distance_yards(
				home_waypoint_utm,
				waypoint_utm->utm_x,
				waypoint_utm->utm_y );

		if ( distance_yards < minimum_distance )
		{
			start_waypoint_utm = waypoint_utm;
			minimum_distance = distance_yards;
		}

	} while ( list_next( canvass_waypoint_utm_list ) );

	return start_waypoint_utm;
}

LIST *canvass_waypoint_radius_utm_list(
		int radius_yards,
		LIST *canvass_waypoint_utm_list,
		WAYPOINT_UTM *canvass_waypoint_start_utm )
{
	LIST *radius_utm_list = list_new();
	WAYPOINT_UTM *waypoint_utm;
	CANVASS_STREET *canvass_street;
	int distance_yards;

	if ( !canvass_waypoint_start_utm )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: canvass_waypoint_start_utm is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( canvass_waypoint_utm_list ) )
	do {
		waypoint_utm = list_get( canvass_waypoint_utm_list );

		canvass_street = waypoint_utm->record;

		distance_yards =
			waypoint_utm_distance_yards(
				canvass_waypoint_start_utm,
				waypoint_utm->utm_x,
				waypoint_utm->utm_y );

		if (distance_yards <= radius_yards
		||  canvass_street->include_boolean )
		{
			waypoint_utm->distance_yards = distance_yards;
			list_set( radius_utm_list, waypoint_utm );
		}

	} while ( list_next( canvass_waypoint_utm_list ) );

	if ( !list_length( radius_utm_list ) )
	{
		list_free( radius_utm_list );
		radius_utm_list = NULL;
	}

	return radius_utm_list;
}
