/* $APPASERVER_HOME/utility/round2five_minutes.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"

int main( int argc, char **argv )
{
	char input_buffer[ 4096 ];
	char date_buffer[ 1024 ];
	char time_buffer[ 1024 ];
	char *date_display;
	char *time_display;
	DATE *date;
	int date_piece_offset, time_piece_offset, delimiter;

	if ( argc != 4 )
	{
		fprintf( stderr,
		"Usage: %s date_piece_offset time_piece_offset delimiter\n",
			 argv[ 0 ] );
		fprintf( stderr, "Note: date is expected as YYYY-MM-DD\n" );
		fprintf( stderr, "Note: time is expected as HHMM\n" );
		exit( 1 );
	}

	date_piece_offset = atoi( argv[ 1 ] );
	time_piece_offset = atoi( argv[ 2 ] );
	delimiter = *argv[ 3 ];

	while( get_line( input_buffer, stdin ) )
	{
		if ( !piece(	date_buffer,
				delimiter,
				input_buffer,
				date_piece_offset ) )
		{
			fprintf( stderr, "%s\n", input_buffer );
			continue;
		}

		if ( !piece(	time_buffer,
				delimiter,
				input_buffer,
				time_piece_offset ) )
		{
			fprintf( stderr, "%s\n", input_buffer );
			continue;
		}

		date = date_yyyy_mm_dd_new( date_buffer );

		date_set_time_hhmm( date, time_buffer );

		date_round2five_minutes( date );

		time_display =
			date_hhmm( date );

		date_display =
			date_yyyy_mm_dd(
				date );

		piece_replace(	input_buffer,
				delimiter,
				date_display,
				date_piece_offset );

		piece_replace(	input_buffer,
				delimiter,
				time_display,
				time_piece_offset );

		free( date_display );
		free( time_display );

		printf( "%s\n", input_buffer );

		date_free( date );
	}
	return 0;
}

