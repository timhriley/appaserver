/* utility/julian_time_list.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "julian.h"
#include "piece.h"
#include "julian_time_double_list.h"

int main( int argc, char **argv )
{
	char *begin_yyyy_mm_dd;
	char *begin_hhmm = {0};
	char *end_yyyy_mm_dd;
	char *end_hhmm = {0};
	double time_segments_per_day;
	double *julian_time;
	LIST *julian_time_double_list;
	char buffer[ 128 ];

	if ( argc != 4 )
	{
		fprintf( stderr, 
"Usage: %s begin_yyyy_mm_dd[:hhmm] end_yyyy_mm_dd[:hhmm] time_segments_per_day\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	begin_yyyy_mm_dd = argv[ 1 ];

	if ( character_count( ':', begin_yyyy_mm_dd ) == 1 )
	{
		piece( buffer, ':', begin_yyyy_mm_dd, 1 );
		begin_hhmm = strdup( buffer );

		piece( buffer, ':', begin_yyyy_mm_dd, 0 );
		begin_yyyy_mm_dd = strdup( buffer );
	}

	end_yyyy_mm_dd = argv[ 2 ];

	if ( character_count( ':', end_yyyy_mm_dd ) == 1 )
	{
		piece( buffer, ':', end_yyyy_mm_dd, 1 );
		end_hhmm = strdup( buffer );

		piece( buffer, ':', end_yyyy_mm_dd, 0 );
		end_yyyy_mm_dd = strdup( buffer );
	}

	time_segments_per_day = atof( argv[ 3 ] );

	julian_time_double_list =
		get_julian_time_double_list(
			begin_yyyy_mm_dd,
			begin_hhmm,
			end_yyyy_mm_dd,
			end_hhmm,
			time_segments_per_day );

	if ( list_rewind( julian_time_double_list ) )
	{
		do {
			julian_time = (double *)
				list_get_pointer( julian_time_double_list );
			
			printf( "%.10lf=%s:%s\n",
				*julian_time,
				julian_to_yyyy_mm_dd( *julian_time ),
				julian_to_hhmm( *julian_time ) );
		} while( list_next( julian_time_double_list ) );
	}
	return 0;
}
