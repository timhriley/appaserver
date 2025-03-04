/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/escape_character.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
 
int main( int argc, char **argv )
{
	char input_buffer[ STRING_64K ];
	char *ptr;
	char character;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s character\n", argv[ 0 ] );
		exit( 1 );
	}

	character = *argv[ 1 ];

	while( string_input( input_buffer, stdin, sizeof ( input_buffer ) ) )
	{
		for( ptr = input_buffer; *ptr; ptr++ )
		{
			if ( *ptr == character ) printf( "\\" );
			printf( "%c", *ptr );
		}
		printf( "\n" );
	}

	return 0;
}
 
