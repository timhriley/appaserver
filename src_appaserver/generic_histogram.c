/* src_appaserver/generic_histogram.c				*/
/* ------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "grace.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "column.h"
#include "process_generic_output.h"

/* Constants */
/* --------- */
#define DELIMITER	'^'

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *post_dictionary;
	char *sys_string;
	char histogram_sys_string[ 1024 ];
	char *process_set_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *where_clause = {0};
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *units_label;
	FILE *histogram_pipe;
	FILE *input_stream;
	char input_record[ 128 ];
	char grace_histogram_filename[ 256 ];
	char title[ 256 ];
	char sub_title[ 256 ];
	char input_buffer[ 256 ];
	char measurement[ 128 ];
	char ftp_agr_filename[ 256 ];
	char ftp_output_filename[ 256 ];
	char output_filename[ 256 ];
	int value_piece = 0;
	char *heading_string;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s ignored process_set parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_set_name = argv[ 2 ];
	post_dictionary =
		dictionary_string2dictionary( argv[ 3 ] );

	appaserver_parameter_file = appaserver_parameter_file_new();

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			0 /* accumulate_flag */ );

	process_generic_output->value_folder->datatype =
		process_generic_datatype_new(
			application_name,
			process_generic_output->
				value_folder->
					foreign_folder->
						foreign_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						datatype_folder_name,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_aggregation_sum,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_bar_graph,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_scale_graph_zero,
			process_generic_output->
				value_folder->
					units_folder_name,
			post_dictionary,
			0 /* dictionary_index */ );

	if ( !process_generic_output->value_folder->datatype )
	{
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h3>ERROR: insufficient input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !	process_generic_output->
		value_folder->
		datatype->
		foreign_attribute_data_list )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get foreign_attribute_data_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_name =
		process_generic_output_get_process_name(
			process_set_name,
			post_dictionary );

	document_quick_output_body(	application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

	if ( !process_generic_output_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					process_generic_output->
						value_folder->
						value_folder_name,
					process_generic_output->
						value_folder->
						date_attribute_name,
					post_dictionary ) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date_string,
			&end_date_string,
			&where_clause,
			&units_label,
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			(int *)0 /* date_piece */,
			(int *)0 /* time_piece */,
			&value_piece,
			(int *)0 /* length_select_list */,
			application_name,
			process_generic_output,
			post_dictionary,
			DELIMITER,
			aggregate_level,
			aggregate_statistic,
			0 /* append_low_high */,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */,
			process_generic_output->accumulate );

/*
	sprintf( grace_histogram_filename,
		 "%s/%s/grace_histogram_%d.dat",
		 appaserver_parameter_file->appaserver_mount_point,
		 application_name,
		 getpid() );
*/
	sprintf( grace_histogram_filename,
		 "%s/grace_histogram_%d.dat",
		 appaserver_parameter_file_get_data_directory(),
		 getpid() );

	sprintf(	title,
			"%s %s Histogram", 
			process_generic_output->
				value_folder->
				value_folder_name,
			aggregate_level_get_string(
				aggregate_level ) );

	format_initial_capital( title, title );

	sprintf(	sub_title,
			"%s From: %s To: %s",
			list_display_delimited(
				process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
				'/' ),
			begin_date_string,
			end_date_string );

	format_initial_capital( sub_title, sub_title );

	heading_string =
	 	process_generic_output_get_heading_string(
			process_generic_output->value_folder,
			DELIMITER,
			aggregate_level );

	sprintf(	histogram_sys_string,
			"grace_histogram.e %s %d \"%s\" \"%s\" \"%s\" > %s",
			application_name,
			getpid(),
			title,
			sub_title,
			"",
			grace_histogram_filename );

	input_stream = popen( sys_string, "r" );
	histogram_pipe = popen( histogram_sys_string, "w" );
	while( get_line( input_record, input_stream ) )
	{
		if ( timlib_strncmp( input_record, heading_string ) == 0 )
		{
			continue;
		}

		piece(	measurement,
			DELIMITER,
			input_record,
			value_piece );
		fprintf( histogram_pipe, "%s\n", measurement );
	}
	pclose( histogram_pipe );
	pclose( input_stream );

	input_stream = fopen( grace_histogram_filename, "r" );
	if ( !input_stream )
	{
		fprintf( stderr,
			 "%s/%s(): cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 grace_histogram_filename );
		exit( 1 );
	}

	if ( !get_line( input_buffer, input_stream ) )
	{
		fprintf( stderr,
			 "%s/%s(): unexpected end of file in %s\n",
			 __FILE__,
			 __FUNCTION__,
			 grace_histogram_filename );
		exit( 1 );
	}

	column( ftp_agr_filename, 0, input_buffer );
	column( ftp_output_filename, 1, input_buffer );
	column( output_filename, 2, input_buffer );

	fclose( input_stream );
	sprintf( histogram_sys_string, "rm -f %s", grace_histogram_filename );
	if ( system( histogram_sys_string ) ){};

	grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				0 /* not with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					process_generic_output->
						value_folder->
							value_folder_name,
					1 ) );
	document_close();
	return 0;
}

