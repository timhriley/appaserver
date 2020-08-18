/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/offering.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "enrollment.h"
#include "offering.h"

OFFERING *offering_new(	char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ! ( offering = calloc( 1, sizeof( OFFERING ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	offering->course_name = course_name;

	offering->course =
		course_fetch(
			offering->course_name );

	offering->season_name = season_name;
	offering->year = year;
	return offering;
}

OFFERING *offering_getset(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ! ( offering =
			offering_seek(
				semester_offering_list,
				course_name,
				season_name,
				year ) ) )
	{
		offering =
			offering_new(
				course_name,
				season_name,
				year );

		list_set( semester_offering_list, offering );
	}
	return offering;
}

OFFERING *offering_seek(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( !list_rewind( semester_offering_list ) ) return (OFFERING *)0;

	do {
		offering = list_get( semester_offering_list );

		if ( strcmp( offering->course_name, course_name ) == 0
		&&   strcmp( offering->season_name, season_name ) == 0
		&&   offering->year == year )
		{
			return offering;
		}
	} while ( list_next( semester_offering_list ) );

	return (OFFERING *)0;
}

double offering_course_price(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ( offering =
			offering_seek(
				semester_offering_list,
				course_name,
				season_name,
				year ) ) )
	{
		return offering->course->course_price;
	}
	else
	{
		return 0.0;
	}
}

int offering_class_capacity(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ( offering =
			offering_seek(
				semester_offering_list,
				course_name,
				season_name,
				year ) ) )
	{
		return offering->class_capacity;
	}
	else
	{
		return 0.0;
	}
}

char *offering_select( void )
{
	return "course_name,season_name,year,full_name,street_address,class_capacity,enrollment_count,capacity_available,revenue_account";
}

OFFERING *offering_parse( char *input_buffer )
{
	char course_name[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	OFFERING *offering;

	if ( !input_buffer ) return (OFFERING *)0;

	piece( course_name, SQL_DELIMITER, input_buffer, 0 );
	piece( season_name, SQL_DELIMITER, input_buffer, 1 );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 2 );
	year = atoi( piece_buffer );

	offering = offering_new(
			strdup( course_name ),
			strdup( season_name ),
			year );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 3 );
	offering->instructor_full_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	offering->instructor_street_address = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	offering->class_capacity = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	offering->offering_enrollment_count = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 7 );
	offering->offering_capacity_available = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 8 );
	offering->revenue_account = strdup( piece_buffer );

	offering->enrollment_list =
		enrollment_offering_enrollment_list(
			offering->course_name,
			offering->season_name,
			offering->year );

	return offering;
}

OFFERING *offering_fetch(
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 offering_select(),
		 "offering",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 offering_primary_where(
			course_name,
			season_name,
			year ),
		 offering_select() );

	return offering_parse( pipe2string( sys_string ) );
}

char *offering_escape_course_name(
			char *course_name )
{
	static char escape_course_name[ 256 ];

	string_escape_quote( escape_course_name, course_name );
	return escape_course_name;
}

/* Safely returns heap memory */
/* -------------------------- */
char *offering_primary_where(
			char *course_name,
			char *season_name,
			int year )
{
	char where[ 1024 ];

	sprintf( where,
		 "course_name = '%s' and season_name = '%s' and year = %d",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 offering_escape_course_name( course_name ),
		 season_name,
		 year );

	return strdup( where );
}

LIST *offering_list( char *where_clause )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	LIST *offering_list;
	char input_buffer[ 1024 ];
	OFFERING *offering;

	if ( !where_clause ) return (LIST *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 offering_select(),
		 "offering",
		 where_clause,
		 offering_select() );

	input_pipe = popen( sys_string, "r" );

	offering_list = list_new();

	while ( string_input( input_buffer, input_pipe, 1024 ) )
	{
		offering = offering_parse( input_buffer );
		list_set( offering_list, offering );
	}

	pclose( input_pipe );
	return offering_list;
}

int offering_enrollment_count(
			LIST *offering_enrollment_list )
{
	return list_length( offering_enrollment_list );
}

int offering_capacity_available(
			int class_capacity,
			int offering_enrollment_count )
{
	return class_capacity - offering_enrollment_count;
}

/* Safely returns heap memory */
/* -------------------------- */
char *semester_where(
			char *season_name,
			int year )
{
	char where[ 1024 ];

	sprintf( where,
		 "season_name = '%s' and year = %d",
		 season_name,
		 year );

	return strdup( where );
}

void offering_refresh(
			int offering_enrollment_count,
			int offering_capacity_available,
			LIST *enrollment_list,
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "offering",
		 OFFERING_PRIMARY_KEY );

	update_pipe = popen( sys_string, "w" );

	fprintf( update_pipe,
		 "%s^%s^%d^enrollment_count^%d\n",
		 course_name,
		 season_name,
		 year,
		 offering_enrollment_count );

	fprintf( update_pipe,
		 "%s^%s^%d^capacity_available^%d\n",
		 course_name,
		 season_name,
		 year,
		 offering_capacity_available );

	pclose( update_pipe );

	enrollment_list_refresh( enrollment_list );
}

