/* tolower.c */
/* --------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 2048 ];

	if ( argc > 1 )
		while( --argc )
			printf( "%s\n", low_string( *++argv ) );
	else
	while( get_line( buffer, stdin ) )
		printf( "%s\n", low_string( buffer ) );
	return 0;
} /* main() */
