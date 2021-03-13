/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/water_budget_flow.c  	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "easycharts.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "dictionary_appaserver.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define REPORT_TITLE				"Water Budget Flow"
#define DEFAULT_OUTPUT_MEDIUM			"table"
#define KEY_DELIMITER				'/'
#define FILENAME_STEM				"water_budger_flow"

#define ROWS_BETWEEN_HEADING			20
#define SELECT_LIST				 "measurement_date,measurement_time,measurement_value"

#define GRACE_TICKLABEL_ANGLE			90
#define DATE_PIECE		 		0
#define TIME_PIECE		 		1
#define VALUE_PIECE		 		2
#define COUNT_PIECE		 		3
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */
typedef struct
{
	double value;
	boolean is_null;
	int count;
} WATER_BUDGET_FLOW_MEASUREMENT;

typedef struct
{
	char *date_colon_time;
	LIST *inflow_measurement_list;
	LIST *outflow_measurement_list;
	double inflow_sum;
	double outflow_sum;
	double budget;
	double cumulative;
} WATER_BUDGET_INFLOW_OUTFLOW;

typedef struct
{
	HASH_TABLE *inflow_outflow_hash_table;
	LIST *inflow_station_name_list;
	LIST *inflow_datatype_name_list;
	LIST *outflow_station_name_list;
	LIST *outflow_datatype_name_list;
} WATER_BUDGET_FLOW;

	
/* Prototypes */
/* ---------- */
void get_first_inflow_names(	char **first_inflow_station_name,
				char **first_inflow_datatype_name,
				DICTIONARY *parameter_dictionary );

boolean water_budget_flow_populate_easycharts_input_chart_list_data(
				LIST *input_chart_list,
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level);

void water_budget_flow_populate_easycharts_input_chart_list_datatypes(
				LIST *input_chart_list );

void get_report_title(		char *title,
				enum aggregate_level,
				char *flow_units,
				char *flow_units_converted );

void water_budget_flow_output_transmit_measurement_list(
				FILE *output_pipe,
				LIST *measurement_list,
				enum aggregate_level aggregate_level,
				boolean display_count );

void water_budget_flow_output_transmit_heading_list(
				FILE *output_pipe,
				LIST *station_name_list,
				enum aggregate_level aggregate_level,
				boolean display_count );

void water_budget_flow_append_table_measurement_list(
				LIST *data_list,
				LIST *measurement_list,
				enum aggregate_level aggregate_level,
				boolean display_count );

void water_budget_flow_append_table_heading_list(
				LIST *heading_list,
				LIST *station_name_list,
				enum aggregate_level aggregate_level,
				boolean display_count );

void water_budget_flow_pad_missing_data(
				HASH_TABLE *inflow_outflow_hash_table,
				int station_name_list_length,
				boolean is_inflow );

double water_budget_flow_get_flow_sum(
				LIST *measurement_list );

void water_budget_flow_populate_budget_and_cumulative(
				HASH_TABLE *inflow_outflow_hash_table );

WATER_BUDGET_INFLOW_OUTFLOW *water_budget_flow_hash_table_get_inflow_outflow(
				HASH_TABLE *inflow_outflow_hash_table,
				char *date_colon_time );

void with_sys_string_populate_inflow_outfile_hash_table(
				HASH_TABLE *inflow_outflow_hash_table,
				char *sys_string,
				int is_inflow,
				int list_offset );

WATER_BUDGET_INFLOW_OUTFLOW *water_budget_flow_inflow_outflow_new(
					char *date_colon_time );

WATER_BUDGET_FLOW *water_budget_flow_new(
					char *application_name,
					char *begin_date,
					char *end_date,
					enum aggregate_level aggregate_level,
					DICTIONARY *parameter_dictionary,
					char *flow_units,
					char *flow_units_converted );

HASH_TABLE *get_merged_date_colon_time_key_hash_table(
					LIST *station_datatype_list );

boolean populate_inflow_outflow_hash_table(
					HASH_TABLE *inflow_outflow_hash_table,
					char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_name_list,
					LIST *datatype_name_list,
					enum aggregate_level,
					boolean is_inflow,
					char *flow_units,
					char *flow_units_converted );

WATER_BUDGET_FLOW_MEASUREMENT *new_measurement(
					void );

boolean water_budget_flow_output_googlechart(
				char *application_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *process_name,
				char *flow_units,
				char *flow_units_converted );

boolean water_budget_flow_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *argv_0,
				char *flow_units,
				char *flow_units_converted,
				LIST *inflow_station_name_list,
				LIST *outflow_station_name_list );

void water_budget_flow_output_table(
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level aggregate_level,
				LIST *inflow_station_name_list,
				LIST *outflow_station_name_list,
				boolean display_count,
				char *flow_units,
				char *flow_units_converted,
				char *application_name );

void water_budget_flow_output_transmit(
					FILE *output_pipe,
					HASH_TABLE *inflow_outflow_hash_table,
					enum aggregate_level aggregate_level,
					LIST *inflow_station_name_list,
					LIST *outflow_station_name_list,
					boolean display_count );

void build_sys_string(
					char *sys_string,
					char *application_name,
					char *begin_date,
					char *end_date,
					char *station_name,
					char *datatype_name,
					enum aggregate_level aggregate_level,
					char *flow_units,
					char *flow_units_converted );

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	char *parameter_dictionary_string;
	WATER_BUDGET_FLOW *water_budget_flow;
	char *station_name = {0};
	char *begin_date, *end_date;
	char *output_medium = {0};
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *process_name;
	char *display_count_yn = {0};
	char *flow_units;
	char *flow_units_converted;
	int right_justified_columns_from_right;
	char *first_inflow_station_name = {0};
	char *first_inflow_datatype_name = {0};
	char dictionary_buffer[ 65536 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s ignored role parameter_dictionary|-\n",
			argv[ 0 ] );
		fprintf(stderr,
			"Use dictionary placeholder of '-' for stdin.\n" );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	role_name = argv[ 2 ];
	parameter_dictionary_string = argv[ 3 ];

	if ( strcmp( parameter_dictionary_string, "-" ) == 0 )
	{
		timlib_get_line( dictionary_buffer, stdin, 65536 );
		parameter_dictionary_string = dictionary_buffer;
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	dictionary_appaserver_parse_multi_attribute_keys(
		parameter_dictionary,
		(char *)0 /* prefix */ );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_FROM_STARTING_LABEL );

	dictionary_get_index_data( 	&begin_date, 
					parameter_dictionary, 
					"begin_date", 
					0 );

	dictionary_get_index_data( 	&end_date, 
					parameter_dictionary, 
					"end_date", 
					0 );

	dictionary_get_index_data( 	&station_name,
					parameter_dictionary, 
					"station", 
					0 );

	dictionary_get_index_data( 	&process_name,
					parameter_dictionary, 
					"process", 
					0 );

	dictionary_get_index_data( 	&aggregate_level_string,
					parameter_dictionary,
					"aggregate_level",
					0 );

	aggregate_level = 
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	dictionary_get_index_data( 	&output_medium, 
					parameter_dictionary, 
					"output_medium", 
					0 );

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	dictionary_get_index_data( 	&display_count_yn,
					parameter_dictionary,
					"display_count_yn",
					0 );

	dictionary_get_index_data( 	&flow_units_converted,
					parameter_dictionary,
					"flow_units_converted",
					0 );

	flow_units = hydrology_library_get_flow_units( application_name );

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
	}

	get_first_inflow_names(	&first_inflow_station_name,
				&first_inflow_datatype_name,
				parameter_dictionary );

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					first_inflow_station_name,
					first_inflow_datatype_name );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	water_budget_flow = water_budget_flow_new(
				application_name,
				begin_date,
				end_date,
				aggregate_level,
				parameter_dictionary,
				flow_units,
				flow_units_converted );

	water_budget_flow_populate_budget_and_cumulative(
		water_budget_flow->inflow_outflow_hash_table );

	water_budget_flow_pad_missing_data(
		water_budget_flow->inflow_outflow_hash_table,
		list_length( water_budget_flow->inflow_station_name_list ),
		1 /* is_inflow */ );

	water_budget_flow_pad_missing_data(
		water_budget_flow->inflow_outflow_hash_table,
		list_length( water_budget_flow->outflow_station_name_list ),
		0 /* not is_inflow */ );

	right_justified_columns_from_right =
		list_length( water_budget_flow->inflow_station_name_list ) +
		list_length( water_budget_flow->outflow_station_name_list ) +
		4;

	if ( *display_count_yn == 'y' )
	{
		right_justified_columns_from_right +=
			( list_length( water_budget_flow->
					inflow_station_name_list ) +
			  list_length( water_budget_flow->
					outflow_station_name_list ) );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		water_budget_flow_output_table(
				water_budget_flow->inflow_outflow_hash_table,
				aggregate_level,
				water_budget_flow->inflow_station_name_list,
				water_budget_flow->outflow_station_name_list,
				*display_count_yn == 'y',
				flow_units,
				flow_units_converted,
				application_name );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *output_pipename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" );

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

		if ( list_rewind( water_budget_flow->
					inflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						inflow_station_name_list );

				hydrology_library_output_station_text_filename(
					output_pipename,
					application_name,
					station_name,
					0 /* not with_zap_file */ );

			} while( list_next( water_budget_flow->
						inflow_station_name_list ) );
		}

		if ( list_rewind( water_budget_flow->
					outflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						outflow_station_name_list );

				hydrology_library_output_station_text_filename(
					output_pipename,
					application_name,
					station_name,
					0 /* not with_zap_file */ );

			} while( list_next( water_budget_flow->
						outflow_station_name_list ) );
		}

		sprintf( sys_string,
			 "tr '|' ',' >> %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		water_budget_flow_output_transmit(
					output_pipe,
					water_budget_flow->
						inflow_outflow_hash_table,
					aggregate_level,
					water_budget_flow->
						inflow_station_name_list,
					water_budget_flow->
						outflow_station_name_list,
					*display_count_yn == 'y' );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", REPORT_TITLE );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "transmit" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		char *output_pipename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" );

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

		if ( list_rewind( water_budget_flow->
					inflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						inflow_station_name_list );

				hydrology_library_output_station_text_filename(
					output_pipename,
					application_name,
					station_name,
					0 /* not with_zap_file */ );

			} while( list_next( water_budget_flow->
						inflow_station_name_list ) );
		}

		if ( list_rewind( water_budget_flow->
					outflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						outflow_station_name_list );

				hydrology_library_output_station_text_filename(
					output_pipename,
					application_name,
					station_name,
					0 /* not with_zap_file */ );

			} while( list_next( water_budget_flow->
						outflow_station_name_list ) );
		}

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' %d >> %s",
		 	 right_justified_columns_from_right,
			 output_pipename );
*/
		sprintf(sys_string,
		 	"tr '|' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );

		output_pipe = popen( sys_string, "w" );

		water_budget_flow_output_transmit(
					output_pipe,
					water_budget_flow->
						inflow_outflow_hash_table,
					aggregate_level,
					water_budget_flow->
						inflow_station_name_list,
					water_budget_flow->
						outflow_station_name_list,
					*display_count_yn == 'y' );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", REPORT_TITLE );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );

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

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];

		if ( list_rewind( water_budget_flow->
					inflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						inflow_station_name_list );

				hydrology_library_output_station_text_file(
					stdout,
					application_name,
					station_name );

			} while( list_next( water_budget_flow->
						inflow_station_name_list ) );
		}

		if ( list_rewind( water_budget_flow->
					outflow_station_name_list ) )
		{
			do {
				station_name =
					list_get_pointer(
						water_budget_flow->
						outflow_station_name_list );

				hydrology_library_output_station_text_file(
					stdout,
					application_name,
					station_name );

			} while( list_next( water_budget_flow->
						outflow_station_name_list ) );
		}

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' %d",
		 	 right_justified_columns_from_right );
*/
		sprintf(sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		water_budget_flow_output_transmit(
					output_pipe,
					water_budget_flow->
						inflow_outflow_hash_table,
					aggregate_level,
					water_budget_flow->
						inflow_station_name_list,
					water_budget_flow->
						outflow_station_name_list,
					*display_count_yn == 'y' );

		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "gracechart" ) == 0 )
	{
		if ( !water_budget_flow_output_gracechart(
					application_name,
					role_name,
					begin_date,
					end_date,
					water_budget_flow->
						inflow_outflow_hash_table,
					aggregate_level,
					appaserver_parameter_file->
						document_root,
					argv[ 0 ],
					flow_units,
					flow_units_converted,
					water_budget_flow->
						inflow_station_name_list,
					water_budget_flow->
						outflow_station_name_list ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( output_medium, "googlechart" ) == 0 )
	{
/*
		if ( !water_budget_flow_output_googlechart(
					application_name,
					begin_date,
					end_date,
					water_budget_flow->
						inflow_outflow_hash_table,
					aggregate_level,
					appaserver_parameter_file->
						document_root,
					process_name,
					flow_units,
					flow_units_converted ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
*/
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
}

void water_budget_flow_output_transmit(
					FILE *output_pipe,
					HASH_TABLE *inflow_outflow_hash_table,
					enum aggregate_level aggregate_level,
					LIST *inflow_station_name_list,
					LIST *outflow_station_name_list,
					boolean display_count )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;
	char key_label[ 32 ];

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			inflow_outflow_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		printf( "<h2>ERROR 1: insufficient data to output</h2>\n" );
		return;
	}

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		fprintf( output_pipe, "#Date:Time" );
	}
	else
	{
		fprintf( output_pipe, "#Date" );
	}

	fprintf( output_pipe, "|Cumulative" );
	fprintf( output_pipe, "|Budget" );
	fprintf( output_pipe, "|Inflow_Total" );

	water_budget_flow_output_transmit_heading_list(
				output_pipe,
				inflow_station_name_list,
				aggregate_level,
				display_count );

	fprintf( output_pipe, "|Outflow_Total" );

	water_budget_flow_output_transmit_heading_list(
				output_pipe,
				outflow_station_name_list,
				aggregate_level,
				display_count );

	fprintf( output_pipe, "\n" );

	list_rewind( date_colon_time_key_list );

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );


		if ( aggregate_level != real_time
		&&   aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			piece( key_label, ':', date_colon_time, 0 );
		}
		else
		{
			strcpy( key_label, date_colon_time );
		}

		fprintf( output_pipe, "%s", key_label );

		inflow_outflow =
			hash_table_get_pointer(
				inflow_outflow_hash_table,
				date_colon_time );

		fprintf(output_pipe,
			"|%.3lf",
			inflow_outflow->cumulative );

		fprintf(output_pipe,
			"|%.3lf",
			inflow_outflow->budget );

		fprintf(output_pipe,
			"|%.3lf",
			inflow_outflow->inflow_sum );

		water_budget_flow_output_transmit_measurement_list(
					output_pipe,
					inflow_outflow->
						inflow_measurement_list,
					aggregate_level,
					display_count );

		fprintf(output_pipe,
			"|%.3lf",
			inflow_outflow->outflow_sum );

		water_budget_flow_output_transmit_measurement_list(
					output_pipe,
					inflow_outflow->
						outflow_measurement_list,
					aggregate_level,
					display_count );

		fprintf( output_pipe, "\n" );
	} while( list_next( date_colon_time_key_list ) );	

}

void water_budget_flow_output_table(
					HASH_TABLE *inflow_outflow_hash_table,
					enum aggregate_level aggregate_level,
					LIST *inflow_station_name_list,
					LIST *outflow_station_name_list,
					boolean display_count,
					char *flow_units,
					char *flow_units_converted,
					char *application_name )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char buffer[ 512 ];
	register int count = 0;
	char title[ 512 ];
	char key_label[ 32 ];
	char *station_name;

	if ( list_rewind( inflow_station_name_list ) )
	{
		do {
			station_name =
				list_get_pointer(
					inflow_station_name_list );

			hydrology_library_output_station_table(
				application_name,
				station_name );

		} while( list_next( inflow_station_name_list ) );
	}

	if ( list_rewind( outflow_station_name_list ) )
	{
		do {
			station_name =
				list_get_pointer(
					outflow_station_name_list );

			hydrology_library_output_station_table(
				application_name,
				station_name );

		} while( list_next( outflow_station_name_list ) );
	}

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			inflow_outflow_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		printf( "<h2>ERROR 2: insufficient data to output</h2>\n" );
		return;
	}

	get_report_title(	title,
				aggregate_level,
				flow_units,
				flow_units_converted );

	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		list_append_pointer( heading_list, "Date:Time" );
	}
	else
	{
		list_append_pointer( heading_list, "Date" );
	}

	list_append_pointer( heading_list, "Cumulative" );
	list_append_pointer( heading_list, "Budget" );
	list_append_pointer( heading_list, "Inflow Total" );

	water_budget_flow_append_table_heading_list(
				heading_list,
				inflow_station_name_list,
				aggregate_level,
				display_count );

	list_append_pointer( heading_list, "Outflow Total" );

	water_budget_flow_append_table_heading_list(
				heading_list,
				outflow_station_name_list,
				aggregate_level,
				display_count );

	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 99;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	list_rewind( date_colon_time_key_list );

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );


		if ( aggregate_level != real_time
		&&   aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			piece( key_label, ':', date_colon_time, 0 );
		}
		else
		{
			strcpy( key_label, date_colon_time );
		}

		html_table_set_data(	html_table->data_list,
					strdup( key_label ) );

		inflow_outflow =
			hash_table_get_pointer(
				inflow_outflow_hash_table,
				date_colon_time );

		sprintf(buffer,
			"%.3lf",
			inflow_outflow->cumulative );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		sprintf(buffer,
			"%.3lf",
			inflow_outflow->budget );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		sprintf(buffer,
			"%.3lf",
			inflow_outflow->inflow_sum );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );


		water_budget_flow_append_table_measurement_list(
					html_table->data_list,
					inflow_outflow->
						inflow_measurement_list,
					aggregate_level,
					display_count );

		sprintf(buffer,
			"%.3lf",
			inflow_outflow->outflow_sum );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		water_budget_flow_append_table_measurement_list(
					html_table->data_list,
					inflow_outflow->
						outflow_measurement_list,
					aggregate_level,
					display_count );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();

	} while( list_next( date_colon_time_key_list ) );	

	html_table_close();

}

void build_sys_string(
		char *sys_string,
		char *application_name,
		char *begin_date,
		char *end_date,
		char *station_name,
		char *datatype_name,
		enum aggregate_level aggregate_level,
		char *flow_units,
		char *flow_units_converted )
{
	char where_clause[ 1024 ];
	char intermediate_process[ 1024 ];
	char *optional_sort_descending_process = {0};
	enum aggregate_statistic aggregate_statistic;
	char round_down_hourly_process[ 128 ];
	char units_converted_process[ 256 ];

	if ( aggregate_level == hourly )
	{
		sprintf(round_down_hourly_process,
			"sed 's/50%c/00%c/'",
			PIECE_DELIMITER,
			PIECE_DELIMITER );
	}
	else
	{
		strcpy( round_down_hourly_process, "cat" );
	}

	if ( flow_units_converted
	&&   *flow_units_converted
	&&   strcmp( flow_units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s '%s' %d '%c'",
			 flow_units,
			 flow_units_converted,
			 VALUE_PIECE,
			 PIECE_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
			application_name,
			datatype_name,
			aggregate_statistic_none );

	if ( aggregate_level == real_time || aggregate_level == daily )
	{
		optional_sort_descending_process = "cat";
	}
	else
	{
		optional_sort_descending_process = "sort -r";
	}

	sprintf( where_clause,
 	"station = '%s' and 				      "
 	"datatype = '%s' and 				      "
 	"measurement_date >= '%s' and measurement_date <= '%s'",
		station_name,
		datatype_name,
		begin_date,
		end_date );

	if ( aggregate_level == real_time )
	{
		strcpy( intermediate_process, "cat" );
	}
	else
	{
		sprintf( intermediate_process, 
	 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		TIME_PIECE,
	 		VALUE_PIECE,
			PIECE_DELIMITER,
	 		aggregate_level_get_string(
				aggregate_level ),
			end_date );
	}

	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		   |"
	"tr '%c' '%c' 					   |"
	"%s	 					   |"
	"%s	 					   |"
	"%s						   |"
	"%s						   |"
	"cat						    ",
		application_name,
		SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		PIECE_DELIMITER,
		units_converted_process,
		optional_sort_descending_process,
		intermediate_process,
		round_down_hourly_process );

}

WATER_BUDGET_FLOW_MEASUREMENT *new_measurement( void )
{
	WATER_BUDGET_FLOW_MEASUREMENT *m =
		(WATER_BUDGET_FLOW_MEASUREMENT *)
			calloc( 1, sizeof( WATER_BUDGET_FLOW_MEASUREMENT ) );
	if ( !m )
	{
		fprintf( stderr,
			 "Memory allocation ERROR in %s/%s()\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	return m;
}

WATER_BUDGET_FLOW *water_budget_flow_new(
					char *application_name,
					char *begin_date,
					char *end_date,
					enum aggregate_level aggregate_level,
					DICTIONARY *parameter_dictionary,
					char *flow_units,
					char *flow_units_converted )
{
	WATER_BUDGET_FLOW *water_budget_flow;

	water_budget_flow =
		(WATER_BUDGET_FLOW *)
			calloc( 1, sizeof( WATER_BUDGET_FLOW ) );

	water_budget_flow->inflow_outflow_hash_table =
		hash_table_new( hash_table_medium );

	/* Build inflow station/datatype list */
	/* ---------------------------------- */
	water_budget_flow->inflow_station_name_list =
		dictionary_get_index_list( 	parameter_dictionary,
						"inflow_station" );

	water_budget_flow->inflow_datatype_name_list =
		dictionary_get_index_list( 	parameter_dictionary,
						"inflow_datatype" );

	if ( ! populate_inflow_outflow_hash_table(
					water_budget_flow->
						inflow_outflow_hash_table,
					application_name,
					begin_date,
					end_date,
					water_budget_flow->
						inflow_station_name_list,
					water_budget_flow->
						inflow_datatype_name_list,
					aggregate_level,
					1 /* is_inflow */,
					flow_units,
					flow_units_converted ) )
	{
/*
		document_quick_output_body(
					application_name,
					appaserver_mount_point );
*/

		printf( "<h2>ERROR: insufficient inflow input.</h2>\n" );
		document_close();
		exit( 0 );
	}

	/* Build outflow station/datatype list */
	/* ----------------------------------- */
	water_budget_flow->outflow_station_name_list =
		dictionary_get_index_list( 	parameter_dictionary,
						"outflow_station" );

	water_budget_flow->outflow_datatype_name_list =
		dictionary_get_index_list( 	parameter_dictionary,
						"outflow_datatype" );

	if ( ! populate_inflow_outflow_hash_table(
					water_budget_flow->
						inflow_outflow_hash_table,
					application_name,
					begin_date,
					end_date,
					water_budget_flow->
						outflow_station_name_list,
					water_budget_flow->
						outflow_datatype_name_list,
					aggregate_level,
					0 /* not is_inflow */,
					flow_units,
					flow_units_converted ) )
	{
/*
		document_quick_output_body(
					application_name,
					appaserver_mount_point );

		printf( "<h2>ERROR: insufficient outflow input.</h2>\n" );
		document_close();
		exit( 0 );
*/
	}

	return water_budget_flow;

}

WATER_BUDGET_INFLOW_OUTFLOW *water_budget_flow_hash_table_get_inflow_outflow(
				HASH_TABLE *inflow_outflow_hash_table,
				char *date_colon_time )
{
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;

	if ( !( inflow_outflow = hash_table_get_pointer(
					inflow_outflow_hash_table,
					date_colon_time ) ) )
	{
		inflow_outflow =
			water_budget_flow_inflow_outflow_new(
				strdup( date_colon_time ) );

		hash_table_add_pointer(
			inflow_outflow_hash_table,
			inflow_outflow->date_colon_time,
			inflow_outflow );
	}
	return inflow_outflow;
}

WATER_BUDGET_INFLOW_OUTFLOW *water_budget_flow_inflow_outflow_new(
					char *date_colon_time )
{
	WATER_BUDGET_INFLOW_OUTFLOW *water_budget_inflow_outflow;

	water_budget_inflow_outflow =
		(WATER_BUDGET_INFLOW_OUTFLOW *)
			calloc( 1, sizeof( WATER_BUDGET_INFLOW_OUTFLOW ) );

	if ( !water_budget_inflow_outflow )
	{
		fprintf( stderr,
			 "Memory allocation ERROR in %s/%s()\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	water_budget_inflow_outflow->date_colon_time = date_colon_time;
	water_budget_inflow_outflow->inflow_measurement_list = list_new_list();
	water_budget_inflow_outflow->outflow_measurement_list = list_new_list();

	return water_budget_inflow_outflow;
}

boolean populate_inflow_outflow_hash_table(
					HASH_TABLE *inflow_outflow_hash_table,
					char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_name_list,
					LIST *datatype_name_list,
					enum aggregate_level aggregate_level,
					boolean is_inflow,
					char *flow_units,
					char *flow_units_converted )
{
	char sys_string[ 1024 ];
	char *station_name, *datatype_name;
	int list_offset = 0;

	if ( !list_rewind( datatype_name_list ) )
		return 0;

	if ( !list_rewind( station_name_list ) )
		return 0;

	if (	list_length( datatype_name_list ) !=
		list_length( station_name_list ) )
	{
		return 0;
	}

	do {
		station_name = 
			list_get_pointer( station_name_list );

		datatype_name = 
			list_get_pointer( datatype_name_list );

		build_sys_string(
				sys_string,
				application_name,
				begin_date,
				end_date,
				station_name,
				datatype_name,
				aggregate_level,
				flow_units,
				flow_units_converted );

		with_sys_string_populate_inflow_outfile_hash_table(
				inflow_outflow_hash_table,
				sys_string,
				is_inflow,
				list_offset++ );

		list_next( datatype_name_list );
	} while( list_next( station_name_list ) );

	return 1;
}

void with_sys_string_populate_inflow_outfile_hash_table(
				HASH_TABLE *inflow_outflow_hash_table,
				char *sys_string,
				int is_inflow,
				int list_offset )
{
	char measurement_date[ 16 ];
	char measurement_time[ 16 ];
	char measurement_buffer[ 128 ];
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;
	WATER_BUDGET_FLOW_MEASUREMENT *measurement;
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char date_colon_time[ 32 ];
	char count_buffer[ 16 ];
	LIST *measurement_list;
	int length;

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date,
			PIECE_DELIMITER,
			input_buffer,
			DATE_PIECE );

		piece(	measurement_time,
			PIECE_DELIMITER,
			input_buffer,
			TIME_PIECE );

		sprintf( date_colon_time,
			 "%s:%s",
			 measurement_date,
			 measurement_time );

		inflow_outflow =
			water_budget_flow_hash_table_get_inflow_outflow(
				inflow_outflow_hash_table,
				date_colon_time );

		measurement = new_measurement();

		piece(	measurement_buffer,
			PIECE_DELIMITER,
			input_buffer,
			VALUE_PIECE );

		if ( *measurement_buffer
		&&   strncmp( measurement_buffer, "null", 4 ) != 0 )
		{
			measurement->value =
				atof( measurement_buffer );

		}
		else
		{
			measurement->is_null = 1;
		}

		if ( piece(	count_buffer,
				PIECE_DELIMITER,
				input_buffer,
				COUNT_PIECE ) )
		{
			measurement->count = atoi( count_buffer );
		}

		if ( is_inflow )
		{
			measurement_list =
				inflow_outflow->inflow_measurement_list;
		}
		else
		{
			measurement_list =
				inflow_outflow->outflow_measurement_list;
		}

		length = list_length( measurement_list );
		if ( length < list_offset )
		{
			WATER_BUDGET_FLOW_MEASUREMENT *null_measurement;

			while( length < list_offset )
			{
				null_measurement = new_measurement();
				null_measurement->is_null = 1;
				list_append_pointer(	measurement_list,
							null_measurement );
				length++;
			}
		}
		list_append_pointer( measurement_list, measurement );
	}

	pclose( input_pipe );

}

void water_budget_flow_populate_budget_and_cumulative(
				HASH_TABLE *inflow_outflow_hash_table )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;
	double old_cumulative = 0.0;

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			inflow_outflow_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
/*
		printf( "<h2>ERROR 3: insufficient data to output</h2>\n" );
		document_close();
		exit( 1 );
*/
		return;
	}

	list_rewind( date_colon_time_key_list );

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		inflow_outflow =
				hash_table_get_pointer(
					inflow_outflow_hash_table,
					date_colon_time );

		inflow_outflow->inflow_sum =
			water_budget_flow_get_flow_sum(
				inflow_outflow->inflow_measurement_list );

		inflow_outflow->outflow_sum =
			water_budget_flow_get_flow_sum(
				inflow_outflow->outflow_measurement_list );

		inflow_outflow->budget =
			inflow_outflow->inflow_sum -
			inflow_outflow->outflow_sum;

		old_cumulative += inflow_outflow->budget;
		inflow_outflow->cumulative += old_cumulative;

	} while( list_next( date_colon_time_key_list ) );	

	list_free_container( date_colon_time_key_list );

}

double water_budget_flow_get_flow_sum( LIST *measurement_list )
{
	WATER_BUDGET_FLOW_MEASUREMENT *measurement;
	double flow_sum = 0.0;

	if ( list_rewind( measurement_list ) )
	{
		do {
			measurement = list_get_pointer( measurement_list );
			if ( !measurement->is_null )
			{
				flow_sum += measurement->value;
			}
		} while( list_next( measurement_list ) );
	}
	return flow_sum;
}

void water_budget_flow_pad_missing_data(
		HASH_TABLE *inflow_outflow_hash_table,
		int station_name_list_length,
		boolean is_inflow )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	WATER_BUDGET_INFLOW_OUTFLOW *inflow_outflow;
	int length;
	LIST *measurement_list;

	date_colon_time_key_list =
		 hash_table_get_key_list(
			inflow_outflow_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
/*
		printf( "<h2>ERROR 4: insufficient data to output</h2>\n" );
		document_close();
		exit( 1 );
*/
		return;
	}

	list_rewind( date_colon_time_key_list );

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		inflow_outflow =
				hash_table_get_pointer(
					inflow_outflow_hash_table,
					date_colon_time );

		if ( is_inflow )
		{
			measurement_list =
				inflow_outflow->inflow_measurement_list;
		}
		else
		{
			measurement_list =
				inflow_outflow->outflow_measurement_list;
		}

		length = list_length( measurement_list );
		if ( length < station_name_list_length )
		{
			WATER_BUDGET_FLOW_MEASUREMENT *null_measurement;

			while( length < station_name_list_length )
			{
				null_measurement = new_measurement();
				null_measurement->is_null = 1;
				list_append_pointer(	measurement_list,
							null_measurement );
				length++;
			}
		}
	} while( list_next( date_colon_time_key_list ) );	

	list_free_container( date_colon_time_key_list );

}

void water_budget_flow_output_transmit_heading_list(
				FILE *output_pipe,
				LIST *station_name_list,
				enum aggregate_level aggregate_level,
				boolean display_count )
{
	char *station_name;

	if ( list_rewind( station_name_list ) )
	{
		do {
			station_name =
				list_get_pointer(
					station_name_list );
	
			fprintf(output_pipe,
				"|%s",
				station_name );
	
			if ( display_count
			&&   aggregate_level != real_time )
			{
				fprintf( output_pipe, "|Count" );
			}
		} while( list_next( station_name_list ) );
	}

}

void water_budget_flow_append_table_heading_list(
				LIST *heading_list,
				LIST *station_name_list,
				enum aggregate_level aggregate_level,
				boolean display_count )
{
	char *station_name;
	char buffer[ 128 ];

	if ( list_rewind( station_name_list ) )
	{
		do {
			station_name =
				list_get_pointer(
					station_name_list );
	
			sprintf(buffer,
				"%s",
				station_name );
	
			list_append_pointer( heading_list, strdup( buffer ) );

			if ( display_count
			&&   aggregate_level != real_time )
			{
				list_append_pointer( heading_list, "Count" );
			}
		} while( list_next( station_name_list ) );
	}

}

void water_budget_flow_output_transmit_measurement_list(
					FILE *output_pipe,
					LIST *measurement_list,
					enum aggregate_level aggregate_level,
					boolean display_count )
{
	WATER_BUDGET_FLOW_MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) )
	{
/*
		fprintf( stderr,
			 "ERROR in %s/%s(): empty measurement_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 0 );
*/
		return;
	}

	do {
		measurement =
			list_get_pointer(
				measurement_list );

		if ( measurement->is_null )
		{
			fprintf( output_pipe, "|null" );
		}
		else
		{
			fprintf(output_pipe,
				"|%.3lf",
				measurement->value );
		}

		if ( display_count
		&&   aggregate_level != real_time )
		{
			fprintf(output_pipe,
				"|%d",
				measurement->count );

		}

	} while( list_next( measurement_list ) );

}

void water_budget_flow_append_table_measurement_list(
					LIST *data_list,
					LIST *measurement_list,
					enum aggregate_level aggregate_level,
					boolean display_count )
{
	char buffer[ 128 ];
	WATER_BUDGET_FLOW_MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) )
	{
/*
		fprintf( stderr,
			 "ERROR in %s/%s(): empty measurement_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 0 );
*/
		return;
	}

	do {
		measurement =
			list_get_pointer(
				measurement_list );

		if ( measurement->is_null )
		{
			strcpy( buffer, "null" );
		}
		else
		{
			sprintf(buffer,
				"%.3lf",
				measurement->value );
		}

		html_table_set_data(	data_list,
					strdup( buffer ) );

		if ( display_count
		&&   aggregate_level != real_time )
		{
			sprintf(buffer,
				"%d",
				measurement->count );

			html_table_set_data(	data_list,
						strdup( buffer ) );
		}

	} while( list_next( measurement_list ) );

}

boolean water_budget_flow_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *argv_0,
				char *flow_units,
				char *flow_units_converted,
				LIST *inflow_station_name_list,
				LIST *outflow_station_name_list )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	char point_buffer[ 128 ];
	WATER_BUDGET_INFLOW_OUTFLOW *water_budget_inflow_outflow;
	char date_string[ 32 ];
	char time_string[ 32 ];

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			inflow_outflow_hash_table );

	if ( !list_length( date_colon_time_key_list ) ) return 0;

	get_report_title(	title,
				aggregate_level,
				flow_units,
				flow_units_converted );

	sprintf(sub_title,
		"In: %s Out: %s From %s To %s",
		list_display( inflow_station_name_list ),
		list_display( outflow_station_name_list ),
		begin_date,
		end_date );

/*
	format_initial_capital( sub_title, sub_title );
*/

	grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				0 /* grace_datatype_entity_piece */,
				1 /* grace_datatype_piece */,
				2 /* grace_date_piece */,
				3 /* grace_time_piece */,
				4 /* grace_value_piece */,
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
					begin_date,
					end_date ) )
	{
		printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	/* Create the budget graph */
	/* ----------------------- */
	grace_graph = grace_new_grace_graph();
	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;
	list_append_pointer( grace->graph_list, grace_graph );

	grace_datatype =
		grace_new_grace_datatype( "budget", "" );
	grace_datatype->legend = "Budget";
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	/* Create the cumulative graph */
	/* --------------------------- */
	grace_graph = grace_new_grace_graph();
	grace_graph_make_yaxis_opposite( grace_graph );
	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;
	list_append_pointer( grace->graph_list, grace_graph );

	grace_datatype =
		grace_new_grace_datatype( "cumulative", "" );
	grace_datatype->legend = "Cumulative";
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	if ( !list_rewind( date_colon_time_key_list ) ) return 0;

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		water_budget_inflow_outflow =
				hash_table_get_pointer(
					inflow_outflow_hash_table,
					date_colon_time );

		if ( aggregate_level == real_time
		&&   aggregate_level == half_hour
		&&   aggregate_level == hourly )
		{
			piece( date_string, ':', date_colon_time, 0 );
			piece( time_string, ':', date_colon_time, 1 );
		}
		else
		{
			strcpy( date_string, date_colon_time );
			*time_string = '\0';
		}

		sprintf( point_buffer,
			 "cumulative||%s|%s|%.3lf",
			 date_string,
			 time_string,
			 water_budget_inflow_outflow->cumulative );

		grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				point_buffer,
				grace->grace_graph_type,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

		sprintf( point_buffer,
			 "budget||%s|%s|%.3lf",
			 date_string,
			 time_string,
			 water_budget_inflow_outflow->budget );

		grace_set_string_to_point_list(
				grace->graph_list,
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				point_buffer,
				grace->grace_graph_type,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

	} while( list_next( date_colon_time_key_list ) );	

	grace->grace_output =
		application_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&grace->agr_filename,
			&grace->ftp_agr_filename,
			&grace->postscript_filename,
			&grace->output_filename,
			&grace->ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	grace_graph_set_no_yaxis_grid_lines_offset(
			grace->graph_list,
			0 /* graph_offset */ );

	if ( !grace_set_structures(
				&grace->page_width_pixels,
				&grace->page_length_pixels,
				&grace->distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		return 0;
	}

	/* Move the first legend */
	/* --------------------- */
	grace_set_first_graph_top_of_second( grace->graph_list );

	list_rewind( grace->graph_list );
	grace_graph = list_get_pointer( grace->graph_list );
	grace_move_legend_bottom_right(
		grace_graph,
		grace->landscape_mode );

	/* Change the line color */
	/* --------------------- */
	grace_graph_increment_line_color( grace_graph->datatype_list );

	/* Move the second legend */
	/* ---------------------- */
	list_next( grace->graph_list );
	grace_graph = list_get_pointer( grace->graph_list );
	grace_move_legend_bottom_left( grace_graph, grace->landscape_mode );

	if ( grace_contains_multiple_datatypes( grace->graph_list ) )
		grace->dataset_no_cycle_color = 1;

	if ( !grace_output_charts(
				grace->output_filename, 
				grace->postscript_filename,
				grace->agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				grace->page_width_pixels,
				grace->page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				grace->distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		return 0;
	}
	else
	{
		grace_output_graph_window(
				application_name,
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* with_document_output */,
				(char *)0 /* where_clause */ );
		return 1;
	}
}

void get_report_title(	char *title,
			enum aggregate_level aggregate_level,
			char *flow_units,
			char *flow_units_converted )
{
	char *units_display;

	if ( flow_units_converted
	&&   *flow_units_converted )
	{
		units_display = flow_units_converted;
	}
	else
	{
		units_display = flow_units;
	}

	if ( aggregate_level == real_time )
	{
		sprintf(	title,
				"%s (In %s)",
				REPORT_TITLE,
				units_display );
	}
	else
	{
		sprintf(	title,
				"%s (%s In %s)",
				REPORT_TITLE,
				aggregate_level_get_string( aggregate_level ),
				units_display );
	}
	format_initial_capital( title, title );
}

#ifdef NOT_DEFINED
boolean water_budget_flow_output_googlechart(
				char *application_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *inflow_outflow_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *process_name,
				char *flow_units,
				char *flow_units_converted )
{

	if ( !water_budget_flow_populate_easycharts_input_chart_list_data(
			easycharts->input_chart_list,
			inflow_outflow_hash_table,
			aggregate_level ) )
	{
		printf( "<p>There are no charts to display\n" );
		document_close();
		exit( 0 );
	}

	easycharts->output_chart_list =
		easycharts_timeline_get_output_chart_list(
			easycharts->input_chart_list );

	return 1;
}
#endif

boolean water_budget_flow_populate_easycharts_input_chart_list_data(
			LIST *input_chart_list,
			HASH_TABLE *inflow_outflow_hash_table,
			enum aggregate_level aggregate_level )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	WATER_BUDGET_INFLOW_OUTFLOW *water_budget_inflow_outflow;
	char date_string[ 32 ];

	date_colon_time_key_list =
		hash_table_get_key_list(
			inflow_outflow_hash_table );

	if ( !list_rewind( date_colon_time_key_list ) ) return 0;

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		water_budget_inflow_outflow =
				hash_table_get_pointer(
					inflow_outflow_hash_table,
					date_colon_time );

		if ( aggregate_level != real_time
		&&   aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			piece( date_string, ':', date_colon_time, 0 );
			date_colon_time = strdup( date_string );
		}

		if ( !easycharts_set_input_value(
				input_chart_list,
				"budget",
				date_colon_time,
			 	water_budget_inflow_outflow->budget,
				0 /* not is_null */ ) )
		{
			printf( "<h2>ERROR: cannot allocate memory.</h2>\n" );
			document_close();
			exit( 0 );
		}

		if ( !easycharts_set_input_value(
				input_chart_list,
				"cumulative",
				date_colon_time,
			 	water_budget_inflow_outflow->cumulative,
				0 /* not is_null */ ) )
		{
			printf( "<h2>ERROR: cannot allocate memory.</h2>\n" );
			document_close();
			exit( 0 );
		}

	} while( list_next( date_colon_time_key_list ) );
	return 1;
}

void water_budget_flow_populate_easycharts_input_chart_list_datatypes(
					LIST *input_chart_list )
{
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;

	input_chart = easycharts_new_input_chart();
	list_append_pointer( input_chart_list, input_chart );

	input_chart->double_range_adjusters = 1;

	input_datatype =
		easycharts_new_input_datatype(
			"budget",
			(char *)0 /* units */ );

	list_append_pointer(	input_chart->datatype_list,
				input_datatype );

	input_datatype =
		easycharts_new_input_datatype(
			"cumulative",
			(char *)0 /* units */ );

	list_append_pointer(	input_chart->datatype_list,
				input_datatype );

	input_chart->applet_library_code =
		EASYCHARTS_APPLET_LIBRARY_LINE_CHART;

}

void get_first_inflow_names(	char **first_inflow_station_name,
				char **first_inflow_datatype_name,
				DICTIONARY *parameter_dictionary )
{
	dictionary_get_index_data(
			first_inflow_station_name,
			parameter_dictionary,
			"inflow_station",
			1 );

	dictionary_get_index_data(
			first_inflow_datatype_name,
			parameter_dictionary,
			"inflow_datatype",
			1 );
}

