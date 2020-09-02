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
#include "offering_fns.h"
#include "registration_fns.h"

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
			boolean fetch_offering_list,
			boolean fetch_registration_list )
{
	SEMESTER *semester =
		semester_new(
			strdup( season_name ),
			year );

	if ( fetch_offering_list )
	{
		semester->semester_offering_list =
			semester_offering_list(
				season_name,
				year );
	}

	if ( fetch_registration_list )
	{
		semester->semester_registration_list =
			semester_registration_list(
				season_name,
				year );
	}

	return semester;
}

char *semester_primary_where(
			char *season_name,
			int year )
{
	static char where[ 256 ];

	sprintf( where,
		 "season_name = '%s' and	"
		 "year = %d			",
		 season_name,
		 year );

	return where;
}

LIST *semester_offering_list(
			char *season_name,
			int year )
{
	return	offering_system_list(
			offering_sys_string(
				semester_primary_where(
					season_name,
					year ) ),
			1 /* fetch_course */,
			1 /* fetch_enrollment_list */ );
}

LIST *semester_registration_list(
			char *season_name,
			int year )
{
	return	registration_system_list(
			registration_sys_string(
				semester_primary_where(
					season_name,
					year ) ),
			1 /* fetch_enrollment_list */ );
}

