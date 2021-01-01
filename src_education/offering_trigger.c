/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/offering_trigger.c	*/
/* ---------------------------------------------------- */
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

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
#include "semester.h"
#include "offering.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
/*
void offering_trigger(
				char *course_name,
				char *season_name,
				int year );
*/

int main( int argc, char **argv )
{
	char *application_name;
	char *course_name;
	char *season_name;
	int year;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
			 "Usage: %s course_name season_name year state\n",
			 argv[ 0 ] );
		fprintf( stderr,
			 "state in {insert,update,delete,enrollment}\n" );
		exit ( 1 );
	}

	course_name = argv[ 1 ];
	season_name = argv[ 2 ];
	year = atoi( argv[ 3 ] );
	state = argv[ 4 ];

	if ( !*season_name
	||   strcmp( season_name, "season_name" ) == 0 )
	{
		exit( 0 );
	}

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "enrollment" ) ==  0 )
	{
		offering_fetch_update(
			course_name,
			season_name,
			year );
	}
	return 0;
}

#ifdef NOT_DEFINED
void offering_trigger(
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;

	if ( ! ( offering =
			offering_fetch(
				course_name,
				season_name,
				year,
				1 /* fetch_course */,
				1 /* fetch_enrollment_list */ ) ) )
	{
		return;
	}

	if ( ! ( offering =
			offering_steady_state(
				offering,
				offering->offering_enrollment_list ) ) )
	{
		fprintf(stderr,
			"%s/%s()/%d: offering_steady_state() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	offering_update(
		offering->offering_enrollment_count,
		offering->offering_capacity_available,
		offering->course->course_name,
		offering->semester->season_name,
		offering->semester->year );
}
#endif

