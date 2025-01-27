/* html_encode_quote_and_ampersand.c			 */
/* ----------------------------------------------------- */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */
 
#include <stdio.h>
#include "timlib.h"
 
int main( int argc, char **argv )
{
	char input_buffer[ 65536 ];
	char *ptr;


	while( get_line( input_buffer, stdin ) )
	{
		for( ptr = input_buffer; *ptr; ptr++ )
		{
			if ( *ptr == '\'' )
			{
				printf( "&#039" );
			}
			else
			if ( *ptr == '&' )
			{
				printf( "&#038" );
			}
			else
				printf( "%c", *ptr );
		}
		printf( "\n" );
	}
}
 
