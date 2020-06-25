/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/spreadsheet.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include "spreadsheet.h"

LIST *spreadsheet_datatype_list( char *application_name )
{
}

LIST *spreadsheet_units_list( char *application_name )
{
}

SPREADSHEET *spreadsheet_new( void )
{
	SPREADSHEET *s;

	if ( ! ( s = (SPREADSHEET *)calloc( 1, sizeof( SPREADSHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return s;
}

UNITS *spreadsheet_translate_units(
				LIST *spreadsheet_units_list,
				char *units_label )
{
}

double spreadsheet_multiply_by(	LIST *spreadsheet_units_list,
				UNITS *units,
				char *units_label )
{
}

LIST *spreadsheet_header_cell_list(
				char *station_name,
				char *filename,
				char *date_heading_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list )
{
	LIST *header_cell_list;

	
	return header_cell_list;
}

LIST *spreadsheet_output_datatype_list(
				LIST *spreadsheet_header_cell_list )
{
}

SPREADHSEET *spreadsheet_fetch(
				char *application_name,
				char *station_name,
				char *filename,
				char *date_heading_label,
				boolean two_lines )
{
	SPREADSHEET *spreadsheet;

	spreadsheet = spreadsheet_new()

	spreadsheet->spreadsheet_datatype_list =
		spreadsheet_datatype_list(
			application_name );

	if ( !list_length( spreadsheet->spreadsheet_datatype_list ) )
	{
		fprintf( stderr,
"WARNING in %s/%s()/%d: spreadsheet_datatype_list() returned nothing.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (SPREADSHEET *)0;
	}

	spreadsheet->spreadsheet_units_list =
		spreadsheet_units_list(
			application_name );

	spreadsheet->spreadsheet_header_cell_list =
		spreadsheet_header_cell_list(
			station_name,
			filename,
			date_heading_label,
			two_lines,
			spreadsheet->spreadsheet_datatype_list,
			spreadsheet->spreadsheet_units_list );
}


	LIST *datatype_list;
	DATATYPE *datatype;
	char datatype_heading_first_line[ 128 ];
	char datatype_heading_second_line[ 128 ];
	char datatype_units_seek_phrase[ 256 ];
	int column_piece;
	STATION *station;

	station =
		station_get_or_set_station(
			list_new() /* station_list */,
			application_name,
			station_name );

	datatype_list = list_new();
	*datatype_heading_second_line = '\0';

	for(	column_piece = 0;
		piece_quoted(	datatype_heading_first_line,
				',',
				heading_buffer,
				column_piece,
				'"' );
		column_piece++ )
	{
		if ( !*datatype_heading_first_line ) continue;

		if ( *second_line )
		{
			piece_quoted(	datatype_heading_second_line,
					',',
					second_line,
					column_piece,
					'"' );
		}

		if ( *datatype_heading_second_line )
		{
			sprintf(datatype_units_seek_phrase,
			 	"%s %s",
			 	datatype_heading_first_line,
			 	datatype_heading_second_line );
		}
		else
		{
			strcpy( datatype_units_seek_phrase,
			 	datatype_heading_first_line );
		}

		trim( datatype_units_seek_phrase );

		if ( ! ( datatype =
				/* -------------------------------- */
				/* Sets units_converted_multiply_by */
				/* -------------------------------- */
				datatype_seek_phrase(
					station->station_datatype_list,
					station->station_name,
					datatype_units_seek_phrase ) ) )
		{
			continue;
		}

		datatype->column_piece = column_piece;
		list_append_pointer( datatype_list, datatype );
	}

