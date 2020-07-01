/* utility/julian_add_hours.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "julian.h"
#include "piece.h"

int main( int argc, char **argv )
{
	int hours_offset;
	int date_piece_offset;
	int time_piece_offset;
	char buffer[ 1024 ];
	char date_buffer[ 128 ];
	char time_buffer[ 128 ];
	JULIAN *j;
	int delimiter;
	char append_time_character = {0};

	if ( argc < 5 )
	{
		fprintf( stderr,
"Usage: %s hours_offset date_piece_offset time_piece_offset delimiter [append_time_character]\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	hours_offset = atoi( argv[ 1 ] );
	date_piece_offset = atoi( argv[ 2 ] );
	time_piece_offset = atoi( argv[ 3 ] );
	delimiter = *argv[ 4 ];

	if ( argc == 6 )
		append_time_character = *argv[ 5 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer ) continue;

		if ( !piece( 	date_buffer, 
				delimiter, 
				buffer, 
				date_piece_offset ) )
		{
			fprintf( stderr, "%s\n", buffer );
			continue;
		}

		if ( !piece( 	time_buffer, 
				delimiter, 
				buffer, 
				time_piece_offset ) )
		{
			fprintf( stderr, "%s\n", buffer );
			continue;
		}

fprintf( stderr, "got date_buffer = [%s]\n", date_buffer );
fprintf( stderr, "got time_buffer = [%s]\n", time_buffer );

		j = julian_yyyy_mm_dd_hhmm_new( date_buffer, time_buffer );
		j->current = julian_increment_hours( j->current, hours_offset );

fprintf( stderr, "before buffer = [%s]\n", buffer );

		piece_replace(	buffer,
				delimiter,
				julian_display_yyyy_mm_dd( j->current ),
				date_piece_offset );

fprintf( stderr, "after buffer = [%s]\n", buffer );

		if ( append_time_character )
		{
			sprintf( time_buffer,
				 "%s%c",
				julian_display_hhmm( j->current ),
				append_time_character );
		}
		else
		{
			strcpy( time_buffer,
				julian_display_hhmm( j->current ) );
		}

		piece_replace(	buffer,
				delimiter,
				time_buffer,
				time_piece_offset );

		printf( "%s\n", buffer );
		julian_free( j );
	}
	return 0;
} /* main() */

