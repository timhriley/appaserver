/* toupper_to_delimiter.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char delimiter;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s: delimiter\n", argv[ 0 ] );
		exit( 1 );
	}
	delimiter = *argv[ 1 ];

	while( get_line( buffer, stdin ) )
		printf( "%s\n", up_string_to_delimiter( buffer, delimiter ) );
	return 0;
} /* main() */
