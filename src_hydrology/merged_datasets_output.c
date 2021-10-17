/* ------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/merged_datasets_output.c     	*/
/* ------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "hash_table.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "date.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "session.h"
#include "hydrology_library.h"
#include "expected_count.h"
#include "application_constants.h"
#include "appaserver_link.h"
#include "google_chart.h"
#include "validation_level.h"
#include "merged.h"

/* Constants */
/* --------- */
#define MIN_DAYS_FOR_SYNCH_CHECK	32
#define PROCESS_NAME			"output_merged_datasets"
#define KEY_DELIMITER				'/'

#define ROWS_BETWEEN_HEADING			20
#define SELECT_LIST				 "measurement_date,measurement_time,measurement_value"

#define GRACE_DATATYPE_ENTITY_PIECE		0
#define GRACE_DATATYPE_PIECE			1
#define GRACE_DATE_PIECE			2
#define GRACE_TIME_PIECE			-1
#define GRACE_VALUE_PIECE			3
#define GRACE_TICKLABEL_ANGLE			90
#define DATE_PIECE		 		0
#define TIME_PIECE		 		1
#define VALUE_PIECE		 		2

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
int get_days_between(	char *begin_date_string,
			char *end_date_string );

void get_changed_to_daily_message(
			char *changed_to_daily_message,
			boolean data_collection_frequency_out_of_sync,
			boolean date_range_period_of_record );

boolean any_missing_measurements(	LIST *station_datatype_list,
					char *date_time_string );

boolean get_has_bar_graph(		LIST *station_datatype_list );

HASH_TABLE *get_merged_date_space_time_key_hash_table(
					LIST *station_datatype_list );

LIST *get_datatype4station_list(	char *application_name,
					char *login_name,
					char *station,
					char *appaserver_mount_point,
					char *relative_source_directory,
					char *merged_output );

MERGED_STATION_DATATYPE *get_station_datatype(
					char *application_name,
					char *sys_string,
					char *station_name,
					char *datatype_name );

LIST *get_station_datatype_list(	char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_name_list,
					LIST *datatype_name_list,
					enum aggregate_level,
					enum validation_level );

MERGED_STATION_DATATYPE *new_station_datatype(
					char *application_name,
					char *station_name,
					char *datatype_name );

boolean merged_datasets_output_google_chart(
					char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_datatype_list,
					enum aggregate_level,
					enum validation_level,
					char *document_root_directory,
					char *omit_output_if_any_missing_yn );

boolean merged_datasets_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				LIST *station_name_list,
				LIST *datatype_name_list,
				enum aggregate_level,
				enum validation_level,
				char *document_root_directory,
				char *argv_0 );

void merged_datasets_output_table(
				LIST *station_datatype_list,
				enum aggregate_level,
				enum validation_level,
				char *omit_output_if_any_missing_yn,
				boolean display_count,
				char *begin_date,
				char *end_date,
				boolean data_collection_frequency_out_of_sync,
				boolean date_range_period_of_record );

void merged_datasets_output_transmit(
					FILE *output_pipe,
					LIST *station_datatype_list,
					enum aggregate_level,
					enum validation_level,
					char *omit_output_if_any_missing_yn,
					char *begin_date,
					char *end_date );

void build_sys_string(
					char *sys_string,
					char *application_name,
					char *begin_date,
					char *end_date,
					char *station_name,
					char *datatype_name,
					enum aggregate_level,
					enum validation_level );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *role_name;
	char *parameter_dictionary_string;
	LIST *station_name_list;
	LIST *datatype_name_list;
	char *station_name = {0};
	char *begin_date = {0};
	char *end_date = {0};
	char *merged_output = {0};
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *validation_level_string = {0};
	enum validation_level validation_level;
	LIST *station_datatype_list = {0};
	char *omit_output_if_any_missing_yn;
	char *display_count_yn = {0};
	boolean data_collection_frequency_out_of_sync = 0;
	boolean date_range_period_of_record = 0;
	APPASERVER_LINK *appaserver_link;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s login_name ignored ignored role aggregate_level parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* application_name = argv[ 3 ]; */
	role_name = argv[ 4 ];
	aggregate_level_string = argv[ 5 ];

	aggregate_level = 
		aggregate_level_get_aggregate_level(
				aggregate_level_string );

	parameter_dictionary_string = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

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

	dictionary_get_index_data( 	&merged_output,
					parameter_dictionary, 
					"merged_output", 
					0 );

	if ( !*merged_output
	||   strcmp( merged_output, "merged_output" ) == 0 )
	{
		merged_output = "table";
	}

	if ( strcmp( merged_output, "gracechart" ) != 0
	&&   ( !*begin_date || strcmp( begin_date, "begin_date" ) == 0 )
	&&   ( !*end_date || strcmp( end_date, "end_date" ) == 0 ) )
	{
		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			aggregate_level = daily;
			date_range_period_of_record = 1;
		}
	}

	dictionary_get_index_data( 	&station_name,
					parameter_dictionary, 
					"station", 
					0 );

	dictionary_get_index_data( 	&display_count_yn,
					parameter_dictionary,
					"display_count_yn",
					0 );

	dictionary_get_index_data( 	&omit_output_if_any_missing_yn, 
					parameter_dictionary, 
					"omit_output_if_any_missing_yn", 
					0 );

	dictionary_get_index_data( 	&validation_level_string, 
					parameter_dictionary, 
					"validation_level", 
					0 );

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	station_name_list = 
		dictionary_get_index_list( 	parameter_dictionary,
						"station" );

	datatype_name_list = 
		dictionary_get_index_list( 	parameter_dictionary,
						"datatype" );


	if ( list_length( datatype_name_list ) == 0 )
	{
		if ( !*station_name )
		{
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
			printf( "<h3>ERROR: insufficient input.</h3>" );
			document_close();
			exit( 1 );
		}

		datatype_name_list = get_datatype4station_list(
				application_name,
				login_name,
				station_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				application_relative_source_directory(
					application_name ),
				merged_output );

		station_name_list =
			list_replicate_string_list(
				station_name,
				list_length( datatype_name_list ) );
	}
	else
	{
		datatype_name_list =
			appaserver_library_trim_carrot_number(
				datatype_name_list );
	}

	hydrology_library_with_list_get_clean_begin_end_date(
				&begin_date,
				&end_date,
				application_name,
				station_name_list,
				datatype_name_list );

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

	if ( strcmp( merged_output, "gracechart" ) != 0
	&& ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly ) )
	{
		if ( date_days_between(	begin_date,
					end_date ) > MIN_DAYS_FOR_SYNCH_CHECK )
		{
			if ( !expected_count_synchronized(
					application_name,
					station_name_list,
					datatype_name_list,
					begin_date,
					end_date ) )
			{
				data_collection_frequency_out_of_sync = 1;
				aggregate_level = daily;
			}
		}
	}

	if ( strcmp( merged_output, "stdout" ) != 0 )
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

	if ( strcmp( merged_output, "gracechart" ) != 0
	&& ! ( station_datatype_list =
				get_station_datatype_list(
					application_name,
					begin_date,
					end_date,
					station_name_list,
					datatype_name_list,
					aggregate_level,
					validation_level ) ) )
	{
		printf( "<h3>ERROR: insufficient input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( merged_output, "googlechart" ) == 0
	||   strcmp( merged_output, "easychart" ) == 0 )
	{
		if ( !merged_datasets_output_google_chart(
					application_name,
					begin_date,
					end_date,
					station_datatype_list,
					aggregate_level,
					validation_level,
					appaserver_parameter_file->
						document_root,
					omit_output_if_any_missing_yn ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}

		if ( data_collection_frequency_out_of_sync
		||   date_range_period_of_record )
		{
			char changed_to_daily_message[ 512 ];

			get_changed_to_daily_message(
				changed_to_daily_message,
				data_collection_frequency_out_of_sync,
				date_range_period_of_record );

			printf( "<h3>%s</h3>\n",
				changed_to_daily_message );
		}
	}
	else
	if ( strcmp( merged_output, "chart" ) == 0
	||   strcmp( merged_output, "gracechart" ) == 0 )
	{
		if ( !merged_datasets_output_gracechart(
					application_name,
					role_name,
					begin_date,
					end_date,
					station_name_list,
					datatype_name_list,
					aggregate_level,
					validation_level,
					appaserver_parameter_file->
						document_root,
					argv[ 0 ] ) )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}

		if ( data_collection_frequency_out_of_sync
		||   date_range_period_of_record )
		{
			char changed_to_daily_message[ 512 ];

			get_changed_to_daily_message(
				changed_to_daily_message,
				data_collection_frequency_out_of_sync,
				date_range_period_of_record );

			printf( "<h2>%s</h2>\n",
				changed_to_daily_message );
		}
	}
	else
	if ( strcmp( merged_output, "table" ) == 0 )
	{
		merged_datasets_output_table(
					station_datatype_list,
					aggregate_level,
					validation_level,
					omit_output_if_any_missing_yn,
					(*display_count_yn == 'y'),
					begin_date,
					end_date,
					data_collection_frequency_out_of_sync,
					date_range_period_of_record );
	}
	else
	if ( strcmp( merged_output, "spreadsheet" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char sys_string[ 1024 ];

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				PROCESS_NAME /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				begin_date,
				end_date,
				"csv" );

		output_filename = appaserver_link->output->filename;
		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<h3>ERROR: Cannot open output file %s</h3>\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		sprintf( sys_string,
			 "tr '|' ',' > %s",
			 output_filename );

		output_pipe = popen( sys_string, "w" );

		merged_datasets_output_transmit(
					output_pipe,
					station_datatype_list,
					aggregate_level,
					validation_level,
					omit_output_if_any_missing_yn,
					begin_date,
					end_date );

		pclose( output_pipe );

		printf( "<h1>Merged Datasets Transmission<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h2>\n" );
	
		if ( data_collection_frequency_out_of_sync
		||   date_range_period_of_record )
		{
			char changed_to_daily_message[ 512 ];

			get_changed_to_daily_message(
				changed_to_daily_message,
				data_collection_frequency_out_of_sync,
				date_range_period_of_record );

			printf( "<h2>%s</h2>\n",
				changed_to_daily_message );
		}

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( merged_output, "transmit" ) == 0
	||   strcmp( merged_output, "text_file" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char sys_string[ 1024 ];

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				PROCESS_NAME /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				begin_date,
				end_date,
				"txt" );

		output_filename = appaserver_link->output->filename;
		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<h3>ERROR: Cannot open output file %s</h3>\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

/*
	int right_justified_columns_from_right;
	right_justified_columns_from_right = list_length( station_name_list );
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' %d > %s",
		 	 right_justified_columns_from_right,
			 output_filename );
*/
		sprintf(sys_string,
		 	"tr '|' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( sys_string, "w" );

		merged_datasets_output_transmit(
					output_pipe,
					station_datatype_list,
					aggregate_level,
					validation_level,
					omit_output_if_any_missing_yn,
					begin_date,
					end_date );

		pclose( output_pipe );

		printf( "<h1>Merged Datasets Transmission<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h2>\n" );
	
		if ( data_collection_frequency_out_of_sync
		||   date_range_period_of_record )
		{
			char changed_to_daily_message[ 512 ];

			get_changed_to_daily_message(
				changed_to_daily_message,
				data_collection_frequency_out_of_sync,
				date_range_period_of_record );

			printf( "<h2>%s</h2>\n",
				changed_to_daily_message );
		}

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( merged_output, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' %d",
		 	 right_justified_columns_from_right );
*/
		sprintf(sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		merged_datasets_output_transmit(
					output_pipe,
					station_datatype_list,
					aggregate_level,
					validation_level,
					omit_output_if_any_missing_yn,
					begin_date,
					end_date );
		pclose( output_pipe );
	}

	if ( strcmp( merged_output, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

void merged_datasets_output_transmit(
					FILE *output_pipe,
					LIST *station_datatype_list,
					enum aggregate_level aggregate_level,
					enum validation_level validation_level,
					char *omit_output_if_any_missing_yn,
					char *begin_date,
					char *end_date )
{
	LIST *date_space_time_key_list;
	char *date_space_time;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	char buffer[ 512 ];
	HASH_TABLE *merged_date_space_time_key_hash_table;
	char aggregate_buffer[ 512 ];
	char validation_buffer[ 512 ];

	fprintf(output_pipe,
		"#Merged Datasets %s%s from %s to %s\n",
		format_initial_capital(
			aggregate_buffer,
			aggregate_level_get_string( aggregate_level ) ),
		format_initial_capital(
			validation_buffer,
			validation_level_get_title_string( validation_level ) ),
		begin_date,
		end_date );

	if ( !list_rewind( station_datatype_list ) ) return;

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

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		fprintf(
			output_pipe,
			"|%s/%s",
			station_datatype->station,
			station_datatype->datatype );

	} while( list_next( station_datatype_list ) );

	fprintf( output_pipe, "\n" );

	merged_date_space_time_key_hash_table =
		get_merged_date_space_time_key_hash_table(
			station_datatype_list );

	date_space_time_key_list =
		 hash_table_get_ordered_key_list(
			merged_date_space_time_key_hash_table );

	if ( !list_length( date_space_time_key_list ) )
	{
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		return;
	}

	list_rewind( date_space_time_key_list );

	do {
		date_space_time =
			list_get_pointer(
				date_space_time_key_list );

		if ( *omit_output_if_any_missing_yn == 'y' )
		{
			if ( any_missing_measurements(
					station_datatype_list,
					date_space_time ) )
			{
				continue;
			}

		}

		fprintf( output_pipe, "%s", date_space_time );

		list_rewind( station_datatype_list );

		do {
			station_datatype =
				list_get_pointer( station_datatype_list );

			measurement =
				hash_table_get_pointer(
					station_datatype->
						measurement_hash_table,
					date_space_time );

			if ( measurement && !measurement->is_null )
			{
				sprintf(buffer,
					"%.3lf",
					measurement->measurement_value );
			}
			else
			{
				strcpy( buffer, "null" );
			}

			fprintf( output_pipe, "|%s", buffer );

		} while( list_next( station_datatype_list ) );

		fprintf( output_pipe, "\n" );

	} while( list_next( date_space_time_key_list ) );	

} /* merged_datasets_output_transmit() */

void merged_datasets_output_table(
				LIST *station_datatype_list,
				enum aggregate_level aggregate_level,
				enum validation_level validation_level,
				char *omit_output_if_any_missing_yn,
				boolean display_count,
				char *begin_date,
				char *end_date,
				boolean data_collection_frequency_out_of_sync,
				boolean date_range_period_of_record )
{
	LIST *date_space_time_key_list;
	char *date_space_time;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char aggregation_buffer[ 512 ];
	char validation_buffer[ 512 ];
	char buffer[ 512 ];
	char initial_capital_buffer[ 512 ];
	HASH_TABLE *merged_date_space_time_key_hash_table;
	register int count = 0;
	char title[ 512 ];
	char changed_to_daily_message[ 512 ];

	if ( !list_rewind( station_datatype_list ) ) return;

	if ( data_collection_frequency_out_of_sync
	||   date_range_period_of_record )
	{
		get_changed_to_daily_message(
			changed_to_daily_message,
			data_collection_frequency_out_of_sync,
			date_range_period_of_record );
	}
	else
	{
		*changed_to_daily_message = '\0';
	}

	sprintf(title,
		"Merged Datasets %s%s from %s to %s",
		format_initial_capital(
			aggregation_buffer,
			aggregate_level_get_string( aggregate_level ) ),
		format_initial_capital(
			validation_buffer,
			validation_level_get_title_string( validation_level ) ),
		begin_date,
		end_date );

	html_table = new_html_table( title, changed_to_daily_message );
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

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		sprintf(buffer,
			"%s %s",
			station_datatype->station,
			format_initial_capital(
				initial_capital_buffer,
				station_datatype->datatype ) );

		list_append_pointer(
			heading_list,
			strdup( buffer ) );

		if ( display_count )
			list_append_pointer( heading_list, "Count" );
	
	} while( list_next( station_datatype_list ) );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns =
		list_length( heading_list ) - 1;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	merged_date_space_time_key_hash_table =
		get_merged_date_space_time_key_hash_table(
			station_datatype_list );

	date_space_time_key_list =
		 hash_table_get_ordered_key_list(
			merged_date_space_time_key_hash_table );

	if ( !list_length( date_space_time_key_list ) )
	{
		printf( "</table>\n" );
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		return;
	}

	list_rewind( date_space_time_key_list );

	do {
		date_space_time =
			list_get_pointer(
				date_space_time_key_list );

		if ( *omit_output_if_any_missing_yn == 'y' )
		{
			if ( any_missing_measurements(
					station_datatype_list,
					date_space_time ) )
			{
				continue;
			}

		}

		html_table_set_data(	html_table->data_list,
					strdup( date_space_time ) );

		list_rewind( station_datatype_list );

		do {
			station_datatype =
				list_get_pointer( station_datatype_list );

			measurement =
				hash_table_get_pointer(
					station_datatype->
						measurement_hash_table,
					date_space_time );

			if ( measurement && !measurement->is_null )
			{
				sprintf(buffer,
					"%.3lf",
					measurement->measurement_value );
			}
			else
			if ( measurement )
			{
				strcpy( buffer, "null" );
			}
			else
			{
				strcpy( buffer, "missing" );
			}

			html_table_set_data(	html_table->data_list,
						strdup( buffer ) );

			if ( display_count )
			{
				if ( measurement )
				{
					sprintf(buffer,
						"%d",
						measurement->count );
				}
				else
				{
					*buffer = '\0';
				}
	
				html_table_set_data(	html_table->data_list,
							strdup( buffer ) );
			}

		} while( list_next( station_datatype_list ) );

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
		html_table->data_list = list_new();

	} while( list_next( date_space_time_key_list ) );	

	html_table_close();

} /* merged_datasets_output_table() */

LIST *get_station_datatype_list(	char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_name_list,
					LIST *datatype_name_list,
					enum aggregate_level aggregate_level,
					enum validation_level validation_level )
{
	LIST *station_datatype_list;
	char sys_string[ 1024 ];
	char *station_name, *datatype_name;
	MERGED_STATION_DATATYPE *station_datatype;

	if ( !list_rewind( datatype_name_list ) )
		return (LIST *)0;

	station_datatype_list = list_new_list();
	if ( list_rewind( station_name_list ) )
	{
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
					validation_level );

			station_datatype =
				get_station_datatype(
					application_name,
					sys_string,
					station_name,
					datatype_name );

			list_append_pointer(	station_datatype_list,
						station_datatype );

			if ( !list_next( datatype_name_list ) ) break;
		} while( list_next( station_name_list ) );
	}

	return station_datatype_list;
} /* get_station_datatype_list() */

void build_sys_string(
		char *sys_string,
		char *application_name,
		char *begin_date,
		char *end_date,
		char *station_name,
		char *datatype_name,
		enum aggregate_level aggregate_level,
		enum validation_level validation_level )
{
	char where_clause[ 1024 ];
	char *validation_where;
	char intermediate_process[ 1024 ];
	char *pre_intermediate_process_sort;
	enum aggregate_statistic aggregate_statistic;
	char round_down_hourly_process[ 128 ];

	if ( aggregate_level == hourly )
	{
		sprintf(round_down_hourly_process,
			"sed 's/50\\%c/00\\%c/'",
			FOLDER_DATA_DELIMITER,
			FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( round_down_hourly_process, "cat" );
	}

	aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
			application_name,
			datatype_name,
			aggregate_statistic_none );

	if ( aggregate_level == real_time )
	{
		pre_intermediate_process_sort = "cat";
	}
	else
	if ( aggregate_level == half_hour
	||   aggregate_level == hourly
	||   aggregate_level == daily )
	{
		/* -------------------------------------------------- */
		/* real_time2aggregate_process.e needs grouped input. */
		/* -------------------------------------------------- */
		pre_intermediate_process_sort = "sort";
	}
	else
	{
		/* ------------------------------------ */
		/* Accumulate to the end of the period. */
		/* ------------------------------------ */
		pre_intermediate_process_sort = "sort -r";
	}

	validation_where =
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level );

	sprintf( where_clause,
 	"station = '%s' and 				      	"
 	"datatype = '%s' and 				      	"
 	"measurement_date >= '%s' and measurement_date <= '%s' 	"
	"%s							",
		station_name,
		datatype_name,
		begin_date,
		end_date,
		validation_where );

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
			FOLDER_DATA_DELIMITER,
	 		aggregate_level_get_string(
				aggregate_level ),
			end_date );
	}

	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		    "
	"			quick=yes		   |"
	"%s	 					   |"
	"%s	 					   |"
	"%s						   |"
	"cat						    ",
		application_name,
		SELECT_LIST,
		where_clause,
		pre_intermediate_process_sort,
		intermediate_process,
		round_down_hourly_process );

} /* build_sys_string() */

MERGED_STATION_DATATYPE *new_station_datatype(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	MERGED_STATION_DATATYPE *station_datatype;
	char sys_string[ 1024 ];
	char *results_string;

	station_datatype =
		(MERGED_STATION_DATATYPE *)
			calloc( 1, sizeof( MERGED_STATION_DATATYPE ) );

	station_datatype->station = station_name;
	station_datatype->datatype = datatype_name;
	station_datatype->
		measurement_hash_table =
			hash_table_new_hash_table(
				hash_table_large );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=bar_graph_yn,units	"
		"			folder=datatype			"
		"			where=\"datatype='%s'\"		",
		application_name,
		datatype_name );

	results_string = pipe2string( sys_string );

	if ( results_string )
	{
		char units_string[ 128 ];
		station_datatype->bar_graph = ( *results_string == 'y' );
		piece( units_string, FOLDER_DATA_DELIMITER, results_string, 1 );
		station_datatype->units = strdup( units_string );
		free( results_string );
	}

	return station_datatype;

} /* new_station_datatype() */

MERGED_STATION_DATATYPE *get_station_datatype(
				char *application_name,
				char *sys_string,
				char *station_name,
				char *datatype_name )
{
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char date_buffer[ 16 ];
	char time_buffer[ 16 ];
	char measurement_buffer[ 16 ];
	char key_buffer[ 128 ];
	char count_buffer[ 128 ];

	station_datatype =
		new_station_datatype(
			application_name,
			station_name,
			datatype_name );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( time_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( strcmp( time_buffer, "null" ) == 0 )
		{
			strcpy( key_buffer, date_buffer );
		}
		else
		{
			date_place_colon_in_time( time_buffer );

			sprintf( key_buffer,
				 "%s %s",
				 date_buffer,
				 time_buffer );
		}

		piece(	measurement_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		measurement = merged_measurement_new( strdup( key_buffer ) );

		if ( *measurement_buffer
		&&   timlib_strncmp( measurement_buffer, "null" ) != 0 )
		{
			measurement->measurement_value =
				atof( measurement_buffer );

			if ( piece(	count_buffer,
					FOLDER_DATA_DELIMITER,
					input_buffer,
					3 ) )
			{
				measurement->count = atoi( count_buffer );
			}
			else
			{
				measurement->count = 1;
			}
		}
		else
		{
			measurement->is_null = 1;
		}

		hash_table_set_pointer(
			station_datatype->measurement_hash_table,
			measurement->measurement_date_string,
			measurement );
	}

	pclose( input_pipe );

	return station_datatype;

} /* get_station_datatype() */

LIST *get_datatype4station_list(char *application_name,
				char *login_name,
				char *station,
				char *appaserver_mount_point,
				char *relative_source_directory,
				char *merged_output )
{
	char sys_string[ 1024 ];

	if ( strcmp( merged_output, "googlechart" ) == 0
	||   strcmp( merged_output, "gracechart" ) == 0 )
	{
		sprintf( sys_string,
		 	"%s/%s/datatype_chart4station_list.sh %s %s %s",
		 	appaserver_mount_point,
		 	relative_source_directory,
		 	application_name,
		 	login_name,
		 	station );
	}
	else
	{
		sprintf( sys_string,
		 	"%s/%s/datatype4station_list.sh %s %s %s",
		 	appaserver_mount_point,
		 	relative_source_directory,
		 	application_name,
		 	login_name,
		 	station );
	}

	return pipe2list( sys_string );

} /* get_datatype4station_list() */

HASH_TABLE *get_merged_date_space_time_key_hash_table(
				LIST *station_datatype_list )
{
	HASH_TABLE *merged_date_space_time_key_hash_table;
	MERGED_STATION_DATATYPE *station_datatype;
	LIST *date_space_time_key_list;
	char *date_space_time_key;

	merged_date_space_time_key_hash_table =
		hash_table_new_hash_table(
			hash_table_large );

	if ( !list_rewind( station_datatype_list ) )
		return merged_date_space_time_key_hash_table;

	do {
		station_datatype =
			list_get_pointer(
				station_datatype_list );

		date_space_time_key_list =
			hash_table_get_key_list(
				station_datatype->measurement_hash_table );

		if ( list_rewind( date_space_time_key_list ) )
		{
			do {
				date_space_time_key =
					list_get_pointer(
						date_space_time_key_list );

				hash_table_set_pointer(
					merged_date_space_time_key_hash_table,
					date_space_time_key,
					"" );

			} while( list_next( date_space_time_key_list ) );
		}
	} while( list_next( station_datatype_list ) );

	return merged_date_space_time_key_hash_table;

} /* get_merged_date_space_time_key_hash_table() */

boolean merged_datasets_output_gracechart(
				char *application_name,
				char *role_name,
				char *begin_date,
				char *end_date,
				LIST *station_name_list,
				LIST *datatype_name_list,
				enum aggregate_level aggregate_level,
				enum validation_level validation_level,
				char *document_root_directory,
				char *argv_0 )
{
	char station_datatype_input_buffer[ 512 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char legend[ 128 ];
	LIST *date_space_time_key_list;
	char *date_space_time;
	char buffer[ 512 ];
	char *station_name;
	char *datatype_name;
	char sys_string[ 1024 ];
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;

	strcpy( title, 
		"Merged Datasets Gracechart" );

	sprintf(sub_title,
		"%s%s from %s to %s",
		aggregate_level_get_string( aggregate_level ),
		validation_level_get_title_string( validation_level ),
		begin_date,
		end_date );

	format_initial_capital( sub_title, sub_title );

	grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	grace->dataset_no_cycle_color = 1;

	if ( !grace_set_begin_end_date(
		grace,
		begin_date,
		end_date ) )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}


	if (	list_length( station_name_list ) !=
	 	list_length( datatype_name_list ) )
	{
		printf(
"<h3>An internal error occurred. The station_name_list should be paired with the datatype_name_list. However length( station_name_list ) = %d != length( datatype_name_list ) = %d. The station_name_list is (%s) and the datatype_name_list is (%s).</h3>\n",
		list_length( station_name_list ),
		list_length( datatype_name_list ),
		list_display( station_name_list ),
		list_display( datatype_name_list ) );
		return 0;
	}

	if ( !list_rewind( datatype_name_list ) ) return 0;
	list_rewind( station_name_list );

	grace_graph = grace_new_grace_graph();
	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;
	list_append_pointer( grace->graph_list, grace_graph );

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
				validation_level );

		station_datatype =
			get_station_datatype(
				application_name,
				sys_string,
				station_name,
				datatype_name );

		grace_datatype =
			grace_new_grace_datatype(
				station_name,
				datatype_name );

		sprintf(legend,
			"%s/%s (%s)",
			station_datatype->station,
			station_datatype->datatype,
			station_datatype->units );

		strcpy(	legend,
			format_initial_capital( buffer, legend ) );

		grace_datatype->legend = strdup( legend );

		if ( station_datatype->bar_graph )
		{
			grace_datatype->datatype_type_bar_xy_xyhilo =
				"bar";
			grace_datatype->line_linestyle = 0;
		}
		else
		{
			grace_datatype->datatype_type_bar_xy_xyhilo =
				"xy";
		}

		list_append_pointer(	grace_graph->datatype_list,
					grace_datatype );

		date_space_time_key_list =
			hash_table_get_ordered_key_list(
				station_datatype->
					measurement_hash_table );

		if ( !list_rewind( date_space_time_key_list ) )
		{
			list_next( datatype_name_list );
			continue;
		}

		do {
			date_space_time =
				list_get_pointer(
					date_space_time_key_list );

			measurement =
				hash_table_get_pointer(
					station_datatype->
						measurement_hash_table,
					date_space_time );

			if ( !measurement || measurement->is_null ) continue;

			sprintf(station_datatype_input_buffer,
		 		"%s|%s|%s|%.3lf",
		 		station_name,
		 		datatype_name,
		 		date_space_time,
				measurement->measurement_value );

			grace_set_string_to_point_list(
				grace->graph_list, 
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				station_datatype_input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

		} while( list_next( date_space_time_key_list ) );	

		list_next( datatype_name_list );

	} while( list_next( station_name_list ) );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
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
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<h2>Warning: no graphs to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	grace_move_legend_bottom_left(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			grace->landscape_mode );

	grace_increase_legend_char_size(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.15 );

	/* Make the graph wider -- 95% of the page */
	/* --------------------------------------- */
	grace_set_view_maximum_x(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.95 );

	/* Move the legend down a little */
	/* ----------------------------- */
	grace_lower_legend(	grace->graph_list,
				0.04 );

	if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
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
		printf( "<h2>No data for selected parameters.</h2>\n" );
		document_close();
		exit( 0 );
	}
	else
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );

	}

	return 1;

} /* merged_datasets_output_gracechart() */

boolean merged_datasets_output_google_chart(
					char *application_name,
					char *begin_date,
					char *end_date,
					LIST *station_datatype_list,
					enum aggregate_level aggregate_level,
					enum validation_level validation_level,
					char *document_root_directory,
					char *omit_output_if_any_missing_yn )
{
	LIST *date_space_time_key_list;
	char *date_space_time;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	HASH_TABLE *merged_date_space_time_key_hash_table;
	char validation_buffer[ 512 ];
	char aggregation_buffer[ 512 ];
	GOOGLE_OUTPUT_CHART *google_chart;
	char *output_filename;
	char *prompt_filename;
	FILE *output_file;
	char chart_title[ 256 ];
	char google_datatype_name[ 128 ];

	google_chart =
		google_output_chart_new(
			GOOGLE_CHART_POSITION_LEFT,
			GOOGLE_CHART_POSITION_TOP,
			GOOGLE_CHART_WIDTH,
			GOOGLE_CHART_HEIGHT );

	sprintf(chart_title,
		"Merged Datasets %s%s from %s to %s",
		format_initial_capital(
			aggregation_buffer,
			aggregate_level_get_string( aggregate_level ) ),
		format_initial_capital(
			validation_buffer,
			validation_level_get_title_string( validation_level ) ),
		begin_date,
		end_date );

	if ( !list_rewind( station_datatype_list ) ) return 0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		sprintf(google_datatype_name,
			"%s/%s",
			station_datatype->station,
			station_datatype->datatype );

		list_append_pointer(
			google_chart->datatype_name_list,
			strdup( google_datatype_name ) );

	} while( list_next( station_datatype_list ) );

	merged_date_space_time_key_hash_table =
		get_merged_date_space_time_key_hash_table(
			station_datatype_list );

	date_space_time_key_list =
		 hash_table_get_ordered_key_list(
			merged_date_space_time_key_hash_table );

	if ( !list_length( date_space_time_key_list ) )
	{
		printf( "<h3>ERROR: insufficient data to output</h3>\n" );
		return 0;
	}

	list_rewind( date_space_time_key_list );

	do {
		date_space_time =
			list_get_pointer(
				date_space_time_key_list );

		if ( *omit_output_if_any_missing_yn == 'y' )
		{
			if ( any_missing_measurements(
					station_datatype_list,
					date_space_time ) )
			{
				continue;
			}

		}

		list_rewind( station_datatype_list );

		do {
			station_datatype =
				list_get_pointer(
					station_datatype_list );

			sprintf(google_datatype_name,
				"%s/%s",
				station_datatype->station,
				station_datatype->datatype );

			measurement =
				hash_table_get_pointer(
					station_datatype->
						measurement_hash_table,
					date_space_time );

			if ( measurement && !measurement->is_null )
			{
				google_timeline_set_point(
					google_chart->timeline_list,
					google_chart->datatype_name_list,
					date_space_time /* date_string */,
					(char *)0 /* time_hhmm */,
					google_datatype_name,
					measurement->measurement_value );
			}

		} while( list_next( station_datatype_list ) );

	} while( list_next( date_space_time_key_list ) );	

	appaserver_link_get_pid_filename(
			&output_filename,
			&prompt_filename,
			application_name,
			document_root_directory,
			getpid(),
			PROCESS_NAME /* filename_stem */,
			"html" /* extension */ );

	output_file = fopen( output_filename, "w" );

	if ( !output_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			output_filename );
		exit( 1 );
	}

	document_output_html_stream( output_file );

	fprintf( output_file, "<head>\n" );

	google_chart_include( output_file );

	google_chart_output_visualization_non_annotated(
				output_file,
				google_chart->google_chart_type,
				google_chart->timeline_list,
				google_chart->barchart_list,
				google_chart->datatype_name_list,
				chart_title,
				(char *)0 /* yaxis_label */,
				google_chart->width,
				google_chart->height,
				google_chart->background_color,
				google_chart->legend_position_bottom,
				0 /* not chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				aggregate_level,
				google_chart->chart_number );

	fprintf( output_file, "</head>\n" );
	fprintf( output_file, "<body>\n" );

	google_chart_float_chart(
				output_file,
				chart_title,
				google_chart->width,
				google_chart->height,
				google_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_chart->chart_number );

	fprintf( output_file, "</body>\n" );
	fprintf( output_file, "</html>\n" );

	fclose( output_file );

	google_chart_output_prompt(
		application_name,
		prompt_filename,
		PROCESS_NAME,
		(char *)0 /* where_clause */ );

	return 1;

} /* merged_datasets_output_google_chart() */

boolean get_has_bar_graph( LIST *station_datatype_list )
{
	MERGED_STATION_DATATYPE *station_datatype;

	if ( !list_rewind( station_datatype_list ) ) return 0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		if ( station_datatype->bar_graph ) return 1;
	} while( list_next( station_datatype_list ) );

	return 0;
} /* get_has_bar_graph() */

boolean any_missing_measurements(	LIST *station_datatype_list,
					char *date_time_string )
{
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;

	if ( !list_rewind( station_datatype_list ) ) return 1;

	do {
		station_datatype =
			list_get_pointer( station_datatype_list );

		measurement =
			hash_table_get_pointer(
				station_datatype->
					measurement_hash_table,
				date_time_string );

		if ( !measurement || measurement->is_null )
		{
			return 1;
		}

	} while( list_next( station_datatype_list ) );

	return 0;

} /* any_missing_measurements() */

void get_changed_to_daily_message(
			char *changed_to_daily_message,
			boolean data_collection_frequency_out_of_sync,
			boolean date_range_period_of_record )
{
	strcpy(	changed_to_daily_message,
		"(Aggregate Level Changed to Daily" );

	if ( data_collection_frequency_out_of_sync )
	{
		strcat( changed_to_daily_message,
			": Data collection frequency not synchronized)" );
	}
	else
	if ( date_range_period_of_record )
	{
		strcat( changed_to_daily_message,
			": Choosing period of record)" );
	}
	else
	{
		strcat( changed_to_daily_message, ")" );
	}
} /* get_changed_to_daily_message() */

#ifdef NOT_DEFINED
int get_days_between(	char *begin_date_string,
			char *end_date_string )
{
	JULIAN *begin_date;
	JULIAN *end_date;
	int days_between;

	begin_date = julian_yyyy_mm_dd_new( begin_date_string );
	end_date = julian_yyyy_mm_dd_new( end_date_string );

	days_between = (int)end_date->current - (int)begin_date->current + 1;
	julian_free( begin_date );
	julian_free( end_date );
	return days_between;
} /* get_days_between() */
#endif

