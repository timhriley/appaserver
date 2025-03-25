/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/unescape_single_quotes.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/
 
#include <stdio.h>
#include "String.h"
 
int main( void )
{
	char input_buffer[ 65536 ];
	char *ptr;

	while( string_input( input_buffer, stdin, sizeof ( input_buffer ) ) )
	{
		for( ptr = input_buffer; *ptr; ptr++ )
		{
			if ( *ptr == '\\' )
			{
				if ( *(ptr + 1) == '\'' ) continue;
			}
			printf( "%c", *ptr );
		}
		printf( "\n" );
	}

	return 0;
}
 
