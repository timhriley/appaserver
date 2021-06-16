/* ----------------------------------------------------- */
/* $APPASERVER_HOME/src_education/registration_trigger.c */
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
#include "registration.h"
#include "offering.h"
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "course.h"
#include "registration.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

void registration_trigger_insert(
			REGISTRATION *registration );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *student_street_address;
	char *season_name;
	int year;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s student_full_name student_street_address season_name year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	student_street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	state = argv[ 5 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		REGISTRATION *registration;

		if ( ! ( registration =
				registration_fetch(
					student_full_name,
					student_street_address,
					season_name,
					year ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: registration_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		registration_trigger_insert( registration );
	}

	return 0;
}

void registration_trigger_insert(
			REGISTRATION *registration )
{
	char system_string[ 1024 ];

	registration->registration_date_time =
		date_now19( date_utc_offset() );

	registration_update(
		registration->registration_date_time,
		registration->student_entity->full_name,
		registration->student_entity->street_address,
		registration->semester->season_name,
		registration->semester->year );

	sprintf(system_string,
		"registration_invoice_amount_due.sh \"%s\" \"%s\" \"%s\" %d",
		registration->student_entity->full_name,
		registration->student_entity->street_address,
		registration->semester->season_name,
		registration->semester->year );

	if ( system( system_string ) ){};
}
