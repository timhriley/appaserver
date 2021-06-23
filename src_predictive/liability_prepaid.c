/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/liability_prepaid.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "transaction.h"
#include "entity.h"
#include "liability.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *payor_full_name;
	char *payor_street_address;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s payor_full_name payor_street_address\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	payor_full_name = argv[ 1 ];
	payor_street_address = argv[ 2 ];

	printf( "%.2lf\n",
		entity_liability_prepaid(
			payor_full_name,
			payor_street_address ) );

	return 0;
}

