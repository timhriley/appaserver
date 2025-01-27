/* utility/random_stream.c */
/* ----------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	int up_to;
	char delimiter;
	char input_buffer[ 1024 ];

	if ( argc != 3 )
	{
		fprintf(	stderr,
				"Usage: %s up_to delimiter\n",
				argv[ 0 ] );
		exit( 1 );
	}

	up_to = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	srandom( time( 0 ) );
	while( get_line( input_buffer, stdin ) )
	{
		printf( "%s%c%ld\n",
			input_buffer,
			delimiter,
			timlib_random( up_to ) );
	}
	return 0;
}

