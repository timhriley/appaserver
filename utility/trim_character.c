/* trim_character.c */
/* ---------------- */
/* Tim Riley	    */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	int c;
	int c_to_skip;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s ascii_to_trim\n",
			argv[ 0 ] );
		exit( 1 );
	}
	else
		c_to_skip = atoi( argv[ 1 ] );

	while( ( c = getchar() ) != EOF )
	{
		if ( c != c_to_skip ) putchar( c );
	}

	return 0;

} /* main() */
