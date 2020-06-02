/* ----------------------------------------------------	*/
/* src_hydrology/missing_measurement_report.c		*/
/* ----------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "process.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "application.h"
#include "session.h"
#include "boolean.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM	"table"
#define LINES_TO_QUEUE		500
#define MEASUREMENT_DATE_PIECE	2
#define MEASUREMENT_TIME_PIECE	3

#define FILENAME_STEM		"missing_report"

/* Prototypes */
/* ---------- */
char *get_trim_time_process(	boolean omit_time );

char *get_heading(		boolean omit_time );

void missing_measurement_report_spreadsheet_text_file(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time,
				char *document_root_directory,
				char *output_medium );

void missing_measurement_report_stdout(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time );

void missing_measurement_report_table(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *station, *datatype, *begin_date, *end_date;
	char *output_medium;
	boolean omit_time;
	char *process_name;
	char buffer[ 1024 ];
	char sub_title[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
				
	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s application process_name station datatype begin_date end_date omit_time_yn output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	begin_date = argv[ 5 ];
	end_date = argv[ 6 ];
	omit_time = ( *argv[ 7 ] == 'y' );
	output_medium = argv[ 8 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = DEFAULT_OUTPUT_MEDIUM;

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<h3>ERROR: no data available for these dates.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
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
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );
	
		sprintf( sub_title,
			 "For: %s/%s From: %s To: %s",
			 station,
			 datatype,
			 begin_date,
			 end_date );
	
		printf( "<h2>%s\n",
			format_initial_capital( buffer, process_name ) );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		printf( "</h2>\n" );
		printf( "<h2>%s</h2>\n", sub_title );
		fflush( stdout );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		missing_measurement_report_table(
				application_name,
				station,
				datatype,
				begin_date,
				end_date,
				omit_time );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		missing_measurement_report_spreadsheet_text_file(
				application_name,
				station,
				datatype,
				begin_date,
				end_date,
				omit_time,
				appaserver_parameter_file->
					document_root,
				output_medium );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		missing_measurement_report_stdout(
				application_name,
				station,
				datatype,
				begin_date,
				end_date,
				omit_time );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );

} /* main() */

void missing_measurement_report_stdout(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time )
{
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char buffer[ 1024 ];
	char *heading;
	char *trim_time_process;

	hydrology_library_output_station_text_filename(
				"stdout",
				application_name,
				station,
				1 /* with_zap_file */ );

	hydrology_library_output_data_collection_frequency_text_file(
				stdout,
				application_name,
				station,
				datatype );

	heading = get_heading( omit_time );
	trim_time_process = get_trim_time_process( omit_time );

	sprintf( sys_string,
		 "echo %s; %s",
		 heading,
		 trim_time_process );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
"measurement_data_collection_frequency_list %s %s %s real_time %s %s y |"
"grep '\\^$'",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	input_pipe = popen( sys_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_TIME_PIECE );

		fprintf(	output_pipe,
				"%s%c%s\n",
				measurement_date,
				FOLDER_DATA_DELIMITER,
				measurement_time );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* missing_measurement_report_stdout() */

void missing_measurement_report_table(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time )
{
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char buffer[ 1024 ];
	char *heading;
	char *trim_time_process;

	hydrology_library_output_station_table(
			application_name,
			station );

	hydrology_library_output_data_collection_frequency_table(
			application_name,
			station,
			datatype );

	heading = get_heading( omit_time );
	trim_time_process = get_trim_time_process( omit_time );

	sprintf( sys_string,
		 "%s							|"
		 "queue_top_bottom_lines.e %d				|"
		 "html_table.e '^^Missing Measurements' '%s' '%c'	 ",
		 trim_time_process,
		 LINES_TO_QUEUE,
		 heading,
		 FOLDER_DATA_DELIMITER );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
"measurement_data_collection_frequency_list %s %s %s real_time %s %s y |"
"grep '\\^$'",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	input_pipe = popen( sys_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_TIME_PIECE );

		fprintf(	output_pipe,
				"%s%c%s\n",
				measurement_date,
				FOLDER_DATA_DELIMITER,
				measurement_time );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* missing_measurement_report_table() */

void missing_measurement_report_spreadsheet_text_file(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				boolean omit_time,
				char *document_root_directory,
				char *output_medium )
{
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char buffer[ 1024 ];
	char *heading;
	char *trim_time_process;
	char *ftp_filename = {0};
	char *output_pipename = {0};
	pid_t process_id = getpid();
	unsigned int row_count;
	char *extension;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
		extension = "csv";
	else
		extension = "txt";

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			extension );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	output_pipename =
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

	if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_pipename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_pipe );
	}

	hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				station,
				1 /* with_zap_file */ );

	output_pipe = fopen( output_pipename, "a" );

	hydrology_library_output_data_collection_frequency_text_file(
				output_pipe,
				application_name,
				station,
				datatype );
	fclose( output_pipe );

	heading = get_heading( omit_time );
	trim_time_process = get_trim_time_process( omit_time );

	sprintf( sys_string,
		 "(echo %s; %s | tr '%c' ',') >> %s",
		 heading,
		 trim_time_process,
		 FOLDER_DATA_DELIMITER,
		 output_pipename );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
"measurement_data_collection_frequency_list %s %s %s real_time %s %s y |"
"grep '\\^$'",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	input_pipe = popen( sys_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			buffer,
			MEASUREMENT_TIME_PIECE );

		fprintf(	output_pipe,
				"%s%c%s\n",
				measurement_date,
				FOLDER_DATA_DELIMITER,
				measurement_time );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	sprintf( sys_string,
		 "cat %s | grep -v '^#' | grep -v '^$' | wc -l",
		 output_pipename );

	row_count = atoi( pipe2string( sys_string ) ) - 1;

	printf( "<p>Row count: %s</br>\n",
		 place_commas_in_unsigned_int( row_count ) );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* missing_measurement_report_spreadsheet() */

char *get_heading( boolean omit_time )
{
	char *heading;

	if ( !omit_time )
	{
		heading = "measurement_date,measurement_time";
	}
	else
	{
		heading = "measurement_date";
	}
	return heading;

} /* get_heading() */

char *get_trim_time_process( boolean omit_time )
{
	char trim_time_process[ 128 ];

	if ( !omit_time )
	{
		strcpy( trim_time_process, "cat" );
	}
	else
	{
		sprintf( trim_time_process,
			 "piece.e '%c' 0 | sort -u",
			 FOLDER_DATA_DELIMITER );
	}

	return strdup( trim_time_process );

} /* get_trim_time_process() */

