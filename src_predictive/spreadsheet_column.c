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

SPREADSHEET_COLUMN *spreadsheet_column_new(
			char *heading,
			int piece_offset )
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
	spreadsheet_column->piece_offset = piece_offset;

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

