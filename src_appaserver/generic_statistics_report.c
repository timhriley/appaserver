/* ---------------------------------------------------	*/
/* src_appaserver/generic_statistics_report.c		*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "process.h"
#include "boolean.h"
#include "process_generic_output.h"
#include "application.h"
#include "statistics_weighted.h"

/* Constants */
/* --------- */
#define DATE_PIECE					0
#define VALUE_PIECE					1

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date = {0};
	char *end_date = {0};
	char date_string[ 128 ];
	char value_string[ 128 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	char label[ 256 ];
	FILE *input_pipe;
	FILE *statistics_output_pipe;
	FILE *date_min_max_output_pipe;
	FILE *input_file;
	char statistics_temp_file[ 128 ];
	char date_min_max_temp_file[ 128 ];
	DOCUMENT *document;
	DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *where_clause;
	int process_id = getpid();
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_value;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_date;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *sys_string;
	char *process_set_name;
	char *process_name;
	char *units_label;
	int datatype_entity_piece;
	int datatype_piece;
	int date_piece;
	int value_piece;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
	"Usage: %s ignored process_set dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 2 ];

	post_dictionary =
		dictionary_string2dictionary( argv[ 3 ] );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	dictionary_get_index_data(
				&process_name,
				post_dictionary,
				"process",
				0 );

	if ( !*process_set_name
	||   strcmp( process_set_name, "process_set" ) == 0
	||   strcmp( process_set_name, "$process_set" ) == 0 )
	{
		process_generic_output =
			process_generic_output_new(
				application_name,
				process_name,
				(char *)0 /* process_set_name */,
				0 /* accumulate */ );
	}
	else
	{
		process_generic_output =
			process_generic_output_new(
				application_name,
				(char *)0 /* process_name */,
				process_set_name,
				0 /* accumulate */ );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	dictionary_get_index_data( 	&begin_date,
					post_dictionary,
					"begin_date",
					0 );

	dictionary_get_index_data( 	&end_date,
					post_dictionary,
					"end_date",
					0 );

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date/* in/out */,
			&end_date/* in/out */,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name,
			post_dictionary
				/* query_removed_post_dictionary */ ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h1>Statistics Report\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h1>\n" );
	fflush( stdout );

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date,
			&end_date,
			&where_clause,
			&units_label,
			&datatype_entity_piece,
			&datatype_piece,
			&date_piece,
			(int *)0 /* time_piece */,
			&value_piece,
			(int *)0 /* length_select_list */,
			application_name,
			process_generic_output,
			post_dictionary,
			FOLDER_DATA_DELIMITER,
			real_time,
			aggregate_statistic_none,
			0 /* append_low_high */,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */,
			process_generic_output->accumulate );

/*
fprintf(	stderr,
		"%s/%s()/%d: got sys_string = %s\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		sys_string );
*/

	input_pipe = popen( sys_string, "r" );

	sprintf(statistics_temp_file,
		"/tmp/measurement_statistics_%d",
		process_id );

	sprintf(sys_string,
		"statistics_weighted.e '%c' 0 > %s",
		FOLDER_DATA_DELIMITER,
		statistics_temp_file );

	statistics_output_pipe = popen( sys_string, "w" );

	sprintf(date_min_max_temp_file,
		"/tmp/date_min_max_%d",
		process_id );

	sprintf(sys_string,
		"date_min_max.e %d '%c' '%s' > %s",
		0 /* date_piece */,
		FOLDER_DATA_DELIMITER,
		appaserver_parameter_file->database_management_system,
		date_min_max_temp_file );

	date_min_max_output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( *input_buffer == '#' ) continue;

		piece(	date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			date_piece );

		piece(	value_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			value_piece );

		fprintf( statistics_output_pipe,
			 "%s\n",
			 value_string );

		fprintf( date_min_max_output_pipe,
			 "%s\n",
			 date_string );
	}
	pclose( input_pipe );
	pclose( statistics_output_pipe );
	pclose( date_min_max_output_pipe );

	/* Use query_attribute_statistics_list for the output */
	/* -------------------------------------------------- */
	query_attribute_statistics_list =
		query_attribute_statistics_list_new(
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name );

	query_attribute_statistics_list->list = list_new_list();

	query_attribute_statistics_measurement_value =
		query_attribute_statistics_new(
					process_generic_output->
						value_folder->
						value_attribute_name,
					"float",
					0 /* input_piece */ );

	query_attribute_statistics_measurement_date =
		query_attribute_statistics_new(
					process_generic_output->
						value_folder->
						date_attribute_name,
					"date",
					0 /* input_piece */ );

	list_append_pointer(	query_attribute_statistics_list->list,
				query_attribute_statistics_measurement_value );

	list_append_pointer(	query_attribute_statistics_list->list,
				query_attribute_statistics_measurement_date );

	/* Get statistics */
	/* -------------- */
	if ( !statistics_weighted_fetch_statistics(
		&query_attribute_statistics_measurement_value->average,
		&query_attribute_statistics_measurement_value->
			standard_deviation,
		(double *)0 /* standard_error_of_mean */,
		&query_attribute_statistics_measurement_value->count,
		&query_attribute_statistics_measurement_value->sum,
		&query_attribute_statistics_measurement_value->
			coefficient_of_variation,
		&query_attribute_statistics_measurement_value->minimum,
		&query_attribute_statistics_measurement_value->median,
		&query_attribute_statistics_measurement_value->maximum,
		&query_attribute_statistics_measurement_value->range,
		&query_attribute_statistics_measurement_value->percent_missing,
		statistics_temp_file ) )
	{
		fprintf( stderr,
"ERROR in %s/%s/%d(): cannot fetch statistics using filename = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_temp_file );
		exit( 1 );
	}

	/* Get date min/max */
	/* ---------------- */
	input_file = fopen( date_min_max_temp_file, "r" );

	while( get_line( input_buffer, input_file ) )
	{
		piece( label, ':', input_buffer, 0 );
	
		if ( !piece( piece_buffer, ':', input_buffer, 1 ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot piece(1) in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}
	
		if ( strcmp( label, DATE_MINIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				begin_date = strdup( piece_buffer );
		}
		else
		if ( strcmp( label, DATE_MAXIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				end_date = strdup( piece_buffer );
		}
		else
		if ( strcmp( label, DATE_COUNT ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				count = atoi( piece_buffer );
		}
		else
		if ( strcmp( label, DATE_PERCENT_MISSING ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				percent_missing = atof( piece_buffer );
		}
		else
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: invalid label = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 label );
			exit( 1 );
		}
	}
	fclose( input_file );

	query_attribute_statistics_list_set_units_string(
		query_attribute_statistics_list->list,
		process_generic_output->value_folder->value_attribute_name,
		units_label );

	query_attribute_statistics_list_output_table(
			process_generic_output->value_folder->value_folder_name,
			where_clause,
			query_attribute_statistics_list->list,
			application_name );

	sprintf( sys_string, "/bin/rm -f %s", statistics_temp_file );
	system( sys_string );

	sprintf( sys_string, "/bin/rm -f %s", date_min_max_temp_file );
	system( sys_string );

	document_close();
	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */

