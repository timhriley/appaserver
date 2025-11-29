/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/html.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "html.h"

HTML_TABLE *html_table_new(
		char *title,
		char *sub_title,
		char *sub_sub_title )
{
	HTML_TABLE *html_table = html_table_calloc();

	html_table->title = title;
	html_table->sub_title = sub_title;
	html_table->sub_sub_title = sub_sub_title;

	return html_table;
}

HTML_TABLE *html_table_calloc( void )
{
	HTML_TABLE *html_table;

	if ( ! ( html_table = calloc( 1, sizeof ( HTML_TABLE ) ) ) )
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

void html_table_title_output(
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

char *html_table_close_tag( void )
{
	return "</table>";
}

void html_table_output(
		HTML_TABLE *html_table,
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

	html_table_title_output(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	html_column_list_output( html_table->column_list );

	if ( !list_rewind( html_table->row_list ) )
	{
		printf(	"%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			html_table_close_tag() );
		return;
	}

	do {
		html_row = list_get( html_table->row_list );

		if ( ++row_count == rows_between_heading )
		{
			html_column_list_output( html_table->column_list );
			row_count = 0;
		}

		html_row_output(
			html_table->column_list,
			html_row );

	} while ( list_next( html_table->row_list ) );

	printf(	"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		html_table_close_tag() );
}

HTML_COLUMN *html_column_new(
		char *heading,
		boolean right_justify_boolean )
{
	HTML_COLUMN *html_column = html_column_calloc();

	html_column->heading = heading;
	html_column->right_justify_boolean = right_justify_boolean;

	return html_column;
}

HTML_COLUMN *html_column_calloc( void )
{
	HTML_COLUMN *html_column;

	if ( ! ( html_column = calloc( 1, sizeof ( HTML_COLUMN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return html_column;
}

void html_column_list_output( LIST *column_list )
{
	HTML_COLUMN *html_column;

	if ( !list_rewind( column_list ) ) return;

	printf( "<tr>\n" );

	do {
		html_column = list_get( column_list );

		printf( "%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			html_column_tag(
				html_column->heading ) );

	} while ( list_next( column_list ) );
}

char *html_column_tag( char *heading )
{
	static char tag[ 132 ];
	char buffer[ 128 ];

	if ( !heading ) heading = "";

	if ( strlen( heading ) > 127 )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: heading of [%s] is larger than 127 characters.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			heading );
		exit( 1 );
	}

	if ( string_character_exists( heading, '&' )
	&&   string_character_exists( heading, ';' ) )
	{
		strcpy( buffer, heading );
	}
	else
	{
		string_initial_capital(
			buffer,
			heading );
	}

	snprintf(
		tag,
		sizeof ( tag ),
		"<th>%s",
		buffer );

	return tag;
}

HTML_CELL *html_cell_new(
		char *datum,
		boolean large_boolean,
		boolean bold_boolean )
{
	HTML_CELL *html_cell = html_cell_calloc();

	html_cell->datum = datum;
	html_cell->large_boolean = large_boolean;
	html_cell->bold_boolean = bold_boolean;

	return html_cell;
}

HTML_CELL *html_cell_calloc( void )
{
	HTML_CELL *html_cell;

	if ( ! ( html_cell = calloc( 1, sizeof ( HTML_CELL ) ) ) )
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

LIST *html_column_list(
		LIST *heading_list,
		LIST *justify_list )
{
	LIST *list = list_new();
	char *heading;
	char *justify;
	boolean right_justify_boolean = 0;
	HTML_COLUMN *html_column;

	if ( list_length( justify_list ) ) list_rewind( justify_list );

	if ( list_rewind( heading_list ) )
	do {
		heading = list_get( heading_list );

		if ( list_length( justify_list ) )
		{
			justify = list_get( justify_list );

			right_justify_boolean =
				html_column_right_justify_boolean(
					justify );
		}

		html_column =
			html_column_new(
				heading,
				right_justify_boolean );

		list_set( list, html_column );

		if ( list_length( justify_list ) ) list_next( justify_list );

	} while ( list_next( heading_list ) );

	return list;
}

boolean html_column_right_justify_boolean( char *justify )
{
	return ( string_strcmp( justify, "right" ) == 0 );
}

void html_cell_output(
		HTML_COLUMN *html_column,
		HTML_CELL *html_cell,
		boolean background_shaded_boolean )
{
	char *tag;

	if ( !html_column
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
		html_cell_datum_tag(
			html_cell->datum,
			html_cell->large_boolean,
			html_cell->bold_boolean,
			html_column->right_justify_boolean,
			background_shaded_boolean );

	printf( "%s\n", tag );
	free( tag );
}
			
char *html_cell_datum_tag(
		char *datum,
		boolean large_boolean,
		boolean bold_boolean,
		boolean right_justify_boolean,
		boolean background_shaded_boolean )
{
	char tag[ 65536 ];
	char *ptr = tag;

	if ( !datum ) datum = "";

	if ( strlen( datum ) > 65500 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: datum too big.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "<td" );

	if ( background_shaded_boolean )
	{
		ptr += sprintf( ptr, " bgcolor=\"#CCCCCC\"" );
	}

	if ( *datum && right_justify_boolean )
	{
		ptr += sprintf( ptr, " align=right" );
	}

	ptr += sprintf( ptr, ">" );

	if ( *datum && large_boolean )
	{
		ptr += sprintf( ptr, "<big>" );
	}

	if ( *datum && bold_boolean )
	{
		ptr += sprintf( ptr, "<b>" );
	}

	ptr += sprintf( ptr, "%s", datum );

	if ( *datum && bold_boolean )
	{
		ptr += sprintf( ptr, "</b>" );
	}

	if ( *datum && large_boolean )
	{
		ptr += sprintf( ptr, "</big>" );
	}

	return strdup( tag );
}

void html_row_output(
		LIST *column_list,
		HTML_ROW *html_row )
{
	HTML_CELL *html_cell;

	if ( !list_rewind( column_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: column_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !html_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: html_row is empty.\n",
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
			(HTML_COLUMN *)list_get( column_list ),
			html_cell,
			html_row->background_shaded_boolean );

		list_next( column_list );

	} while ( list_next( html_row->cell_list ) );
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

HTML_ROW *html_row_new(
		LIST *cell_list )
{
	HTML_ROW *html_row;

	if ( !list_length( cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "cell_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	html_row = html_row_calloc();
	html_row->cell_list = cell_list;

	return html_row;
}

HTML_ROW *html_row_calloc( void )
{
	HTML_ROW *html_row;

	if ( ! ( html_row = calloc( 1, sizeof ( HTML_ROW ) ) ) )
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

void html_table_list_output(
		LIST *table_list,
		int html_table_rows_between_heading )
{
	HTML_TABLE *html_table;

	if ( list_rewind( table_list ) )
	do {
		html_table = list_get( table_list );

		html_table_output(
			html_table,
			html_table_rows_between_heading );

	} while ( list_next( table_list ) );
}

void html_string_list_stdout(
		int html_table_queue_top_bottom,
		char *title,
		char *heading_string,
		LIST *string_list )
{
	char *system_string;
	FILE *file;

	if ( !title
	||   !heading_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		html_table_system_string(
			html_table_queue_top_bottom,
			'^' /* delimiter */,
			title,
			heading_string );

	file = appaserver_output_pipe( system_string );

	if ( list_rewind( string_list ) )
	{
		do {
			fprintf(file,
				"%s\n",
				(char *)list_get( string_list ) );

		} while ( list_next( string_list ) );
	}

	pclose( file );
}

char *html_table_system_string(
		int html_table_queue_top_bottom,
		char delimiter,
		char *title,
		char *heading_string )
{
	char system_string[ 1024 ];

	if ( !delimiter
	||   !heading_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"queue_top_bottom_lines.e %d |"
		"html_table.e '^^%s' '%s' '%c'",
		html_table_queue_top_bottom,
		(title) ? title : "",
		heading_string,
		delimiter);

	return strdup( system_string );
}

LIST *html_cell_list( LIST *data_list )
{
	LIST *cell_list = list_new();

	if ( list_rewind( data_list ) )
	do {
		list_set(
			cell_list,
			html_cell_new(
				(char *)list_get( data_list ) /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );

	} while ( list_next( data_list ) );

	return cell_list;
}

LIST *html_column_right_list( LIST *heading_list )
{
	LIST *column_list = list_new();
	char *heading;

	if ( list_rewind( heading_list ) )
	do {
		heading = list_get( heading_list );

		list_set(
			column_list,
			html_column_new(
				heading,
				1 /* right_justify_boolean */ ) );

	} while ( list_next( heading_list ) );

	return column_list;
}
