/* utility/increment.c		*/
/* ---------------------- 	*/

#include <stdio.h>
#include <stdlib.h>


int main( int argc, char **argv )
{
	int up_to;
	int i;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s up_to\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	up_to = atoi( argv[ 1 ] );

	for( i = 0; i < up_to; i++ ) printf( "%d\n", i + 1 );
	return 0;

} /* main() */
