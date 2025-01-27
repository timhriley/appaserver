/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/spreadsheet.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "application.h"
#include "spreadsheet.h"

SPREADSHEET_CELL *spreadsheet_cell_new(
			char *data,
			boolean string_boolean )
{
	SPREADSHEET_CELL *spreadsheet_cell;

	if ( !data )
	{
		char message[ 128 ];

		sprintf(message, "data is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	spreadsheet_cell = spreadsheet_cell_calloc();

	spreadsheet_cell->data = data;
	spreadsheet_cell->string_boolean = string_boolean;

	return spreadsheet_cell;
}

SPREADSHEET_CELL *spreadsheet_cell_calloc( void )
{
	SPREADSHEET_CELL *spreadsheet_cell;

	if ( ! ( spreadsheet_cell = calloc( 1, sizeof ( SPREADSHEET_CELL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return spreadsheet_cell;
}

void spreadsheet_cell_output(
			SPREADSHEET_CELL *spreadsheet_cell,
			FILE *output_file )
{
	if ( spreadsheet_cell->string_boolean )
		fprintf( output_file, "%c", '"' );

	fprintf( output_file, "%s", spreadsheet_cell->data );

	if ( spreadsheet_cell->string_boolean )
		fprintf( output_file, "%c", '"' );
}

SPREADSHEET_ROW *spreadsheet_row_new( void )
{
	SPREADSHEET_ROW *spreadsheet_row = spreadsheet_row_calloc();

	spreadsheet_row->cell_list = list_new();
	return spreadsheet_row;
}

SPREADSHEET_ROW *spreadsheet_row_calloc( void )
{
	SPREADSHEET_ROW *spreadsheet_row;

	if ( ! ( spreadsheet_row = calloc( 1, sizeof ( SPREADSHEET_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return spreadsheet_row;
}

void spreadsheet_row_output(
			SPREADSHEET_ROW *spreadsheet_row,
			FILE *output_file )
{
	SPREADSHEET_CELL *spreadsheet_cell;

	if ( !spreadsheet_row
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( spreadsheet_row->cell_list ) ) return;

	do {
		spreadsheet_cell =
			list_get(
				spreadsheet_row->cell_list );

		spreadsheet_cell_output(
			spreadsheet_cell,
			output_file );

		if ( !list_at_first( spreadsheet_row->cell_list ) )
			fprintf( output_file, "," );

	} while ( list_next( spreadsheet_row->cell_list ) );

	fprintf( output_file, "\n" );
}

SPREADSHEET_HEADING *spreadsheet_heading_new(
			char *heading_string )
{
	SPREADSHEET_HEADING *spreadsheet_heading;

	if ( !heading_string )
	{
		char message[ 128 ];

		sprintf(message, "heading_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	spreadsheet_heading = spreadsheet_heading_calloc();
	spreadsheet_heading->heading_string = heading_string;
	return spreadsheet_heading;
}

SPREADSHEET_HEADING *spreadsheet_heading_calloc( void )
{
	SPREADSHEET_HEADING *spreadsheet_heading;

	if ( ! ( spreadsheet_heading =
			calloc( 1,
				sizeof ( SPREADSHEET_HEADING ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return spreadsheet_heading;
}

void spreadsheet_heading_list_output(
			LIST *heading_list,
			FILE *output_file )
{
	SPREADSHEET_HEADING *spreadsheet_heading;
	char buffer[ 128 ];

	if ( !list_rewind( heading_list )
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		spreadsheet_heading = list_get( heading_list );

		if ( !list_at_first( heading_list ) )
			fprintf( output_file, "," );

		fprintf(
			output_file,
			"\"%s\"",
			string_initial_capital(
				buffer,
				spreadsheet_heading->heading_string ) );

	} while ( list_next( heading_list ) );

	fprintf( output_file, "\n" );
}

SPREADSHEET *spreadsheet_new(
			char *application_name,
			char *data_directory,
			pid_t process_id )
{
	SPREADSHEET *spreadsheet;

	if ( !application_name
	||   !data_directory
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	spreadsheet = spreadsheet_calloc();

	spreadsheet->heading_list = list_new();
	spreadsheet->row_list = list_new();

	spreadsheet->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			"spreadsheet" /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	spreadsheet->output_filename =
		spreadsheet->
			appaserver_link->
			appaserver_link_output->
			filename;

	spreadsheet->appaserver_link_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			spreadsheet->
				appaserver_link->
				appaserver_link_prompt->
				filename,
			"_new" /* target_frame */,
			"Link to spreadsheet" /* prompt_message */ );

	return spreadsheet;
}

SPREADSHEET *spreadsheet_calloc( void )
{
	SPREADSHEET *spreadsheet;

	if ( ! ( spreadsheet = calloc( 1, sizeof ( SPREADSHEET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return spreadsheet;
}

void spreadsheet_output(
			SPREADSHEET *spreadsheet,
			FILE *output_file )
{
	SPREADSHEET_ROW *spreadsheet_row;

	if ( !spreadsheet
	||   !list_length( spreadsheet->heading_list )
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		if ( output_file ) fclose( output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	spreadsheet_heading_list_output(
		spreadsheet->heading_list,
		output_file );

	if ( !list_rewind( spreadsheet->row_list ) ) return;

	do {
		spreadsheet_row = list_get( spreadsheet->row_list );

		spreadsheet_row_output(
			spreadsheet_row,
			output_file );

	} while ( list_next( spreadsheet->row_list ) );
}

