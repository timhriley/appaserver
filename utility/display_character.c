/* display_character.c  */
/* -------------------- */
/* Tim Riley		*/
/* -------------------- */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s ascii\n",
			argv[ 0 ] );
		exit( 1 );
	}

	printf( "%c", atoi( argv[ 1 ] ) );
	return 0;

} /* main() */
