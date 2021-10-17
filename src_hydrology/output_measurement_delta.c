/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_delta.c	*/
/* ---------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

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
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "google_chart.h"
#include "datatype.h"
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define QUEUE_LINES			100

#define SELECT_LIST			\
"station,datatype,measurement_date,measurement_time,measurement_value"

#define DATATYPE_PIECE			0
#define DATE_TIME_PIECE			1
#define VALUE_PIECE			2
#define DELTA_VALUE_PIECE		4
#define DELIMITER			','
#define STATION_DATATYPE_DELIMITER	'/'
#define DATE_TIME_DELIMITER		':' 
#define FILENAME_STEM			"measurement_delta"

/* Prototypes */
/* ---------- */
boolean populate_datatype_chart_list_data(
				LIST *datatype_chart_list,
				LIST *station_list,
				LIST *datatype_list,
				char *application_name,
				char *begin_date,
				char *end_date,
				double delta_threshold );

char *get_google_station_datatype_name(
				char *station,
				char *datatype );

LIST *get_datatype_chart_list(	char *application_name,
				LIST *station_list,
				LIST *datatype_list );

GOOGLE_CHART *get_google_datatype_chart(
				char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold );

boolean populate_input_chart_list_data(
				LIST *input_chart_list,
				LIST *station_list,
				LIST *datatype_list,
				char *application_name,
				char *begin_date,
				char *end_date,
				double delta_threshold );

void populate_input_chart_list_datatypes(
				LIST *input_chart_list,
				LIST *station_list,
				LIST *datatype_list,
				char *application_name );

void output_google_chart(	char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name,
				double delta_threshold );

void output_table(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold );

void output_spreadsheet(	char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				double delta_threshold );

void output_text_file(		char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				double delta_threshold );

void output_stdout(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold );

char *get_where_clause(		char *begin_date,
				char *end_date,
				char *station,
				char *datatype );

char *get_heading_string(
			char *application_name,
			char *datatype );

char *get_heading_sys_string(
			char *output_filename,
			char *application_name,
			char *datatype );

char *get_sys_string(	char *output_filename,
			char *table_name,
			char *where_clause,
			double delta_threshold,
			boolean order_by_delta_value );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_begin_date, *parameter_end_date;
	char *station_list_string;
	char *datatype_list_string;
	LIST *station_list;
	LIST *datatype_list;
	char *session;
	char *process_name;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *output_medium;
	char *delta_threshold_string;

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application session process station datatype begin_date end_date delta_threshold output_medium\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: output_medium = {table,spreadsheet,text_file,stdout}\n" );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	process_name = argv[ 3 ];
	station_list_string = argv[ 4 ];
	datatype_list_string = argv[ 5 ];
	parameter_begin_date = argv[ 6 ];
	parameter_end_date = argv[ 7 ];
	delta_threshold_string = argv[ 8 ];
	output_medium = argv[ 9 ];

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

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( station_list_string, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: please choose a station/datatype.\n" );
		document_close();
		exit( 0 );
	}

	station_list = list_string2list( station_list_string, ',' );
	datatype_list = list_string2list( datatype_list_string, ',' );

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   list_length( station_list ) != list_length( datatype_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: station and datatype list don't match.\n" );
		document_close();
		exit( 0 );
	}

	hydrology_library_get_clean_begin_end_date(
					&parameter_begin_date,
					&parameter_end_date,
					application_name,
					(char *)0 /* station_name */,
					(char *)0 /* datatype_name */ );

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   !appaserver_library_validate_begin_end_date(
					&parameter_begin_date,
					&parameter_end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
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
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
		
		document_output_body(
					document->application_name,
					document->onload_control_string );
	
		printf( "<h1>%s<br></h1>\n",
			format_initial_capital( buffer, process_name ) );
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
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
				application_name,
				session,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					document_root,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_text_file(
				application_name,
				session,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					document_root,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		output_stdout(
				application_name,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "googlechart" ) == 0 )
	{
		output_google_chart(
				application_name,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					document_root,
				process_name,
				atof( delta_threshold_string ) );
	}
	else
	{
		printf( "<h3>Error: invalid output medium.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
		document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file->
					database_management_system );
	return 0;

} /* main() */

void output_stdout(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *where_clause;
	char sys_string[ 1024 ];
	char *table_name;
	char *heading;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];

	if ( !list_rewind( station_list ) ) return;

	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		hydrology_library_output_station_text_file(
			stdout,
			application_name,
			station );

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e ',' 2" );
*/
		sprintf(sys_string,
		 	"tr ',' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		heading = get_heading_string(
				application_name,
				datatype );

		search_replace_character( heading, ' ', '_' );

		fprintf( output_pipe, "#%s\n", heading );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
			fprintf( output_pipe, "%s\n", input_buffer );

		pclose( input_pipe );
		pclose( output_pipe );

		list_next( datatype_list );
	} while( list_next( station_list ) );

} /* output_stdout() */

void output_text_file(		char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *output_filename;
	char *ftp_filename;
	char end_date_suffix[ 128 ];
	char *where_clause;
	int record_count;
	char sys_string[ 1024 ];
	char *table_name;
	char *heading;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char date_station_datatype_string[ 256 ];
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"txt" );

	printf( "<table>\n" );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( end_date, "end_date" ) != 0 )
			sprintf( end_date_suffix, "_%s", end_date );
		else
			*end_date_suffix = '\0';

		sprintf(date_station_datatype_string,
			"%s_%s_%s%s",
			station,
			datatype,
			begin_date,
			end_date_suffix );

		appaserver_link->begin_date_string =
			date_station_datatype_string;

		output_filename =
			appaserver_link_output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		ftp_filename =
			appaserver_link_prompt_filename(
				appaserver_link_prompt_link_half(
					appaserver_link->prepend_http,
					appaserver_link->http_prefix,
					appaserver_link->server_address ),
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension );

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

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

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

		heading = get_heading_string(
				application_name,
				datatype );

		fprintf( output_pipe, "#%s\n", heading );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
			fprintf( output_pipe, "%s\n", input_buffer );

		pclose( input_pipe );
		pclose( output_pipe );

		sprintf( sys_string,
		 	"cat %s | grep -v '^#' | grep -v '^$' | wc -l",
		 	output_filename );
		record_count = atoi( pipe2string( sys_string ) );

		printf( "<tr><td>For %s/%s generated %d differences:\n",
			station,
			datatype,
			record_count );
		
		printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
			ftp_filename,
			TRANSMIT_PROMPT );

		list_next( datatype_list );
	} while( list_next( station_list ) );

	printf( "</table>\n" );

} /* output_text_file() */

void output_spreadsheet(	char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *output_filename;
	FILE *output_file;
	char *ftp_filename;
	char end_date_suffix[ 128 ];
	char *where_clause;
	int record_count;
	char sys_string[ 1024 ];
	char *table_name;
	char date_station_datatype_string[ 256 ];
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	printf( "<table>\n" );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( end_date, "end_date" ) != 0 )
			sprintf( end_date_suffix, "_%s", end_date );
		else
			*end_date_suffix = '\0';

		sprintf(date_station_datatype_string,
			"%s_%s_%s%s",
			station,
			datatype,
			begin_date,
			end_date_suffix );

		appaserver_link->begin_date_string =
			date_station_datatype_string;

		output_filename =
			appaserver_link_output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		ftp_filename =
			appaserver_link_prompt_filename(
				appaserver_link_prompt_link_half(
					appaserver_link->prepend_http,
					appaserver_link->http_prefix,
					appaserver_link->server_address ),
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension );

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

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		strcpy( sys_string,
			get_heading_sys_string(
				output_filename,
				application_name,
				datatype ) );

		system( sys_string );

		strcpy( sys_string,
			get_sys_string(
				output_filename,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		system( sys_string );

		sprintf( sys_string,
		 	"cat %s | grep -v '^#' | grep -v '^$' | wc -l",
		 	output_filename );
		record_count = atoi( pipe2string( sys_string ) );

		printf( "<tr><td>For %s/%s generated %d differences:\n",
			station,
			datatype,
			record_count );
		
		printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
			ftp_filename,
			TRANSMIT_PROMPT );

		list_next( datatype_list );

	} while( list_next( station_list ) );

	printf( "</table>\n" );

} /* output_spreadsheet() */

char *get_sys_string(	char *output_filename,
			char *table_name,
			char *where_clause,
			double delta_threshold,
			boolean order_by_delta_value )
{
	static char sys_string[ 1024 ];
	char output_clause[ 256 ];
	char *additional_sort;

	if ( order_by_delta_value )
		additional_sort = "sort -t',' -n -k5";
	else
		additional_sort = "cat";

	if ( output_filename )
	{
		sprintf( output_clause,
			 "cat >> %s",
			 output_filename );
	}
	else
	{
		strcpy( output_clause, "cat" );
	}

	sprintf(sys_string,
	 	"echo \"select %s from %s where %s order by %s;\"	|"
		"sql.e '%c'						|"
		"delta_values.e %d previous '%c'			|"
		"filter_anomaly.e '%c' %d %.3lf				|"
		"%s							|"
		"%s							 ",
		SELECT_LIST,
		table_name,
		where_clause,
		SELECT_LIST,
		DELIMITER,
		DELTA_VALUE_PIECE,
		DELIMITER,
		DELIMITER,
		DELTA_VALUE_PIECE,
		delta_threshold,
		additional_sort,
		output_clause );

	return sys_string;

} /* get_sys_string() */

char *get_where_clause(		char *begin_date,
				char *end_date,
				char *station,
				char *datatype )
{
	static char where_clause[ 1024 ];
	char date_where_clause[ 128 ];

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf(date_where_clause,
			"measurement_date between '%s' and '%s'",
			begin_date,
			end_date );
	}
	else
	{
		sprintf(date_where_clause,
			"measurement_date = '%s'",
			begin_date );
	}

	sprintf(where_clause,
	 	"station = '%s' and datatype = '%s' and %s",
	 	station,
	 	datatype,
	 	date_where_clause );

	return where_clause;

} /* get_where_clause() */

char *get_heading_string(	char *application_name,
				char *datatype )
{
	static char heading[ 1024 ];
	boolean bar_graph;

	sprintf(heading,
	 	"station,datatype,date,time,delta value (%s)",
		datatype_get_units_string(
				&bar_graph,
				application_name,
				datatype ) );
	return heading;
} /* get_heading_string() */

char *get_heading_sys_string(	char *output_filename,
				char *application_name,
				char *datatype )
{
	static char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"echo \"#%s\" >> %s",
		get_heading_string( application_name, datatype ),
	 	output_filename );
	return sys_string;
} /* get_heading_sys_string() */

void output_google_chart(	char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name,
				double delta_threshold )
{
	GOOGLE_CHART *google_chart;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char title[ 256 ];
	char sub_title[ 256 ];

	appaserver_link_get_pid_filename(
		&chart_filename,
		&prompt_filename,
		application_name,
		document_root_directory,
		getpid(),
		process_name /* filename_stem */,
		"html" /* extension */ );

	chart_file = fopen( chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			chart_filename );
		exit( 1 );
	}

	if ( ! ( google_chart =
			get_google_datatype_chart(
				application_name,
				station_list,
				datatype_list,
				begin_date,
				end_date,
				delta_threshold ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot get google_chart.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		fclose( chart_file );
		return;
	}

	hydrology_library_get_title(
		title,
		sub_title,
		either /* validation_level */,
		aggregate_statistic_none,
		delta /* aggregate_level */,
		(char *)0 /* station */,
		(char *)0 /* datatype */,
		begin_date,
		end_date,
		'n' /* accumulate_yn */ );

	sprintf( title + strlen( title ),
		 " %s",
		 sub_title );

	google_chart->title = title;

	google_chart->output_chart_list =
		google_chart_datatype_get_output_chart_list(
			google_chart->datatype_chart_list,
			GOOGLE_CHART_WIDTH,
			GOOGLE_CHART_HEIGHT );

	google_chart_output_all_charts(
			chart_file,
			google_chart->output_chart_list,
			google_chart->title,
			google_chart->sub_title,
			google_chart->stylesheet );

	fclose( chart_file );

	google_chart_output_graph_window(
				application_name,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				process_name,
				prompt_filename,
				(char *)0 /* where_clause */ );

} /* output_google_chart() */

GOOGLE_CHART *get_google_datatype_chart(
				char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold )
{
	GOOGLE_CHART *google_chart;

	google_chart = google_chart_new();

	if ( ! ( google_chart->datatype_chart_list =
			get_datatype_chart_list(
				application_name,
				station_list,
				datatype_list ) ) )
	{
		printf(
		"<h2>Warning: no datatypes to display.</h2>\n" );
		return (GOOGLE_CHART *)0;
	}

	if ( !populate_datatype_chart_list_data(
			google_chart->datatype_chart_list,
			station_list,
			datatype_list,
			application_name,
			begin_date,
			end_date,
			delta_threshold ) )
	{
		printf(
		"<h2>Warning: nothing was selected to display.</h2>\n" );
		return (GOOGLE_CHART *)0;
	}

	return google_chart;

} /* get_google_datatype_chart() */

LIST *get_datatype_chart_list(
			char *application_name,
			LIST *station_list,
			LIST *datatype_list )
{
	char yaxis_label[ 128 ];
	GOOGLE_DATATYPE_CHART *datatype_chart;
	char *google_station_datatype_name;
	char *station;
	char *datatype;
	boolean bar_graph;
	LIST *datatype_chart_list;

	if ( list_length( station_list ) != list_length( datatype_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: station_list of %d <> datatype_list of %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( station_list ),
			 list_length( datatype_list ) );
		return (LIST *)0;
	}

	if ( !list_rewind( station_list ) ) return (LIST *)0;

	list_rewind( datatype_list );

	datatype_chart_list = list_new();

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		google_station_datatype_name =
			get_google_station_datatype_name(
				station,
				datatype );

		datatype_chart =
			google_datatype_chart_new(
				google_station_datatype_name );

		sprintf(yaxis_label,
			"%s (%s)",
			google_station_datatype_name,
			datatype_get_units_string(
				&bar_graph,
				application_name,
				datatype ) );
	
		datatype_chart->yaxis_label = strdup( yaxis_label );
	
		list_append_pointer(	datatype_chart_list,
					datatype_chart );

		list_next( datatype_list );

	} while( list_next( station_list ) );

	return datatype_chart_list;

} /* get_datatype_chart_list() */

boolean populate_datatype_chart_list_data(
			LIST *datatype_chart_list,
			LIST *station_list,
			LIST *datatype_list,
			char *application_name,
			char *begin_date,
			char *end_date,
			double delta_threshold )
{
	boolean got_input = 0;
	char sys_string[ 1024 ];
	char *table_name;
	char *station;
	char *datatype;
	GOOGLE_DATATYPE_CHART *datatype_chart;
	char *where_clause;

	if ( list_length( station_list ) != list_length( datatype_list )
	&&   list_length( station_list ) != list_length( datatype_chart_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: list lengths do not match.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}


	list_rewind( station_list );
	list_rewind( datatype_list );
	list_rewind( datatype_chart_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );
		datatype_chart = list_get_pointer( datatype_chart_list );

		where_clause =
			get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		sprintf( sys_string + strlen( sys_string ),
			 " | sed 's|%c|%c|1' | sed 's|%c|%c|2'",
			 DELIMITER,
			 STATION_DATATYPE_DELIMITER,
			 DELIMITER,
			 DATE_TIME_DELIMITER );

		datatype_chart->input_value_list = list_new();

		if ( google_datatype_chart_input_value_list_set(
				datatype_chart->input_value_list,
				sys_string,
				DATE_TIME_PIECE,
				-1 /* time_piece */,
				VALUE_PIECE,
				DELIMITER ) )
		{
			got_input = 1;
		}

		list_next( datatype_list );
		list_next( datatype_chart_list );

	} while( list_next( station_list ) );

	return got_input;

} /* populate_datatype_chart_list_data() */

void output_table(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *where_clause;
	char sys_string[ 1024 ];
	char *table_name;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *heading;
	char *justify;
	char title[ 256 ];
	LIST *done_station_name_list = list_new();

	if ( list_rewind( station_list ) )
	{
		do {
			station = list_get_pointer( station_list );

			if ( !list_exists_string(
				station,
				done_station_name_list ) )
			{
				hydrology_library_output_station_table(
					application_name,
					station );

				list_append_pointer(	done_station_name_list,
							station );
			}
		} while( list_next( station_list ) );
	}

	heading = "station,datatype,date,time,delta_value";
	justify = "left,left,left,left,right";

	printf(
"<p>Only the top/bottom %d records are displayed in delta value order.\n",
		QUEUE_LINES );
	fflush( stdout );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		sprintf( title,
			 "%s/%s",
			 station,
			 datatype );

		sprintf( sys_string,
			 "queue_top_bottom_lines.e %d			|"
			 "html_table.e '^^%s' '%s' '%c' '%s' 		|"
			 "cat						 ",
			 QUEUE_LINES,
			 title,
			 heading,
			 DELIMITER,
			 justify );

		output_pipe = popen( sys_string, "w" );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				1 /* order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

		list_next( datatype_list );
	} while( list_next( station_list ) );

} /* output_table() */

char *get_google_station_datatype_name(
				char *station,
				char *datatype )
{
	char station_datatype_name[ 128 ];

	sprintf( station_datatype_name,
		 "%s%c%s",
		 station,
		 STATION_DATATYPE_DELIMITER,
		 datatype );

	return strdup( station_datatype_name );

} /* get_google_station_datatype_name() */
