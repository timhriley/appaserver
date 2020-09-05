/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_education/registration_trigger.c	*/
/* ------------------------------------------------------------ */
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

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
#include "registration.h"
#include "registration_fns.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void registration_trigger(
				char *student_full_name,
				char *street_address,
				char *season_name,
				int year );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s student_full_name street_address season_name year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	state = argv[ 5 ];

	if ( !*season_name
	||   strcmp( season_name, "season_name" ) == 0 )
	{
		exit( 0 );
	}

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		registration_trigger(
			student_full_name,
			street_address,
			season_name,
			year );
	}
	return 0;
}

void registration_trigger(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year )
{
	REGISTRATION *registration;

	if ( ! ( registration =
			registration_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				1 /* fetch_enrollment_list */ ) ) )
	{
		return;
	}

	if ( ! ( registration =
			registration_steady_state(
				registration->student_full_name,
				registration->street_address,
				registration->season_name,
				registration->year,
				registration->registration_enrollment_list,
				registration->registration_payment_list ) ) )
	{
		fprintf(stderr,
		"%s/%s()/%d: registration_steady_state() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	registration_update(
		registration->registration_tuition,
		registration->registration_payment_total,
		registration->registration_invoice_amount_due,
		registration->student_full_name,
		registration->street_address,
		registration->season_name,
		registration->year );
}

