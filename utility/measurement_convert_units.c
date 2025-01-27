/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/measurement_convert_units.c			*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "sql.h"
#include "environ.h"
#include "dictionary.h"
#include "piece.h"
#include "units.h"

int main( int argc, char **argv )
{
	char *units_name;
	char *units_converted_name;
	int value_piece;
	char delimiter;
	int station_piece;
	UNITS_CONVERTED_STDIN *units_converted_stdin;

	appaserver_error_stderr( argc, argv );

	if ( argc < 5 )
	{
		fprintf(stderr,
"Usage: %s units units_converted value_piece delimiter [station_piece]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	units_name = argv[ 1 ];
	units_converted_name = argv[ 2 ];
	value_piece = atoi( argv[ 3 ] );
	delimiter = *argv[ 4 ];

	if ( argc == 6 )
		station_piece = atoi( argv[ 5 ] );
	else
		station_piece = -1;

	units_converted_stdin =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		units_converted_stdin_new(
			units_name,
			units_converted_name,
			value_piece,
			delimiter,
			station_piece );

	units_converted_stdin_execute( units_converted_stdin );

	return 0;
}

