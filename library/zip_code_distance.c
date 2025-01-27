/* ------------------------------------------------------------------
zip_code_distance.c

Tim Riley
-------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include "timlib.h"
#include "bsrch.h"
#include "piece.h"
#include "zip_code_distance.h"

double compute_distance_between_zips( char *zip_code1, char *zip_code2 )
{
	void convert_zip_to_longitude_latitude( double *longitude, 
						double  *latitude, 
						char *zip_code,
						B_SEARCH *b );
	double longitude_1, latitude_1;
	double longitude_2, latitude_2;
	double compute_distance_between_points();
        B_SEARCH *b;

        b = init_bsearch( SEARCH_ZIP_FILE, 58 );

	if ( !b )
	{
               	fprintf( stderr, "Invalid zip code filename: %s\n",
			 SEARCH_ZIP_FILE );
		exit( 1 );
	}

	convert_zip_to_longitude_latitude(&longitude_1, 
					  &latitude_1, 
					  zip_code1,
					  b );

	convert_zip_to_longitude_latitude(&longitude_2, 
					  &latitude_2, 
					  zip_code2,
					  b );
        free_bsearch( b );

	return compute_distance_between_points( 	longitude_1,
							latitude_1,
							longitude_2,
							latitude_2 );

} /* compute_distance_between_zips() */


/* compute the distance between two points */
/* --------------------------------------- */
double compute_distance_between_points( double longitude1, double latitude1,
                         		double longitude2, double latitude2 )
{
	double diff_longitude, diff_latitude;
	double return_value;

        if( ( longitude1 == 0 )
        ||  ( latitude1  == 0 )
        ||  ( longitude2 == 0 )
        ||  ( latitude2  == 0 ) )
                return -1.0;


	diff_longitude = fabs( longitude1 - longitude2 );
	diff_latitude = fabs( latitude1 - latitude2 );

	return_value = 
	(diff_longitude + diff_latitude) * 69.1 * cos( latitude1 / 57.3 );

/* W/O cos()
	return_value = 
	(diff_longitude + diff_latitude) * 69.1;
*/

	return return_value;

#ifdef NEVER_DEFINED
        /* convert from degrees to radians, note: 57.3 = 180/pi */
        /* ---------------------------------------------------- */
        longitude1 /= 57.3;
        latitude1  /= 57.3;
        longitude2 /= 57.3;
        latitude2  /= 57.3;

        return 3959.0 * acos( (sin(latitude1) * sin(latitude2)) +
                        ( cos(latitude1) * cos(latitude2) *
                         cos( longitude2 - longitude1 ) ) );
#endif

} /* end compute_distance_between_points */


double compute_distance_between_zip_and_points(
					char *zip_code,
                         		double longitude_2, double latitude_2 )
{
	void convert_zip_to_longitude_latitude( double *longitude, 
						double  *latitude, 
						char *zip_code,
						B_SEARCH *b );
	double longitude_1, latitude_1;
        B_SEARCH *b;

        b = init_bsearch( SEARCH_ZIP_FILE, 58 );

	if ( !b )
	{
               	fprintf( stderr, "Invalid zip code filename: %s\n",
			 SEARCH_ZIP_FILE );
		exit( 1 );
	}

	convert_zip_to_longitude_latitude( &longitude_1, 
					   &latitude_1, 
					   zip_code,
					   b );

        free_bsearch( b );

	return compute_distance_between_points( 	longitude_1,
							latitude_1,
							longitude_2,
							latitude_2 );

} /* compute_distance_between_zip_and_points() */



void convert_zip_to_longitude_latitude( double *longitude, 
					double  *latitude, 
					char *zip_code,
					B_SEARCH *b )
{
        char *full_row;
	char piece_buffer[ 100 ];

        if( reset_fetch_bsearch( b, zip_code ) == -1 )
	{
		*longitude = -1.0;
		*latitude = -1.0;
		return;
	}

        full_row = fetch_each_bsearch( b );

	*longitude = atof( piece( piece_buffer, '|', full_row, 4 ) );
	*latitude = atof( piece( piece_buffer, '|', full_row, 5 ) );
}

