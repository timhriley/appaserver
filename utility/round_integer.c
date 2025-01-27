/* round_integer.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char delimiter;
	int piece_integer;
	double float_value;
	int integer_value;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s piece_integer delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	piece_integer = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	while( get_line( input_buffer, stdin ) )
	{
		if ( !piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_integer ) )
		{
			fprintf( stderr, "%s warning: cannot piece (%s)\n",
				 argv[ 0 ],
				 input_buffer );
			continue;
		}

		float_value = atof( piece_buffer );

		if ( float_value == 0.0 )
			integer_value = 0;
		else
		if ( float_value > 0.0 )
			integer_value = (int)(float_value + 0.5);
		else
			integer_value = (int)(float_value - 0.5);

		sprintf( piece_buffer, "%d", integer_value );

		piece_replace(	input_buffer,
				delimiter,
				piece_buffer,
				piece_integer );

		printf( "%s\n", input_buffer );
	}

	return 0;

} /* main() */

