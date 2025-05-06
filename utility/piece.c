/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/piece.c		 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char offset_str[ 128 ];
	char component[ 65536 ];
	char delimiter;
	int offset, i;
	int line_number = 0;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			"Usage: %s delimiter offset_1[,...,offset_n]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	while( string_input( buffer, stdin, 65536 ) )
	{
		line_number++;

		for(	i = 0;
			piece( offset_str, ',', argv[ 2 ], i );
			i++ )
		{
			offset = atoi( offset_str );

			if ( offset < 0 )
			{
				fprintf( stderr,
			 "ERROR in :%s: Offset (%d) is less than zero\n",
			 		argv[ 0 ],
			 		offset );
				exit( 1 );
			}
	
			if ( !piece(	component,
					delimiter,
					buffer,
					offset ) )
			{
				fprintf( stderr,
"Warning %s for line=%d: There are less than %d delimiters (%c) in: (%s)\n",
					argv[ 0 ],
					line_number,
				 	offset,
					delimiter,
				 	buffer );
				continue;
			}
			else
			{
				if ( i == 0 )
					printf( "%s", component );
				else
					printf( "%c%s", delimiter, component );
			}
		}
		printf( "\n" );
		fflush( stdout );
	}
	return 0;
}

