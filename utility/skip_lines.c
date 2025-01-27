/* skip_lines.c 				*/
/* -------------------------------------------- */
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	int lines2skip;
	int line = 0;

	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: %s lines2skip\n", argv[ 0 ] );
		exit( 1 );
	}

	lines2skip = atoi( argv[ 1 ] );
	
	while( get_line( buffer, stdin ) )
	{
		if ( line++ >= lines2skip )
			printf( "%s\n", buffer );
	}

	return 0;
} /* main() */

