/* range.c 			*/
/* ---------------------------- */
/* Copyright 1999 By Tim Riley  */
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	int i;
	int range;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s zero_based_range\n", argv[ 0 ] );
		exit( 1 );
	}

	range = atoi( argv[ 1 ] );
	for( i = 0; i < range; i++ )
		printf( "%d ", i );
	printf( "\n" );
	return 0;
} /* main() */
