/* utility/piece_quoted.c 				*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char offset_str[ 128 ];
	char component[ 4096 ];
	char delimiter;
	char quote_delimiter;
	char *offset_list_string;
	int offset, i;

	if ( argc != 4 )
	{
		fprintf( stderr, 
	"Usage: %s delimiter quote_delimiter offset_1[,...,offset_n]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	quote_delimiter = *argv[ 2 ];
	offset_list_string = argv[ 3 ];

	while( get_line( buffer, stdin ) )
	{
		for( i = 0; piece(	offset_str,
					',',
					offset_list_string,
					i ); i++ )
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
	
			if ( !piece_quoted(
						component,
						delimiter,
						buffer,
						offset,
						quote_delimiter ) )
			{
				fprintf( stderr,
		"Warning %s: There are less than %d delimiters (%c) in: (%s)\n",
					argv[ 0 ],
				 	offset,
					delimiter,
				 	buffer );
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
	}

	return 0;
} /* main() */

