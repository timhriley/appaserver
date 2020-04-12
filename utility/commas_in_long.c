/* commas_in_long.c				*/
/* -------------------------------------------- */
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "column.h"

char *commas_in_long( long n );

char *commas_in_long( long n )
{
	return place_commas_in_long( n );

} /* commas_in_long() */

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char number_string[ 1024 ];
	char *source;

	if ( argc == 1 )
	{
		while( get_line( buffer, stdin ) )
		{
			if ( column_last( number_string, buffer ) )
			{
				source = number_string;
			}
			else
			{
				source = buffer;
			}

			printf( "%s\n",
				commas_in_long(
					atol( source ) ) );
		}
	}
	else
	{
		while( --argc )
		{
			source = *++argv;

			if ( column_last( number_string, source ) )
			{
				source = number_string;
			}

			printf( "%s\n",
				commas_in_long(
					atol( source ) ) );
		}
	}

	return 0;

} /* main() */

