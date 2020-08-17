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
#include "enrollment.h"
#include "offering.h"
#include "registration.h"
#include "semester.h"

SEMESTER *semester_new(	char *season_name,
			int year )
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

	semester->season_name = season_name;
	semester->year = year;

	semester->semester_offering_list =
		semester_offering_list(
			season_name,
			year );

	semester->semester_registration_list =
		semester_registration_list(
			season_name,
			year );

	return semester;
}

/* Safely returns heap memory */
/* -------------------------- */
char *semester_primary_where(
			char *season_name,
			int year )
{
	char where[ 256 ];

	sprintf( where,
		 "season_name = '%s' and	"
		 "year = %d			",
		 season_name,
		 year );

	return strdup( where );
}

LIST *semester_offering_list(
			char *season_name,
			int year )
{
	return offering_list(
			semester_primary_where(
				season_name,
			year ) );
}

LIST *semester_registration_list(
			char *season_name,
			int year )
{
	return registration_list(
			semester_primary_where(
				season_name,
			year ) );
}

ENROLLMENT *semester_enrollment_getset(
			LIST *semester_registration_list,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;
	REGISTRATION *registration;

	registration =
		/* Getset()s always succeed */
		/* ------------------------ */
		registration_getset(
			semester_registration_list,
			student_full_name,
			student_street_address,
			season_name,
			year );

	if ( !registration->registration_enrollment_list )
		registration->registration_enrollment_list =
			list_new();

	enrollment =
		/* Getset()s always succeed */
		/* ------------------------ */
		enrollment_getset(
			registration->registration_enrollment_list,
			student_full_name,
			student_street_address,
			course_name,
			season_name,
			year );

	if ( !enrollment->registration )
		enrollment->registration =
			registration;

	return enrollment;
}
