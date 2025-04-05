/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass_street.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "canvass_waypoint.h"

#define UTM_ZONE 10

typedef struct
{
	char *street_name;
	char *longitude_string;
	char *latitude_string;
	int weight;
} CANVASS_STREET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CANVASS_STREET *canvass_street_new(
		char *street_name,
		char *longitude_string,
		char *latitude_string,
		int weight );

/* Process */
/* ------- */
CANVASS_STREET *canvass_street_calloc(
		void );

LIST *canvass_street_include_list(
		char *canvass_name );

int canvass_street_include_list_weight(
		LIST *include_street_list );

LIST *canvass_street_not_include_list(
		char *canvass_name );

LIST *canvass_street_list(
		char *canvass_name,
		char include_yn );

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
		char *canvass_name );

/* Process */
/* ------- */
CANVASS *canvass_calloc(
		void );

char *canvass_start_record(
		void );

LIST *canvass_waypoint_lonlat_list(
		LIST *canvass_street_list );

int main( int argc, char **argv )
{
	char *canvass_name;
	int maximum_weight;
	CANVASS *canvass;
	CANVASS_WAYPOINT *canvass_waypoint;
	WAYPOINT_UTM *waypoint_utm;
	CANVASS_STREET *canvass_street;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s canvass maximum_weight\n",
			argv[ 0 ] );
		exit( 1 );
	}

	canvass_name = argv[ 1 ];
	maximum_weight = atoi( argv[ 2 ] );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	canvass = canvass_new( canvass_name );

	if ( list_length( canvass->include_waypoint_lonlat_list ) )
	{
		canvass_waypoint =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			canvass_waypoint_new(
				canvass->start_longitude_string,
				canvass->start_latitude_string,
				UTM_ZONE,
				maximum_weight,
				canvass->include_waypoint_lonlat_list );

		if ( list_rewind( canvass_waypoint->waypoint_utm_list ) )
		do {
			waypoint_utm =
				list_get(
					canvass_waypoint->
						waypoint_utm_list );

			canvass_street = waypoint_utm->record;

			printf(	"%s\n",
				canvass_street->street_name );

		} while ( list_next( canvass_waypoint->waypoint_utm_list ) );
	}

	if ( list_length( canvass->not_include_waypoint_lonlat_list ) )
	{
		canvass_waypoint =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			canvass_waypoint_new(
				canvass->start_longitude_string,
				canvass->start_latitude_string,
				UTM_ZONE,
				maximum_weight -
				canvass->canvass_street_include_list_weight,
				canvass->not_include_waypoint_lonlat_list );

		if ( list_rewind( canvass_waypoint->waypoint_utm_list ) )
		do {
			waypoint_utm =
				list_get(
					canvass_waypoint->
						waypoint_utm_list );

			canvass_street = waypoint_utm->record;

			printf(	"%s\n",
				canvass_street->street_name );

		} while ( list_next( canvass_waypoint->waypoint_utm_list ) );
	}

	return 0;
}

CANVASS_STREET *canvass_street_new(
		char *street_name,
		char *longitude_string,
		char *latitude_string,
		int weight )
{
	CANVASS_STREET *canvass_street;

	if ( !street_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: street_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !longitude_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: longitude_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !latitude_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: latitude_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !weight )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: weight is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	canvass_street = canvass_street_calloc();

	canvass_street->street_name = street_name;
	canvass_street->longitude_string = longitude_string;
	canvass_street->latitude_string = latitude_string;
	canvass_street->weight = weight;

	return canvass_street;
}

CANVASS_STREET *canvass_street_calloc( void )
{
	CANVASS_STREET *canvass_street;

	if ( ! ( canvass_street = calloc( 1, sizeof ( CANVASS_STREET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return canvass_street;
}

CANVASS *canvass_new( char *canvass_name )
{
	CANVASS *canvass;
	char buffer[ 128 ];

	canvass = canvass_calloc();

	if ( ! ( canvass->start_record = canvass_start_record() ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: canvass_start_record() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	piece( buffer, SQL_DELIMITER, canvass->start_record, 0 );
	canvass->start_longitude_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, canvass->start_record, 1 );
	canvass->start_latitude_string = strdup( buffer );

	canvass->canvass_street_include_list =
		canvass_street_include_list(
			canvass_name );

	canvass->canvass_street_include_list_weight =
		canvass_street_include_list_weight(
			canvass->canvass_street_include_list );

	canvass->include_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_include_list );

	canvass->canvass_street_not_include_list =
		canvass_street_not_include_list(
			canvass_name );

	canvass->not_include_waypoint_lonlat_list =
		canvass_waypoint_lonlat_list(
			canvass->canvass_street_not_include_list );

	return canvass;
}

CANVASS *canvass_calloc( void )
{
	CANVASS *canvass;

	if ( ! ( canvass = calloc( 1, sizeof ( CANVASS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return canvass;
}

char *canvass_start_record( void )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe( "canvass_start.sh" );
}

LIST *canvass_street_include_list( char *canvass_name )
{
	return
	canvass_street_list(
		canvass_name,
		'y' /* include_yn */ );
}

LIST *canvass_street_not_include_list( char *canvass_name )
{
	return
	canvass_street_list(
		canvass_name,
		'n' /* include_yn */ );
}

LIST *canvass_street_list(
		char *canvass_name,
		char include_yn )
{
	LIST *street_list = list_new();
	char system_string[ 128 ];
	FILE *input_pipe;
	char input[ 256 ];
	char street_name[ 128 ];
	char longitude_string[ 128 ];
	char latitude_string[ 128 ];
	char weight[ 128 ];
	CANVASS_STREET *canvass_street;

	snprintf(
		system_string,
		sizeof ( system_string ),
		"canvass_street.sh '%s' %c",
		canvass_name,
		include_yn );

	input_pipe =
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		piece( street_name, SQL_DELIMITER, input, 0 );
		piece( longitude_string, SQL_DELIMITER, input, 1 );
		piece( latitude_string, SQL_DELIMITER, input, 2 );
		piece( weight, SQL_DELIMITER, input, 5 );

		canvass_street =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			canvass_street_new(
				strdup( street_name ),
				strdup( longitude_string ),
				strdup( latitude_string ),
				atoi( weight ) );

		list_set( street_list, canvass_street );
	}

	pclose( input_pipe );

	if ( !list_length( street_list ) )
	{
		list_free( street_list );
		street_list = NULL;
	}

	return street_list;
}

LIST *canvass_waypoint_lonlat_list( LIST *canvass_street_list )
{
	CANVASS_STREET *canvass_street;
	LIST *waypoint_lonlat_list = list_new();
	WAYPOINT_LONLAT *waypoint_lonlat;

	if ( list_rewind( canvass_street_list ) )
	do {
		canvass_street = list_get( canvass_street_list );

		if ( !canvass_street->weight
		||   !canvass_street->longitude_string
		||   !canvass_street->latitude_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: canvass_street is incomplete\n",
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
				canvass_street /* record */,
				canvass_street->weight,
				canvass_street->longitude_string,
				canvass_street->latitude_string );

		list_set( waypoint_lonlat_list, waypoint_lonlat );

	} while ( list_next( canvass_street_list ) );

	if ( !list_length( waypoint_lonlat_list ) )
	{
		list_free( waypoint_lonlat_list );
		waypoint_lonlat_list = NULL;
	}

	return waypoint_lonlat_list;
}

int canvass_street_include_list_weight( LIST *canvass_street_include_list )
{
	int weight = 0;
	CANVASS_STREET *canvass_street;

	if ( list_rewind( canvass_street_include_list ) )
	do {
		canvass_street = list_get( canvass_street_include_list );
		weight += canvass_street->weight;

	} while ( list_next( canvass_street_include_list ) );

	return weight;
}
