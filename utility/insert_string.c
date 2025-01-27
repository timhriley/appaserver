/* insert_string.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ] = {0};
	char *sub_string;
	int position;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			"Usage: %s sub_string position\n", argv[ 0 ] );
		exit( 1 );
	}

	sub_string = argv[ 1 ];
	position = atoi( argv[ 2 ] );

	if ( position < 0 )
	{
		fprintf( stderr, "Only natural values allowed for position\n" );
		exit( 1 );
	}
	
	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n", insert_string( buffer, sub_string, position ) );
	}

	return 0;
} /* main() */
