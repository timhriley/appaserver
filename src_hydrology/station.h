/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/station.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef STATION_H
#define STATION_H

#include "boolean.h"
#include "list.h"

#define STATION_TABLE		"station"

#define STATION_SELECT		"station,"				\
				"basin,"				\
				"agency,"				\
				"park,"					\
				"station_type,"				\
				"lat_nad83,"				\
				"long_nad83,"				\
				"area_sqmi,"				\
				"ground_surface_elevation_ft,"		\
				"vertical_datum,"			\
				"comments,"				\
				"ngvd29_navd88_increment"

typedef struct
{
	char *station_name;
	char *basin;
	char *agency;
	char *park;
	char *station_type;
	double lat_nad83;
	double long_nad83;
	double area_sqmi;
	double ground_surface_elevation_ft;
	char *vertical_datum;
	char *comments;
	double ngvd29_navd88_increment;
	LIST *station_datatype_list;
} STATION;

/* Usage */
/* ----- */
LIST *station_list(	char *agency_name,
			boolean fetch_station_datatype_list,
			boolean fetch_datatype );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *station_where(	char *agency_name );

/* Usage */
/* ----- */
STATION *station_fetch(
			char *station_name,
			boolean fetch_station_datatype_list,
			boolean fetch_datatype );


/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *station_primary_where(
			char *station_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATION *station_parse(
			boolean fetch_station_datatype_list,
			boolean fetch_datatype,
			char *string_pipe_input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATION *station_new(	char *station_name );

/* Process */
/* ------- */
STATION *station_calloc(
			void );

#endif
