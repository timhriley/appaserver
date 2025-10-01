/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/waypoint.h		   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "boolean.h"
#include "list.h"

#define WAYPOINT_CS2CS_EXECUTABLE	"cs2cs_lonlat.sh"

typedef struct
{
	void *record;
	char *longitude_string;
	char *latitude_string;
} WAYPOINT_LONLAT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_LONLAT *waypoint_lonlat_new(
		void *record,
		char *longitude_string,
		char *latitude_string );

/* Process */
/* ------- */
WAYPOINT_LONLAT *waypoint_lonlat_calloc(
		void );

typedef struct
{
	void *record;
	int utm_x;
	int utm_y;

	/* Set externally */
	/* -------------- */
	int distance_yards;
} WAYPOINT_UTM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_UTM *home_waypoint_utm(
		char *home_longitude_string,
		char *home_latitude_string,
		int utm_zone );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_UTM *waypoint_utm_column_new(
		void *record,
		char *column_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_UTM *waypoint_utm_new(
		void *record,
		char *utm_x_string,
		char *utm_y_string );

/* Process */
/* ------- */
WAYPOINT_UTM *waypoint_utm_calloc(
		void );

/* Usage */
/* ----- */
void waypoint_utm_distance_set(
		WAYPOINT_UTM *home_waypoint_utm,
		LIST *waypoint_utm_list /* in/out */ );

/* Usage */
/* ----- */
int waypoint_utm_distance_yards(
		WAYPOINT_UTM *home_waypoint_utm,
		int utm_x,
		int utm_y );

/* Usage */
/* ----- */
LIST *waypoint_utm_distance_sort_list(
		LIST *waypoint_utm_list );

/* Usage */
/* ----- */
int waypoint_utm_compare(
		WAYPOINT_UTM *waypoint_utm_from_list,
		WAYPOINT_UTM *waypoint_utm_to_set );

typedef struct
{
	/* Stub */
} WAYPOINT;

/* Usage */
/* ----- */
LIST *waypoint_utm_list(
		int utm_zone,
		LIST *waypoint_lonlat_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *waypoint_system_string(
		const char *waypoint_cs2cs_executable,
		int utm_zone );

#endif
