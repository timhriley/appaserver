/* search_replace_character_limited.c */
/* ---------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char search_delimiter;
	char replace_delimiter;
	int how_many;

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s: search_delimiter replace_delimiter how_many\n",
			argv[ 0 ] );
		exit( 1 );
	}

	search_delimiter = *argv[ 1 ];
	replace_delimiter = *argv[ 2 ];
	how_many = atoi( argv[ 3 ] );

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n",
			timlib_search_replace_character_limited(
				buffer,
				search_delimiter,
				replace_delimiter,
				how_many ) );
	}
	return 0;
} /* main() */
