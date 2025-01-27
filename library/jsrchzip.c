/* ---------- */
/* jsrchzip.c */
/* ---------- */
/* ------------------------------------------------------------------
This function inputs the ZIP_CODE, does a binary search in a fixed
length file, and outputs CITY, STATE, LONGITUDE, and LATITUDE 
to the proper places.

It also computes the distance between two zip codes given their
longitude and latitude.

This must be linked with jam!

Tim Riley
-------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "bsrch.h"
#include <limits.h>
#include <math.h>
#include <float.h>

populate_city_state(	char *field_heading, 
			char *with_suppress, 
			char *zipcode_filename )
{
        FILE *f;
        B_SEARCH *b;
        char *full_row;
	char field_full[ 50 ];
	char buffer[ 100 ];
        char zip_code[ 10 ];

	/* Get the zip code */
	/* ---------------- */
	sprintf( field_full, "%s_zip_code", field_heading );
        sm_n_getfield( zip_code, field_full );

        /* If zip code is empty then let protection to the validation */
        /* ---------------------------------------------------------- */
        if ( !*zip_code ) return 0;

        b = init_bsearch( zipcode_filename, 58 );

	if ( !b )
	{
               	sm_err_reset( "Invalid zip code filename" );
		return 0;
	}

        if( reset_fetch_bsearch( b, zip_code ) == -1 )
	{
		if ( *with_suppress != 'Y' )
                	sm_err_reset( "Zip Code Not Found" );
		return 0;
	}

        full_row = fetch_each_bsearch( b );

        piece( buffer, '|', full_row, 1 );
	sprintf( field_full, "%s_city", field_heading );
        sm_n_putfield( field_full, buffer );

        piece( buffer, '|', full_row, 2 );
	sprintf( field_full, "%s_state", field_heading );
        sm_n_putfield( field_full, buffer );

        piece( buffer, '|', full_row, 4 );
	sprintf( field_full, "%s_longitude", field_heading );
        sm_n_putfield( field_full, buffer );

        piece( buffer, '|', full_row, 5 );
	sprintf( field_full, "%s_latitude", field_heading );
        sm_n_putfield( field_full, buffer );

        free_bsearch( b );

        return 0;

} /* populate_city_state() */




get_longitude_latitude(	char *zip_code, 
			double *longitude,
			double *latitude,
			char *zipcode_filename )
{
        FILE *f;
        B_SEARCH *b;
        char *full_row;
	char buffer[ 100 ];

	if ( !*zip_code ) return 0;

        b = init_bsearch( zipcode_filename, 58 );

	if ( !b )
	{
               	sm_err_reset( "Invalid zip code filename" );
		return 0;
	}

        if( reset_fetch_bsearch( b, zip_code ) == -1 )
	{
		return 0;
	}

        full_row = fetch_each_bsearch( b );

        piece( buffer, '|', full_row, 4 );
	*longitude = atof( buffer );

        piece( buffer, '|', full_row, 5 );
	*latitude = atof( buffer );

        free_bsearch( b );

        return 1;

} /* get_longitude_latitude() */




populate_zip_distance( char *this_zip_code, char *that_zip_code )
{
	double compute_distance();
	double this_longitude, this_latitude;
	double that_longitude, that_latitude;
	char buffer[ 50 ];

	if ( !get_longitude_latitude(	this_zip_code,
					&this_longitude,
					&this_latitude,
					"..\\lib\\search_z.rnd" ) )
	{
		sm_err_reset( "Cannot get_longitude_latitude()" );
		return 0;
	}

	if ( !get_longitude_latitude(	that_zip_code,
					&that_longitude,
					&that_latitude,
					"..\\lib\\search_z.rnd" ) )
	{
		sm_err_reset( "Cannot get_longitude_latitude()" );
		return 0;
	}


	sprintf( buffer, "%.0lf", 
		compute_distance( 	this_longitude,
					this_latitude,
					that_longitude,
					that_latitude ) );

        sm_n_putfield( "zip_distance", buffer );
	return 0;

} /* populate_zip_distance() */



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
