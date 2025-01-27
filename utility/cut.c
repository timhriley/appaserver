/* utility/cut.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"

typedef struct
{
	int offset_from;
	int offset_to;
} CHUNK;

CHUNK *new_chunk( void );
LIST *get_chunk_list( char *argv_1 );

int main( int argc, char **argv )
{
	char input_buffer[ 4096 ];
	char chunk_buffer[ 4096 ];
	CHUNK *chunk;
	LIST *chunk_list;
	char delimiter;
	int length;

	if ( argc < 2 )
	{
		fprintf( stderr, 
"Usage: %s offset_from_1-offset_to_1[,offset_from_n-offset_to_n] [output_delimiter]\n",
			argv[ 0 ] );
		fprintf( stderr,
			 "Note: offsets are zero based\n" );
		exit( 1 );
	}

	chunk_list = get_chunk_list( argv[ 1 ] );

	if ( !chunk_list || !list_length( chunk_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: invalid offset list\n", argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 3 )
		delimiter = *argv[ 2 ];
	else
		delimiter = '\0';

	while( get_line( input_buffer, stdin ) )
	{
		if ( !list_rewind( chunk_list ) ) break;

		do {
			chunk = list_get_pointer( chunk_list );

			length = chunk->offset_to - chunk->offset_from + 1;

			strncpy(chunk_buffer,
				input_buffer + chunk->offset_from,
				length );

			*(chunk_buffer + length) = '\0';

			if ( !list_at_start( chunk_list ) )
				printf( "%c", delimiter );

			printf( "%s", trim( chunk_buffer ) );
		} while( list_next( chunk_list ) );
		printf( "\n" );
	}

	return 0;
} /* main() */

LIST *get_chunk_list( char *argv_1 )
{
	LIST *chunk_list;
	CHUNK *chunk;
	char both[ 128 ];
	char single[ 128 ];
	int i;

	chunk_list = list_new_list();

	for( i = 0; piece( both, ',', argv_1, i ); i++ )
	{
		chunk = new_chunk();
		piece( single, '-', both, 0 );
		chunk->offset_from = atoi( single );

		if ( !piece( single, '-', both, 1 ) )
			return (LIST *)0;

		chunk->offset_to = atoi( single );

		list_append_pointer( chunk_list, chunk );
	}
	return chunk_list;
}

CHUNK *new_chunk( void )
{
	return calloc( 1, sizeof( CHUNK ) );
}
