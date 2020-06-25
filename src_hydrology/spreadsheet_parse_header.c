/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/spreadsheet_parse_header.c	*/
/* ---------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "query.h"
#include "date.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "julian.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"
#include "name_arg.h"
#include "spreadsheet.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean spreadsheet_parse_heading_label_success(
			char *date_heading_label,
			char *heading_buffer );

void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **filename,
			char **station,
			char **date_heading_label,
			char **two_lines_yn,
			NAME_ARG *arg );

void spreadsheet_parse_display(		LIST *datatype_list );

LIST *input_buffer_datatype_list(	char *application_name,
					char *station_name,
					char *heading_buffer,
					char *second_line );

LIST *spreadsheet_parse_datatype_list(
					char *application_name,
					char *station_name,
					char *input_filespecification,
					char *date_heading_label,
					boolean two_lines );

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filespecification;
	char *date_heading_label;
	char *station;
	LIST *datatype_list;
	boolean two_lines;
	char *two_lines_yn;
	NAME_ARG *arg;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
			&input_filespecification,
			&station,
			&date_heading_label,
			&two_lines_yn,
			arg );

	two_lines = ( *two_lines_yn == 'y' );

	if ( ( datatype_list =
			spreadsheet_parse_datatype_list(
				application_name,
				station,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		spreadsheet_parse_display( datatype_list );
	}


	return 0;

} /* main() */

void spreadsheet_parse_display( LIST *datatype_list )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		datatype = list_get_pointer( datatype_list );

		printf( "%s^%d^.5lf\n",
			datatype->datatype_name,
			datatype->column_piece,
			datatype->units_converted_multiply_by );

	} while ( list_next( datatype_list ) );

} /* spreadsheet_parse_display() */

LIST *spreadsheet_parse_datatype_list(
				char *application_name,
				char *station_name,
				char *input_filespecification,
				char *date_heading_label,
				boolean two_lines )
{
	SPREADSHEET *spreadsheet;

	if ( ! ( spreadsheet =
			spreadsheet_fetch(
				application_name,
				station_name,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		return (LIST *)0;
	}

} /* spreadsheet_parse_datatype_list() */

#ifdef NOT_DEFINED
LIST *spreadsheet_parse_datatype_list(
				char *application_name,
				char *station_name,
				char *input_filespecification,
				char *date_heading_label,
				boolean two_lines )
{
	FILE *input_file;
	char heading_buffer[ 1024 ];
	char second_line[ 1024 ];
	LIST *datatype_list = {0};

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		fflush( stderr );
		exit( 1 );
	}

	*second_line = '\0';
	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( heading_buffer, input_file, 1024 ) )
	{
		if ( spreadsheet_parse_heading_label_success(
			date_heading_label,
			heading_buffer ) )
		{
			if ( two_lines ) 
			{
				timlib_get_line(
					second_line,
					input_file,
					1024 );
			}

			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			datatype_list =
				input_buffer_datatype_list(
					application_name,
					station_name,
					heading_buffer,
					second_line );

			return datatype_list;
		}
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

	return (LIST *)0;

} /* spreadsheet_parse_datatype_list() */
#endif

void fetch_parameters(	char **filename,
			char **station,
			char **date_heading_label,
			char **two_lines_yn,
			NAME_ARG *arg )
{
	*filename = fetch_arg( arg, "filename" );
	*station = fetch_arg( arg, "station" );
	*date_heading_label = fetch_arg( arg, "date_heading_label" );
	*two_lines_yn = fetch_arg( arg, "two_lines" );

} /* fetch_parameters() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "filename" );
        ticket = add_valid_option( arg, "station" );

        ticket = add_valid_option( arg, "date_heading_label" );
        set_default_value( arg, ticket, "date" );

        ticket = add_valid_option( arg, "two_lines" );
	add_valid_value( arg, ticket, "yes" );
	add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

boolean spreadsheet_parse_heading_label_success(
				char *date_heading_label,
				char *heading_buffer )
{
	if ( instr( date_heading_label, heading_buffer, 1 ) > -1 )
		return 1;
	else
		return 0;

}

/* ------------------------------------------ */
/* Sets datatype->column_piece		      */
/*  and datatype->units_converted_multiply_by */
/* ------------------------------------------ */
LIST *input_buffer_datatype_list(	char *application_name,
					char *station_name,
					char *heading_buffer,
					char *second_line )
{
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

	return datatype_column_piece_datatype_list( datatype_list ); 

} /* input_buffer_datatype_list() */

