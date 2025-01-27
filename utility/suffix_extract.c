/* suffix_extract.c */
/* ---------------- */
/* Tim Riley        */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *suffix_extract( char *filename, char *prefix );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s filename prefix\n",
			argv[ 0 ] );
		exit( 1 );
	}
	printf( "%s\n", suffix_extract( argv[ 1 ], argv[ 2 ] ) );
	return 0;

} /*  main() */


char *suffix_extract( char *filename, char *prefix )
{
	return filename + strlen( prefix );
}

