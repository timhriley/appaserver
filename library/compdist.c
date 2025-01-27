#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <float.h>

/* compute the distance between two points */
/* --------------------------------------- */
double compute_distance( double longitude1, double latitude1,
                         double longitude2, double latitude2 )
{
        if( ( longitude1 == 0 )
        ||  ( latitude1  == 0 )
        ||  ( longitude2 == 0 )
        ||  ( latitude2  == 0 ) )
                return INT_MAX;

        /* convert from degrees  to radians ,note: 57.3 = 180/pi */
        /* ----------------------------------------------------- */
        longitude1 /= 57.3;
        latitude1  /= 57.3;
        longitude2 /= 57.3;
        latitude2  /= 57.3;

        return 3959.0 * acos( (sin(latitude1) * sin(latitude2)) +
                        ( cos(latitude1) * cos(latitude2) *
                         cos( longitude2 - longitude1 ) ) );

} /* end compute_distance */

main( int argc, char **argv )
{
	double compute_distance();

	if ( argc != 5 )
	{
		fprintf( stderr, "usage: compdist lng1 lat1 lng2 lat2\n" );
		exit( 1 );
	}

	printf( "distance: %lf\n",
		compute_distance( 	atof( argv[ 1 ] ),
					atof( argv[ 2 ] ),
					atof( argv[ 3 ] ),
					atof( argv[ 4 ] ) ) );
}
