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

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

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
		fprintf( stderr,
			"state in {insert,update}\n" );
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
		registration_fetch_update(
			student_full_name,
			street_address,
			season_name,
			year );
	}
	return 0;
}

