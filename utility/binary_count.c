/* binary_count.c					*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	unsigned count_up_to;
	unsigned count;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s count_up_to\n", argv[ 0 ] );
		return 1;
	}

	count_up_to = atoi( argv[ 1 ] );

	for ( count = 0; count < count_up_to; count++ )
		printf( "%s\n", timlib_integer2binary( count ) );

	return 0;
}
