/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet_column.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "spreadsheet_column.h"

int spreadsheet_column_piece(
			char *heading_line,
			char *heading )
{
	int p;
	char heading_data[ 128 ];

	for (	p = 0;
		piece_quote_comma(
			heading_data /* destination */,
			heading_line /* source */,
			p /* offset */ );
		p++ )
	{
		if ( string_strcmp(
			heading_data,
			heading ) == 0 )
		{
			return p;
		}
	}
	return -1;
}

char *spreadhsheet_column_cell_extract(
			char *input_row,
			int spreadsheet_column_piece )
{
	char data[ 1024 ];

	if ( piece_quote_comma(
			data /* destination */,
			input_row /* source */,
			spreadsheet_column_piece /* offset */ ) )
	{
		return strdup( data );
	}

	return (char *)0;
}

SPREADSHEET_COLUMN *spreadsheet_column_new( char *heading )
{
	SPREADSHEET_COLUMN *spreadsheet_column;

	if ( ! ( spreadsheet_column =
			 calloc( 1, sizeof( SPREADSHEET_COLUMN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	spreadsheet_column->heading = heading;
	return spreadsheet_column;
}

SPREADSHEET_COLUMN *spreadsheet_column_heading_seek(
			LIST *spreadsheet_column_list,
			char *heading )
{
	SPREADSHEET_COLUMN *spreadsheet_column;

	if ( !list_rewind( spreadsheet_column_list ) )
		return (SPREADSHEET_COLUMN *)0;

	do {
		spreadsheet_column = list_get( spreadsheet_column_list );

		if ( string_strcmp(	spreadsheet_column->heading,
					heading ) == 0 )
		{
			return spreadsheet_column;
		}

	} while( list_next( spreadsheet_column_list ) );

	return (SPREADSHEET_COLUMN *)0;
}

SPREADSHEET_COLUMN *spreadsheet_column_attribute_seek(
			LIST *spreadsheet_column_list,
			char *folder_name,
			char *attribute_name )
{
	SPREADSHEET_COLUMN *spreadsheet_column;

	if ( !list_rewind( spreadsheet_column_list ) )
		return (SPREADSHEET_COLUMN *)0;

	do {
		spreadsheet_column = list_get( spreadsheet_column_list );

		if ( string_strcmp(	spreadsheet_column->folder_name,
					folder_name ) == 0
		&&   string_strcmp(	spreadsheet_column->attribute_name,
					attribute_name ) == 0 )
		{
			return spreadsheet_column;
		}

	} while( list_next( spreadsheet_column_list ) );

	return (SPREADSHEET_COLUMN *)0;
}

SPREADSHEET_COLUMN *spreadsheet_column_parse( char *input )
{
	char heading[ 128 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char description[ 1024 ];
	SPREADSHEET_COLUMN *spreadsheet_column;

	if ( !input || !*input ) return (SPREADSHEET_COLUMN *)0;

	piece( heading, SQL_DELIMITER, input, 0 );

	spreadsheet_column =
		spreadsheet_column_new(
			strdup( heading ) );

	piece( folder_name, SQL_DELIMITER, input, 1 );
	spreadsheet_column->folder_name = strdup( folder_name );

	piece( attribute_name, SQL_DELIMITER, input, 2 );
	spreadsheet_column->attribute_name = strdup( attribute_name );

	piece( description, SQL_DELIMITER, input, 3 );
	spreadsheet_column->description = strdup( description );

	return spreadsheet_column;
}

LIST *spreadsheet_column_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *column_list;

	column_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( column_list, spreadsheet_column_parse( input ) );
	}

	pclose( input_pipe );
	return column_list;
}

char *spreadsheet_column_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" none",
		 "spreadsheet_column",
		 where );

	return strdup( sys_string );
}

