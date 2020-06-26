/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/spreadsheet.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "datatype.h"
#include "units.h"
#include "spreadsheet.h"

LIST *spreadsheet_datatype_list(	char *application_name,
					char *station_name )
{
	return datatype_list(
			application_name,
			station_name );
}

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_new( void )
{
	SPREADSHEET_HEADER_CELL *s;

	if ( ! ( s = (SPREADSHEET_HEADER_CELL *)
			calloc( 1, sizeof( SPREADSHEET_HEADER_CELL ) ) ) )
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

SPREADSHEET *spreadsheet_new(	char *application_name,
				char *station_name,
				char *date_header_label,
				char *filename,
				boolean two_lines )
{
	SPREADSHEET *s;

	if ( ! ( s = (SPREADSHEET *)calloc( 1, sizeof( SPREADSHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (SPREADSHEET *)0;
	}

	s->application_name = application_name;
	s->station_name = station_name;
	s->date_header_label = date_header_label;
	s->filename = filename;
	s->two_lines = two_lines;
	return s;
}

DATATYPE *spreadsheet_translate_datatype(
				char *spreadsheet_datatype_label,
				LIST *spreadsheet_datatype_list )
{
	DATATYPE *datatype;
	DATATYPE_ALIAS *datatype_alias;
	LIST *datatype_alias_list;

	if ( !list_rewind( spreadsheet_datatype_list ) )
		return (DATATYPE *)0;

	do {
		datatype = list_get_pointer( spreadsheet_datatype_list );

		if ( timlib_strcmp(
			spreadsheet_datatype_label,
			datatype->datatype_name ) == 0 )
		{
			return datatype;
		}

		datatype_alias_list = datatype->datatype_alias_list;

		if ( !list_rewind( datatype_alias_list ) ) continue;

		do {
			datatype_alias =
				list_get_pointer(
					datatype_alias_list );

			if ( timlib_strcmp( 
				datatype_alias->datatype_alias,
				spreadsheet_datatype_label ) == 0 )
			{
				return datatype;
			}

		} while ( list_next( datatype_alias_list ) );

	} while ( list_next( spreadsheet_datatype_list ) );

	return (DATATYPE *)0;
}

LIST *spreadsheet_header_cell_list(
				char *spreadsheet_header_row,
				char *second_line,
				LIST *spreadsheet_datatype_list )
{
	LIST *header_cell_list;
	SPREADSHEET_HEADER_CELL *spreadsheet_header_cell;
	int column_piece;

	header_cell_list = list_new();

	for (	column_piece = 0;
		( spreadsheet_header_cell =
			spreadsheet_header_cell_parse(
				spreadsheet_header_row,
				second_line,
				column_piece,
				spreadsheet_datatype_list ) );
		column_piece++ )
	{
		list_append_pointer(
			header_cell_list,
			spreadsheet_header_cell );
	}
		
	return header_cell_list;
}

LIST *spreadsheet_output_datatype_list(
				LIST *spreadsheet_header_cell_list )
{
	LIST *output_datatype_list;
	SPREADSHEET_HEADER_CELL *spreadsheet_header_cell;

	if ( !list_rewind( spreadsheet_header_cell_list ) )
		return (LIST *)0;

	output_datatype_list = list_new();

	do {
		spreadsheet_header_cell =
			list_get_pointer(
				spreadsheet_header_cell_list );

		if ( spreadsheet_header_cell->spreadsheet_translate_datatype )
		{
			list_append_pointer(
				output_datatype_list,
				spreadsheet_header_cell->
					spreadsheet_translate_datatype );
		}
	} while ( list_next( spreadsheet_header_cell_list ) );

	return output_datatype_list;
}

SPREADSHEET *spreadsheet_fetch(
				char *application_name,
				char *station_name,
				char *filename,
				char *date_header_label,
				boolean two_lines )
{
	SPREADSHEET *spreadsheet;
	char *second_line = {0};

	if ( ! ( spreadsheet =
			spreadsheet_new(
				application_name,
				station_name,
				date_header_label,
				filename,
				two_lines ) ) )
	{
		return (SPREADSHEET *)0;
	}

	spreadsheet->spreadsheet_datatype_list =
		spreadsheet_datatype_list(
			spreadsheet->application_name,
			spreadsheet->station_name );

	if ( !list_length( spreadsheet->spreadsheet_datatype_list ) )
	{
		fprintf( stderr,
"WARNING in %s/%s()/%d: spreadsheet_datatype_list() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (SPREADSHEET *)0;
	}

	/* Returns heap memory and populates heap memory. */
	/* ---------------------------------------------- */
	if ( ! ( spreadsheet->spreadsheet_header_row =
			spreadsheet_header_row(
				&second_line,
				spreadsheet->filename,
				spreadsheet->date_header_label,
				spreadsheet->two_lines ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: spreadsheet_header_row() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (SPREADSHEET *)0;
	}

	spreadsheet->spreadsheet_header_cell_list =
		spreadsheet_header_cell_list(
			spreadsheet->spreadsheet_header_row,
			second_line,
			spreadsheet->spreadsheet_datatype_list );

	return spreadsheet;
}

/* Returns heap memory and populates heap memory. */
/* ---------------------------------------------- */
char *spreadsheet_header_row(
				char **second_line,
				char *filename,
				char *date_header_label,
				boolean two_lines )
{
	FILE *input_file;
	char header_buffer[ 2048 ];
	char second_line_buffer[ 2048 ];

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	*second_line = '\0';
	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( header_buffer, input_file, 1024 ) )
	{
		if ( spreadsheet_header_label_success(
			date_header_label,
			header_buffer ) )
		{
			if ( two_lines ) 
			{
				timlib_get_line(
					second_line_buffer,
					input_file,
					1024 );

				*second_line = strdup( second_line_buffer );
			}

			fclose( input_file );
			timlib_reset_get_line_check_utf_16();
			return strdup( header_buffer );
		}
	}
	fclose( input_file );
	timlib_reset_get_line_check_utf_16();
	return (char *)0;
}

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_parse(
				char *spreadsheet_header_row,
				char *second_line,
				int column_piece,
				LIST *spreadsheet_datatype_list )
{
	SPREADSHEET_HEADER_CELL *spreadsheet_header_cell;
	char *header_label;
	char *datatype_label;
	char *units_label;

	if ( ! ( header_label =
			/* -------------------------------------------- */
			/* Returns heap memory.				*/
			/* Header label looks like: Salinity (PSU)	*/
			/* -------------------------------------------- */
			spreadsheet_header_label(
				spreadsheet_header_row,
				second_line,
				column_piece ) ) )
	{
		return (SPREADSHEET_HEADER_CELL *)0;
	}

	if ( ! ( datatype_label =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			spreadsheet_datatype_label(
				header_label ) ) )
	{
		return (SPREADSHEET_HEADER_CELL *)0;
	}

	/* -------------------- */
	/* Returns heap memory. */
	/* -------------------- */
	units_label = spreadsheet_units_label( header_label );

	spreadsheet_header_cell = spreadsheet_header_cell_new();

	spreadsheet_header_cell->spreadsheet_datatype_label = datatype_label;
	spreadsheet_header_cell->spreadsheet_units_label = units_label;
	spreadsheet_header_cell->column_piece = column_piece;

	spreadsheet_header_cell->spreadsheet_translate_datatype =
		spreadsheet_translate_datatype(
			spreadsheet_header_cell->
				spreadsheet_datatype_label,
			spreadsheet_datatype_list );

	if ( !spreadsheet_header_cell->spreadsheet_translate_datatype )
	{
		return (SPREADSHEET_HEADER_CELL *)0;
	}

	spreadsheet_header_cell->spreadsheet_translate_units_name =
		spreadsheet_translate_units_name(
			spreadsheet_header_cell->
				spreadsheet_units_label,
			spreadsheet_header_cell->
				spreadsheet_translate_datatype->
				units->
				units_name,
			spreadsheet_header_cell->
				spreadsheet_translate_datatype->
				units->
				units_alias_list );

	spreadsheet_header_cell->spreadsheet_units_converted_multiply_by =
		spreadsheet_units_converted_multiply_by(
			spreadsheet_header_cell->
				spreadsheet_translate_units_name,
			spreadsheet_header_cell->
			spreadsheet_translate_datatype->
				units->
				units_converted_list );

	return spreadsheet_header_cell;
}

/* -------------------------------------------- */
/* Returns heap memory.				*/
/* Header label looks like: Salinity (PSU)	*/
/* -------------------------------------------- */
char *spreadsheet_header_label(
				char *spreadsheet_header_row,
				char *second_line,
				int column_piece )
{
	char datatype_heading_first_line[ 128 ];
	char datatype_heading_second_line[ 128 ];
	char header_label[ 256 ];

	*header_label = '\0';
	*datatype_heading_second_line = '\0';

	piece_quoted(	datatype_heading_first_line,
			',',
			spreadsheet_header_row,
			column_piece,
			'"' );

	if ( !*datatype_heading_first_line ) return (char *)0;

	if ( second_line && *second_line )
	{
		piece_quoted(	datatype_heading_second_line,
				',',
				second_line,
				column_piece,
				'"' );

		if ( *datatype_heading_second_line )
		{
			sprintf(header_label,
			 	"%s %s",
			 	datatype_heading_first_line,
			 	datatype_heading_second_line );
		}
	}

	if ( !*header_label )
	{
		strcpy( header_label, spreadsheet_header_row );
	}

	trim( header_label );
	return strdup( header_label );
}

boolean spreadsheet_header_label_success(
				char *date_header_label,
				char *header_buffer )
{
	if ( instr( date_header_label, header_buffer, 1 ) > -1 )
		return 1;
	else
		return 0;
}

/* Returns heap memory. */
/* -------------------- */
char *spreadsheet_datatype_label(
				/* --------------------------------------- */
				/* Header label looks like: Salinity (PSU) */
				/* --------------------------------------- */
				char *spreadsheet_header_label )
{
	char datatype_label[ 256 ];

	if ( !spreadsheet_header_label || !*spreadsheet_header_label )
		return (char *)0;

	piece( datatype_label, '(', spreadsheet_header_label, 0 );
	return strdup( datatype_label );
}

/* Returns heap memory. */
/* -------------------- */
char *spreadsheet_units_label(
				/* --------------------------------------- */
				/* Header label looks like: Salinity (PSU) */
				/* --------------------------------------- */
				char *spreadsheet_header_label )
{
	char units_label[ 256 ];

	if ( !spreadsheet_header_label || !*spreadsheet_header_label )
		return (char *)0;

	if ( timlib_character_exists( spreadsheet_header_label, '(' ) )
	{
		char buffer[ 256 ];

		piece( buffer, '(', spreadsheet_header_label, 1 );
		piece( units_label, ')', buffer, 0 );
		return strdup( units_label );
	}
	return (char *)0;
}

char *spreadsheet_translate_units_name(
				char *spreadsheet_units_label,
				char *units_name,
				LIST *units_alias_list )
{
	UNITS_ALIAS *units_alias;

	if ( timlib_strcmp( 
		spreadsheet_units_label,
		units_name ) == 0 )
	{
		return units_name;
	}

	/* Just check for existance */
	/* ------------------------ */
	if ( ( units_alias =
			units_alias_seek(
				units_alias_list,
				spreadsheet_units_label
					/* units_alias */ ) ) )
	{
		return units_name;
	}

	return (char *)0;
}

double spreadsheet_units_converted_multiply_by(
				char *spreadsheet_translate_units_name,
				LIST *units_converted_list )
{
	return units_converted_multiply_by(
			spreadsheet_translate_units_name
				/* units_converted */,
			units_converted_list );
}

