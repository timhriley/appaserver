#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* compute the bearing between two points */
/* -------------------------------------- */
double compute_bearing( double longitude1, double latitude1,
                        double longitude2, double latitude2 )
{
	double N, D1, D2, B, BEARING;

        if( ( longitude1 == 0 )
        ||  ( latitude1  == 0 )
        ||  ( longitude2 == 0 )
        ||  ( latitude2  == 0 ) )
                return -1.0;

        /* convert from degrees  to radians, note: 57.3 = 180/pi */
        /* ----------------------------------------------------- */
/*
        longitude1 /= 57.3;
        latitude1  /= 57.3;
        longitude2 /= 57.3;
        latitude2  /= 57.3;
*/

/*
printf( "N = sin( %lf / 57.3 ) *  	    sin( %lf / 57.3 - %lf / 57.3\n",
latitude1, longitude2, longitude1 );
*/

	N = sin( latitude1 / 57.3 ) * 
	    sin( longitude2 / 57.3 - longitude1 / 57.3 );

/*
printf( "N = :%lf\n", N );

printf( "D1 = sin( %lf / 57.3 ) * cos( %lf / 57.3",
latitude2, latitude1 );
*/

	D1 = sin( latitude2 / 57.3 ) * cos( latitude1 / 57.3 );
	D2 = cos( latitude2 / 57.3 ) * sin( latitude1 / 57.3 ) * 
	     cos( longitude2 / 57.3 - longitude1 / 57.3 );

/*
printf( "D1 = :%lf\n", D1 );
printf( "D2 = :%lf\n", D2 );
*/

	B = atan( N / ( D1 - D2 ) ) * 57.3;

/*
printf( "B = %lf\n", B );
*/

	if ( ( D1 - D2 ) > 0 )
		BEARING = 180 + B;
	else
		BEARING = B;

	if ( B < 0 )
		BEARING = 360 + B;

	return BEARING;

} /* end compute_bearing */


int main( int argc, char **argv )
{
	double compute_bearing();

/*
	if ( argc != 5 )
	{
		fprintf( stderr, "usage: compbear lng1 lat1 lng2 lat2\n" );
		exit( 1 );
	}

	printf( "bearing: %lf\n",
		compute_bearing( 	atof( argv[ 1 ] ),
					atof( argv[ 2 ] ),
					atof( argv[ 3 ] ),
					atof( argv[ 4 ] ) ) );
*/
	printf( "bearing: %lf\n",
		compute_bearing( 	
					73.9380, 42.8145,
					118.2420, 34.0515 ) );
	printf( "bearing: %lf\n",
		compute_bearing( 	
					118.2420, 34.0515,
					73.9380, 42.8145 ) );
	return 0;
}
