/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_googlechart.c	*/
/* ----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "application.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "process_generic_output.h"
#include "appaserver.h"
#include "google_chart.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *get_title(	char *value_folder_name,
			LIST *foreign_attribute_data_list,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date,
			char *end_date,
			char *accumulate_label );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *login_name;
	DICTIONARY *post_dictionary;
	char *begin_date = {0};
	char *end_date = {0};
	DOCUMENT *document;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *sys_string = {0};
	char *where_clause = {0};
	int length_select_list = 0;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *units_label;
	char delimiter = FOLDER_DATA_DELIMITER;
	char *accumulate_yn;
	boolean accumulate;
	int time_piece = -1;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	FILE *output_pipe;
	char *title;
	char google_chart_output_file[ 128 ];
	char accumulate_label[ 32 ];
	char *prompt_filename;
	int date_piece = 0;
	int value_piece = 0;
	char date_buffer[ 128 ];
	char value_buffer[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s process_set login_name dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 1 ];
	if ( ( login_name = argv[ 2 ] ) ){};
	post_dictionary = dictionary_string2dictionary( argv[ 3 ] );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

	accumulate_yn =
		dictionary_fetch_index_zero(
			post_dictionary,
			"accumulate_yn" );

	accumulate = (accumulate_yn && *accumulate_yn == 'y' );

	if ( accumulate )
	{
		strcpy( accumulate_label, "(Accumulated)" );
	}
	else
	{
		*accumulate_label = '\0';
	}

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			accumulate );

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
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h1>%s</h1><h3>ERROR: insufficient input.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	process_name =
		process_generic_output_get_process_name(
			process_set_name,
			post_dictionary );

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

	if ( aggregate_statistic == aggregate_statistic_none
	&&   aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		aggregate_statistic =
			process_generic_output_get_database_aggregate_statistic(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list,
				process_generic_output->
					value_folder->
					datatype_folder->
					exists_aggregation_sum );
	}

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date,
			&end_date,
			&where_clause,
			&units_label,
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			&date_piece,
			&time_piece,
			&value_piece,
			&length_select_list,
			application_name,
			process_generic_output,
			post_dictionary,
			delimiter,
			aggregate_level,
			aggregate_statistic,
			0 /* append_low_high */,
			0 /* not concat_datatype_entity */,
			0 /* not concat_datatype */,
			process_generic_output->accumulate );

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
	
	input_pipe = popen( sys_string, "r" );

	title = get_title(	process_generic_output->
					value_folder->value_folder_name,
				process_generic_output->
					value_folder->
					datatype->
					foreign_attribute_data_list,
				aggregate_level,
				aggregate_statistic,
				begin_date,
				end_date,
				accumulate_label );

	sprintf( google_chart_output_file,
		 "/tmp/%s_%d.dat",
		 process_name,
		 getpid() );

	sprintf( sys_string,
		 "google_timechart.e %s \"%s\" '' \"%s\" '%c' > %s",
		 application_name,
		 title,
		 list_display(
			process_generic_output->
			value_folder->
			datatype->
			primary_attribute_data_list ),
		 delimiter,
		 google_chart_output_file );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
#ifdef NOT_DEFINED
		/* If aggregated, remove the time column */
		/* ------------------------------------- */
		if ( process_generic_output->
			value_folder->
			time_attribute_name
		&&   *process_generic_output->
			value_folder->
			time_attribute_name
		&&   aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			piece_inverse( input_buffer, '^', time_piece );
		}

		if ( *input_buffer == '#' ) continue;

		if ( !piece_delete_multiple(
			input_buffer /* source_destination */,
			delimiter,
			length_select_list - 2 /* columns_to_piece */ ) )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: cannot piece_delete_multiple(%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}
#endif

		if ( !piece(
			date_buffer,
			delimiter,
			input_buffer,
			date_piece ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: date_buffer cannot piece(%d) in [%s]\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				date_piece,
				input_buffer );
		}

		if ( strcmp( date_buffer, "Date" ) == 0 ) continue;

		if ( !piece(
			value_buffer,
			delimiter,
			input_buffer,
			value_piece ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: value_buffer cannot piece(%d) in [%s]\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				value_piece,
				input_buffer );
		}

		fprintf(
			output_pipe,
			"%s%c%s\n",
			date_buffer,
			delimiter,
			value_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	sprintf( sys_string,
		 "cat %s | column.e 0",
		 google_chart_output_file );

	prompt_filename = pipe2string( sys_string );

	sprintf( sys_string,
		 "rm -f %s",
		 google_chart_output_file );

	system( sys_string );

	google_chart_output_prompt(
		application_name,
		prompt_filename,
		process_name /* target_window */,
		(char *)0 /* where_clause */ );

	document_close();

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;
}

char *get_title(		char *value_folder_name,
				LIST *foreign_attribute_data_list,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *begin_date,
				char *end_date,
				char *accumulate_label )
{
	static char title[ 512 ];
	char buffer[ 512 ];

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf(title,
		 	"%s %s %s",
			aggregate_level_get_string( aggregate_level ),
			aggregate_statistic_get_string( aggregate_statistic ),
			value_folder_name );
	}
	else
	{
		sprintf(title,
		 	"%s",
			value_folder_name );
	}

	sprintf(title + strlen( title ),
		" %s%s From: %s To: %s",
		format_initial_capital( buffer,
			list_display_delimited(
				foreign_attribute_data_list,
				',' ) ),
		accumulate_label,
		begin_date,
		end_date );

	format_initial_capital( title, title );

	return title;

} /* get_title() */

