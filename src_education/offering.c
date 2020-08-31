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
#include "semester.h"
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

	offering->course =
		course_new(
			course_name );

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

OFFERING *offering_parse( char *input )
{
	char course_name[ 128 ];
	char season_name[ 128 ];
	int year;
	char piece_buffer[ 128 ];
	OFFERING *offering;

	if ( !input || !*input ) return (OFFERING *)0;

	/* See: attribute_list offering */
	/* ---------------------------- */
	piece( course_name, SQL_DELIMITER, input, 0 );
	piece( season_name, SQL_DELIMITER, input, 1 );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	year = atoi( piece_buffer );

	offering = offering_new(
			strdup( course_name ),
			strdup( season_name ),
			year );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	offering->instructor_full_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	offering->street_address = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	offering->class_capacity = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	offering->offering_enrollment_count = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	offering->offering_capacity_available = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	offering->revenue_account = strdup( piece_buffer );

	offering->enrollment_list =
		offering_enrollment_list(
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
	if ( !course_name || !course_name || !year )
	{
		return (OFFERING *)0;
	}

	return offering_parse(
			pipe2string(
				offering_sys_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			offering_primary_where(
						course_name,
						season_name,
						year ) ) ) );
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

LIST *offering_list(	char *season_name,
			int year )
{
	return offering_system_list(
			semester_primary_where(
				season_name,
				year ) );
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

FILE *offering_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y | sql",
		 "offering",
		 OFFERING_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void offering_update(	int enrollment_count,
			int capacity_available,
			char *course_name,
			char *season_name,
			int year )
{
	FILE *update_pipe;

	update_pipe = offering_update_open();

	fprintf( update_pipe,
		 "%s^%s^%d^enrollment_count^%d\n",
		 course_name,
		 season_name,
		 year,
		 enrollment_count );

	fprintf( update_pipe,
		 "%s^%s^%d^capacity_available^%d\n",
		 course_name,
		 season_name,
		 year,
		 capacity_available );

	pclose( update_pipe );
}

LIST *offering_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *offering_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			offering_list,
			offering_parse(
				input ) );
	}
	pclose( input_pipe );
	return offering_list;
}

char *offering_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" '%s'",
		"offering",
		where,
		"year, season" );

	return strdup( sys_string );
}

/* Usage: offering_list_fetch( season_primary_where() ); */
/* ----------------------------------------------------- */
LIST *offering_list_fetch( char *where )
{
	return	offering_system_list(
			offering_sys_string(
				where ) );
}

LIST *offering_enrollment_list(
			char *course_name,
			char *season_name,
			int year )
{
	return	enrollment_system_list(
			enrollment_sys_string(
				offering_primary_where(
					course_name,
					season_name,
					year ) ) );
}

