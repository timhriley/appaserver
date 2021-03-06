/* oracle_date_to_mysql.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

int main( int argc, char **argv )
{
	int piece_offset;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char *date_string;
	char delimiter;

	if ( argc != 3 )
	{
		fprintf( stderr, 
		"Usage: %s piece_offset delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	while( get_line( input_buffer, stdin ) )
	{

		if ( ! piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		date_string =
			timlib_oracle_date2mysql_date_string(
				piece_buffer );

		printf( "%s\n", 
			replace_piece(	input_buffer,
					delimiter,
					date_string, 
					piece_offset ) );
	}

	return 0;

} /* main() */

