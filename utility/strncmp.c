/* strncmp.c 					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char **argv )
{
	char *str1, *str2;
	int length;

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s str1 str2 length\n",
			argv[ 0 ] );
		exit( 1 );
	}

	str1 = argv[ 1 ];
	str2 = argv[ 2 ];
	length = atoi( argv[ 3 ] );

	printf( "%d\n", strncmp( str1, str2, length ) );
	return 0;
} /* main() */

