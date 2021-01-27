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
			char *season_name,
			int year )
{
	OFFERING *offering = offering_calloc();

	offering->course =
		course_new(
			course_name );

	offering->semester =
		semester_new(
			season_name,
			year );

	return offering;
}

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_enrollment_list )
{
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char instructor_full_name[ 128 ];
	char instructor_street_address[ 128 ];
	char course_price[ 128 ];
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
		/* Executes course_new() */
		/* --------------------- */
		offering_new(
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ) );

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
		if ( ! ( offering->course =
				course_fetch(
					offering->
						course->
						course_name,
					fetch_program,
					0 /* not fetch_alias_list */ ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: fetch_course(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				course_name );
			exit( 1 );
		}
	}

	if ( fetch_enrollment_list )
	{
		offering->enrollment_list =
			offering_enrollment_list(
				offering->course->course_name,
				offering->semester->season_name,
				offering->semester->year );
	}

	return offering;
}

OFFERING *offering_fetch(
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_enrollment_list )
{
	if ( !course_name || !season_name || !year )
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
						year ) ) ),
			fetch_course,
			fetch_program,
			fetch_enrollment_list );
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

int offering_enrollment_count(
			LIST *offering_enrollment_list )
{
	return list_length( offering_enrollment_list );
}

int offering_capacity_available(
			int class_capacity,
			int enrollment_count )
{
	return class_capacity - enrollment_count;
}

char *semester_where(	char *season_name,
			int year )
{
	static char where[ 256 ];

	sprintf( where,
		 "season_name = '%s' and year = %d",
		 season_name,
		 year );

	return where;
}

FILE *offering_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh			|"
		 "sql						 ",
		 OFFERING_TABLE,
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

LIST *offering_system_list(
			char *sys_string,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_enrollment_list )
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
				fetch_program,
				fetch_enrollment_list ) );
	}
	pclose( input_pipe );
	return offering_list;
}

char *offering_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" '%s'",
		OFFERING_TABLE,
		where,
		"year desc, season_name" );

	return strdup( sys_string );
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
					year ) ),
			1 /* fetch_offering */,
			1 /* fetch_course */,
			1 /* fetch_program */,
			0 /* not fetch_registration */ );
}

OFFERING *offering_steady_state(
			OFFERING *offering,
			LIST *offering_enrollment_list )
{
	offering->enrollment_count =
		offering_enrollment_count(
			offering_enrollment_list );

	offering->capacity_available =
		offering_capacity_available(
			offering->class_capacity,
			offering->enrollment_count );

	return offering;
}

boolean offering_exists(
			char *season_name,
			int year,
			char *course_name )
{
	if ( offering_fetch(
			course_name,
			season_name,
			year,
			0 /* not fetch_course */,
			0 /* not fetch_program */,
			0 /* not fetch_enrollment_list */ ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
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
			LIST *course_name_list,
			char *season_name,
			int year )
{
	char *course_name;

	if ( !list_rewind( course_name_list ) ) return;

	do {
		course_name = list_get( course_name_list );

		offering_fetch_update(
			course_name,
			season_name,
			year );

	} while ( list_next( course_name_list ) );
}

OFFERING *offering_course_name_seek(
			char *course_name,
			LIST *offering_list )
{
	return offering_seek(
			course_name,
			offering_list );
}

OFFERING *offering_seek(
			char *course_name,
			LIST *offering_list )
{
	OFFERING *offering;

	if ( !list_rewind( offering_list ) ) return (OFFERING *)0;

	do {
		offering = list_get( offering_list );

		if ( !offering->course )
		{
			fprintf(stderr,
				"Warning in %s/%s()/%d: empty course.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		if ( string_strcmp(
			offering->course->course_name,
			course_name ) == 0 )
		{
			return offering;
		}

	} while ( list_next( offering_list ) );

	return (OFFERING *)0;
}

LIST *offering_name_list( LIST *offering_list )
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

void offering_trigger(
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"offering_trigger \"%s\" '%s' %d update",
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}
}

