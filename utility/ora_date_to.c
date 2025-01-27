/* ora_date_to.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

int main( int argc, char **argv )
{
	int piece_offset;
	char *format_mask;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char *date_string;

	if ( argc < 2 )
	{
		fprintf( stderr, 
		"Usage: %s piece_offset [yyyy_mm_dd]\n", argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );

	if ( argc == 3 )
		format_mask = argv[ 2 ];
	else
		format_mask = "yyyy_mm_dd";

	while( get_line( input_buffer, stdin ) )
	{

		if ( ! piece( piece_buffer, '|', input_buffer, piece_offset ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		if ( strcmp( format_mask, "yyyy_mm_dd" ) == 0 )
		{
			date_string =
				timlib_ora_date_to_yyyy_mm_dd(
					piece_buffer );

		}
		else
		if ( strcmp( format_mask, "julian" ) == 0 )
		{
			date_string =
				timlib_yyyy_mm_dd_to_julian(
					timlib_ora_date_to_yyyy_mm_dd(
						piece_buffer ) );
		}
		else
		{
			date_string = piece_buffer;
		}

		printf( "%s\n", 
			replace_piece(	input_buffer,
					'|',
					date_string, 
					piece_offset ) );
	}

	return 0;

} /* main() */

