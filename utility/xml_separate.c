/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/xml_separate.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main( void )
{
	int ch;

	while ( ( ch = fgetc( stdin ) ) != EOF )
	{
		if ( ch == '<' )
		{
			printf( "\n<" );
			continue;
		}

		if ( ch == '>' )
		{
			printf( ">\n" );
			continue;
		}

		putc( ch, stdout );
	}

	return 0;
}

