#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define PRIMARY_DELIMITER	','
#define SECONDARY_DELIMITER	'|'

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char character;
	char delimiter;
	int count;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s character\n", argv[ 0 ] );
		exit( 1 );
	}

	character = *argv[ 1 ];

	if ( character == PRIMARY_DELIMITER )
		delimiter = SECONDARY_DELIMITER;
	else
		delimiter = PRIMARY_DELIMITER;

	while( get_line( buffer, stdin ) )
	{
		count = character_count( character, buffer );
		printf( "%s%c%d\n", buffer, delimiter, count );
	}
	return 0;
}
