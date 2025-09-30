/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "boolean.h"
#include "list.h"
#include "street.h"
#include "canvass_waypoint.h"

#ifndef CANVASS_H
#define CANVASS_H

typedef struct
{
	STREET *start_street;
	LIST *street_list;
	LIST *canvass_street_list;
	LIST *canvass_waypoint_lonlat_list;
	LIST *canvass_waypoint_utm_list;
	CANVASS_WAYPOINT *canvass_waypoint;
} CANVASS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CANVASS *canvass_new(
		char *start_street_name,
		char *city,
		char *state_code,
		char *canvass_name,
		int radius_yards,
		int utm_zone );

/* Process */
/* ------- */
CANVASS *canvass_calloc(
		void );

/* Usage */
/* ----- */
LIST *canvass_waypoint_lonlat_list(
		LIST *canvass_street_list );

/* Usage */
/* ----- */
LIST *canvass_waypoint_utm_list(
		int utm_zone,
		LIST *canvass_waypoint_lonlat_list );

/* Driver */
/* ------ */
void canvass_output(
		LIST *radius_utm_list );

#endif
