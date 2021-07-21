/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/date_convert.c	       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "date_convert.h"

LIST *get_position_integer_list( char *position_comma_list_string );

int main( int argc, char **argv )
{
	char delimiter;
	LIST *position_integer_list;
	char input_buffer[ 65536 ];
	char piece_buffer[ 128 ];
	int *position;
	DATE_CONVERT *date_convert = {0};
	enum date_convert_format date_convert_format;

	if ( argc != 4 )
	{
		fprintf( stderr,
"Usage: %s delimiter position_comma_list military|international|american\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	position_integer_list = get_position_integer_list( argv[ 2 ] );
	date_convert_format = date_convert_date_time_evaluate( argv[ 3 ] );

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

			if ( !date_convert )
			{
				date_convert =
					date_convert_new_date_convert( 
							date_convert_format,
							piece_buffer );
			}
			else
			{
				date_convert_populate_return_date(
					date_convert->return_date,
					date_convert->source_format,
					date_convert->destination_format,
					piece_buffer );
			}

			piece_replace(	input_buffer,
					delimiter,
					date_convert->return_date, 
					*position );

		} while( list_next( position_integer_list ) );

		printf( "%s\n", input_buffer );
	}

	if ( date_convert ) date_convert_free( date_convert );

	return 0;

} /* main() */

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

