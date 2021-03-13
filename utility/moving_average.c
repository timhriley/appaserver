/* $APPASERVER_HOME/utility/moving_average.c			*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "piece.h"
#include "moving_average.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char delimiter;
	char buffer[ 1024 ];
	MOVING_AVERAGE *moving_average;
	LIST *value_list;
	int number_of_days;
	char value_string[ 128 ];
	char date_label_string[ 128 ];
	char *aggregate_statistic_string;
	char *begin_date_string;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 5 )
	{
		fprintf(stderr,
	"Usage: %s number_of_days delimiter aggregate_statistic begin_date\n",
			argv[ 0 ] );
		exit( 1 );
	}

	number_of_days = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];
	aggregate_statistic_string = argv[ 3 ];
	begin_date_string = argv[ 4 ];

	if ( !*aggregate_statistic_string
	||   strcmp( aggregate_statistic_string, "aggregate_statistic" ) == 0 )
	{
		aggregate_statistic_string = "average";
	}

	moving_average =
		moving_average_new(
			number_of_days,
			delimiter,
			aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string,
				daily ),
			begin_date_string );

	value_list = moving_average->value_list;

	while( get_line( buffer, stdin ) )
	{
		piece(	date_label_string,
			moving_average->delimiter,
			buffer,
			0 );

		if ( !piece(	value_string,
				moving_average->delimiter,
				buffer,
				1 ) )
		{
			continue;
		}

		if ( *value_string
		&&   strcasecmp( value_string, "null" ) != 0 )
		{
			moving_average_set(	value_list,
						atof( value_string ),
						strdup( date_label_string ),
						0 /* not is_null */ );
		}
		else
		{
			moving_average_set(	value_list,
						0.0,
						strdup( date_label_string ),
						1 /* is_null */ );
		}
	}
	moving_average_display(
				value_list,
				moving_average->number_of_days,
				moving_average->delimiter,
				moving_average->aggregate_statistic,
				moving_average->begin_date_string );

	return 0;
}

