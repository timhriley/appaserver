/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/canvass_waypoint.c	   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "canvass_waypoint.h"

CANVASS_WAYPOINT *canvass_waypoint_new(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone,
		int maximum_weight,
		LIST *waypoint_lonlat_list )
{
	CANVASS_WAYPOINT *canvass_waypoint;

	if ( !start_longitude_string
	||   !start_latitude_string
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

	canvass_waypoint = canvass_waypoint_calloc();

	canvass_waypoint->waypoint =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		waypoint_new(
			start_longitude_string,
			start_latitude_string,
			utm_zone,
			waypoint_lonlat_list );

	canvass_waypoint->waypoint_utm_list =
		canvass_waypoint_utm_list(
			maximum_weight,
			canvass_waypoint->
				waypoint->
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

LIST *canvass_waypoint_utm_list(
		int maximum_weight,
		LIST *distance_sort_list )
{
	LIST *waypoint_utm_list = list_new();
	int total_weight = 0;
	WAYPOINT_UTM *waypoint_utm;

	while ( 1 )
	{
		if ( !list_rewind( distance_sort_list ) ) break;

		waypoint_utm =
			list_get(
				distance_sort_list );

		total_weight += waypoint_utm->weight;

		if ( total_weight > maximum_weight ) break;

		list_set( waypoint_utm_list, waypoint_utm );

		list_delete( distance_sort_list );

		waypoint_utm_distance_set(
			waypoint_utm /* waypoint_utm_start */,
			distance_sort_list /* in/out */ );

		distance_sort_list =
			waypoint_utm_distance_sort_list(
				distance_sort_list );
	}
 
	return waypoint_utm_list;
}
