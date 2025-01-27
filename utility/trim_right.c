/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/trim_right.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "String.h"

int main( int argc, char **argv )
{
	int length;
	char buffer[ 65536 ];

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s length\n",
			argv[ 0 ] );
		exit( 1 );
	}

	length = atoi( argv[ 1 ] );

	while( string_input( buffer, stdin, 65536 ) )
	{
		string_trim_right( buffer, length );

		printf( "%s\n", buffer );
	}

	return 0;
}

