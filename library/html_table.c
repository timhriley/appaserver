/* $APPASERVER_HOME/library/html_table.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "html_table.h"

HTML_TABLE *html_table_new(
			char *title,
			char *sub_title,
			char *sub_sub_title )
{
	HTML_TABLE *html_table = html_table_calloc();

	html_table->title = title;
	html_table->sub_title = sub_title;
	html_table->sub_sub_title = sub_sub_title;
	html_table->heading_list = list_new();
	html_table->data_list = list_new();
	return html_table;
}

HTML_TABLE *html_table_calloc( void )
{
	HTML_TABLE *html_table;

	if ( ! ( html_table = calloc( 1, sizeof( HTML_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return html_table;
}

void html_table_heading(
			char *title,
			char *sub_title,
			char *sub_sub_title )
{
	if ( title && *title )
		printf( "<h1>%s</h1>\n", title );

	if ( sub_title && *sub_title )
		printf( "<h2>%s</h2>\n", sub_title );

	if ( sub_sub_title && *sub_sub_title )
		printf( "<h3>%s</h3>\n", sub_sub_title );

	printf( "<table border>\n" );
}

void html_table_close( void )
{
	printf( "</table>\n" );
}

void html_table_output(	HTML_TABLE *html_table,
			int rows_between_heading )
{
	HTML_ROW *html_row;
	int row_count = 0;

	if ( !html_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: html_table is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_table_heading(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	html_heading_list_output( html_table->heading_list );

	if ( !list_rewind( html_table->row_list ) )
	{
		html_table_close();
		return;
	}

	do {
		html_row = list_get( html_table->row_list );

		if ( ++row_count == rows_between_heading )
		{
			html_heading_list_output( html_table->heading_list );
			row_count = 0;
		}

		html_row_output(
			html_table->heading_list,
			html_row );

	} while ( list_next( html_table->row_list ) );

	html_table_close();
}

HTML_HEADING *html_heading_new(
			char *heading,
			boolean right_justify_boolean )
{
	HTML_HEADING *html_heading = html_heading_calloc();

	html_heading->heading = heading;
	html_heading->right_justify_boolean = right_justify_boolean;

	return html_heading;
}

HTML_HEADING *html_heading_calloc( void )
{
	HTML_HEADING *html_heading;

	if ( ! ( html_heading = calloc( 1, sizeof( HTML_HEADING ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return html_heading;
}

void html_heading_list_output( LIST *heading_list )
{
	HTML_HEADING *html_heading;

	if ( !list_rewind( heading_list ) ) return;

	printf( "<tr>\n" );

	do {
		html_heading = list_get( heading_list );

		printf( "%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			html_heading_tag(
				html_heading->heading ) );

	} while ( list_next( heading_list ) );
}

char *html_heading_tag(	char *heading )
{
	static char tag[ 128 ];
	char buffer[ 128 ];

	if ( !heading ) heading = "";

	if ( character_exists( heading, '&' )
	&&   character_exists( heading, ';' ) )
	{
		strcpy( buffer, heading );
	}
	else
	{
		format_initial_capital(
			buffer,
			heading );
	}

	sprintf( tag, "<th>%s", buffer );

	return tag;
}

void html_heading_list_right_justify_set(
			LIST *heading_list,
			LIST *label_list )
{
	if ( !heading_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: heading_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( label_list ) ) return;

	do {
		list_set(
			heading_list,
			html_heading_new(
				(char *)list_get( label_list ),
				1 /* right_justify_boolean */ ) );

	} while ( list_next( label_list ) );
}

HTML_CELL *html_cell_new(
			char *data,
			boolean large_boolean,
			boolean bold_boolean )
{
	HTML_CELL *html_cell = html_cell_calloc();

	html_cell->data = data;
	html_cell->large_boolean = large_boolean;
	html_cell->bold_boolean = bold_boolean;

	return html_cell;
}

HTML_CELL *html_cell_calloc( void )
{
	HTML_CELL *html_cell;

	if ( ! ( html_cell = calloc( 1, sizeof( HTML_CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return html_cell;
}

void html_cell_data_set(
			LIST *cell_list,
			char *data,
			boolean large_boolean,
			boolean bold_boolean )
{
	HTML_CELL *html_cell =
		html_cell_new(
			data,
			large_boolean,
			bold_boolean );

	list_set( cell_list, html_cell );
}

void html_cell_data_list_set(
			LIST *cell_list,
			LIST *data_list )
{
	if ( !cell_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cell_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( !list_rewind( data_list ) ) return;

	do {
		html_cell_data_set(
			cell_list,
			(char *)list_get( data_list ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );

	} while ( list_next( data_list ) );
}

void html_cell_output(	HTML_HEADING *html_heading,
			HTML_CELL *html_cell )
{
	char *tag;

	if ( !html_heading
	||   !html_cell )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tag =
		/* Returns heap memory */
		/* ------------------- */
		html_cell_tag(
			html_cell->data,
			html_cell->large_boolean,
			html_cell->bold_boolean,
			html_heading->right_justify_boolean );

	printf( "%s\n", tag );
	free( tag );
}
			
char *html_cell_tag(	char *html_cell_data,
			boolean large_boolean,
			boolean bold_boolean,
			boolean right_justify_boolean )
{
	char tag[ 1024 ];
	char *ptr = tag;

	if ( !html_cell_data || !*html_cell_data )
	{
		return strdup( "<td>" );
	}

	ptr += sprintf( ptr, "<td" );

	if ( right_justify_boolean )
	{
		ptr += sprintf( ptr, " align=right" );
	}

	ptr += sprintf( ptr, ">" );

	if ( large_boolean )
	{
		ptr += sprintf( ptr, "<big>" );
	}

	if ( bold_boolean )
	{
		ptr += sprintf( ptr, "<b>" );
	}

	ptr += sprintf( ptr, "%s", html_cell_data );

	if ( bold_boolean )
	{
		ptr += sprintf( ptr, "</b>" );
	}

	if ( large_boolean )
	{
		ptr += sprintf( ptr, "</big>" );
	}

	return strdup( tag );
}

void html_row_output(	LIST *heading_list,
			HTML_ROW *html_row )
{
	HTML_CELL *html_cell;

	if ( !list_rewind( heading_list )
	||   !html_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	printf( "<tr>\n" );

	if ( !list_rewind( html_row->cell_list ) ) return;

	do {
		html_cell = list_get( html_row->cell_list );

		html_cell_output(
			(HTML_HEADING *)list_get( heading_list ),
			html_cell );

		list_next( heading_list );

	} while ( list_next( html_row->cell_list ) );
}

HTML_TABLE *new_html_table( char *title, char *sub_title )
{
	return
	html_table_new( title, sub_title, (char *)0 /* sub_sub_title */ );
}

void html_table_output_table_heading(
			char *title,
			char *sub_title )
{
	if ( title && *title )
		printf( "<h1>%s</h1>\n", title );

	if ( sub_title && *sub_title )
		printf( "<h2>%s</h2>\n", sub_title );

	printf( "<table border>\n" );
}

void html_table_output_table_heading2(	char *title,
					char *sub_title,
					char *sub_sub_title )
{
	html_table_heading(
			title,
			sub_title,
			sub_sub_title );
}

void html_table_output_data_heading(
			LIST *heading_list,
			int number_left_justified_columns,
			int number_right_justified_columns,
			LIST *justify_list )
{
	int column_number = 0;
	char buffer[ 1024 ];
	char *heading;

	if ( justify_list ) list_rewind( justify_list );

	printf( "<tr>\n" );

	if ( list_rewind( heading_list ) )
	{
		do {
			heading = list_get_string( heading_list );

			if ( character_exists( heading, '&' )
			&&   character_exists( heading, ';' ) )
			{
				strcpy( buffer, heading );
			}
			else
			{
				format_initial_capital(
					buffer,
					heading );
			}

			if ( justify_list )
			{
				printf( "<th align=%s>%s",
					list_get_string( justify_list ),
					buffer );
			}
			else
			{
				printf( "<th align=%s>%s\n", 
					html_table_get_alignment(
						column_number++,
						number_left_justified_columns,
						number_right_justified_columns),
					buffer );
			}

			printf( "</th>\n" );

			if ( justify_list ) list_next( justify_list );

		} while( list_next( heading_list ) );
	}
}

void html_table_output_data( 	LIST *data_list,
				int number_left_justified_columns,
				int number_right_justified_columns,
				int background_shaded,
				LIST *justify_list )
{
	char buffer[ 65536 ];
	int column_number = 0;

	if ( justify_list ) list_rewind( justify_list );
	printf( "<tr>\n" );

	if ( !list_rewind( data_list ) ) return;

	do {
		strcpy( buffer, list_get_string( data_list ) );
		printf( "<td" );

		if ( background_shaded )
			printf( " bgcolor=\"#CCCCCC\"" );

		if ( justify_list )
		{
			printf( " align=%s>\n",
				list_get_string( justify_list ) );
		}
		else
		{
			printf( " align=%s>",
				html_table_get_alignment(
				column_number++,
				number_left_justified_columns,
				number_right_justified_columns ) );
		}

		search_replace_string(
			buffer,
			"%0D%0A",
			"<br>" );

		printf( "%s</td>\n", buffer );

		if ( justify_list ) list_next( justify_list );

	} while( list_next( data_list ) );

}

void html_table_set_heading( HTML_TABLE *d, char *heading )
{
	list_append_string( d->heading_list, heading );
}

void html_table_set_record_data(	HTML_TABLE *d,
					char *record_data,
					char delimiter )
{
	char data[ 65536 ];
	int i;

	if ( !delimiter )
	{
		html_table_set_data( d->data_list, strdup( record_data ) );
	}
	else
	for( i = 0; piece( data, delimiter, record_data, i ); i++ )
	{
		html_table_set_data( d->data_list, strdup( data ) );
	}
}

void html_table_set_data( LIST *data_list, char *data )
{
	list_set( data_list, data );
}

void html_table_reset_data_heading( HTML_TABLE *d )
{
	d->heading_list = list_new();
}

void html_table_set_number_left_justified_columns( HTML_TABLE *d, int i )
{
	d->number_left_justified_columns = i;
}

void html_table_set_number_right_justified_columns( HTML_TABLE *d, int i )
{
	d->number_right_justified_columns = i;
}

void html_table_set_heading_list(
			HTML_TABLE *html_table,
			LIST *heading_list )
{
	html_table->heading_list = heading_list;
}

void html_table_set_background_shaded(	HTML_TABLE *d )
{
	d->background_shaded = 1;
}

void html_table_set_background_unshaded(HTML_TABLE *d )
{
	d->background_shaded = 0;
}

char *html_table_get_alignment(	int column_number,
				int number_left_justified_columns,
				int number_right_justified_columns )
{
	if ( number_left_justified_columns
	&&   column_number < number_left_justified_columns )
	{
		return "left";
	}
	else
	if ( number_right_justified_columns
	&&   column_number < number_left_justified_columns +
			     number_right_justified_columns )
	{
		return "right";
	}
	else
	{
		return "left";
	}
}

HTML_ROW *html_row_new(	void )
{
	HTML_ROW *html_row = html_row_calloc();
	html_row->cell_list = list_new();

	return html_row;
}

HTML_ROW *html_row_calloc( void )
{
	HTML_ROW *html_row;

	if ( ! ( html_row = calloc( 1, sizeof( HTML_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return html_row;
}

void html_table_data_heading( LIST *heading_list )
{
	char buffer[ 1024 ];
	char *heading;

	printf( "<tr>\n" );

	if ( list_rewind( heading_list ) )
	{
		do {
			heading = list_get_string( heading_list );

			if ( character_exists( heading, '&' )
			&&   character_exists( heading, ';' ) )
			{
				strcpy( buffer, heading );
			}
			else
			{
				format_initial_capital(
					buffer,
					heading );
			}

			printf( "<th>%s\n", buffer );

		} while( list_next( heading_list ) );
	}
}

