/* utility/nl.c */
/* ------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	int with_ff = 0;

	if ( argc == 2 )
	{
		if ( strcmp( argv[ 1 ], "ff" ) != 0 )
		{
			fprintf(stderr, "Usage: nl.e [ff]\n" );
			exit( 1 );
		}
		else
			with_ff = 1;
	}

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\r\n", buffer );
	}

	if ( with_ff ) printf( "%c", 12 );
	return 0;
}
