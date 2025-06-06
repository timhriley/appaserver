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
	WAYPOINT *waypoint;
	LIST *waypoint_utm_list;
	int total_weight;
} CANVASS_WAYPOINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CANVASS_WAYPOINT *canvass_waypoint_new(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone,
		int maximum_weight,
		LIST *waypoint_lonlat_list );

/* Process */
/* ------- */
CANVASS_WAYPOINT *canvass_waypoint_calloc(
		void );

int canvass_waypoint_total_weight(
		LIST *waypoint_utm_list );

/* Usage */
/* ----- */
LIST *canvass_waypoint_lonlat_list(
		LIST *canvass_street_list );

/* Usage */
/* ----- */
LIST *canvass_waypoint_utm_list(
		int maximum_weight,
		LIST *utm_distance_sort_list /* in/out */ );

#endif
