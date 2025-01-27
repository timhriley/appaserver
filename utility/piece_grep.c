/* $APPASERVER_HOME/utility/piece_grep.c */
/* ------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

int main( int argc, char **argv )
{
	int piece_offset;
	int delimiter;
	char *pattern_to_search;
	char input_buffer[ 65536 ];
	boolean grep_exclude = 0;

	if ( argc < 4 )
	{
		fprintf(
		stderr,
		"Usage: %s delimiter offset search_1[,search_n] [-v]\n",
		argv[ 0 ] );

		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	piece_offset = atoi( argv[ 2 ] );
	pattern_to_search = argv[ 3 ];

	if ( argc == 5 && strcmp( argv[ 4 ], "-v" ) == 0 )
	{
		grep_exclude = 1;
	}

	while( string_input( input_buffer, stdin, sizeof ( input_buffer ) ) )
	{
		if ( piece_grep(
			input_buffer,
			delimiter, 
			pattern_to_search,
			piece_offset,
			grep_exclude ) )
		{
			printf( "%s\n", input_buffer );
		}
	}
	return 0;
}

