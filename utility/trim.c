/* utility/trim.c					*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char *ptr;
	int how_many = 0;

	if ( argc == 2 )
	{
		how_many = atoi( argv[ 1 ] );
	}

	while( get_line( buffer, stdin ) )
	{
		ptr = trim( buffer );

		if ( how_many )
		{
			*( ptr + how_many ) = '\0';
		}

		printf( "%s\n", buffer );
	}

	return 0;

} /* main() */

