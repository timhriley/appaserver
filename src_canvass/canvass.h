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
	char *start_record;
	char *start_longitude_string;
	char *start_latitude_string;
	LIST *canvass_street_include_list;
	int canvass_street_include_list_weight;
	LIST *include_waypoint_lonlat_list;
	LIST *canvass_street_not_include_list;
	LIST *not_include_waypoint_lonlat_list;
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

char *canvass_start_record(
		char *city,
		char *start_street_address );

#endif
