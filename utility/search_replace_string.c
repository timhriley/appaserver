/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/search_replace_string.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char *search_string;
	char *replace_string;

	if ( argc != 3 )
	{
		fprintf(stderr,
		"Usage: %s: search replace\n",
			argv[ 0 ] );
		exit( 1 );
	}

	search_string = argv[ 1 ];
	replace_string = argv[ 2 ];

	while( string_input( buffer, stdin, 2048 ) )
	{
		printf( "%s\n",
			search_replace_string(
				buffer,
				search_string,
				replace_string ) );
	}
	return 0;
}
