#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( argc, argv )
int argc;
char **argv;
{
	double array[ 10 ];

	array[ 0 ] = 1.0;
	array[ 2 ] = 3.0;

	linear_interpolate( array, 0, 2 );
	printf( "should be 1.0 is %lf\n", array[ 0 ] );
	printf( "should be 2.0 is %lf\n", array[ 1 ] );
	printf( "should be 3.0 is %lf\n", array[ 2 ] );
	return 0;
}
