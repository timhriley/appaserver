/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/waypoint.h			   		*/
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
	int weight;
	char *longitude_string;
	char *latitude_string;
} WAYPOINT_LONLAT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_LONLAT *waypoint_lonlat_new(
		void *record,
		int weight,
		char *longitude_string,
		char *latitude_string );

/* Process */
/* ------- */
WAYPOINT_LONLAT *waypoint_lonlat_calloc(
		void );

typedef struct
{
	void *record;
	int weight;
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
WAYPOINT_UTM *waypoint_utm_column_new(
		void *record,
		int weight,
		char *column_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT_UTM *waypoint_utm_new(
		void *record,
		int weight,
		char *utm_x_string,
		char *utm_y_string );

/* Process */
/* ------- */
WAYPOINT_UTM *waypoint_utm_calloc(
		void );

/* Usage */
/* ----- */
WAYPOINT_UTM *waypoint_utm_start(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone );

/* Usage */
/* ----- */
void waypoint_utm_distance_set(
		WAYPOINT_UTM *waypoint_utm_start,
		LIST *utm_list /* in/out */ );

/* Usage */
/* ----- */
int waypoint_utm_distance_yards(
		WAYPOINT_UTM *waypoint_utm_start,
		int utm_x,
		int utm_y );

/* Usage */
/* ----- */
LIST *waypoint_utm_distance_sort_list(
		LIST *utm_list );

/* Usage */
/* ----- */
int waypoint_utm_compare(
		WAYPOINT_UTM *waypoint_utm_from_list,
		WAYPOINT_UTM *waypoint_utm_to_set );

typedef struct
{
	LIST *waypoint_lonlat_list;
	WAYPOINT_UTM *waypoint_utm_start;
	LIST *utm_list;
	LIST *waypoint_utm_distance_sort_list;
} WAYPOINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WAYPOINT *waypoint_new(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone,
		LIST *waypoint_lonlat_list );

/* Process */
/* ------- */
WAYPOINT *waypoint_calloc(
		void );

/* Usage */
/* ----- */
LIST *waypoint_utm_list(
		int utm_zone,
		LIST *lonlat_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *waypoint_system_string(
		const char *waypoint_cs2cs_executable,
		int utm_zone );

#endif
