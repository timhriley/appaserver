/* utility/piece_swap.c */
/* ----------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char delimiter;
	int piece1_offset, piece2_offset;
	char input_buffer[ 4096 ];

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s delimiter piece1_offset piece2_offset\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	piece1_offset = atoi( argv[ 2 ] );
	piece2_offset = atoi( argv[ 3 ] );

	while( get_line( input_buffer, stdin ) )
	{
		printf( "%s\n", 
			piece_swap( 	input_buffer,
					delimiter, 
					piece1_offset, 
					piece2_offset ) );
	}
	return 0;
} /* main() */

