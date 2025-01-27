/* output_numbers.sh */
/* ----------------- */

#include <stdio.h>

main( int argc, char **argv )
{
	int start, how_many;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s start how_many\n",
			argv[ 0 ] );
		exit( 1 );
	}

	start = atoi( argv[ 1 ] );
	how_many = atoi( argv[ 2 ] );

	while( how_many-- ) printf( "%d\n", start++ );
	return 0;
}
