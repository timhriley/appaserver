/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_statistics_report.c	*/
/* -----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "sql.h"
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
#include "boolean.h"
#include "process_generic.h"
#include "application.h"
#include "statistics_weighted.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void generic_output_statistics(
			char *application_name,
			char *input_system_string,
			pid_t process_id,
			int date_piece,
			int value_piece,
			char *value_folder_name,
			char *date_attribute_name,
			char *value_attribute_name,
			char *units,
			char *units_converted,
			char *where_clause );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	PROCESS_GENERIC *process_generic;
	char title[ 128 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
	"Usage: %s process_set process dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 1 ];
	process_name = argv[ 2 ];

	post_dictionary =
		dictionary_string2dictionary(
			argv[ 3 ] );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->appaserver_mount_point );

	process_generic =
		process_generic_fetch(
			process_set_name,
			process_name,
			(char *)0 /* output_medium_string */,
			post_dictionary );

	if ( !process_generic )
	{
		printf( "<h3>Insufficient input</h3>\n" );
		document_close();
		exit( 0 );
	}

	printf(	"<h1>%s</h1>\n",
		format_initial_capital(
			title,
			process_generic->process_name ) );

	printf( "<h2>\n" );
	printf( "%s<br>\n", process_generic->process_generic_subtitle );
	printf( "</h2>\n" );
	fflush( stdout );

	generic_output_statistics(
		application_name,
		process_generic->process_generic_system_string,
		process_generic->parameter->process_id,
		process_generic->parameter->date_piece,
		process_generic->parameter->value_piece,
		process_generic->value_folder->value_folder_name,
		process_generic->value_folder->date_attribute_name,
		process_generic->value_folder->value_attribute_name,
		process_generic->value_folder->datatype->unit,
		process_generic->parameter->units_converted,
		process_generic->process_generic_where );

	document_close();

	process_increment_execution_count(
		application_name,
		process_generic->process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;
}

void generic_output_statistics(
			char *application_name,
			char *input_system_string,
			pid_t process_id,
			int date_piece,
			int value_piece,
			char *value_folder_name,
			char *date_attribute_name,
			char *value_attribute_name,
			char *unit,
			char *units_converted,
			char *where_clause )
{
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_value;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_date;
	FILE *input_pipe;
	FILE *statistics_output_pipe;
	FILE *date_min_max_output_pipe;
	FILE *input_file;
	char sys_string[ 1024 ];
	char statistics_temp_file[ 128 ];
	char date_min_max_temp_file[ 128 ];
	char input_buffer[ 1024 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char label[ 128 ];
	char piece_buffer[ 128 ];

	input_pipe = popen( input_system_string, "r" );

	sprintf(statistics_temp_file,
		"/tmp/measurement_statistics_%d",
		process_id );

	sprintf(sys_string,
		"statistics_weighted.e '%c' 0 > %s",
		SQL_DELIMITER,
		statistics_temp_file );

	statistics_output_pipe = popen( sys_string, "w" );

	sprintf(date_min_max_temp_file,
		"/tmp/date_min_max_%d",
		process_id );

	sprintf(sys_string,
		"date_min_max.e 0 '%c' ignored > %s",
		SQL_DELIMITER,
		date_min_max_temp_file );

	date_min_max_output_pipe = popen( sys_string, "w" );

	while( string_input( input_buffer, input_pipe, 1024) )
	{
		if ( *input_buffer == '#' ) continue;

		piece(	date_string,
			SQL_DELIMITER,
			input_buffer,
			date_piece );

		piece(	value_string,
			SQL_DELIMITER,
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
			value_folder_name );

	query_attribute_statistics_list->list = list_new_list();

	query_attribute_statistics_measurement_value =
		query_attribute_statistics_new(
			value_attribute_name,
			"float",
			0 /* input_piece */ );

	query_attribute_statistics_measurement_date =
		query_attribute_statistics_new(
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

	while( string_input( input_buffer, input_file, 1024) )
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
		value_attribute_name,
		process_generic_units_label(
			unit,
			units_converted,
			aggregate_statistic_none ) );

	query_attribute_statistics_list_output_table(
		value_folder_name,
		where_clause,
		query_attribute_statistics_list->list,
		application_name );

	sprintf( sys_string, "/bin/rm -f %s", statistics_temp_file );
	if ( system( sys_string ) ){};

	sprintf( sys_string, "/bin/rm -f %s", date_min_max_temp_file );
	if ( system( sys_string ) ){};
}

