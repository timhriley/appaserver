/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date_convert.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "spreadsheet_column.h"
#include "spreadsheet.h"

SPREADSHEET *spreadsheet_calloc( void )
{
	SPREADSHEET *spreadsheet;

	if ( ! ( spreadsheet = calloc( 1, sizeof( SPREADSHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return spreadsheet;
}

SPREADSHEET *spreadsheet_fetch(
			char *spreadsheet_filename,
			char *date_label )
{
	SPREADSHEET *spreadsheet = spreadsheet_calloc();

	spreadsheet->spreadsheet_filename = spreadsheet_filename;

	spreadsheet->
		spreadsheet_column_list =
			spreadsheet_column_list(
				spreadsheet_filename,
				date_label );

	if ( !list_length( 
		spreadsheet->
			spreadsheet_column_list ) )
	{
		return (SPREADSHEET *)0;
	}
	return spreadsheet;
}

LIST *spreadsheet_column_list(
			char *spreadsheet_filename,
			char *date_label )
{
	char *header_row;
	int p;
	char header_data[ 1024 ];
	LIST *column_list;

	if ( ! ( header_row =
			spreadsheet_header_row(
				spreadsheet_filename,
				date_label ) ) )
	{
		return (LIST *)0;
	}

	column_list = list_new();

	for(	p = 0;
		piece_quote_comma(
			header_data,
			header_row,
			p );
		p++ )
	{
		list_set(
			column_list,
			spreadsheet_column_new(
				strdup( header_data ),
				p ) );
	}
	return column_list;
}

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_minimum_date(
			char **maximum_date,
			int *row_count,
			char *filename )
{
	FILE *input_file;
	char input_buffer[ 65536 ];
	char input_date[ 128 ];
	char minimum_date[ 128 ];
	char compare_date[ 16 ];
	char local_maximum_date[ 128 ];

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filename );
		return (char *)0;
	}

	*minimum_date = '\0';
	*local_maximum_date = '\0';
	*row_count = 0;

	while( string_input( input_buffer, input_file, 65536 ) )
	{
		(*row_count)++;

		piece_quote(
			input_date,
			input_buffer,
			0 );

		if ( !date_convert_source_american(
			compare_date,
			international,
			input_date ) )
		{
			continue;
		}

		if ( !*minimum_date )
		{
			strcpy( minimum_date, compare_date );
		}

		if ( !*local_maximum_date )
		{
			strcpy( local_maximum_date, compare_date );
		}

		if ( strcmp( compare_date, minimum_date ) < 0 )
		{
			strcpy( minimum_date, compare_date );
		}

		if ( strcmp( compare_date, local_maximum_date ) > 0 )
		{
			strcpy( local_maximum_date, compare_date );
		}
	}

	fclose( input_file );

	*maximum_date = strdup( local_maximum_date );

	if ( !*minimum_date )
		return (char *)0;
	else
		return strdup( minimum_date );
}

char *spreadsheet_heading_data(
			LIST *spreadsheet_column_list,
			char *input_row,
			char *heading )
{
	char data[ 65536 ];
	SPREADSHEET_COLUMN *spreadsheet_column;

	if ( ! ( spreadsheet_column =
			spreadsheet_column_heading_seek(
				spreadsheet_column_list,
				heading ) ) )
	{
		return (char *)0;
	}

	if ( !piece_quote(
			data /* destination */,
			input_row,
			spreadsheet_column->
				piece_offset ) )
	{
		return (char *)0;
	}
	return strdup( data );
}

char *spreadsheet_header_row(
			char *filename,
			char *date_label )
{
	FILE *input_file;
	char header_buffer[ 65536 ];

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filename );
		return (char *)0;
	}

	while( string_input( header_buffer, input_file, 65536 ) )
	{
		if ( spreadsheet_header_label_success(
			date_label,
			header_buffer ) )
		{
			fclose( input_file );
			return strdup( header_buffer );
		}
	}
	fclose( input_file );
	return (char *)0;
}

boolean spreadsheet_header_label_success(
			char *date_label,
			char *header_buffer )
{
	if ( instr( date_label, header_buffer, 1 ) > -1 )
		return 1;
	else
		return 0;
}

