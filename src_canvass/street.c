/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/street.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "street.h"

STREET *street_fetch(
		char *street_name,
		char *city,
		char *state_code )
{
	char *primary_where;
	char *system_string;
	char *fetch;

	if ( !street_name
	||   !city
	||   !state_code )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		street_primary_where(
			street_name,
			city,
			state_code );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			STREET_SELECT,
			STREET_TABLE,
			primary_where );

	if ( ! ( fetch = string_fetch( system_string ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: string_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );
		exit( 1 );
	}

	free( system_string );

	return
	street_parse(
		street_name,
		city,
		state_code,
		fetch /* input */ );
}

char *street_where(
		char *city,
		char *state_code )
{
	static char where[ 128 ];

	if ( !city
	||   !state_code )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		where,
		sizeof ( where ),
		"city = '%s' and state_code = '%s'",
		city,
		state_code );

	return where;
}

char *street_primary_where(
		char *street_name,
		char *city,
		char *state_code )
{
	static char primary_where[ 256 ];

	if ( !street_name
	||   !city
	||   !state_code )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"street_name = '%s' and %s",
		street_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		street_where(
			city,
			state_code ) );

	return primary_where;
}

STREET *street_parse(
		char *street_name,
		char *city,
		char *state_code,
		char *input )
{
	char input_street_name[ 128 ];
	char input_city[ 128 ];
	char input_state_code[ 128 ];
	char buffer[ 128 ];
	STREET *street;

	if ( !input || !*input ) return NULL;

	/* See STREET_SELECT */
	/* ----------------- */
	if ( street_name )
		strcpy( input_street_name, street_name );
	else
		piece( input_street_name, SQL_DELIMITER, input, 0 );

	if ( city )
		strcpy( input_city, city );
	else
		piece( input_city, SQL_DELIMITER, input, 1 );

	if ( state_code )
		strcpy( input_state_code, state_code );
	else
		piece( input_state_code, SQL_DELIMITER, input, 2 );

	street =
		street_new(
			strdup( input_street_name ),
			strdup( input_city ),
			strdup( input_state_code ) );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) street->zip_code = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) street->house_count = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) street->apartment_count = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) street->longitude_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) street->latitude_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) street->county_district = atoi( buffer );

	return street;
}

STREET *street_new(
		char *street_name,
		char *city,
		char *state_code )
{
	STREET *street;

	if ( !street_name
	||   !city
	||   !state_code )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	street = street_calloc();

	street->street_name = street_name;
	street->city = city;
	street->state_code = state_code;

	return street;
}

STREET *street_calloc( void )
{
	STREET *street;

	if ( ! ( street = calloc( 1, sizeof ( STREET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return street;
}

LIST *street_list(
		char *city,
		char *state_code )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			STREET_SELECT,
			STREET_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			street_where(
				city,
				state_code ) );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			street_parse(
				(char *)0 /* street_name */,
				(char *)0 /* city */,
				(char *)0 /* state_code */,
				input ) );
	}

	pclose( input_pipe );
	free( system_string );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

STREET *street_seek(
		LIST *street_list,
		char *street_name,
		char *city,
		char *state_code )
{
	STREET *street;

	if ( !street_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: street_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( street_list ) )
	do {
		street = list_get( street_list );

		if ( strcmp( street->street_name, street_name ) != 0 )
			continue;

		if ( city
		&&   strcmp( street->city, city ) != 0 )
			continue;

		if ( state_code
		&&   strcmp( street->state_code, state_code ) != 0 )
			continue;

		return street;

	} while ( list_next( street_list ) );

	return NULL;
}
