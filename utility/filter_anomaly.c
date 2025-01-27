/* utility/filter_anomaly.c				*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

void filter_threshold(	char delimiter,
			int piece_offset,
			double threshold,
			char *argv_0 );

int main( int argc, char **argv )
{
	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter piece_offset threshold\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	filter_threshold(	*argv[ 1 ],
				atoi( argv[ 2 ] ),
				atof( argv[ 3 ] ),
				argv[ 0 ] );

	return 0;

} /* main() */

void filter_threshold(	char delimiter,
			int piece_offset,
			double threshold,
			char *argv_0 )
{
	char buffer[ 4096 ];
	char value_string[ 4096 ];
	double absolute_value;
	double difference;

	while( get_line( buffer, stdin ) )
	{
		if ( !piece(	value_string,
				delimiter,
				buffer,
				piece_offset ) )
		{
			fprintf( stderr,
			 "Warning in %s: cannot piece(%d) in %s\n",
			 		argv_0,
			 		piece_offset,
					buffer );
			continue;
		}

		if (	!*value_string
		||	timlib_strcmp( value_string, "null" ) == 0 )
		{
			continue;
		}

		absolute_value = abs_float( atof( value_string ) );

		difference = threshold - absolute_value;

		if ( difference < 0.0 ) printf( "%s\n", buffer );
	}

} /* filter_threshold() */

