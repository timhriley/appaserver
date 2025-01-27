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
		int piece_offset,
		int decimal_places );

int main( int argc, char **argv )
{
	int piece_offset;
	int decimal_places;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s piece_offset decimal_places\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	decimal_places = atof( argv[ 2 ] );

	if ( !decimal_places ) decimal_places = 2;

	piece_commas_double( piece_offset, decimal_places );

	return 0;
}

void piece_commas_double(
		int piece_offset,
		int decimal_places )
{
	char buffer[ 1024 ];
	char piece_buffer[ 128 ];

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		if ( ! piece( piece_buffer, '^', buffer, piece_offset ) )
		{
			fprintf( stderr, 
				 "%s: cannot get piece: %d in (%s)\n",
				 __FILE__,
				 piece_offset,
				 buffer );

			printf( "%s\n", buffer );
			continue;
		}

		printf( "%s\n",
			piece_replace(
				buffer,
				'^',
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_double(
					atof( piece_buffer ),
					decimal_places /* decima_count */ ),
				piece_offset ) );
	}
}


