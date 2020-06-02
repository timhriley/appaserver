/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_differences.c     	*/
/* --------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "dictionary_appaserver.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "appaserver_link_file.h"
#include "google_chart.h"

/* Constants */
/* --------- */
#define REPORT_TITLE				"Measurement Differences"
#define DEFAULT_OUTPUT_MEDIUM			"table"
#define KEY_DELIMITER				'/'
#define CUMULATIVE_DATATYPE			"Cumulative"
#define DIFFERENCE_DATATYPE			"Difference"

#define ROWS_BETWEEN_HEADING			20
#define SELECT_LIST				 "station,measurement_date,measurement_time,measurement_value,station"

#define GRACE_TICKLABEL_ANGLE			90
#define STATION_PIECE		 		0
#define DATE_PIECE		 		1
#define TIME_PIECE		 		2
#define VALUE_PIECE		 		3
#define COUNT_PIECE		 		4
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */
typedef struct
{
	double value;
	boolean is_null;
	int count;
} MEASUREMENT_DIFFERENCES_MEASUREMENT;

typedef struct
{
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_MEASUREMENT *positive_measurement;
	MEASUREMENT_DIFFERENCES_MEASUREMENT *negative_measurement;
	double difference;
	double cumulative;
	boolean difference_is_null;
} MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE;

typedef struct
{
	HASH_TABLE *positive_negative_hash_table;
	char *positive_station_name;
	char *positive_datatype_name;
	char *negative_station_name;
	char *negative_datatype_name;
} MEASUREMENT_DIFFERENCES;

	
/* Prototypes */
/* ---------- */
boolean populate_google_datatype_chart_list_data(
				LIST *datatype_chart_list,
				HASH_TABLE *positive_negative_hash_table,
				LIST *date_colon_time_key_list );

boolean measurement_differences_output_googlechart(
				char *application_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *process_name,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *units_display );

boolean measurement_differences_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *argv_0,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *units_display );

GOOGLE_CHART *get_google_datatype_chart(
				HASH_TABLE *positive_negative_hash_table,
				LIST *date_colon_time_key_list );

char *get_cumulative_difference_string(
				double cumulative_difference,
				boolean difference_is_null );

void get_report_title_sub_title(char *title,
				char *sub_title,
				enum aggregate_level,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *begin_date,
				char *end_date,
				char *units_display );

void differences_output_transmit_measurement_list(
				FILE *output_pipe,
				LIST *measurement_list,
				enum aggregate_level aggregate_level,
				boolean display_count );

/*
void differences_pad_missing_data(
				HASH_TABLE *positive_negative_hash_table,
				int station_name_list_length,
				boolean is_positive );
*/

void measurement_differences_populate_difference_and_cumulative(
				HASH_TABLE *positive_negative_hash_table,
				char *application_name,
				char *appaserver_mount_point );

MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		measurement_differences_hash_table_get_positive_negative(
				HASH_TABLE *positive_negative_hash_table,
				char *date_colon_time );

void with_sys_string_populate_positive_negative_hash_table(
				HASH_TABLE *positive_negative_hash_table,
				char *sys_string,
				int is_positive );

MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		measurement_differences_positive_negative_new(
					char *date_colon_time );

MEASUREMENT_DIFFERENCES *measurement_differences_new(
					char *application_name,
					char *begin_date,
					char *end_date,
					char *appaserver_mount_point,
					enum aggregate_level aggregate_level,
					char *positive_station_name,
					char *positive_datatype_name,
					char *negative_station_name,
					char *negative_datatype_name,
					char *datatype_units,
					char *datatype_units_converted );

HASH_TABLE *get_merged_date_colon_time_key_hash_table(
					char *station_datatype );

boolean populate_positive_negative_hash_table(
				HASH_TABLE *positive_negative_hash_table,
				char *application_name,
				char *begin_date,
				char *end_date,
				char *station_name,
				char *datatype_name,
				enum aggregate_level,
				boolean is_positive,
				char *datatype_units,
				char *datatype_units_converted );

MEASUREMENT_DIFFERENCES_MEASUREMENT *measurement_differences_new_measurement(
				void );

boolean measurement_differences_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *argv_0,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *units_display );

void measurement_differences_output_table(
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				boolean display_count,
				char *units_display,
				char *application_name );

void measurement_differences_output_transmit(
				FILE *output_pipe,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				boolean display_count,
				char *units_display );

void build_sys_string(
					char *sys_string,
					char *application_name,
					char *begin_date,
					char *end_date,
					char *station_name,
					char *datatype_name,
					enum aggregate_level aggregate_level,
					char *datatype_units,
					char *datatype_units_converted );

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	char *parameter_dictionary_string;
	MEASUREMENT_DIFFERENCES *measurement_differences;
	char *begin_date, *end_date;
	char *output_medium;
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *database_string = {0};
	char *process_name = {0};
	char *display_count_yn = {0};
	char *units_display;
	char *datatype_units;
	char *datatype_units_converted;
	char *positive_station_name;
	char *negative_station_name;
	char *positive_datatype_name;
	char *negative_datatype_name;

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s application role parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	role_name = argv[ 2 ];
	parameter_dictionary_string = argv[ 3 ];

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

	dictionary_get_index_data( 	&positive_station_name,
					parameter_dictionary, 
					"positive_station", 
					0 );

	dictionary_get_index_data( 	&negative_station_name,
					parameter_dictionary, 
					"negative_station", 
					0 );

	dictionary_get_index_data( 	&positive_datatype_name,
					parameter_dictionary, 
					"positive_datatype", 
					0 );

	dictionary_get_index_data( 	&negative_datatype_name,
					parameter_dictionary, 
					"negative_datatype", 
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
		aggregate_level_get_aggregate_level( aggregate_level_string );

	dictionary_get_index_data( 	&output_medium, 
					parameter_dictionary, 
					"output_medium", 
					0 );

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	dictionary_get_index_data( 	&display_count_yn,
					parameter_dictionary,
					"display_count_yn",
					0 );

	dictionary_get_index_data( 	&datatype_units_converted,
					parameter_dictionary,
					"units_converted",
					0 );

	datatype_units = hydrology_library_get_datatype_units(
				application_name,
				positive_datatype_name );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			positive_datatype_name,
			datatype_units,
			datatype_units_converted,
			aggregate_statistic_none );

	if ( !units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				positive_datatype_name,
				datatype_units,
				datatype_units_converted ) );
		document_close();
		exit( 0 );
	}

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					positive_station_name,
					positive_datatype_name );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	measurement_differences =
			measurement_differences_new(
				application_name,
				begin_date,
				end_date,
				appaserver_parameter_file->
					appaserver_mount_point,
				aggregate_level,
				positive_station_name,
				positive_datatype_name,
				negative_station_name,
				negative_datatype_name,
				datatype_units,
				datatype_units_converted );

	measurement_differences_populate_difference_and_cumulative(
		measurement_differences->positive_negative_hash_table,
		application_name,
		appaserver_parameter_file->appaserver_mount_point );

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
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		measurement_differences_output_table(
			measurement_differences->positive_negative_hash_table,
			aggregate_level,
			measurement_differences->positive_station_name,
			measurement_differences->positive_datatype_name,
			measurement_differences->negative_station_name,
			measurement_differences->negative_datatype_name,
			( *display_count_yn == 'y' ),
			units_display,
			application_name );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
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

		hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				positive_station_name,
				1 /* with_zap_file */ );

		hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				negative_station_name,
				1 /* with_zap_file */ );

		sprintf( sys_string,
			 "tr '|' ',' >> %s",
			output_pipename );

		output_pipe = popen( sys_string, "w" );

		measurement_differences_output_transmit(
					output_pipe,
					measurement_differences->
						positive_negative_hash_table,
					aggregate_level,
					measurement_differences->
						positive_station_name,
					measurement_differences->
						positive_datatype_name,
					measurement_differences->
						negative_station_name,
					measurement_differences->
						negative_datatype_name,
					( *display_count_yn == 'y' ),
					units_display );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", REPORT_TITLE );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( timlib_system_date_string() ) ){};
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
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
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
				positive_station_name,
				1 /* with_zap_file */ );

		hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				negative_station_name,
				1 /* with_zap_file */ );

		sprintf(sys_string,
		 	"tr '|' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );

		output_pipe = popen( sys_string, "w" );

		measurement_differences_output_transmit(
					output_pipe,
					measurement_differences->
						positive_negative_hash_table,
					aggregate_level,
					measurement_differences->
						positive_station_name,
					measurement_differences->
						positive_datatype_name,
					measurement_differences->
						negative_station_name,
					measurement_differences->
						negative_datatype_name,
					( *display_count_yn == 'y' ),
					units_display );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", REPORT_TITLE );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( timlib_system_date_string() ) ){};
		fflush( stdout );
		printf( "</h1>\n" );
	
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

		hydrology_library_output_station_text_file(
				stdout,
				application_name,
				positive_station_name );

		hydrology_library_output_station_text_file(
				stdout,
				application_name,
				negative_station_name );

		sprintf(sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		measurement_differences_output_transmit(
					output_pipe,
					measurement_differences->
						positive_negative_hash_table,
					aggregate_level,
					measurement_differences->
						positive_station_name,
					measurement_differences->
						positive_datatype_name,
					measurement_differences->
						negative_station_name,
					measurement_differences->
						negative_datatype_name,
					( *display_count_yn == 'y' ),
					units_display );

		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "gracechart" ) == 0 )
	{
		if ( !measurement_differences_output_gracechart(
					application_name,
					role_name,
					begin_date,
					end_date,
					measurement_differences->
						positive_negative_hash_table,
					aggregate_level,
					appaserver_parameter_file->
						document_root,
					argv[ 0 ],
					measurement_differences->
						positive_station_name,
					measurement_differences->
						positive_datatype_name,
					measurement_differences->
						negative_station_name,
					measurement_differences->
						negative_datatype_name,
					units_display ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( output_medium, "googlechart" ) == 0 )
	{
		if ( !measurement_differences_output_googlechart(
					application_name,
					begin_date,
					end_date,
					measurement_differences->
						positive_negative_hash_table,
					aggregate_level,
					appaserver_parameter_file->
						document_root,
					process_name,
					measurement_differences->
						positive_station_name,
					measurement_differences->
						positive_datatype_name,
					measurement_differences->
						negative_station_name,
					measurement_differences->
						negative_datatype_name,
					units_display ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
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

void measurement_differences_output_transmit(
				FILE *output_pipe,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				boolean display_count,
				char *units_display )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;
	char key_label[ 32 ];
	char *cumulative_difference_string;
	char title[ 512 ];

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			positive_negative_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		return;
	}

	get_report_title_sub_title(
				title,
				(char *)0 /* sub_title */,
				aggregate_level,
				positive_station_name,
				positive_datatype_name,
				negative_station_name,
				negative_datatype_name,
				(char *)0 /* begin_date */,
				(char *)0 /* end_date */,
				units_display );

	fprintf( output_pipe, "#%s\n", title );

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
	fprintf( output_pipe, "|Difference" );
	fprintf( output_pipe, "|Positive_%s/%s",
		 positive_station_name,
		 positive_datatype_name );

	if ( display_count && aggregate_level != real_time )
	{
		fprintf( output_pipe, "|Count" );
	}

	fprintf( output_pipe, "|Negative_%s/%s",
		 negative_station_name,
		 negative_datatype_name );

	if ( display_count && aggregate_level != real_time )
	{
		fprintf( output_pipe, "|Count" );
	}

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

		positive_negative =
			hash_table_get_pointer(
				positive_negative_hash_table,
				date_colon_time );

		cumulative_difference_string =
			get_cumulative_difference_string(
				positive_negative->cumulative,
				positive_negative->difference_is_null );

		fprintf(output_pipe,
			"|%s",
			cumulative_difference_string );

		cumulative_difference_string =
			get_cumulative_difference_string(
				positive_negative->difference,
				positive_negative->difference_is_null );

		fprintf(output_pipe,
			"|%s",
			cumulative_difference_string );

		if ( positive_negative->positive_measurement
		&&  !positive_negative->positive_measurement->is_null )
		{
			fprintf(output_pipe,
				"|%.3lf",
				positive_negative->
					positive_measurement->value );
		}
		else
		{
			fprintf( output_pipe, "|null" );
		}

		if ( display_count
		&&   aggregate_level != real_time )
		{
			if ( positive_negative->positive_measurement
			&&  !positive_negative->positive_measurement->is_null )
			{
				fprintf(output_pipe,
					"|%d",
					positive_negative->
						positive_measurement->count );
			}
			else
			{
				fprintf( output_pipe, "|null" );
			}
		}

		if ( positive_negative->negative_measurement
		&&  !positive_negative->negative_measurement->is_null )
		{
			fprintf(output_pipe,
				"|%.3lf",
				positive_negative->
					negative_measurement->value );
		}
		else
		{
			fprintf( output_pipe, "|null" );
		}

		if ( display_count
		&&   aggregate_level != real_time )
		{
			if ( positive_negative->negative_measurement
			&&  !positive_negative->negative_measurement->is_null )
			{
				fprintf(output_pipe,
					"|%d",
					positive_negative->
						negative_measurement->count );
			}
			else
			{
				fprintf( output_pipe, "|null" );
			}
		}

		fprintf( output_pipe, "\n" );
	} while( list_next( date_colon_time_key_list ) );

} /* measurement_differences_output_transmit() */

void measurement_differences_output_table(
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				boolean display_count,
				char *units_display,
				char *application_name )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char buffer[ 512 ];
	register int count = 0;
	char title[ 512 ];
	char key_label[ 32 ];
	char heading_buffer[ 128 ];
	char *cumulative_difference_string;

	hydrology_library_output_station_table(
		application_name,
		positive_station_name );

	hydrology_library_output_station_table(
		application_name,
		negative_station_name );

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			positive_negative_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		return;
	}

	get_report_title_sub_title(
				title,
				(char *)0 /* sub_title */,
				aggregate_level,
				positive_station_name,
				positive_datatype_name,
				negative_station_name,
				negative_datatype_name,
				(char *)0 /* begin_date */,
				(char *)0 /* end_date */,
				units_display );

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

	list_append_pointer( heading_list, CUMULATIVE_DATATYPE );
	list_append_pointer( heading_list, DIFFERENCE_DATATYPE );

	sprintf( heading_buffer, "Positive %s/%s",
		 positive_station_name,
		 positive_datatype_name );
	list_append_pointer( heading_list, strdup( heading_buffer ) );

	if ( display_count && aggregate_level != real_time )
	{
		list_append_pointer( heading_list, "Count" );
	}

	sprintf( heading_buffer, "Negative %s/%s",
		 negative_station_name,
		 negative_datatype_name );
	list_append_pointer( heading_list, strdup( heading_buffer ) );

	if ( display_count && aggregate_level != real_time )
	{
		list_append_pointer( heading_list, "Count" );
	}

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

		positive_negative =
			hash_table_get_pointer(
				positive_negative_hash_table,
				date_colon_time );

		cumulative_difference_string =
			get_cumulative_difference_string(
				positive_negative->cumulative,
				positive_negative->difference_is_null );

		html_table_set_data(	html_table->data_list,
					strdup(
					   cumulative_difference_string ) );

		cumulative_difference_string =
			get_cumulative_difference_string(
				positive_negative->difference,
				positive_negative->difference_is_null );

		html_table_set_data(	html_table->data_list,
					strdup(
					   cumulative_difference_string ) );

		if ( positive_negative->positive_measurement
		&&  !positive_negative->positive_measurement->is_null )
		{
			sprintf(buffer,
				"%.3lf",
				positive_negative->
					positive_measurement->value );

			html_table_set_data(	html_table->data_list,
						strdup( buffer ) );
		}
		else
		{	
			html_table_set_data(	html_table->data_list,
						strdup( "null" ) );
		}

		if ( display_count
		&&   aggregate_level != real_time )
		{
			if ( positive_negative->positive_measurement
			&&  !positive_negative->positive_measurement->is_null )
			{
				sprintf(buffer,
					"%d",
					positive_negative->
						positive_measurement->count );

				html_table_set_data(	html_table->data_list,
							strdup( buffer ) );
			}
			else
			{
				html_table_set_data(	html_table->data_list,
							strdup( "null" ) );
			}
		}

		if ( positive_negative->negative_measurement
		&&  !positive_negative->negative_measurement->is_null )
		{
			sprintf(buffer,
				"%.3lf",
				positive_negative->
					negative_measurement->value );

			html_table_set_data(	html_table->data_list,
						strdup( buffer ) );
		}
		else
		{	
			html_table_set_data(	html_table->data_list,
						strdup( "null" ) );
		}

		if ( display_count
		&&   aggregate_level != real_time )
		{
			if ( positive_negative->negative_measurement
			&&  !positive_negative->negative_measurement->is_null )
			{
				sprintf(buffer,
					"%d",
					positive_negative->
						negative_measurement->count );

				html_table_set_data(	html_table->data_list,
							strdup( buffer ) );
			}
			else
			{
				html_table_set_data(	html_table->data_list,
							strdup( "null" ) );
			}
		}

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

} /* measurement_differences_output_table() */

void build_sys_string(
		char *sys_string,
		char *application_name,
		char *begin_date,
		char *end_date,
		char *station_name,
		char *datatype_name,
		enum aggregate_level aggregate_level,
		char *datatype_units,
		char *datatype_units_converted )
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

	if ( datatype_units_converted
	&&   *datatype_units_converted
	&&   strcmp( datatype_units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s '%s' %d '%c' %d",
			 application_name,
			 datatype_units,
			 datatype_units_converted,
			 VALUE_PIECE,
			 PIECE_DELIMITER,
			 STATION_PIECE );
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

} /* build_sys_string() */

MEASUREMENT_DIFFERENCES_MEASUREMENT *
		measurement_differences_new_measurement( void )
{
	MEASUREMENT_DIFFERENCES_MEASUREMENT *m =
		(MEASUREMENT_DIFFERENCES_MEASUREMENT *)
			calloc( 1,
				sizeof( MEASUREMENT_DIFFERENCES_MEASUREMENT ) );
	if ( !m )
	{
		fprintf( stderr,
			 "Memory allocation ERROR in %s/%s()\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	return m;
} /* measurement_differences_new_measurement() */

MEASUREMENT_DIFFERENCES *measurement_differences_new(
					char *application_name,
					char *begin_date,
					char *end_date,
					char *appaserver_mount_point,
					enum aggregate_level aggregate_level,
					char *positive_station_name,
					char *positive_datatype_name,
					char *negative_station_name,
					char *negative_datatype_name,
					char *datatype_units,
					char *datatype_units_converted )
{
	MEASUREMENT_DIFFERENCES *differences;

	differences =
		(MEASUREMENT_DIFFERENCES *)
			calloc( 1, sizeof( MEASUREMENT_DIFFERENCES ) );

	differences->positive_negative_hash_table =
		hash_table_new( hash_table_medium );

	differences->positive_station_name = positive_station_name;
	differences->positive_datatype_name = positive_datatype_name;

	if ( ! populate_positive_negative_hash_table(
					differences->
						positive_negative_hash_table,
					application_name,
					begin_date,
					end_date,
					differences->
						positive_station_name,
					differences->
						positive_datatype_name,
					aggregate_level,
					1 /* is_positive */,
					datatype_units,
					datatype_units_converted ) )
	{
		document_quick_output_body(
					application_name,
					appaserver_mount_point );

		printf( "<h3>ERROR: insufficient positive input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	differences->negative_station_name = negative_station_name;
	differences->negative_datatype_name = negative_datatype_name;
	if ( ! populate_positive_negative_hash_table(
					differences->
						positive_negative_hash_table,
					application_name,
					begin_date,
					end_date,
					differences->
						negative_station_name,
					differences->
						negative_datatype_name,
					aggregate_level,
					0 /* not is_positive */,
					datatype_units,
					datatype_units_converted ) )
	{
		document_quick_output_body(
					application_name,
					appaserver_mount_point );

		printf( "<h3>ERROR: insufficient negative input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	return differences;
} /* measurement_differences_new() */

MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		measurement_differences_hash_table_get_positive_negative(
				HASH_TABLE *positive_negative_hash_table,
				char *date_colon_time )
{
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;

	if ( !( positive_negative = hash_table_get_pointer(
					positive_negative_hash_table,
					date_colon_time ) ) )
	{
		positive_negative =
			measurement_differences_positive_negative_new(
				strdup( date_colon_time ) );

		hash_table_add_pointer(
			positive_negative_hash_table,
			positive_negative->date_colon_time,
			positive_negative );
	}
	return positive_negative;
} /* measurement_differences_hash_table_get_positive_negative() */

MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		measurement_differences_positive_negative_new(
					char *date_colon_time )
{
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;

	positive_negative =
		(MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *)
			calloc( 1,
				sizeof(
				MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE ) );

	if ( !positive_negative )
	{
		fprintf( stderr,
			 "Memory allocation ERROR in %s/%s()\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	positive_negative->date_colon_time = date_colon_time;
	return positive_negative;
} /* measurement_differences_positive_negative_new() */

boolean populate_positive_negative_hash_table(
				HASH_TABLE *positive_negative_hash_table,
				char *application_name,
				char *begin_date,
				char *end_date,
				char *station_name,
				char *datatype_name,
				enum aggregate_level aggregate_level,
				boolean is_positive,
				char *datatype_units,
				char *datatype_units_converted )
{
	char sys_string[ 1024 ];

	if ( !station_name || !*station_name ) return 0;
	if ( !datatype_name || !*datatype_name ) return 0;

	build_sys_string(
				sys_string,
				application_name,
				begin_date,
				end_date,
				station_name,
				datatype_name,
				aggregate_level,
				datatype_units,
				datatype_units_converted );

	with_sys_string_populate_positive_negative_hash_table(
				positive_negative_hash_table,
				sys_string,
				is_positive );

	return 1;
} /* populate_positive_negative_hash_table() */

void with_sys_string_populate_positive_negative_hash_table(
				HASH_TABLE *positive_negative_hash_table,
				char *sys_string,
				int is_positive )
{
	char measurement_date[ 16 ];
	char measurement_time[ 16 ];
	char measurement_buffer[ 128 ];
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;
	MEASUREMENT_DIFFERENCES_MEASUREMENT *measurement;
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char date_colon_time[ 32 ];
	char count_buffer[ 16 ];

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

		positive_negative =
		measurement_differences_hash_table_get_positive_negative(
				positive_negative_hash_table,
				date_colon_time );

		measurement = measurement_differences_new_measurement();

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

		if ( is_positive )
		{
			positive_negative->positive_measurement = measurement;
		}
		else
		{
			positive_negative->negative_measurement = measurement;
		}

/*
		length = list_length( measurement_list );
		if ( length < list_offset )
		{
			MEASUREMENT_DIFFERENCES_MEASUREMENT *null_measurement;

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
*/
	}

	pclose( input_pipe );

} /* with_sys_string_populate_positive_outfile_hash_table() */

void measurement_differences_populate_difference_and_cumulative(
				HASH_TABLE *positive_negative_hash_table,
				char *application_name,
				char *appaserver_mount_point )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;
	double old_cumulative = 0.0;

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			positive_negative_hash_table );

	if ( !list_rewind( date_colon_time_key_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_mount_point );
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		document_close();
		exit( 1 );
	}

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		if ( ! ( positive_negative =
				hash_table_get_pointer(
					positive_negative_hash_table,
					date_colon_time ) ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: cannot get positive_negative from hash table for date = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				date_colon_time );
			exit( 1 );
		}

		if ( positive_negative->positive_measurement
		&&  !positive_negative->positive_measurement->is_null
		&&   positive_negative->negative_measurement
		&&  !positive_negative->negative_measurement->is_null )
		{
			positive_negative->difference =
				positive_negative->positive_measurement->value;

			positive_negative->difference -=
				positive_negative->negative_measurement->value;

			old_cumulative += positive_negative->difference;
			positive_negative->cumulative += old_cumulative;
		}
		else
		{
			positive_negative->difference_is_null = 1;
			positive_negative->cumulative = old_cumulative;
		}

	} while( list_next( date_colon_time_key_list ) );	

	list_free_container( date_colon_time_key_list );

} /* measurement_differences_populate_difference_and_cumulative() */

#ifdef NOT_DEFINED
void differences_pad_missing_data(
		HASH_TABLE *positive_negative_hash_table,
		int station_name_list_length,
		boolean is_positive )
{
	LIST *date_colon_time_key_list;
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *positive_negative;
	int length;
	LIST *measurement_list;

	date_colon_time_key_list =
		 hash_table_get_key_list(
			positive_negative_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		document_close();
		exit( 1 );
	}

	list_rewind( date_colon_time_key_list );

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		positive_negative =
				hash_table_get_pointer(
					positive_negative_hash_table,
					date_colon_time );

		if ( is_positive )
		{
			measurement_list =
				positive_negative->positive_measurement_list;
		}
		else
		{
			measurement_list =
				positive_negative->negative_measurement_list;
		}

		length = list_length( measurement_list );
		if ( length < station_name_list_length )
		{
			MEASUREMENT_DIFFERENCES_MEASUREMENT *null_measurement;

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

} /* differences_pad_missing_data() */
#endif

void differences_output_transmit_measurement_list(
					FILE *output_pipe,
					LIST *measurement_list,
					enum aggregate_level aggregate_level,
					boolean display_count )
{
	MEASUREMENT_DIFFERENCES_MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): empty measurement_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 0 );
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

} /* differences_output_transmit_measurement_list() */

boolean measurement_differences_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *argv_0,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *units_display )
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
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		differences_positive_negative;
	char date_string[ 32 ];
	char time_string[ 32 ];
	char *cumulative_difference_string;

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			positive_negative_hash_table );

	if ( !list_length( date_colon_time_key_list ) ) return 0;

	get_report_title_sub_title(
				title,
				sub_title,
				aggregate_level,
				positive_station_name,
				positive_datatype_name,
				negative_station_name,
				negative_datatype_name,
				begin_date,
				end_date,
				units_display );

/*
	sprintf(sub_title,
		"Positive: %s/%s, Negative: %s/%s, From %s To %s",
		positive_station_name,
		positive_datatype_name,
		negative_station_name,
		negative_datatype_name,
		begin_date,
		end_date );
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
				-1 /* grace_time_piece */,
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
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	/* Create the difference graph */
	/* --------------------------- */
	grace_graph = grace_new_grace_graph();
	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;
	grace_graph->yaxis_grid_lines = 0;
	list_append_pointer( grace->graph_list, grace_graph );

	grace_datatype =
		grace_new_grace_datatype( "difference", "" );
	grace_datatype->legend = "Difference";
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
		grace_new_grace_datatype( CUMULATIVE_DATATYPE, "" );
	grace_datatype->legend = CUMULATIVE_DATATYPE;
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	grace->dataset_no_cycle_color = 1;

	if ( !list_rewind( date_colon_time_key_list ) ) return 0;

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		differences_positive_negative =
				hash_table_get_pointer(
					positive_negative_hash_table,
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

		cumulative_difference_string =
			get_cumulative_difference_string(
				differences_positive_negative->cumulative,
				differences_positive_negative->
					difference_is_null );

		sprintf( point_buffer,
			 "%s||%s|%s|%s",
			 CUMULATIVE_DATATYPE,
			 date_string,
			 time_string,
			 cumulative_difference_string );

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

		cumulative_difference_string =
			get_cumulative_difference_string(
				differences_positive_negative->difference,
				differences_positive_negative->
					difference_is_null );

		sprintf( point_buffer,
			 "%s||%s|%s|%s",
			 DIFFERENCE_DATATYPE,
			 date_string,
			 time_string,
			 cumulative_difference_string );

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
		application_get_grace_output( application_name );

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

	/* Move the legend */
	/* --------------- */
	grace_set_first_graph_top_of_second( grace->graph_list );
	grace_move_legend_bottom_right(
		list_get_first_pointer( grace->graph_list ),
		grace->landscape_mode );
	list_next( grace->graph_list );
	grace_graph = list_get_pointer( grace->graph_list );
	grace_move_legend_bottom_left(	grace_graph,
					grace->landscape_mode );

	/* Change the line color */
	/* --------------------- */
	grace_graph_increment_line_color( grace_graph->datatype_list );

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
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				grace->distill_landscape_flag,
				application_get_ghost_script_directory(
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

} /* measurement_differences_output_gracechart() */

void get_report_title_sub_title(
			char *title,
			char *sub_title,
			enum aggregate_level aggregate_level,
			char *positive_station_name,
			char *positive_datatype_name,
			char *negative_station_name,
			char *negative_datatype_name,
			char *begin_date,
			char *end_date,
			char *units_display )
{
	if ( aggregate_level == real_time )
	{
		sprintf(	title,
				"%s (%s/%s In %s)",
				REPORT_TITLE,
				positive_station_name,
				positive_datatype_name,
				units_display );
	}
	else
	{
		sprintf(	title,
				"%s (%s/%s %s In %s)",
				REPORT_TITLE,
				positive_station_name,
				positive_datatype_name,
				aggregate_level_get_string( aggregate_level ),
				units_display );
	}

	format_initial_capital( title, title );

	if ( sub_title )
	{
		sprintf(sub_title,
			"Positive: %s/%s, Negative: %s/%s, From %s To %s",
			positive_station_name,
			positive_datatype_name,
			negative_station_name,
			negative_datatype_name,
			begin_date,
			end_date );
	}

} /* get_report_title_sub_title() */

boolean measurement_differences_output_googlechart(
				char *application_name,
				char *begin_date,
				char *end_date,
				HASH_TABLE *positive_negative_hash_table,
				enum aggregate_level aggregate_level,
				char *document_root_directory,
				char *process_name,
				char *positive_station_name,
				char *positive_datatype_name,
				char *negative_station_name,
				char *negative_datatype_name,
				char *units_display )
{
	LIST *date_colon_time_key_list;
	char title[ 512 ];
	char sub_title[ 512 ];
	GOOGLE_CHART *google_chart;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;

	date_colon_time_key_list =
		 hash_table_get_ordered_key_list(
			positive_negative_hash_table );

	if ( !list_length( date_colon_time_key_list ) )
	{
		return 0;
	}

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

	get_report_title_sub_title(
				title,
				sub_title,
				aggregate_level,
				positive_station_name,
				positive_datatype_name,
				negative_station_name,
				negative_datatype_name,
				begin_date,
				end_date,
				units_display );

	if ( ! ( google_chart =
			get_google_datatype_chart(
				positive_negative_hash_table,
				date_colon_time_key_list ) ) )
	{
		return 0;
	}

	google_chart->title = title;
	google_chart->sub_title = sub_title;
	google_chart->stylesheet = "style.css";

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

	return 1;

} /* measurement_differences_output_googlechart() */

GOOGLE_CHART *get_google_datatype_chart(
				HASH_TABLE *positive_negative_hash_table,
				LIST *date_colon_time_key_list )
{
	GOOGLE_CHART *google_chart;

	google_chart = google_chart_new();

	google_chart->datatype_chart_list = list_new();

	if ( !populate_google_datatype_chart_list_data(
			google_chart->datatype_chart_list,
			positive_negative_hash_table,
			date_colon_time_key_list ) )
	{
		return (GOOGLE_CHART *)0;
	}

	return google_chart;

} /* get_google_datatype_chart() */

boolean populate_google_datatype_chart_list_data(
			LIST *datatype_chart_list,
			HASH_TABLE *positive_negative_hash_table,
			LIST *date_colon_time_key_list )
{
	char *date_colon_time;
	MEASUREMENT_DIFFERENCES_POSITIVE_NEGATIVE *
		differences_positive_negative;
	char *cumulative_difference_string;
	GOOGLE_INPUT_VALUE *input_value;
	GOOGLE_DATATYPE_CHART *google_datatype_chart;

	if ( !list_rewind( date_colon_time_key_list ) ) return 0;

	do {
		date_colon_time =
			list_get_pointer(
				date_colon_time_key_list );

		differences_positive_negative =
				hash_table_get_pointer(
					positive_negative_hash_table,
					date_colon_time );

		cumulative_difference_string =
			get_cumulative_difference_string(
				differences_positive_negative->cumulative,
				differences_positive_negative->
					difference_is_null );

		google_datatype_chart =
			google_datatype_chart_get_or_set(
					datatype_chart_list,
			 		CUMULATIVE_DATATYPE );

		input_value =
			google_chart_input_value_new(
				date_colon_time );

		input_value->value = atof( cumulative_difference_string );

		input_value->null_value =
			differences_positive_negative->
				difference_is_null;

		list_append_pointer(
			google_datatype_chart->input_value_list,
			input_value );

		cumulative_difference_string =
			get_cumulative_difference_string(
				differences_positive_negative->difference,
				differences_positive_negative->
					difference_is_null );

		google_datatype_chart =
			google_datatype_chart_get_or_set(
					datatype_chart_list,
			 		DIFFERENCE_DATATYPE );

		input_value =
			google_chart_input_value_new(
				date_colon_time );

		input_value->value = atof( cumulative_difference_string );

		input_value->null_value =
			differences_positive_negative->
				difference_is_null;

		list_append_pointer(
			google_datatype_chart->input_value_list,
			input_value );

	} while( list_next( date_colon_time_key_list ) );	

	return 1;

} /* populate_google_datatype_chart_list_data() */

char *get_cumulative_difference_string(
				double cumulative_difference,
				boolean difference_is_null )
{
	static char cumulative_difference_string[ 64 ];

	if ( difference_is_null )
	{
		strcpy( cumulative_difference_string, "null" );
	}
	else
	{
		sprintf(cumulative_difference_string,
			"%.3lf",
			cumulative_difference );
	}

	return cumulative_difference_string;

} /* get_cumulative_difference_string() */

