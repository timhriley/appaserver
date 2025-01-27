/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/station.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "station_datatype.h"
#include "station.h"

LIST *station_list(	char *agency_name,
			boolean fetch_station_datatype_list,
			boolean fetch_datatype )
{
	char *where;
	char *system_string;
	FILE *input_pipe;
	LIST *list;
	char input[ 1024 ];
	STATION *station;

	where =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		station_where(
			agency_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			STATION_SELECT,
			STATION_TABLE,
			where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		station =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			station_parse(
				fetch_station_datatype_list,
				fetch_datatype,
				input );

		list_set( list, station );
	}

	pclose( input_pipe );

	return list;
}

char *station_where( char *agency_name )
{
	static char where[ 128 ];

	if ( !agency_name ) return (char *)0;

	sprintf(where,
		"agency = '%s'",
		agency_name );

	return where;
}

STATION *station_fetch(
			char *station_name,
			boolean fetch_station_datatype_list,
			boolean fetch_datatype )
{
	char *where;
	char *system_string;
	char *input;

	if ( !station_name )
	{
		char message[ 128 ];

		sprintf(message, "station_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		station_primary_where(
			station_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			STATION_SELECT,
			STATION_TABLE,
			where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			system_string );

	if ( !input ) return NULL;

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	station_parse(
		fetch_station_datatype_list,
		fetch_datatype,
		input );
}


char *station_primary_where( char *station_name )
{
	static char where[ 128 ];

	if ( !station_name )
	{
		char message[ 128 ];

		sprintf(message, "station_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"station = '%s'",
		station_name );

	return where;
}

STATION *station_parse(
			boolean fetch_station_datatype_list,
			boolean fetch_datatype,
			char *input )
{
	char station_name[ 128 ];
	char buffer[ 128 ];
	STATION *station;

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See STATION_SELECT */
	/* ------------------ */
	piece( station_name, SQL_DELIMITER, input, 0 );

	station = 
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		station_new(
			strdup( station_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) station->basin = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) station->agency = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) station->park = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) station->station_type = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) station->lat_nad83 = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) station->long_nad83 = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) station->area_sqmi = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) station->ground_surface_elevation_ft = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) station->vertical_datum = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer ) station->comments = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer ) station->ngvd29_navd88_increment = atof( buffer );

	if ( fetch_station_datatype_list )
	{
		station->station_datatype_list =
			station_datatype_list(
				station->station_name,
				(char *)0 /* datatype_name */,
				(char *)0 /* where */,
				fetch_datatype );
	}

	return station;
}

STATION *station_new( char *station_name )
{
	STATION *station;

	if ( !station_name )
	{
		char message[ 128 ];

		sprintf(message, "station_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	station = station_calloc();
	station->station_name = station_name;
	return station;
}

STATION *station_calloc( void )
{
	STATION *station;

	if ( ! ( station = calloc( 1, sizeof ( STATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return station;
}

