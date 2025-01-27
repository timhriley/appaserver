#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];

	if ( argc == 2 && strcmp( argv[ 1 ], "nowait" ) == 0 )
		exit( 0 );

	while( get_line( buffer, stdin ) );

	return 0;
}
