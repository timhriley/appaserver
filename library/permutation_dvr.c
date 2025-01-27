#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

int main( int argc, char **argv )
{
	PERMUTATION *permutation;
	int N;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s N\n", argv[ 0 ] );
		exit( 1 );
	}

	N = atoi( argv[ 1 ] );
	permutation = permutation_new( N );
	permutation_output_permutation_array(
					permutation->permutation_array,
					permutation->n_factoral,
					permutation->N );
	return 0;
} /* main() */

