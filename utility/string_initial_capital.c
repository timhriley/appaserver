/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/string_initial_capital.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "String.h"

int main( int argc, char **argv )
{
	char input_buffer[ 65536 ];
	char piece_buffer[ 1024 ];
	char delimiter;
	int offset;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s delimiter offset\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	offset = atoi( argv[ 2 ] );

	while( string_input( input_buffer, stdin, 65536 ) )
	{
		/* ------------------------------------- */
		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		if ( piece( piece_buffer, delimiter, input_buffer, offset ) )
		{
			string_initial_capital( piece_buffer, piece_buffer );

			piece_replace(
				input_buffer,
				delimiter, 
				piece_buffer /* new_data */, 
				offset );
		}

		printf( "%s\n", input_buffer );
	}

	return 0;
}

