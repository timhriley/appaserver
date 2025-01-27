/* highest_prime_number.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main( int argc, char **argv )
{
	long up_to;
	short *short_array;
	long x, y;
	long square_root;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s up_to\n", argv[ 0 ] );
		exit( 1 );
	}

	up_to = atoi( argv[ 1 ] );

	if ( up_to < 3 )
	{
		fprintf( stderr, "Please enter a number greater than 2\n" );
		exit( 1 );
	}

	short_array = (short*)calloc( up_to + 1, sizeof( short ) );

	if ( !short_array )
	{
		fprintf( stderr, "Error in %s: out of memory.\n", argv[ 0 ] );
		exit( 1 );
	}

	for( x = 2; x <= up_to; x++ ) short_array[ x ] = 1;

	square_root = sqrt( up_to );

	for( x = 2; x <= square_root; x++ )
	{
		for( y = x; y <= up_to; y += x )
		{
			short_array[ y ] = 0;
		}
	}

	for( x = up_to; x; x-- )
	{
		if ( short_array[ x ] )
		{
			printf( "%ld\n", x );
			return 0;
		}
	}
	return 0;
}

