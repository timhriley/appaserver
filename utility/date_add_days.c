/* date_add_days.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"

int main( int argc, char **argv )
{
	double days_offset;
	int source_date_piece_offset;
	int destination_date_piece_offset;
	char buffer[ 1024 ];
	char date_buffer[ 128 ];
	DATE *d;
	int delimiter;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s days_offset source_date_piece_offset destination_date_piece_offset delimiter\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	days_offset = atof( argv[ 1 ] );
	source_date_piece_offset = atoi( argv[ 2 ] );
	destination_date_piece_offset = atoi( argv[ 3 ] );
	delimiter = *argv[ 4 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer ) continue;

		if ( !piece( 	date_buffer, 
				delimiter, 
				buffer, 
				source_date_piece_offset ) )
		{
			fprintf( stderr, "%s\n", buffer );
			continue;
		}

		d = date_yyyy_mm_dd_new( date_buffer );

		date_increment_days( d, days_offset );

		piece_replace(	buffer,
				delimiter,
				date_display_yyyy_mm_dd( d ),
				destination_date_piece_offset );

		printf( "%s\n", buffer );

		date_free( d );
	}
	return 0;
} /* main() */

