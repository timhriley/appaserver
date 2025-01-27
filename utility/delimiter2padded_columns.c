/* utility/delimiter2padded_columns.c */
/* ---------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"
#include "list.h"

typedef struct
{
	int max_length;
	LIST *text_string_list;
} COLUMN;

COLUMN *new_column( void );
void append_column_text(	COLUMN *column, char *text );
void output_column_list(	LIST *column_list,
				int right_justified_columns_from_right,
				boolean keep_delimiter,
				char delimiter );

int main( int argc, char **argv )
{
	char buffer[ 12288 ];
	char piece_buffer[ 4096 ];
	char delimiter;
	int i;
	int first_time = 1;
	LIST *column_list = list_new_list();
	COLUMN *column;
	int right_justified_columns_from_right = 0;
	boolean keep_delimiter = 0;

	if ( argc < 2 )
	{
		fprintf( stderr,
"Usage: %s delimiter [right_justified_columns_from_right] [keep_delimiter_yn]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( argc > 2 )
	{
		right_justified_columns_from_right =
			atoi( argv[ 2 ] );

		if ( argc == 4 )
			keep_delimiter = (*argv[ 3 ] == 'y' );
	}

	/* If tab */
	/* ------ */
	if ( delimiter == 't' ) delimiter = 9;

	while( get_line( buffer, stdin ) )
	{
		if ( !character_exists( buffer, delimiter ) )
		{
			printf( "%s\n", buffer );
			continue;
		}

		for( i = 0; piece( piece_buffer, delimiter, buffer, i ); i++ )
		{
			if ( first_time )
			{
				column = new_column();
				list_append_pointer(
					column_list,
					column );
			}
			else
			{
				column = list_get_pointer( column_list );
				list_next( column_list );
			}
			append_column_text( column, strdup( piece_buffer ) );
		}
		if ( first_time ) first_time = 0;
		list_rewind( column_list );
	}

	output_column_list(	column_list,
				right_justified_columns_from_right,
				keep_delimiter,
				delimiter );

	return 0;
} /* main() */

void output_column_list(	LIST *column_list,
				int right_justified_columns_from_right,
				boolean keep_delimiter,
				char delimiter )
{
	COLUMN *column;
	char buffer[ 1024 ];
	int all_done = 0;
	int column_number = 0;
	int start_right_justify_column_number = 0;
	int pad_width;

	if ( !list_rewind( column_list ) ) return;

	do {
		column = list_get_pointer( column_list );
		if ( !list_rewind( column->text_string_list ) ) return;
	} while( list_next( column_list ) );

	if ( right_justified_columns_from_right )
	{
		start_right_justify_column_number =
			list_length( column_list ) -
			right_justified_columns_from_right + 1;
	}

	while( !all_done )
	{
		list_rewind( column_list );

		if ( start_right_justify_column_number )
			column_number = 1;

		do {
			column = list_get_pointer( column_list );

			if ( keep_delimiter && !list_at_first( column_list ) )
			{
				printf( "%c", delimiter );
			}

			if ( start_right_justify_column_number
			&&  	column_number >=
				start_right_justify_column_number )
			{
				if ( list_at_first( column_list ) )
				{
					pad_width =
						column->max_length;
				}
				else
				{
					pad_width =
						column->max_length + 1;
				}

				printf( "%s",
					timlib_pad_spaces_to_justify_right(
					buffer,
					(char *)list_get_pointer(
						column->text_string_list ),
					pad_width ) );
			}
			else
			{
				printf( "%s",
					timlib_pad_spaces_to_justify_left(
					buffer,
					(char *)list_get_pointer(
						column->text_string_list ),
					column->max_length + 1 ) );
			}

			if ( !list_next( column->text_string_list ) )
				all_done = 1;

			if ( start_right_justify_column_number )
				column_number++;

		} while( list_next( column_list ) );
		printf( "\n" );
	}
} /* output_column_list() */

void append_column_text( COLUMN *column, char *text )
{
	int str_len = strlen( text );

	list_append_pointer( column->text_string_list, text );
	if ( str_len > column->max_length )
		column->max_length = str_len;
}

COLUMN *new_column( void )
{
	COLUMN *column = (COLUMN *)calloc( 1, sizeof( COLUMN ) );
	column->text_string_list = list_new_list();
	return column;
}

