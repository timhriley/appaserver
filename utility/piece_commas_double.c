/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/piece_commas_double.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"

void piece_commas_double(
		char *argv_0,
		char *argv_1,
		int decimal_places,
		char delimiter );

int main( int argc, char **argv )
{
	char *argv_0;
	char *argv_1;
	int decimal_places;
	char delimiter = '^';

	if ( argc < 3 )
	{
		fprintf(stderr,
		"Usage: %s offset[,offset] decimal_places [delimiter]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	argv_0 = argv[ 0 ];
	argv_1 = argv[ 1 ];
	decimal_places = atoi( argv[ 2 ] );

	if ( argc == 4 ) delimiter = *argv[ 3 ];

	piece_commas_double(
		argv_0,
		argv_1,
		decimal_places,
		delimiter );

	return 0;
}

void piece_commas_double(
		char *argv_0,
		char *argv_1,
		int decimal_places,
		char delimiter )
{
	char buffer[ 2048 ];
	int line_number = 0;
	char piece_buffer[ 128 ];
	int i;
	char offset_string[ 128 ];
	int offset;

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		line_number++;

		for(	i = 0;
			piece( offset_string, ',', argv_1, i );
			i++ )
		{
			offset = atoi( offset_string );

			if ( offset < 0 )
			{
				fprintf( stderr,
			 "ERROR in :%s: Offset (%d) is less than zero\n",
			 		argv_0,
			 		offset );
				exit( 1 );
			}
	
			if ( !piece(	piece_buffer,
					delimiter,
					buffer,
					offset ) )
			{
				fprintf( stderr,
"Warning %s for line=%d: There are less than %d delimiters (%c) in: (%s)\n",
					argv_0,
					line_number,
				 	offset,
					delimiter,
				 	buffer );
				continue;
			}

			piece_replace(
				buffer,
				delimiter,
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_double(
					string_atof( piece_buffer ),
					decimal_places /* decimal_count */ ),
				offset );
		}
		printf( "%s\n", buffer );
	}
}


