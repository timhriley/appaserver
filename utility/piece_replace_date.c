/* piece_replace_date.c */
/* -------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"

void piece_replace_date( char delimiter, LIST *position_integer_list );
LIST *get_position_integer_list( char *position_comma_list_string );

int main( int argc, char **argv )
{
	char delimiter;
	LIST *position_integer_list;

	if ( argc != 3 )
	{
		fprintf( stderr, 
		"Usage: %s delimiter position_comma_list\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	position_integer_list = get_position_integer_list( argv[ 2 ] );

	piece_replace_date( delimiter, position_integer_list );
	return 0;

} /* main() */


void piece_replace_date( char delimiter, LIST *position_integer_list )
{
	char input_buffer[ 65536 ];
	char piece_buffer[ 128 ];
	char *ora_date;
	int *position;

	while( get_line( input_buffer, stdin ) )
	{
		if ( !list_rewind( position_integer_list ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		do {
			position = list_get_pointer( position_integer_list );

			if ( ! piece(	piece_buffer,
					delimiter,
					input_buffer,
					*position ) )
			{
				continue;
			}

			ora_date =
				timlib_yyyymmdd_to_ora_date(
					piece_buffer );

			piece_replace(	input_buffer,
					delimiter,
					ora_date, 
					*position );

		} while( list_next( position_integer_list ) );
		printf( "%s\n", input_buffer );
	}

} /* piece_replace_date() */

LIST *get_position_integer_list( char *position_comma_list_string )
{
	int *position = {0};
	char piece_buffer[ 16 ];
	int i;
	LIST *position_integer_list = list_new_list();

	for(	i = 0;
		piece( piece_buffer, ',', position_comma_list_string, i );
		i++ )
	{
		position = (int *)calloc( 1, sizeof( int ) );
		*position = atoi( piece_buffer );
		list_append_pointer( position_integer_list, position );
	}

	return position_integer_list;
} /* get_position_integer_list() */

