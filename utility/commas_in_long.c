/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/commas_in_long.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "column.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char number_string[ 1024 ];
	char *source;

	if ( argc == 1 )
	{
		while( string_input( buffer, stdin, sizeof ( buffer ) ) )
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
				string_commas_number_string(
					source ) );
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
				string_commas_number_string(
					source ) );
		}
	}

	return 0;
}

