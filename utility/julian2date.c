/* julian2date.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "julian.h"

int main( int argc, char **argv )
{
	int piece_offset;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char date_string[ 64 ];

	if ( argc != 2 )
	{
		fprintf( stderr, 
		"Usage: %s piece_offset\n", argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );

	while( get_line( input_buffer, stdin ) )
	{
		if ( !piece( piece_buffer, '|', input_buffer, piece_offset ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		sprintf( date_string,
			 "%s", 
			 julian_display_yyyy_mm_dd( atof( piece_buffer ) ) );

		printf( "%s\n", 
			replace_piece(	input_buffer,
					'|',
					date_string, 
					piece_offset ) );
	}

	return 0;

} /* main() */

