/* ----------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_output_exceedance.c	*/
/* ----------------------------------------------------------- 	*/
/* 						       		*/
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
#include "timlib.h"
#include "query.h"
#include "application.h"
#include "piece.h"
#include "column.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "grace.h"
#include "environ.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "process.h"
#include "session.h"
#include "process_generic_output.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define PDF_PROMPT		"Press to view chart."

/* Prototypes */
/* ---------- */
void output_exceedance_gracechart(
			char *application_name,
			char *sys_string,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			char *begin_date,
			char *end_date,
			DICTIONARY *post_dictionary,
			char *document_root_directory,
			int percent_below_piece,
			char *where_clause,
			char *value_folder_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *login_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	DICTIONARY *original_post_dictionary;
	char *output_medium;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *sys_string;
	char *where_clause = {0};
	int percent_below_piece;
	char *begin_date = {0};
	char *end_date = {0};
	DICTIONARY_APPASERVER *dictionary_appaserver;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s process_set process login_name output_medium dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	output_medium = argv[ 4 ];
	original_post_dictionary = dictionary_string2dictionary( argv[ 5 ] );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	process_generic_output =
		process_generic_output_new(
			application_name,
			process_name,
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
						primary_key_list,
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
			dictionary_appaserver->non_prefixed_dictionary,
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
			dictionary_appaserver->non_prefixed_dictionary );

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date /* in/out */,
			&end_date /* in/out */,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name,
			dictionary_appaserver->
				non_prefixed_dictionary
				/* query_removed_post_dictionary */ ) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		sys_string =
			process_generic_output_get_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary_appaserver->
				non_prefixed_dictionary /* post_dictionary */,
			0 /* not with_html_table */,
			login_name );

		if ( system( sys_string ) ){};
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		sys_string =
			process_generic_output_get_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary_appaserver->
				non_prefixed_dictionary /* post_dictionary */,
			1 /* with_html_table */,
			login_name );

		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<br>Search Criteria: %s\n",
			 query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->
						value_folder_name,
				1 ) );

		fflush( stdout );
		if ( system( sys_string ) ){};
		fflush( stdout );

		document_close();
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		sys_string =
		process_generic_output_get_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary_appaserver->
				non_prefixed_dictionary /* post_dictionary */,
			0 /* not with_html_table */,
			login_name );

		output_exceedance_gracechart(
			application_name,
			sys_string,
			process_generic_output,
			begin_date,
			end_date,
			dictionary_appaserver->
				non_prefixed_dictionary /* post_dictionary */,
			appaserver_parameter_file->
				document_root,
			percent_below_piece,
			where_clause,
			process_generic_output->
				value_folder->
				value_folder_name );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
}

void output_exceedance_gracechart(
			char *application_name,
			char *sys_string,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			char *begin_date,
			char *end_date,
			DICTIONARY *post_dictionary,
			char *document_root_directory,
			int percent_below_piece,
			char *where_clause,
			char *value_folder_name )
{
	GRACE *grace;
	char title[ 512 ];
	char sub_title[ 512 ];
	char *units_converted;
	char *units_label;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	GRACE_GRAPH *grace_graph;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char percent_below[ 128 ];
	char value[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char *chart_email_command_line;
	char *email_address;

	sprintf( title,
		 "%s Exceedance Curve",
		 process_generic_output->value_folder->value_folder_name );

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

	if ( aggregate_level != real_time )
	{
		sprintf( title + strlen( title ),
			 " (%s %s)",
		 	 aggregate_level_get_string(
				aggregate_level ),
		 	 aggregate_statistic_get_string(
				aggregate_statistic ) );
	}

	format_initial_capital( title, title );

	sprintf(	sub_title,
			"%s From: %s To: %s",
			list_display_delimited(
				process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
				'/' ),
			begin_date,
			end_date );

	format_initial_capital( sub_title, sub_title );

	units_converted = dictionary_get_index_zero(
				post_dictionary,
				"units_converted" );

	units_label =
		process_generic_output_get_units_label(
			process_generic_output->
				value_folder->
				datatype->
				units,
			units_converted,
			aggregate_statistic );

	grace = grace_new_xy_grace(
		(char *)0 /* application_name */,
		(char *)0 /* role_name */,
		title,
		sub_title,
		units_label,
		(char *)0 /* legend */ );

	grace->xaxis_ticklabel_precision = 0;
	grace->world_min_x = 0.0;
	grace->world_max_x = 100.0;

	sprintf( graph_identifier, "%d", getpid() );

	grace->grace_output =
		application_grace_output( application_name );

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

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	value,
			OUTPUT_TEXT_FILE_DELIMITER,
			input_buffer,
			0 );

		if ( ! piece(	percent_below,
				OUTPUT_TEXT_FILE_DELIMITER,
				input_buffer,
				percent_below_piece ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot piece (%c) (%d) in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 OUTPUT_TEXT_FILE_DELIMITER,
				 percent_below_piece,
				 input_buffer );
			exit( 1 );
		}

		grace_set_xy_to_point_list(
			grace->graph_list, 
			atof( percent_below ),
			strdup( value ),
			(char *)0 /* optional_label */,
			grace->dataset_no_cycle_color );
	}
	pclose( input_pipe );

	grace_graph =
		(GRACE_GRAPH *)
			list_first(
				grace->graph_list );

	grace_graph->xaxis_label = "Percent Below";

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
		DOCUMENT *document;

		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			(char *)0 /* appaserver_mount_point */,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf( "<p>Warning: no graphs to display.\n" );
		document_close();
		exit( 0 );
	}

	grace->symbols = 1;

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
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<h3>No data for selected parameters.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( chart_email_command_line =
			application_chart_email_command_line(
				application_name ) ) )
	{
		chart_email_command_line = "";
	}

	if ( ! ( email_address =
		dictionary_get_index_zero(
			post_dictionary,
			"email_address" ) ) )
	{
		email_address = "";
	}

	if ( !*chart_email_command_line
	||   *chart_email_command_line == '#'
	||   !*email_address )
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				1 /* with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
				1 ) );
	}
	else
	{
		grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				output_filename,
				(char *)0 /* appaserver_mount_point */,
				1 /* with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
				1 ) );
	}
}

