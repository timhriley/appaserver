/* unescape_single_quotes.c				 */
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
			if ( *ptr == '\\' )
			{
				/* If not at the end */
				/* ----------------- */
				if ( *(ptr + 1) )
				{
					if ( *(ptr + 1) == '\'' ) continue;
				}
			}
			printf( "%c", *ptr );
		}
		printf( "\n" );
	}
}
 
