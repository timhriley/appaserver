/* invest2:$root/utility/tim/piece_zap_if_bigger.c */
/* ----------------------------------------------- */

#include <stdio.h>
#include "piece.h"
#include "timlib.h"

main( argc, argv )
int argc;
char **argv;
{
	int piece_offset;
	int max_width;

	if ( argc != 3 )
	{
		fprintf( stderr, 
		"Usage: piece_zap_if_bigger.e piece_offset max_width\n" );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	max_width = atoi( argv[ 2 ] );

	piece_zap_if_bigger( piece_offset, max_width );
	return 0;

} /* main() */



piece_zap_if_bigger( piece_offset, max_width )
int piece_offset;
int max_width;
{
	char buffer[ 1024 ];
	char piece_buffer[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		piece( piece_buffer, '|', buffer, piece_offset );
		if ( strlen( piece_buffer ) > max_width )
		{
			replace_piece( buffer, '|', "", piece_offset );
		}

		printf( "%s\n", buffer );
	}

} /* piece_zap_if_bigger() */


