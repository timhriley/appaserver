/* ------------------------------------------------------------ */
/* cal2linear.c							*/
/* ------------------------------------------------------------ */
/* Tim Riley							*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "julian.h"

int main( int argc, char **argv )
{
	JULIAN *julian;
	char *year_string;
	char *results;
	char full_date[ 16 ];
	int year;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s year\n", argv[ 0 ] );
		exit( 1 );
	}

	year_string = argv[ 1 ];
	year = atoi( year_string );

	if ( year < 1900 || year > 2400 )
	{
		fprintf( stderr,
			 "Error in %s: invalid year %d.\n",
			 argv[ 0 ],
			 year );
		exit( 1 );
	}

	sprintf( full_date, "%s-01-01", year_string );

	julian = julian_yyyy_mm_dd_new( full_date );

	while( julian_get_year_number( julian->current ) == year )
	{
		results = julian_display_yyyy_mm_dd( julian->current );
		printf( "%s\n", results );
		free( results );
		julian->current = julian_increment_day( julian->current );
	}

	return 0;

} /* main() */

