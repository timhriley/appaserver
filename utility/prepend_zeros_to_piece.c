/* prepend_zeros_to_piece.c */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

/* Prototypes */
/* ---------- */
void prepend_zeros_to_piece(	char *source_destination, 
				int field_size );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char delimiter;
	int piece_offset, field_size;

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s delimiter piece_offset field_size\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	piece_offset = atoi( argv[ 2 ] );
	field_size = atoi( argv[ 3 ] );

	while( get_line( input_buffer, stdin ) )
	{
		if ( *input_buffer && *input_buffer != '#' )
		{

			if ( !piece(	piece_buffer, 
					delimiter, 
					input_buffer, 
					piece_offset ) )
			{
				fprintf(stderr,
				"ERROR in %s: Cannot get piece(%d) from (%s)\n",
					argv[ 0 ],
					piece_offset,
					input_buffer );
				continue;
			}
	
			prepend_zeros_to_piece(	
				piece_buffer,
				field_size );

			replace_piece(	input_buffer, 
					delimiter, 
					piece_buffer, 
					piece_offset );
		}
		printf( "%s\n", input_buffer );
	} /* while( get_line() ) */
	return 0;
} /* main() */

void prepend_zeros_to_piece(	char *source_destination,
				int field_size )
{
	int str_len = strlen( source_destination );
	int i;

	for( i = str_len; i < field_size; i++ )
	{
		insert_str( "0", source_destination, 0 );
	}

} /* prepend_zeros_to_piece() */

