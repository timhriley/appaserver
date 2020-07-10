/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/utility/html_table.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* This is a utility to wrap an html table around delimited	*/
/* data.							*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "html_table.h"
#include "list.h"

#define ROWS_BETWEEN_HEADING	10

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char *title = {0};
	char *sub_title = {0};
	char *sub_sub_title = {0};
	LIST *heading_name_list;
	char delimiter;
	HTML_TABLE *html_table;
	char heading_delimiter;
	int count = -1;

	/* output_starting_argv_stderr( argc, argv ); */

	if ( argc < 4 )
	{
		fprintf(stderr,
"Usage: %s title[^sub_title^sub_sub_title] heading_list delimiter [justify_comma_list]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	title = argv[ 1 ];

	if ( ! ( heading_delimiter = timlib_delimiter( argv[ 2 ] ) ) )
	{
		heading_delimiter = ',';
	}

	heading_name_list = list_string2list( argv[ 2 ], heading_delimiter );

	delimiter = *argv[ 3 ];

/*
fprintf( stderr, "%s/%s()/%d: delimiter = [%c]\n",
__FILE__,
__FUNCTION__,
__LINE__,
delimiter );
fflush( stderr );
*/

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
		html_table->justify_list =
			list_string2list( argv[ 4 ], ',' );
	}

	html_table_output_table_heading2(	html_table->title,
						html_table->sub_title,
						html_table->sub_sub_title );

	if ( list_length( heading_name_list ) )
	{
		html_table_set_heading_list( html_table, heading_name_list );

		html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );
	}

	while( get_line( buffer, stdin ) )
	{
		if ( list_length( heading_name_list )
		&&   ++count == ROWS_BETWEEN_HEADING )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
				number_left_justified_columns,
				html_table->
				number_right_justified_columns,
				html_table->justify_list );
			count = 0;
		}

		html_table_set_record_data(	html_table,
						buffer,
						delimiter );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();
	}
	html_table_close();

	return 0;
} /* main() */

