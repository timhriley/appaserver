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
#include "account.h"
#include "enrollment.h"
#include "course.h"
#include "journal.h"
#include "transaction.h"
#include "semester.h"
#include "offering.h"
#include "offering.h"
#include "enrollment.h"

OFFERING *offering_calloc( void )
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
	return offering;
}

OFFERING *offering_new(	char *course_name,
			SEMESTER *semester )
{
	OFFERING *offering = offering_calloc();

	offering->course_name = course_name;
	offering->semester = semester;

	return offering;
}

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_program )
{
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char course_price[ 128 ];
	char instructor_full_name[ 128 ];
	char instructor_street_address[ 128 ];
	char class_capacity[ 128 ];
	char enrollment_count[ 128 ];
	char capacity_available[ 128 ];
	char revenue_account[ 128 ];
	OFFERING *offering;

	if ( !input || !*input ) return (OFFERING *)0;

	/* See: attribute_list offering */
	/* ---------------------------- */
	piece( course_name, SQL_DELIMITER, input, 0 );
	piece( season_name, SQL_DELIMITER, input, 1 );
	piece( year, SQL_DELIMITER, input, 2 );

	offering =
		offering_new(
			strdup( course_name ),
			semester_new(
				strdup( season_name ),
				atoi( year ) ) );

	piece( course_price, SQL_DELIMITER, input, 3 );
	offering->course_price = atof( course_price );

	piece( instructor_full_name, SQL_DELIMITER, input, 4 );
	piece( instructor_street_address, SQL_DELIMITER, input, 5 );

	offering->instructor_entity =
		entity_new(
			strdup( instructor_full_name ),
			strdup( instructor_street_address ) );

	piece( class_capacity, SQL_DELIMITER, input, 6 );
	offering->class_capacity = atoi( class_capacity );

	piece( enrollment_count, SQL_DELIMITER, input, 7 );
	offering->enrollment_count = atoi( enrollment_count );

	piece( capacity_available, SQL_DELIMITER, input, 8 );
	offering->capacity_available = atoi( capacity_available );

	piece( revenue_account, SQL_DELIMITER, input, 9 );
	offering->revenue_account = strdup( revenue_account );

	if ( fetch_course )
	{
		offering->course =
			course_fetch(
				offering->
					course->
					course_name,
				fetch_program,
				0 /* not fetch_alias_list */ );
	}

	return offering;
}

OFFERING *offering_fetch(
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_course,
			boolean fetch_program )
{
	if ( !course_name || !season_name || !year )
	{
		return (OFFERING *)0;
	}

	return offering_parse(
			string_pipe_fetch(
				offering_system_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			offering_primary_where(
						course_name,
						season_name,
						year ) ) ),
			fetch_course,
			fetch_program );
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
		 course_name_escape( course_name ),
		 season_name,
		 year );

	return strdup( where );
}

LIST *offering_system_list(
			char *sys_string,
			boolean fetch_course,
			boolean fetch_program )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *offering_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			offering_list,
			offering_parse(
				input,
				fetch_course,
				fetch_program ) );
	}
	pclose( input_pipe );
	return offering_list;
}

char *offering_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" '%s'",
		OFFERING_TABLE,
		where,
		"year desc, season_name" );

	return strdup( system_string );
}

void offering_fetch_update(
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"offering_enrollment_count.sh \"%s\" '%s' %d",
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"offering_capacity_available.sh \"%s\" '%s' %d",
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}
}

void offering_list_fetch_update(
			LIST *offering_list )
{
	OFFERING *offering;

	if ( !list_rewind( offering_list ) ) return;

	do {
		offering = list_get( offering_list );

		offering_fetch_update(
			offering->course_name,
			offering->semester->season_name,
			offering->semester->year );

	} while ( list_next( offering_list ) );
}

OFFERING *offering_list_seek(
			char *course_name,
			LIST *offering_list )
{
	OFFERING *offering;

	if ( !list_rewind( offering_list ) ) return (OFFERING *)0;

	do {
		offering = list_get( offering_list );

		if ( string_strcmp(
			offering->course_name,
			course_name ) == 0 )
		{
			return offering;
		}

	} while ( list_next( offering_list ) );

	return (OFFERING *)0;
}

LIST *offering_list_course_name_list( LIST *offering_list )
{
	LIST *name_list;
	OFFERING *offering;

	if ( !list_rewind( offering_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		offering = list_get( offering_list );

		list_set( name_list, offering->course->course_name );

	} while ( list_next( offering_list ) );

	return name_list;
}

