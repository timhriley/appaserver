/* dist_dvr.c */
/* ---------- */
/* Driver program for distance between 2 zip codes */
/* ----------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "zip_code_distance.h"

int main( argc, argv )
int argc;
char **argv;
{
	double compute_distance_between_zips();

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s zip_code1 zip_code2\n", argv[ 0 ] );
		exit( 1 );
	}
	printf( "Distance: %lf\n", 
		compute_distance_between_zips( argv[ 1 ], argv[ 2 ] ) );
	return 0;
}
