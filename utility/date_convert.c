/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/date_convert.c	       			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "date_convert.h"

LIST *date_convert_position_integer_list(
		char *position_comma_list_string );

int main( int argc, char **argv )
{
	char delimiter;
	LIST *position_integer_list;
	char input_buffer[ 65536 ];
	char piece_buffer[ 128 ];
	int *position;
	DATE_CONVERT *date_convert;
	enum date_convert_format_enum destination_enum;

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s delimiter position_comma_list destination_format\n",
			argv[ 0 ] );
		fprintf(stderr,
"Note: destination_format = {military|international|american}\n" );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	position_integer_list = date_convert_position_integer_list( argv[ 2 ] );
	destination_enum = date_convert_format_string_evaluate( argv[ 3 ] );

	while( string_input( input_buffer, stdin, 65536 ) )
	{
		if ( !list_rewind( position_integer_list ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		do {
			position = list_get( position_integer_list );

			if ( !piece(
				piece_buffer,
				delimiter,
				input_buffer,
				*position ) )
			{
				continue;
			}

			date_convert =
				date_convert_new( 
					date_convert_unknown
						/* source_enum */,
					destination_enum,
					piece_buffer );

			if ( date_convert )
			{
				piece_replace(
					input_buffer,
					delimiter,
					date_convert->return_date_string,
					*position );

				date_convert_free( date_convert );
			}

		} while( list_next( position_integer_list ) );

		printf( "%s\n", input_buffer );
	}

	return 0;
}

LIST *date_convert_position_integer_list( char *position_comma_list_string )
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
		list_set( position_integer_list, position );
	}

	return position_integer_list;
}

