/* ----------------------------------------------------- */
/* $APPASERVER_HOME/src_education/post_change_offering.c */
/* ----------------------------------------------------- */
/* 							 */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "enrollment.h"
#include "registration.h"
#include "offering.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_offering(
				char *course_name,
				char *season_name,
				int year );

int main( int argc, char **argv )
{
	char *application_name;
	char *course_name;
	char *season_name;
	int year;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
			 "Usage: %s course_name season_name year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	course_name = argv[ 1 ];
	season_name = argv[ 2 ];
	year = atoi( argv[ 3 ] );
	state = argv[ 4 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		post_change_offering(
			course_name,
			season_name,
			year );
	}

	return 0;

} /* main() */

void post_change_offering(
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ! ( offering =
			offering_fetch(
				course_name,
				season_name,
				year ) ) )
	{
		return;
	}

	offering->offering_enrollment_count =
		offering_enrollment_count(
			offering->enrollment_list );

	offering->offering_capacity_available =
		offering_capacity_available(
			offering->class_capacity,
			offering->offering_enrollment_count );

	offering_refresh(
		offering->offering_enrollment_count,
		offering->offering_capacity_available,
		offering->enrollment_list,
		offering->course_name,
		offering->season_name,
		offering->year );
}
