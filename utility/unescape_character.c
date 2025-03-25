/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/unescape_character.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/
 
#include <stdio.h>
#include "String.h"
 
int main( int argc, char **argv )
{
	char datum[ 65536 ];
	char destination[ 65536 ];
	char character_to_unescape;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s character_to_unescape\n",
			argv[ 0 ] );

		exit( 1 );
	}

	character_to_unescape = *argv[ 1 ];

	while( string_input( datum, stdin, sizeof ( datum ) ) )
	{
		printf(	"%s\n",
			/* ------------------- */
			/* Returns destination */
			/* ------------------- */
			string_unescape_character(
				destination,
				datum,
				character_to_unescape ) );
	}

	return 0;
}
 
