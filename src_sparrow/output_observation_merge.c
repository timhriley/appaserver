/* ---------------------------------------------------	*/
/* src_sparrow/output_observation_merge.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "appaserver_link_file.h"
#include "sparrow_library.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES			600
#define DEFAULT_OUTPUT_MEDIUM			"table"

/* Prototypes */
/* ---------- */
void output_stdout(		char *application_name,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn );

char *get_heading(		char *application_name,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn );

void output_table(
				char *application_name,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn );

void output_spreadsheet(
				char *application_name,
				char *document_root_directory,
				char *session,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn,
				char *process_name );

void output_text_file(
				char *application_name,
				char *document_root_directory,
				char *session,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_visit_date;
	char *end_visit_date;
	char *quad_sheet;
	char *process_name;
	char *session;
	char group_quad_yn;
	char *output_medium;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_name session begin_visit_date end_visit_date quad_sheet group_quad_yn output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	session = argv[ 3 ];
	begin_visit_date = argv[ 4 ];
	end_visit_date = argv[ 5 ];
	quad_sheet = argv[ 6 ];
	group_quad_yn = *argv[ 7 ];
	output_medium = argv[ 8 ];

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		char title[ 128 ];

		document = document_new( "", application_name );
		document_set_output_content_type( document );
		
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
		
		document_output_body(
				document->application_name,
				document->onload_control_string );

		format_initial_capital( title, process_name );

		printf( "<h1>%s</h1>\n", title );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );

	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_table(
			application_name,
			begin_visit_date,
			end_visit_date,
			quad_sheet,
			group_quad_yn );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
			application_name,
			appaserver_parameter_file->document_root,
			session,
			begin_visit_date,
			end_visit_date,
			quad_sheet,
			group_quad_yn,
			process_name );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_text_file(
			application_name,
			appaserver_parameter_file->document_root,
			session,
			begin_visit_date,
			end_visit_date,
			quad_sheet,
			group_quad_yn,
			process_name );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		output_stdout(	application_name,
				begin_visit_date,
				end_visit_date,
				quad_sheet,
				group_quad_yn );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 ) document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_stdout(
			char *application_name,
			char *begin_visit_date,
			char *end_visit_date,
			char *quad_sheet,
			char group_quad_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	printf( "%s\n",
		get_heading(	application_name,
				begin_visit_date,
				end_visit_date,
				quad_sheet,
				group_quad_yn ) );

	sprintf( sys_string,
		 "observation_merge %s %s %s '%s' '%c' | tr '^' ','",
		 application_name,
		 begin_visit_date,
		 end_visit_date,
		 quad_sheet,
		 group_quad_yn );
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		printf( "%s\n", output_buffer );
	}

	pclose( input_pipe );

} /* output_stdout() */

void output_spreadsheet(
			char *application_name,
			char *document_root_directory,
			char *session,
			char *begin_visit_date,
			char *end_visit_date,
			char *quad_sheet,
			char group_quad_yn,
			char *process_name )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"csv" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s</h2>\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf(output_file,
		"%s\n",
		 get_heading(	application_name,
				begin_visit_date,
				end_visit_date,
				quad_sheet,
				group_quad_yn ) );

	sprintf( sys_string,
		 "observation_merge %s %s %s '%s' '%c' | tr '^' ','",
		 application_name,
		 begin_visit_date,
		 end_visit_date,
		 quad_sheet,
		 group_quad_yn );
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		fprintf( output_file, "%s\n", output_buffer );
	}

	pclose( input_pipe );
	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spreadsheet() */

void output_table(
			char *application_name,
			char *begin_visit_date,
			char *end_visit_date,
			char *quad_sheet,
			char group_quad_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	sprintf( sys_string,
		 "observation_merge %s %s %s '%s' '%c'",
		 application_name,
		 begin_visit_date,
		 end_visit_date,
		 quad_sheet,
		 group_quad_yn );
	input_pipe = popen( sys_string, "r" );

	sprintf( sys_string,
		 "queue_top_bottom_lines.e %d | html_table.e '' '%s' '^'",
		 QUEUE_TOP_BOTTOM_LINES,
		 get_heading(	application_name,
				begin_visit_date,
				end_visit_date,
				quad_sheet,
				group_quad_yn ) );
	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		fprintf( output_pipe, "%s\n", output_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_table() */

void output_text_file(
				char *application_name,
				char *document_root_directory,
				char *session,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn,
				char *process_name )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"txt" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf( sys_string,
		 "delimiter2padded_columns.e ',' 0 > %s",
		 output_filename );
*/
	sprintf( sys_string,
		 "tr ',' '%c' > %s",
		 OUTPUT_TEXT_FILE_DELIMITER,
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s\n",
		 get_heading(	application_name,
				begin_visit_date,
				end_visit_date,
				quad_sheet,
				group_quad_yn ) );

	sprintf( sys_string,
		 "observation_merge %s %s %s '%s' '%c' | tr '^' ','",
		 application_name,
		 begin_visit_date,
		 end_visit_date,
		 quad_sheet,
		 group_quad_yn );
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		fprintf( output_pipe, "%s\n", output_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_text_file() */

char *get_heading(		char *application_name,
				char *begin_visit_date,
				char *end_visit_date,
				char *quad_sheet,
				char group_quad_yn )
{
	static char heading[ 2048 ];
	char *static_heading;
	char *physical_observation;
	char *vegetation_observation;
	char *observation_where;

	physical_observation =
		get_table_name(
			application_name,
			"physical_observation" );

	vegetation_observation =
		get_table_name(
			application_name,
			"vegetation_observation" );

	observation_where =
		sparrow_library_get_observation_where(
			quad_sheet,
			begin_visit_date,
			end_visit_date,
			(char *)0 /* table_name */ );

	if ( group_quad_yn == 'y' )
		static_heading = "visit_date,quad_sheet,bird_count";
	else
		static_heading = "visit_date,quad_sheet,site_number,bird_count";

	sprintf( heading,
		"%s,%s,%s",
		static_heading,
		sparrow_library_get_distinct_physical_parameter(
			physical_observation,
			observation_where ),
		sparrow_library_get_distinct_vegetation_name(
			vegetation_observation,
			observation_where ) );

	format_initial_capital( heading, heading );
	return heading;

} /* get_heading() */

