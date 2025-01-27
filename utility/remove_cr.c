/* utility/remove_cr.c 					*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define EOLN	10
#define CR	13

int main( void )
{
	int in_char;
	int inside_cr = 0;
	int inside_space = 0;

	while ( 1 )
	{
		in_char = fgetc( stdin );

		if (in_char == EOF )
		{
			return 0;
		}
		else
		if ( in_char == CR )
		{
			/* Do nothing */
		}
		else
		if ( in_char == EOLN )
		{
			if ( inside_cr )
			{
				putc( EOLN, stdout );
				putc( EOLN, stdout );
				inside_cr = 0;
				continue;
			}
			else
			{
				in_char = ' ';
				inside_cr = 1;
			}
		}

		if ( in_char == ' ' )
		{
			if ( !inside_space ) putc( ' ', stdout );
			inside_space = 1;
		}
		else
		{
			putc( in_char, stdout );
			inside_cr = 0;
			inside_space = 0;
		}
	}
	fprintf( stderr, "Error: should not reach here.\n" );
	return 0;
} /* main() */

