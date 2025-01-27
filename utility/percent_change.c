/* utility/percent_change.c */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	double base = 0.0;
	double number;
	int first_time = 1;
	char delimiter;
	int value_piece;
	char value_buffer[ 128 ];

	if ( argc != 3 )
	{
		fprintf(	stderr,
				"Usage: %s value_piece delimiter\n",
				argv[ 0 ] );
		fprintf( stderr, "If no label, then set to -1\n" );
		exit( 1 );
	}

	value_piece = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !piece(	value_buffer,
				delimiter,
				buffer,
				value_piece ) )
		{
			fprintf(stderr,
			"Warning: cannot get data in (%s)\n",
				buffer );
			continue;
		}
		if ( first_time )
		{
			base = atof( value_buffer );
			first_time = 0;
			printf( "%s: ----\n", buffer );
		}
		else
		{
			number = atof( value_buffer );

			if ( !base )
			{
				printf( "%s: ----\n", buffer );
			}
			else
			{
				printf( "%s: %.2lf\n",
					buffer,
					( number - base ) / base * 100 );
			}
			base = number;
		}
	}
	return 0;

} /* main() */

