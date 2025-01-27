/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/piece_sum.c					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "boolean.h"

#define MAX_COLUMNS	99

void piece_sum(		char delimiter,
			char *piece_offset_list_string,
			boolean sum_only );

int main( int argc, char **argv )
{
	char *piece_offset_list_string;
	char delimiter;
	boolean sum_only = 0;

	if ( argc < 3 )
	{
		fprintf( stderr,
	"Usage: %s delimiter piece_offset_1[,piece_offset_n] [sum_only_yn]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	piece_offset_list_string = argv[ 2 ];

	if ( argc == 4 ) sum_only = ( *argv[ 3 ] == 'y' );

	piece_sum( delimiter, piece_offset_list_string, sum_only );

	return 0;
}

void piece_sum(	char delimiter,
		char *piece_offset_list_string,
		boolean sum_only )
{
	char buffer[ 4096 ];
	char piece_buffer[ 4096 ];
	double sum_array[ MAX_COLUMNS ] = {0};
	int piece_offset_int;
	LIST *piece_offset_list;
	char *piece_offset;
	int prior_piece_offset;
	int i;

	piece_offset_list =
		list_string_to_list(
			piece_offset_list_string, ',' );

	if ( !list_length( piece_offset_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty piece_offset_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( list_length( piece_offset_list ) > MAX_COLUMNS )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: max columns is %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 MAX_COLUMNS );
	}

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		list_rewind( piece_offset_list );

		do {
			piece_offset = list_get_pointer( piece_offset_list );
			piece_offset_int = atoi( piece_offset );

			if ( ! piece(	piece_buffer,
					delimiter,
					buffer,
					piece_offset_int ) )
			{
				fprintf( stderr, 
				 "%s/%s()/%d: cannot get piece(%d) from (%s)\n",
				 	__FILE__,
				 	__FUNCTION__,
					__LINE__,
				 	piece_offset_int,
					buffer );
				continue;
			}

			sum_array[ piece_offset_int ] += atof( piece_buffer );

		} while( list_next( piece_offset_list ) );

		if ( !sum_only ) printf( "%s\n", buffer );
	}

	printf( "Sum%c",
		delimiter );

	list_rewind( piece_offset_list );
	prior_piece_offset = 1;

	do {
		piece_offset = list_get_pointer( piece_offset_list );
		piece_offset_int = atoi( piece_offset );

		for( i = prior_piece_offset; i < piece_offset_int; i++ )
			printf( "%c", delimiter );
		
		printf( "%.2lf",
			sum_array[ piece_offset_int ] );

		prior_piece_offset = piece_offset_int;

	} while( list_next( piece_offset_list ) );

	printf( "\n" );
}
