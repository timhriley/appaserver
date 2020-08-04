/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.c		*/
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
#include "list.h"
#include "transaction.h"
#include "enrollment.h"

/* No need to strdup() in. Returns everything on the heap. */
/* ------------------------------------------------------- */
ENROLLMENT *enrollment_new(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( ! ( enrollment = calloc( 1, sizeof( ENROLLMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	enrollment->offering =
		offering_fetch(
			course_name,
			season_name,
			year );

	enrollment->registration =
		registration_fetch(
			full_name,
			street_address,
			season_name,
			year );

	return enrollment;
}

char *enrollment_select( void )
{
	return "full_name,street_address,course_name,season_name,year";
}

ENROLLMENT *enrollment_parse( char *input_buffer )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char piece_buffer[ 128 ];
	int year;
	ENROLLMENT *enrollment;

	if ( !input_buffer ) return (ENROLLMENT *)0;

	piece( full_name, SQL_DELIMITER, input_buffer, 0 );
	piece( street_address, SQL_DELIMITER, input_buffer, 1 );
	piece( course_name, SQL_DELIMITER, input_buffer, 2 );
	piece( season_name, SQL_DELIMITER, input_buffer, 3 );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	year = atoi( piece_buffer );

	enrollment =
		/* ------------------------------------------------------- */
		/* No need to strdup() in. Returns everything on the heap. */
		/* ------------------------------------------------------- */
		enrollment_new(
			full_name,
			street_address,
			course_name,
			season_name,
			year );

	return enrollment;
}

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			char *course_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 enrollment_select(),
		 "enrollment",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 enrollment_primary_where(
			student_full_name,
			student_street_address,
			course_name,
			season_name,
			year ),
		 enrollment_select() );

	return enrollment_parse( pipe2string( sys_string ) );
}

LIST *enrollment_offering_enrollment_list(
			char *course_name,
			char *season_name,
			int year )
{
	return (LIST *)0;
}

LIST *enrollment_registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return (LIST *)0;
}

void enrollment_list_refresh(
			LIST *enrollment_list )
{
}

/* Safely returns heap memory */
/* -------------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char where[ 1024 ];

	sprintf( where,
"full_name = '%s' and street_address = '%s' and course_name = '%s' and season_name = '%s' and year = %d",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 student_street_address,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 offering_escape_course_name( course_name ),
		 season_name,
		 year );

	return strdup( where );
}

TRANSACTION *enrollment_transaction(
			char *student_full_name,
			char *street_address,
			char *registration_date_time
				/* transaction_date_time */,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account )
{
	return (TRANSACTION *)0;
}

