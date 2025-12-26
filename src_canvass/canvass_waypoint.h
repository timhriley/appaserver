/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/canvass_waypoint.h	   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef CANVASS_WAYPOINT_H
#define CANVASS_WAYPOINT_H

#include "boolean.h"
#include "list.h"
#include "waypoint.h"

typedef struct
{
	WAYPOINT_UTM *home_waypoint_utm;
	WAYPOINT_UTM *start_waypoint_utm;
	LIST *radius_utm_list;
	LIST *waypoint_utm_distance_sort_list;
	LIST *waypoint_utm_canvass_street_list;
} CANVASS_WAYPOINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CANVASS_WAYPOINT *canvass_waypoint_new(
		char *home_longitude_string,
		char *home_latitude_string,
		int radius_yards,
		int utm_zone,
		LIST *canvass_waypoint_utm_list );

/* Process */
/* ------- */
CANVASS_WAYPOINT *canvass_waypoint_calloc(
		void );

/* Usage */
/* ----- */
WAYPOINT_UTM *canvass_waypoint_start_utm(
		LIST *canvass_waypoint_utm_list,
		WAYPOINT_UTM *home_waypoint_utm );

/* Usage */
/* ----- */
LIST *canvass_waypoint_radius_utm_list(
		int radius_yards,
		LIST *canvass_waypoint_utm_list,
		WAYPOINT_UTM *canvass_waypoint_start_utm );

#endif
