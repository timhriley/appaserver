/* utility/piece_replace.c */
/* ----------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

/* Global variables */
/* ---------------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	int piece_offset;
	char delimiter;
	char buffer[ 65536 ];
	char *new_data;

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s delimiter piece_offset new_data\n", 
			 argv[ 0 ] );
		return 0;
	}

	delimiter = *argv[ 1 ];
	piece_offset = atoi( argv[ 2 ] );
	new_data = argv[ 3 ];

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n", piece_replace(	buffer, 
						delimiter,
						new_data,
						piece_offset ) );
	}

	return 0;

} /* main() */


