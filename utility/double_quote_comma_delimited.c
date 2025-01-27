/* $APPASERVER_HOME/utility/double_quote_comma_delimited.c */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"

int main( int argc, char **argv )
{
	char source[ 65536 ];
	char delimiter;

	if ( argc < 2 )
	{
		fprintf( stderr, "Usage: %s delimiter [string]\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( argc == 3 )
	{
		printf( "%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_double_quote_comma(
				argv[ 2 ] /* source */,
				delimiter ) );
	}
	else
	{
		while( string_input( source, stdin, 65536 ) )
		{
			printf( "%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_double_quote_comma(
					source,
					delimiter ) );
		}
	}

	return 0;
}
