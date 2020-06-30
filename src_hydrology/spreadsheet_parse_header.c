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

		printf( "%s^%d^%.6lf\n",
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
			/* Sets:
				spreadsheet->spreadsheet_datatype_list
				spreadsheet->spreadsheet_header_row
				spreadsheet->spreadsheet_header_cell_list
			------------------------------------------------- */
			spreadsheet_fetch(
				application_name,
				station_name,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		return (LIST *)0;
	}

	spreadsheet->spreadsheet_output_datatype_list =
		spreadsheet_output_datatype_list(
			spreadsheet->spreadsheet_header_cell_list );

	return spreadsheet->spreadsheet_output_datatype_list;

} /* spreadsheet_parse_datatype_list() */

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

