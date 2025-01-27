/* utility/piece_shift_right.c */
/* --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char delimiter;
	char input_buffer[ 4096 ];

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s delimiter\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	while( get_line( input_buffer, stdin ) )
	{
		printf( "%s\n", 
			piece_shift_right(
					input_buffer,
					delimiter ) );
	}
	return 0;
} /* main() */

