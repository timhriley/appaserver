/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/semester.c		*/
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
#include "boolean.h"
#include "list.h"
#include "semester.h"
#include "offering.h"
#include "event.h"
#include "registration.h"

SEMESTER *semester_calloc( void )
{
	SEMESTER *semester;

	if ( ! ( semester = calloc( 1, sizeof( SEMESTER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return semester;
}

SEMESTER *semester_new(	char *season_name,
			int year )
{
	SEMESTER *semester = semester_calloc();

	semester->season_name = season_name;
	semester->year = year;
	return semester;
}

SEMESTER *semester_fetch(
			char *season_name,
			int year,
			boolean fetch_offering_list )
{
	if ( !season_name || !season_name || !year )
	{
		return (SEMESTER *)0;
	}

	return semester_parse(
			pipe2string(
				semester_sys_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			semester_primary_where(
						season_name,
						year ) ) ),
			fetch_offering_list );
}

/* Safely returns heap memory */
/* -------------------------- */
char *semester_primary_where(
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

char *semester_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		SEMESTER_TABLE,
		where );

	return strdup( sys_string );
}

SEMESTER *semester_parse(
			char *input,
			boolean fetch_offering_list )
{
	char season_name[ 128 ];
	char year[ 128 ];
	SEMESTER *semester;

	if ( !input || !*input ) return (SEMESTER *)0;

	/* See: attribute_list semester */
	/* ---------------------------- */
	piece( season_name, SQL_DELIMITER, input, 0 );
	piece( year, SQL_DELIMITER, input, 1 );

	semester =
		semester_new(
			strdup( season_name ),
			atoi( year ) );

	if ( fetch_offering_list )
	{
		semester->offering_list =
			semester_offering_list(
				semester->season_name,
				semester->year );
	}

	return semester;
}

LIST *semester_offering_list(
			char *season_name,
			int year )
{
	static LIST *offering_list = {0};

	if ( offering_list ) return offering_list;

	offering_list =
		offering_system_list(
			offering_sys_string(
				semester_primary_where(
					season_name,
					year ) ),
			1 /* fetch_course */,
			1 /* fetch_program */,
			0 /* not fetch_enrollment_list */ );

	return offering_list;
}

