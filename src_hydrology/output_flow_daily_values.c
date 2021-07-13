/* ---------------------------------------------------	*/
/* src_hydrology/output_flow_daily_values.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "hydrology_library.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEFAULT_AGGREGATE_LEVEL	"monthly"
#define SELECT_LIST		"station,datatype,measurement_date,avg( measurement_value )"
#define GROUP_BY		"station,datatype,measurement_date"

#define FILENAME_STEM		"flow_daily_values"

/* Prototypes */
/* ---------- */
char *get_heading(	char *application_name,
			char *datatype,
			char *units_converted );

void output_table(
	 		char *application_name, 
			char *datatype,
	 		char *end_date,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process );

void output_text_file(
	 		char *document_root_directory,
	 		char *application_name, 
			char *station,
			char *datatype,
	 		char *begin_date,
	 		char *end_date,
	 		char *session,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process );

void output_spreadsheet(
	 		char *appaserver_mount_point,
	 		char *application_name, 
			char *station,
			char *datatype,
	 		char *begin_date,
	 		char *end_date,
	 		char *session,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process );

char *get_heading_sys_string(
			char *application_name,
			char *datatype,
			char *output_filename,
			char *units_converted );

char *get_text_file_sys_string(
			char *table_name,
			char *where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date,
			char *units_converted_process );

char *get_sys_string(
			char *table_name,
			char *where_clause,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date,
			char *units_converted_process );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_begin_date, *parameter_end_date;
	char *begin_date, *end_date;
	char *station;
	char *datatype;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *session;
	char *process_name;
	char where_clause[ 4096 ];
	char buffer[ 128 ];
	char date_where_clause[ 128 ];
	char *table_name;
	char *sort_process;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *units_converted;
	char *output_medium;
	char units_converted_process[ 1024 ];
	char *units;
	boolean bar_chart = 0;

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application session process station datatype begin_date end_date aggregate_level units_converted output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	process_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	parameter_begin_date = argv[ 6 ];
	parameter_end_date = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];
	units_converted = argv[ 9 ];
	output_medium = argv[ 10 ];

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

	if ( !*aggregate_level_string
	||   strcmp( aggregate_level_string, "aggregate_level" ) == 0 )
	{
		aggregate_level_string = DEFAULT_AGGREGATE_LEVEL;
	}

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				"flow" /* datatype */ );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s 3 ',' 0",
			 units,
			 units_converted );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	if ( aggregate_level == daily )
	{
		sort_process = "sort -f";
	}
	else
	{
		sort_process = "sort -f -r";
	}

	if ( strcmp( station, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: please choose a station/datatype.\n" );
		document_close();
		exit( 0 );
	}

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

	printf( "<h1>%s<br></h1>\n",
		format_initial_capital( buffer, process_name ) );
	printf( "<h2>Aggregate %s \n", aggregate_level_string );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	printf( "<table>\n" );

	begin_date = parameter_begin_date;
	end_date = parameter_end_date;

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
		printf(
		"<p>Warning: no data available for %s/%s.\n",
			station,
			datatype );
		document_close();
		exit( 0 );
	}

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf(date_where_clause,
			"measurement_date between '%s' and '%s'	",
			begin_date,
			end_date );
	}
	else
	{
		sprintf(date_where_clause,
			"measurement_date = '%s'		",
			begin_date );
	}

	sprintf(where_clause,
	 	"station = '%s' and datatype = '%s' and %s",
	 	station,
	 	datatype,
	 	date_where_clause );

	table_name = get_table_name( application_name, "measurement" );

	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_text_file(
	 		appaserver_parameter_file->document_root,
	 		application_name, 
			station,
			datatype,
	 		begin_date,
	 		end_date,
	 		session,
			where_clause,
			table_name,
			sort_process,
			aggregate_level,
			units_converted,
			units_converted_process );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
	 		appaserver_parameter_file->document_root,
	 		application_name, 
			station,
			datatype,
	 		begin_date,
	 		end_date,
	 		session,
			where_clause,
			table_name,
			sort_process,
			aggregate_level,
			units_converted,
			units_converted_process );
	}
	else
	{
		hydrology_library_output_station_table(
			application_name,
			station );

		output_table(
	 		application_name, 
			datatype,
	 		end_date,
			where_clause,
			table_name,
			sort_process,
			aggregate_level,
			units_converted,
			units_converted_process );
	}

	document_close();
	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file->
					database_management_system );
	return 0;
} /* main() */

void output_spreadsheet(
	 		char *document_root_directory,
	 		char *application_name, 
			char *station,
			char *datatype,
	 		char *begin_date,
	 		char *end_date,
	 		char *session,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process )
{
	char *output_filename;
	char *ftp_filename;
	char end_date_suffix[ 128 ];
	int record_count;
	char sys_string[ 1024 ];
	FILE *output_file;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( strcmp( end_date, "end_date" ) != 0 )
		sprintf( end_date_suffix, "%s_", end_date );
	else
		*end_date_suffix = '\0';

/*
	sprintf(output_filename,
		OUTPUT_FILE_SPREADSHEET,
	 	appaserver_mount_point,
	 	application_name, 
		station,
		datatype,
	 	begin_date,
	 	end_date_suffix,
	 	session );
*/

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date_suffix;

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
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	hydrology_library_output_station_text_filename(
		output_filename,
		application_name,
		station,
		1 /* with_zap_file */ );

	strcpy( sys_string,
		get_heading_sys_string(
			application_name,
			datatype,
			output_filename,
			units_converted ) );

	system( sys_string );

	strcpy( sys_string,
		get_text_file_sys_string(
			table_name,
			where_clause,
			output_filename,
			sort_process,
			aggregate_level,
			end_date,
			units_converted_process ) );

	system( sys_string );

	sprintf( sys_string,
	 	"cat %s | grep -v '^#' | grep -v '^$' | wc -l",
	 	output_filename );
	record_count = atoi( pipe2string( sys_string ) ) - 1;

	printf( "<tr><td>For %s/%s generated %d measurements:\n",
		station,
		datatype,
		record_count );
		
	printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
		ftp_filename,
		TRANSMIT_PROMPT );

	printf( "</table>\n" );

} /* output_spreadsheet */

void output_text_file(
	 		char *document_root_directory,
	 		char *application_name, 
			char *station,
			char *datatype,
	 		char *begin_date,
	 		char *end_date,
	 		char *session,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process )
{
	char *output_filename;
	char *ftp_filename;
	char end_date_suffix[ 128 ];
	int record_count = 0;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char *heading;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( strcmp( end_date, "end_date" ) != 0 )
		sprintf( end_date_suffix, "%s_", end_date );
	else
		*end_date_suffix = '\0';

/*
	sprintf(output_filename,
		OUTPUT_FILE_TEXT_FILE,
	 	appaserver_mount_point,
	 	application_name, 
		station,
		datatype,
	 	begin_date,
	 	end_date_suffix,
	 	session );
*/

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			"txt" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date_suffix;

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

	if ( ! ( output_pipe = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_pipe );
	}

	hydrology_library_output_station_text_filename(
		output_filename,
		application_name,
		station,
		1 /* with_zap_file */ );

	strcpy( sys_string,
		get_sys_string(
			table_name,
			where_clause,
			sort_process,
			aggregate_level,
			end_date,
			units_converted_process ) );

	input_pipe = popen( sys_string, "r" );

/*
	sprintf( sys_string,
		 "delimiter2padded_columns.e ',' 2 >> %s",
		 output_filename );
*/
	sprintf(sys_string,
	 	"tr ',' '%c' >> %s",
		OUTPUT_TEXT_FILE_DELIMITER,
	 	output_filename );

	output_pipe = popen( sys_string, "w" );

	heading = get_heading(	application_name,
				datatype,
				units_converted );

	fprintf( output_pipe, "#%s\n", heading );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
		record_count++;
	}

	pclose( input_pipe );
	pclose( output_pipe );

	printf( "<tr><td>For %s/%s generated %d measurements:\n",
		station,
		datatype,
		record_count );
		
	printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
		ftp_filename,
		TRANSMIT_PROMPT );

	printf( "</table>\n" );

} /* output_text_file */

void output_table(
	 		char *application_name, 
			char *datatype,
	 		char *end_date,
			char *where_clause,
			char *table_name,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *units_converted,
			char *units_converted_process )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *output_pipe;
	char *heading;
	FILE *input_pipe;
	char *justification;

	heading = get_heading(	application_name,
				datatype,
				units_converted );

	justification = "left,left,left,right,right";

	strcpy( sys_string, 
		get_sys_string(
			table_name,
			where_clause,
			sort_process,
			aggregate_level,
			end_date,
			units_converted_process ) );

	input_pipe = popen( sys_string, "r" );

	sprintf( sys_string,
		 "html_table.e '' '%s' ',' '%s'",
		 heading,
		 justification );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
		fprintf( output_pipe, "%s\n", input_buffer );

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_table */

char *get_text_file_sys_string(
			char *table_name,
			char *where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date,
			char *units_converted_process )
{
	static char text_file_sys_string[ 1024 ];
	char *sys_string;

	sys_string = get_sys_string(
			table_name,
			where_clause,
			sort_process,
			aggregate_level,
			end_date,
			units_converted_process );

	sprintf( text_file_sys_string,
		 "%s >> %s",
		 sys_string,
		 output_filename );

	return text_file_sys_string;
} /* get_text_file_sys_string() */

char *get_heading_sys_string(
				char *application_name,
				char *datatype,
				char *output_filename,
				char *units_converted )
{
	static char sys_string[ 1024 ];
	char *heading;

	heading = get_heading(	application_name,
				datatype,
				units_converted );

	sprintf(sys_string,
	 	"echo \"%s\" >> %s",
		heading,
	 	output_filename );

	return sys_string;

} /* get_heading_sys_string() */

char *get_heading(	char *application_name,
			char *datatype,
			char *units_converted )
{
	char units_heading[ 128 ];
	static char heading[ 256 ];

	if (	units_converted
	&& 	*units_converted
	&&	strcmp( units_converted, "units_converted" ) != 0 )
	{
		strcpy( units_heading, units_converted );
	}
	else
	{
		boolean bar_graph;

		strcpy( units_heading,
			hydrology_library_get_units_string(
				&bar_graph,
				application_name,
				datatype ) );
	}

	sprintf(heading,
	 	"station,datatype,date,%s(sum),count",
		units_heading );

	return heading;

} /* get_heading() */

char *get_sys_string(
			char *table_name,
			char *where_clause,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date,
			char *units_converted_process )
{
	static char sys_string[ 1024 ];
	char aggregation_process[ 256 ];

	sprintf( aggregation_process, 
		 "real_time2aggregate_value.e sum %d %d %d ',' %s n %s",
			 2 /* date_piece */,
			 -1 /* time_piece */,
			 3 /* value_piece */,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );

	sprintf(sys_string,
	 	"echo \"select %s from %s where %s group by %s;\"	|"
		"sql_quick.e ','					|"
		"%s							|"
		"%s							|"
		"%s							|"
		"sort -f						 ",
		SELECT_LIST,
		table_name,
		where_clause,
		GROUP_BY,
		units_converted_process,
		sort_process,
		aggregation_process );

	return sys_string;
} /* get_sys_string() */

