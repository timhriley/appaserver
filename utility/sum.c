/* utility/sum.c			   */
/* --------------------------------------- */
/* Usage: sum.e [piece_offset] [delimiter] */
/* --------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

#define DEFAULT_DELIMITER	'^'

int main( int argc, char **argv )
{
	double sum = 0.0;
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char delimiter = DEFAULT_DELIMITER;
	int piece_offset = 0;

	if ( argc > 1 )
	{
		piece_offset = atoi( argv[ 1 ] );

		if ( argc == 2 )
		{
			delimiter = DEFAULT_DELIMITER;
		}
		else
		{
			delimiter = *argv[ 2 ];
		}
	}

	while( get_line( input_buffer, stdin ) )
	{
		*piece_buffer = '\0';

		if ( ! piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot piece(%d): (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				piece_offset,
				piece_buffer );
			continue;
		}

		sum += atof( piece_buffer );
	}
	printf( "%.3lf\n", sum );
	return 0;
} /* main() */

