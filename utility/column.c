/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/column.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "column.h"

int main( int argc, char **argv )
{
	char buffer[ 8192 ];
	char component[ 8192 ];
	char offset_piece[ 20 ];
	int i, offset;
	char *offset_string;
	char destination_delimiter;

	if ( argc < 2 )
	{
		fprintf(stderr,
		"Usage: %s offset_1[,offset_n...] [destination_delimiter]\n",
			argv[ 0 ]);
		exit( 1 );
	}

	offset_string = argv[ 1 ];

	if ( argc == 3 )
		destination_delimiter = *argv[ 2 ];
	else
		destination_delimiter = ' ';

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		/* For each column requested */
		/* ------------------------- */
		for(	i = 0;
			piece( offset_piece, ',', offset_string, i );
			i++ )
		{
			offset = atoi( offset_piece );

			/* Get the column */
			/* -------------- */
			if ( !column(	component, 
					offset, 
					buffer ) )
			{
				continue;
			}

			if ( i ) printf( "%c", destination_delimiter );

			printf( "%s", component );
		}
		printf( "\n" );
		fflush( stdout );
	}
	return 0;
}
