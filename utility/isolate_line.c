/* ~riley/utility/isolate_line.c */
/* ----------------------------- */
/* 				 */
/* -- Tim Riley			 */
/* ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

#define BUFFER_SIZE 4096

int main( int argc, char **argv )
{
	char buffer[ BUFFER_SIZE ];
	int current_line;
	int display_line;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s line_number (one based)\n",
			argv[ 0 ] );
		exit( 1 );
	}

	display_line = atoi( argv[ 1 ] );
	current_line = 0;
	while ( get_line( buffer, stdin ) )
	{
		if ( ++current_line == display_line )
		{
			printf( "%s\n", buffer );
			return 0;
		}
	}

	exit( 1 );

} /* main() */

