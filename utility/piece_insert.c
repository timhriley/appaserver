/* utility/piece_insert.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	int piece_offset;
	char delimiter;
	char *string2insert;
	char input_buffer[ 1024 ];

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s offset delimiter string2insert\n", 
			 argv[ 0 ] );
		return 0;
	}

	piece_offset = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];
	string2insert = argv[ 3 ];

	while( get_line( input_buffer, stdin ) )
	{
		printf( "%s\n",
			piece_insert(	input_buffer, 
					delimiter, 
					string2insert,
					piece_offset ) );
	}
	return 0;

} /* main() */
