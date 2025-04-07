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
	LIST *include_canvass_street_list;
	LIST *not_include_canvass_street_list;
	LIST *include_canvass_waypoint_lonlat_list;
	LIST *not_include_canvass_waypoint_lonlat_list;
	CANVASS_WAYPOINT *include_canvass_waypoint;
	STREET *continue_street;
	CANVASS_WAYPOINT *not_include_canvass_waypoint;
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
		int maximum_weight,
		int utm_zone );

/* Process */
/* ------- */
CANVASS *canvass_calloc(
		void );

/* Returns component of second parameter or first parameter */
/* -------------------------------------------------------- */
STREET *canvass_continue_street(
		STREET *start_street,
		LIST *include_canvass_street_list );

/* Driver */
/* ------ */
void canvass_output(
		LIST *include_waypoint_utm_list,
		LIST *not_include_waypoint_utm_list );

#endif
