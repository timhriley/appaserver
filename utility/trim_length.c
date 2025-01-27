/* utility/trim_length.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	int length;
	char buffer[ 65536 ];
	char component[ 65536 ];
	char delimiter = ':';
	int offset = 0;

	if ( argc == 1 )
	{
		fprintf(	stderr,
				"Usage: %s length [delimiter offset]\n",
				argv[ 0 ] );
		exit( 1 );
	}

	length = atoi( argv[ 1 ] );

	if ( argc == 4 )
	{
		delimiter = *argv[ 2 ];
		offset = atoi( argv[ 3 ] );
	}

	while( get_line( buffer, stdin ) )
	{
		if ( !piece(	component,
				delimiter,
				buffer,
				offset ) )
		{
			fprintf( stderr,
		"Warning %s: There are less than %d delimiters (%c) in: (%s)\n",
				argv[ 0 ],
			 	offset,
				delimiter,
			 	buffer );
		}

		trim_length( component, length );

		printf( "%s\n", piece_replace(	buffer, 
						delimiter,
						component,
						offset ) );
	}

	return 0;

} /* main() */

