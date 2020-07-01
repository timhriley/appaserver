/* validate_midnight.c */
/* ------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "piece.h"
#include "timlib.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void replace_twenty_four_hundred( 	char *source_destination,
					char *date_string,
					int date_offset,
					int time_offset,
					char delimiter );


int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char date_string[ 1024 ];
	char time_string[ 1024 ];
	int date_offset, time_offset, delimiter;

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s date_offset time_offset delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	date_offset = atoi( argv[ 1 ] );
	time_offset = atoi( argv[ 2 ] );
	delimiter = *argv[ 3 ];

	/* Sample input: "NP205|1999-10-27|2400|rain|5.6" */
	/* ---------------------------------------------- */
	while( get_line( buffer, stdin ) )
	{
		if ( !piece( date_string, delimiter, buffer, date_offset ) )
		{
			fprintf(stderr,
			"Warning in %s: cannot piece the date (%d) in (%s)\n",
				argv[ 0 ], date_offset, buffer );
			continue;
		}

		if ( !piece( time_string, delimiter, buffer, time_offset ) )
		{
			fprintf(stderr,
			"Warning in %s: cannot piece the time (%d) in (%s)\n",
				argv[ 0 ], time_offset, buffer );
			continue;
		}

		if ( strcmp( time_string, "2400" ) == 0 )
		{
			replace_twenty_four_hundred( 	buffer,
							date_string,
							date_offset,
							time_offset,	
							delimiter );
		}
		printf( "%s\n", buffer );
	}
	exit( 0 );
} /* main() */
		
void replace_twenty_four_hundred( 	char *source_destination,
					char *date_string,
					int date_offset,
					int time_offset,
					char delimiter )
{
	DATE *date;

	date = date_yyyy_mm_dd_new( date_string );

	date_increment_days(
		date,
		1.0,
		date_utc_offset() );

	replace_piece(	source_destination,
			delimiter,
			date_display_yyyy_mm_dd( date ),
			date_offset );

	replace_piece(	source_destination,
			delimiter,
			"0000",
			time_offset );

	date_free( date );
} /* replace_twenty_four_hundred() */
