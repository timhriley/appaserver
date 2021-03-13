/* $APPASERVER_HOME/utility/moving_average_piece.c		*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "moving_average.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	int number_of_days;
	int date_piece;
	int value_piece;
	char delimiter;
	char input[ 1024 ];
	MOVING_AVERAGE *moving_average;
	char date_buffer[ 128 ];
	char value_buffer[ 128 ];
	FILE *output_pipe;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 5 )
	{
		fprintf(stderr,
	"Usage: %s number_of_days date_piece value_piece delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	number_of_days = atoi( argv[ 1 ] );
	date_piece = atoi( argv[ 2 ] );
	value_piece = atoi( argv[ 3 ] );
	delimiter = *argv[ 4 ];

	moving_average =
		moving_average_new(
			number_of_days,
			delimiter,
			aggregate_statistic_none,
			(char *)0 /* begin_date_string */ );

	while( string_input( input, stdin, 1024 ) )
	{
		if ( !*input ) continue;

		if ( !piece(	date_buffer,
				moving_average->delimiter,
				input,
				date_piece ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: piece(%d:%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				date_piece,
				input );
			continue;
		}

		if ( !piece(	value_buffer,
				moving_average->delimiter,
				input,
				value_piece ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: piece(%d:%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				date_piece,
				input );
			continue;
		}

		if ( *value_buffer
		&&   strcasecmp( value_buffer, "null" ) != 0 )
		{
			moving_average_piece_set(
				moving_average->value_list,
				atof( value_buffer ),
				strdup( date_buffer ),
				strdup( input ) );
		}
	}

	output_pipe = popen( "sort", "w" );

	moving_average_piece_display(
		output_pipe,
		moving_average->value_list,
		moving_average->number_of_days,
		moving_average->delimiter,
		value_piece );

	pclose( output_pipe );
	return 0;
}

