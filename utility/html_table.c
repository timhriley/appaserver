/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/html_table.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "String.h"
#include "list.h"
#include "html.h"

#define ROWS_BETWEEN_HEADING	10

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char *title = {0};
	char *sub_title = {0};
	char *sub_sub_title = {0};
	LIST *heading_list;
	char delimiter;
	HTML_TABLE *html_table;
	LIST *cell_list;
	HTML_ROW *html_row;
	char heading_delimiter;
	LIST *justify_list = {0};
	int p;
	char datum[ 4096 ];
	int row_number = 0;

	if ( argc < 3 )
	{
		fprintf(stderr,
"Usage: %s title[^sub_title^sub_sub_title] heading_list [delimiter] [justify_comma_list]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	title = argv[ 1 ];

	if ( ! ( heading_delimiter = timlib_delimiter( argv[ 2 ] ) ) )
	{
		heading_delimiter = ',';
	}

	heading_list = list_string2list( argv[ 2 ], heading_delimiter );

	if ( argc < 4 )
		delimiter = '^';
	else
		delimiter = *argv[ 3 ];

	if ( character_exists( title, '^' ) )
	{
		int position;

		position = character_position( title, '^' );
		sub_title = title + position + 1;
		*(title + position) = '\0';

		if ( character_exists( sub_title, '^' ) )
		{
			position = character_position( sub_title, '^' );
			sub_sub_title = sub_title + position + 1;
			*(sub_title + position) = '\0';
		}
	}

	html_table = html_table_new( title, sub_title, sub_sub_title );

	if ( argc == 5 )
	{
		justify_list =
			list_string2list( argv[ 4 ], ',' );
	}

	html_table->column_list =
		html_column_list(
			heading_list,
			justify_list );

	html_table->row_list = list_new();

	while( string_input( buffer, stdin, 65536 ) )
	{
		if ( !*buffer ) continue;

		cell_list = list_new();

		for (	p = 0;
			p < list_length( html_table->column_list );
			p++ )
		{
			piece(	datum,
				delimiter,
				buffer,
				p );

			list_set(
				cell_list,
				html_cell_new(
					strdup( datum ),
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );
		}

		html_row =
			html_row_new(
				cell_list );

		html_row->background_shaded_boolean =
			row_number++ % 2;

		list_set(
			html_table->row_list,
			html_row );
	}

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );

	return 0;
}

