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
#include "spreadsheet.h"

LIST *spreadsheet_datatype_list( char *application_name )
{
	return datatype_fetch_list( application_name );
}

LIST *spreadsheet_units_list( char *application_name )
{
	return units_list( application_name );
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

DATATYPE *spreadsheet_translate_datatype(
				char *datatype_label,
				LIST *spreadsheet_datatype_list )
{
	return (DATATYPE *)0;
}

UNITS *spreadsheet_translate_units(
				char *units_label,
				LIST *spreadsheet_units_list )
{
	return (UNITS *)0;
}

double spreadsheet_multiply_by(	LIST *spreadsheet_units_list,
				UNITS *units,
				char *units_label )
{
	return 0.0;
}

LIST *spreadsheet_header_cell_list(
				char *station_name,
				char *filename,
				char *date_header_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list )
{
	LIST *header_cell_list;
	SPREADSHEET_HEADER_CELL *spreadsheet_header_cell;
	char *header_buffer;
	char *second_line;
	int column_piece;

	/* Returns heap memory and populates heap memory. */
	/* ---------------------------------------------- */
	header_buffer =
		spreadsheet_header_buffer(
				&second_line,
				filename,
				date_header_label,
				two_lines );

	if ( !header_buffer ) return (LIST *)0;

	header_cell_list = list_new();

	for (	column_piece = 0;
		( spreadsheet_header_cell =
			spreadsheet_header_cell_parse(
				header_buffer,
				second_line,
				column_piece,
				spreadsheet_datatype_list,
				spreadsheet_units_list ) );
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

	output_datatype_list = list_new();
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

	spreadsheet = spreadsheet_new();

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
			date_header_label,
			two_lines,
			spreadsheet->spreadsheet_datatype_list,
			spreadsheet->spreadsheet_units_list );

	return spreadsheet;
}

/* Returns heap memory and populates heap memory. */
/* ---------------------------------------------- */
char *spreadsheet_header_buffer(
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
				char *header_buffer,
				char *second_line,
				int column_piece,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list )
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
				header_buffer,
				second_line,
				column_piece ) ) )
	{
		return (SPREADSHEET_HEADER_CELL *)0;
	}

	if ( ! ( datatype_label =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			spreadsheet_datatype_label( header_label ) ) )
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

	spreadsheet_header_cell->spreadsheet_translate_units =
		spreadsheet_translate_units(
			spreadsheet_header_cell->
				spreadsheet_units_label,
			spreadsheet_units_list );

	spreadsheet_header_cell->spreadsheet_convert_multiply_by =
		spreadsheet_convert_multiply_by(
			spreadsheet_header_cell->
				spreadsheet_translate_units->
				units_converted_list,
			spreadsheet_header_cell->
				spreadsheet_units_label,
			spreadsheet_units_list );

	return spreadsheet_header_cell;
}

/* -------------------------------------------- */
/* Returns heap memory.				*/
/* Header label looks like: Salinity (PSU)	*/
/* -------------------------------------------- */
char *spreadsheet_header_label(
				char *header_buffer,
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
			header_buffer,
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
		strcpy( header_label, datatype_heading_first_line );
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

