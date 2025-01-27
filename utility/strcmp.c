/* strcmp.c 					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char **argv )
{
	char *str1, *str2;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s str1 str2\n",
			argv[ 0 ] );
		exit( 1 );
	}

	str1 = argv[ 1 ];
	str2 = argv[ 2 ];

	printf( "%d\n", strcmp( str1, str2 ) );
	return 0;
} /* main() */

