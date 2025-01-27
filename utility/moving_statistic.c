/* $APPASERVER_HOME/utility/moving_statistic.c			*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "moving_statistic.h"

int main( int argc, char **argv )
{
	int days_count;
	int value_piece;
	char delimiter;
	char *aggregate_statistic_string;
	char *begin_date_string;
	MOVING_STATISTIC *moving_statistic;
	char input[ 1024 ];

	appaserver_error_stderr( argc, argv );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s days_count value_piece delimiter aggregate_statistic begin_date\n",
			argv[ 0 ] );
		exit( 1 );
	}

	days_count = atoi( argv[ 1 ] );
	value_piece = atoi( argv[ 2 ] );
	delimiter = *argv[ 3 ];
	aggregate_statistic_string = argv[ 4 ];
	begin_date_string = argv[ 5 ];

	if ( !*aggregate_statistic_string
	||   strcmp( aggregate_statistic_string, "aggregate_statistic" ) == 0 )
	{
		aggregate_statistic_string = "average";
	}

	moving_statistic =
		moving_statistic_new(
			days_count,
			value_piece,
			delimiter,
			aggregate_statistic_get(
				aggregate_statistic_string,
				daily ),
			begin_date_string );

	while( string_input( input, stdin, 1024 ) )
	{
		moving_statistic_input(
			moving_statistic->moving_statistic_value_list,
			moving_statistic->delimiter,
			strdup( input ) );
	}

	moving_statistic_display(
		moving_statistic->days_count,
		moving_statistic->value_piece,
		moving_statistic->delimiter,
		moving_statistic->aggregate_statistic,
		moving_statistic->begin_date_string,
		moving_statistic->moving_statistic_value_list );

	return 0;
}

