/* escape_character.c					 */
/* ----------------------------------------------------- */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */
 
#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
 
int main( int argc, char **argv )
{
	char input_buffer[ 65536 ];
	char *ptr;
	char character;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s character\n", argv[ 0 ] );
		exit( 1 );
	}

	character = *argv[ 1 ];

	while( get_line( input_buffer, stdin ) )
	{
		for( ptr = input_buffer; *ptr; ptr++ )
		{
			if ( *ptr == character ) printf( "\\" );
			printf( "%c", *ptr );
		}
		printf( "\n" );
	}
}
 
