/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_sparrow/output_bird_count.c	*/
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
#define DEFAULT_OUTPUT_MEDIUM			"table"
#define DEFAULT_GROUP_BY			"year"

/* Prototypes */
/* ---------- */
void output_table_year_heading( void );

void output_stdout(		char *group_by,
				char *quad_sheet,
				char *sort_by_per_visit_yn );

char *get_heading(		char *group_by,
				char *quad_sheet );

void output_table_year(
				char *sort_by_per_visit_yn );

void output_table_year_location(
				char *quad_sheet,
				char *sort_by_per_visit_yn );

void output_table_location(
				char *sort_by_per_visit_yn );

void output_spreadsheet(
				char *application_name,
				char *document_root_directory,
				char *session,
				char *group_by,
				char *quad_sheet,
				char *sort_by_per_visit_yn,
				char *process_name );

void output_text_file(
				char *application_name,
				char *document_root_directory,
				char *session,
				char *group_by,
				char *quad_sheet,
				char *sort_by_per_visit_yn,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *group_by;
	char *quad_sheet;
	char *sort_by_per_visit_yn;
	char *output_medium;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_name session group_by quad_sheet sort_by_per_visit_yn output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	session = argv[ 3 ];
	group_by = argv[ 4 ];
	quad_sheet = argv[ 5 ];
	sort_by_per_visit_yn = argv[ 6 ];
	output_medium = argv[ 7 ];

	if ( !*group_by
	||   strcmp( group_by, "group_by" ) == 0 )
	{
		group_by = DEFAULT_GROUP_BY;
	}

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

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

		fflush( stdout );
		printf( "<h1>%s</h1>\n", title );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
		fflush( stdout );

	}

	if ( *quad_sheet && strcmp( quad_sheet, "quad_sheet" ) != 0 )
	{
		group_by = "year";
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		if ( *quad_sheet
		&&   strcmp( quad_sheet, "quad_sheet" ) != 0 )
		{
			output_table_year_location(
				quad_sheet,
				sort_by_per_visit_yn );
		}
		else
		if ( strcmp( group_by, "year" ) == 0 )
		{
			output_table_year(
				sort_by_per_visit_yn );
		}
		else
		{
			output_table_location(
				sort_by_per_visit_yn );
		}
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
			application_name,
			appaserver_parameter_file->document_root,
			session,
			group_by,
			quad_sheet,
			sort_by_per_visit_yn,
			process_name );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_text_file(
			application_name,
			appaserver_parameter_file->document_root,
			session,
			group_by,
			quad_sheet,
			sort_by_per_visit_yn,
			process_name );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		output_stdout(	group_by,
				quad_sheet,
				sort_by_per_visit_yn );
	}


	if ( strcmp( output_medium, "stdout" ) != 0 ) document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_table_year( char *sort_by_per_visit_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	int row_count = -1;

	sprintf( sys_string,
		 "bird_count_group_by year '' '%s'",
		 sort_by_per_visit_yn );

	input_pipe = popen( sys_string, "r" );

	printf( "<table border>\n" );

	while( get_line( input_buffer, input_pipe ) )
	{
		row_count++;

		if ( row_count % 10 == 0 )
		{
			output_table_year_heading();
		}

		search_replace_string( input_buffer, "^", "<td align=right>" );
		printf( "<tr><td>%s\n", input_buffer );
	}

	printf( "</table>\n" );

	pclose( input_pipe );

} /* output_table_year() */

void output_table_year_heading( void )
{
	int i;

	printf( "<tr>\n" );
	printf( "<th>\n" );

	for(	i = 0;
		i < BIRD_COUNT_SUBPOPULATION_SIZE;
		i++ )
	{
		printf( "<th colspan=3>%c\n", i + 65 );
	}

/*
	printf( "<th colspan=3>A\n" );
	printf( "<th colspan=3>B\n" );
	printf( "<th colspan=3>C\n" );
	printf( "<th colspan=3>D\n" );
	printf( "<th colspan=3>E\n" );
	printf( "<th colspan=3>F\n" );
	printf( "<th colspan=3>G\n" );
*/
	printf( "<th colspan=3>Total\n" );

	printf( "<tr>\n" );
	printf( "<th>Year\n" );

	for(	i = 0;
		i < BIRD_COUNT_SUBPOPULATION_SIZE;
		i++ )
	{
		printf( "<th>Site Visit\n" );
		printf( "<th>Bird Count\n" );
		printf( "<th>Per Visit\n" );
	}

	printf( "<th>Site Visit\n" );
	printf( "<th>Bird Count\n" );
	printf( "<th>Per Visit\n" );

} /* output_table_year_heading() */

void output_table_year_location(
			char *quad_sheet,
			char *sort_by_per_visit_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	sprintf( sys_string,
		 "bird_count_group_by year '%s' '%s'",
		 quad_sheet,
		 sort_by_per_visit_yn );

	input_pipe = popen( sys_string, "r" );

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' left,right,right,right",
		 get_heading( "year", quad_sheet ) );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		fprintf( output_pipe, "%s\n", output_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_table_year_location() */

void output_table_location(
			char *sort_by_per_visit_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	sprintf( sys_string,
		 "bird_count_group_by location quad_sheet '%s'",
		 sort_by_per_visit_yn );

	input_pipe = popen( sys_string, "r" );

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' left,right,right,right",
		 get_heading( "location", (char *)0 /* quad_sheet */ ) );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		format_initial_capital( output_buffer, input_buffer );
		fprintf( output_pipe, "%s\n", output_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_table_location() */

void output_stdout(
			char *group_by,
			char *quad_sheet,
			char *sort_by_per_visit_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	printf( "%s\n", get_heading( group_by, quad_sheet ) );

	sprintf( sys_string,
		 "bird_count_group_by %s '%s' '%s' | tr '^' ','",
		 group_by,
		 quad_sheet,
		 sort_by_per_visit_yn );
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
			char *group_by,
			char *quad_sheet,
			char *sort_by_per_visit_yn,
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

	fprintf( output_file, "%s\n", get_heading( group_by, quad_sheet ) );

	sprintf( sys_string,
		 "bird_count_group_by %s '%s' '%s' | tr '^' ','",
		 group_by,
		 quad_sheet,
		 sort_by_per_visit_yn );
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

void output_text_file(
			char *application_name,
			char *document_root_directory,
			char *session,
			char *group_by,
			char *quad_sheet,
			char *sort_by_per_visit_yn,
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

	sprintf( sys_string,
		 "tr ',' '%c' > %s",
		 OUTPUT_TEXT_FILE_DELIMITER,
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe, "%s\n", get_heading( group_by, quad_sheet ) );

	sprintf( sys_string,
		 "bird_count_group_by %s '%s' '%s' | tr '^' ','",
		 group_by,
		 quad_sheet,
		 sort_by_per_visit_yn );
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

char *get_heading(	char *group_by,
			char *quad_sheet )
{
	static char heading[ 1024 ];

	if (	quad_sheet &&
		*quad_sheet &&
		strcmp( quad_sheet, "quad_sheet" ) != 0 )
	{
		sprintf( heading,
		 	 "year(%s),site_visit,bird_count,per_visit",
			 quad_sheet );
	}
	else
	if ( strcmp( group_by, "year" ) == 0 )
	{
		char *ptr = heading;
		int i;

		/* Skip year column */
		/* ---------------- */
		ptr += sprintf( ptr, "," );

		for(	i = 0;
			i < BIRD_COUNT_SUBPOPULATION_SIZE;
			i++ )
		{
			ptr += sprintf( ptr,
					"%c,%c,%c,",
					i + 65,
					i + 65,
					i + 65 );
		}

		ptr += sprintf( ptr, "total,total,total\n" );

		ptr += sprintf( ptr, "year" );

		for(	i = 0;
			/* ---------------------------------- */
			/* Include the total group of columns */
			/* ---------------------------------- */
			i <= BIRD_COUNT_SUBPOPULATION_SIZE;
			i++ )
		{
			ptr += sprintf( ptr,
					",site_visit,bird_count,per_year" );
		}

/*
		sprintf(heading,
",a,a,a,b,b,b,c,c,c,d,d,d,e,e,e,f,f,f,g,g,g,total,total,total\n"
"year,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit,site_visit,bird_count,per_visit" );
*/
	}
	else
	{
		strcpy( heading,
		 	"location,site_visit,bird_count,per_visit" );
	}

	format_initial_capital( heading, heading );

	return heading;

} /* get_heading() */

