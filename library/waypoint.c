/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/waypoint.c					*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "column.h"
#include "spool.h"
#include "distance.h"
#include "waypoint.h"

WAYPOINT *waypoint_calloc( void )
{
	WAYPOINT *waypoint;

	if ( ! ( waypoint = calloc( 1, sizeof ( WAYPOINT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return waypoint;
}

WAYPOINT_UTM *waypoint_utm_column_new(
		void *record,
		int weight,
		char *column_string )
{
	char utm_x_string[ 128 ];
	char utm_y_string[ 128 ];

	if ( !column_string
	||   !string_character_exists(
		column_string /* datum */,
		' ' ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty or incomplete\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	column( utm_x_string, 0, column_string );
	column( utm_y_string, 1, column_string );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	waypoint_utm_new(
		record,
		weight,
		strdup( utm_x_string ),
		strdup( utm_y_string ) );
}

WAYPOINT_UTM *waypoint_utm_new(
		void *record,
		int weight,
		char *utm_x_string,
		char *utm_y_string )
{
	WAYPOINT_UTM *waypoint_utm;

	if ( !utm_x_string
	||   !utm_y_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	waypoint_utm = waypoint_utm_calloc();
	waypoint_utm->record = record;
	waypoint_utm->weight = weight;
	waypoint_utm->utm_x = atoi( utm_x_string );
	waypoint_utm->utm_y = atoi( utm_y_string );

	return waypoint_utm;
}

WAYPOINT_UTM *waypoint_utm_calloc( void )
{
	WAYPOINT_UTM *waypoint_utm;

	if ( ! ( waypoint_utm = calloc( 1, sizeof ( WAYPOINT_UTM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return waypoint_utm;
}

WAYPOINT_UTM *waypoint_utm_start(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone )
{
	WAYPOINT_LONLAT *waypoint_lonlat;
	LIST *lonlat_list = list_new();
	LIST *utm_list;

	if ( !start_longitude_string
	||   !start_latitude_string
	||   !utm_zone )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	waypoint_lonlat =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		waypoint_lonlat_new(
			(void *)0 /* record */,
			0 /* weight */,
			start_longitude_string,
			start_latitude_string );

	list_set( lonlat_list, waypoint_lonlat );

	utm_list =
		waypoint_utm_list(
			utm_zone,
			lonlat_list );

	return list_first( utm_list );
}

void waypoint_utm_distance_set(
		WAYPOINT_UTM *waypoint_utm_start,
		LIST *utm_list /* in/out */ )
{
	WAYPOINT_UTM *waypoint_utm;

	if ( !waypoint_utm_start )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( utm_list ) )
	do {
		waypoint_utm = list_get( utm_list );

		waypoint_utm->distance_yards =
			waypoint_utm_distance_yards(
				waypoint_utm_start,
				waypoint_utm->utm_x,
				waypoint_utm->utm_y );

	} while ( list_next( utm_list ) );
}

int waypoint_utm_distance_yards(
		WAYPOINT_UTM *waypoint_utm_start,
		int utm_x,
		int utm_y )
{
	if ( !waypoint_utm_start
	||   !utm_x
	||   !utm_y )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	distance_pythagorean_integer(
		waypoint_utm_start->utm_x /* point_a_x */,
		waypoint_utm_start->utm_y /* point_a_y */,
		utm_x /* point_b_x */,
		utm_y /* point_b_y */ );
}

LIST *waypoint_utm_distance_sort_list( LIST *utm_list )
{
	LIST *sort_list = list_new();
	WAYPOINT_UTM *waypoint_utm;

	if ( list_rewind( utm_list ) )
	do {
		waypoint_utm = list_get( utm_list );

		list_set_order(
			sort_list,
			waypoint_utm,
			waypoint_utm_compare );

	} while ( list_next( utm_list ) );

	if ( !list_length( sort_list ) )
	{
		list_free( sort_list );
		sort_list = NULL;
	}

	return sort_list;
}

int waypoint_utm_compare(
		WAYPOINT_UTM *waypoint_utm_from_list,
		WAYPOINT_UTM *waypoint_utm_to_set )
{
	if ( !waypoint_utm_from_list
	||   !waypoint_utm_to_set )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	waypoint_utm_from_list->distance_yards <
		waypoint_utm_to_set->distance_yards )
	{
		return -1;
	}
	else
	if (	waypoint_utm_from_list->distance_yards ==
		waypoint_utm_to_set->distance_yards )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

WAYPOINT_LONLAT *waypoint_lonlat_new(
		void *record,
		int weight,
		char *longitude_string,
		char *latitude_string )
{
	WAYPOINT_LONLAT *waypoint_lonlat;

	if ( !longitude_string
	||   !latitude_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	waypoint_lonlat  = waypoint_lonlat_calloc();

	waypoint_lonlat->record = record;
	waypoint_lonlat->weight = weight;
	waypoint_lonlat->longitude_string = longitude_string;
	waypoint_lonlat->latitude_string = latitude_string;

	return waypoint_lonlat;
}

WAYPOINT_LONLAT *waypoint_lonlat_calloc( void )
{
	WAYPOINT_LONLAT *waypoint_lonlat;

	if ( ! ( waypoint_lonlat =
			calloc( 1,
				sizeof ( WAYPOINT_LONLAT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return waypoint_lonlat;
}

WAYPOINT *waypoint_new(
		char *start_longitude_string,
		char *start_latitude_string,
		int utm_zone,
		LIST *waypoint_lonlat_list )
{
	WAYPOINT *waypoint;

	if ( !start_longitude_string
	||   !start_latitude_string
	||   !utm_zone )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	waypoint = waypoint_calloc();
	waypoint->waypoint_lonlat_list = waypoint_lonlat_list;

	waypoint->waypoint_utm_start =
		waypoint_utm_start(
			start_longitude_string,
			start_latitude_string,
			utm_zone );

	if ( !waypoint->waypoint_utm_start )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: waypoint_utm_start(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			start_longitude_string,
			start_latitude_string );
		exit( 1 );
	}

	waypoint->utm_list =
		waypoint_utm_list(
			utm_zone,
			waypoint->waypoint_lonlat_list );

	waypoint_utm_distance_set(
		waypoint->waypoint_utm_start,
		waypoint->utm_list /* in/out */ );

	waypoint->waypoint_utm_distance_sort_list =
		waypoint_utm_distance_sort_list(
			waypoint->utm_list );

	return waypoint;
}

LIST *waypoint_utm_list(
		int utm_zone,
		LIST *lonlat_list )
{
	char *system_string;
	WAYPOINT_LONLAT *waypoint_lonlat;
	SPOOL *spool;
	LIST *list;
	LIST *utm_list;
	char *column_string;
	WAYPOINT_UTM *waypoint_utm;

	if ( !utm_zone )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: utm_zone is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( lonlat_list ) ) return (LIST *)0;

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		waypoint_system_string(
			WAYPOINT_CS2CS_EXECUTABLE,
			utm_zone );

	spool =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spool_new(
			system_string,
			0 /* not capture_stderr_boolean */ );

	do {
		waypoint_lonlat = list_get( lonlat_list );

		fprintf(
			spool->output_pipe,
			"%s %s\n",
			waypoint_lonlat->longitude_string,
			waypoint_lonlat->latitude_string );

	} while ( list_next( lonlat_list ) );

	pclose( spool->output_pipe );

	list = spool_list( spool->output_filename );

	if (	list_length( list ) !=
		list_length( lonlat_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list lengths don't match.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	utm_list = list_new();
	list_rewind( lonlat_list );
	list_rewind( list );

	do {
		column_string = list_get( list );
		waypoint_lonlat = list_get( lonlat_list );

		waypoint_utm =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			waypoint_utm_column_new(
				waypoint_lonlat->record,
				waypoint_lonlat->weight,
				column_string );

		list_set( utm_list, waypoint_utm );
		list_next( lonlat_list );

	} while ( list_next( list ) );

	return utm_list;
}

char *waypoint_system_string(
		const char *waypoint_cs2cs_executable,
		int utm_zone )
{
	static char system_string[ 128 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %d",
		waypoint_cs2cs_executable,
		utm_zone );

	return system_string;
}

