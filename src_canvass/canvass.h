/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "boolean.h"
#include "list.h"

#ifndef CANVASS_H
#define CANVASS_H

#define CANVASS_UTM_ZONE 10

typedef struct
{
	STREET *start_street;
	WAYPOINT *start_waypoint;
	LIST *street_list;
	LIST *canvass_street_list;
	LIST *waypoint_lonlat_list;
} CANVASS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CANVASS *canvass_new(
		char *canvass_name,
		char *city,
		char *start_street_address );

/* Process */
/* ------- */
CANVASS *canvass_calloc(
		void );

#endif
