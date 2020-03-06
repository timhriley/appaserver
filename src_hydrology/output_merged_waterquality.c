/* ----------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_merged_waterquality.c 	*/
/* ----------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------- 	*/

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
#include "application_constants.h"
#include "appaserver_link_file.h"
#include "google_chart.h"
#include "dictionary_appaserver.h"
#include "merged.h"

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING	20

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void output_merged_waterquality_textfile(
				FILE *output_pipe,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_space_time_key_list,
				char delimiter );

void output_merged_waterquality_spreadsheet(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_space_time_key_list );

void output_merged_waterquality_table(
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_space_time_key_list );

void output_merged_waterquality(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input );

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_date;
	char *end_date;
	char *output_medium;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	MERGED_INPUT *merged_input;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
	"Usage: %s process begin_date end_date output_medium dictionary\n",
			argv[ 0 ] );

		fprintf( stderr,
	"Note: output_medium = {table,spreadsheet,gracechart,stdout}\n" );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	begin_date = argv[ 2 ];
	end_date = argv[ 3 ];
	output_medium = argv[ 4 ];

	parameter_dictionary =
		output_merged_parameter_dictionary(
			argv[ 5 ]
				/* parameter_dictionary_string */ );


	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		char buffer[ 128 ];

		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n<h2>",
			format_initial_capital(
				buffer,
				process_name ) );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		printf( "</h2>\n" );
		fflush( stdout );
	}

	if ( ! ( merged_input = merged_input_new( parameter_dictionary ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: merged_input_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	output_merged_waterquality(
		application_name,
		process_name,
		appaserver_parameter_file->
			document_root,
		begin_date,
		end_date,
		output_medium,
		merged_input );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string )
{
	DICTIONARY *parameter_dictionary;

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

	return parameter_dictionary;

} /* output_merged_parameter_dictionary() */

void output_merged_waterquality(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input )
{
	MERGED *merged;
	HASH_TABLE *key_hash_table;
	LIST *date_space_time_key_list;

	merged = merged_new( merged_input );

	merged_measurement_hash_table(
		application_name,
		begin_date,
		end_date,
		merged->hydrology_station_datatype_list );

	merged_results_hash_table(
		begin_date,
		end_date,
		merged->waterquality_station_datatype_list );

	/* Get the time sequence from waterquality */
	/* --------------------------------------- */
	key_hash_table =
		merged_key_hash_table(
			merged->waterquality_station_datatype_list );

	date_space_time_key_list =
		 hash_table_get_ordered_key_list(
			key_hash_table );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_merged_waterquality_table(
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_space_time_key_list );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_merged_waterquality_spreadsheet(
			application_name,
			process_name,
			document_root_directory,
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_space_time_key_list );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		LIST *heading_list;

		output_pipe = popen( "cat", "w" );

		heading_list =
			merged_heading_list(
				merged->waterquality_station_datatype_list,
				merged->hydrology_station_datatype_list );

		fprintf( output_pipe,
			 "%s\n",
			 list_display_delimited( heading_list, ',' ) );

		output_merged_waterquality_textfile(
			output_pipe,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_space_time_key_list,
			',' /* delimiter */ );

		pclose( output_pipe );
	}

} /* output_merged_waterquality() */

void output_merged_waterquality_table(
			char *begin_date,
			char *end_date,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_space_time_key_list )
{
	LIST *heading_list;
	char sub_title[ 512 ];
	char sys_string[ 1024 ];
	FILE *output_pipe;

	sprintf(sub_title,
		"Merged from %s to %s",
		begin_date,
		end_date );

	heading_list =
		merged_heading_list(
			waterquality_station_datatype_list,
			hydrology_station_datatype_list );

	sprintf( sys_string,
		 "html_table.e \"^%s\" \"%s\" ',' right",
		 sub_title,
		 list_display_delimited( heading_list, ',' ) );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( date_space_time_key_list ) )
	{
		pclose( output_pipe );
		return;
	}

	output_merged_waterquality_textfile(
		output_pipe,
		waterquality_station_datatype_list,
		hydrology_station_datatype_list,
		date_space_time_key_list,
		',' /* delimiter */ );

	pclose( output_pipe );

} /* output_merged_waterquality_table() */

void output_merged_waterquality_textfile(
			FILE *output_pipe,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_space_time_key_list,
			char delimiter )
{
	char *date_space_time;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	char buffer[ 512 ];

	if ( !list_rewind( date_space_time_key_list ) ) return;

	do {
		date_space_time =
			list_get_pointer(
				date_space_time_key_list );

		fprintf( output_pipe, "%s", date_space_time );

		/* Output waterquality */
		/* ------------------- */
		if ( list_rewind( waterquality_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 waterquality_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_space_time );

				if ( measurement && !measurement->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						measurement->
							measurement_value );
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

				fprintf( output_pipe,
					 "%c%s",
					 delimiter,
					 buffer );

			} while( list_next(
					waterquality_station_datatype_list ) );

		} /* If */

		/* Output hydrology */
		/* ---------------- */
		if ( list_rewind( hydrology_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 hydrology_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_space_time );

				if ( measurement && !measurement->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						measurement->
							measurement_value );
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

				fprintf( output_pipe,
					 "%c%s",
					 delimiter,
					 buffer );

			} while( list_next(
					hydrology_station_datatype_list ) );

		} /* If */

		fprintf( output_pipe, "\n" );

	} while( list_next( date_space_time_key_list ) );	

} /* output_merged_waterquality_textfile() */

void output_merged_waterquality_spreadsheet(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_space_time_key_list )
{
	char sys_string[ 1024 ];
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_pipe;
	FILE *output_file;
	APPASERVER_LINK_FILE *appaserver_link_file;
	LIST *heading_list;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

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
		 "cat > %s",
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	heading_list =
		merged_heading_list(
			waterquality_station_datatype_list,
			hydrology_station_datatype_list );

	fprintf( output_pipe,
		 "%s\n",
		 list_display_delimited( heading_list, ',' ) );

	output_merged_waterquality_textfile(
				output_pipe,
				waterquality_station_datatype_list,
				hydrology_station_datatype_list,
				date_space_time_key_list,
				',' /* delimiter */ );

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_merged_waterquality_spreadsheet() */
