/* $APPASERVER_HOME/src_appaserver/generic_high_average_low.c	*/
/* ----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "hash_table.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "grace.h"
#include "session.h"
#include "process_generic_output.h"

/* Constants */
/* --------- */
#define DELIMITER	'^'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *login_name;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *post_dictionary;
	char *sys_string;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *where_clause = {0};
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *units_label;
	FILE *input_pipe;
	GRACE *grace;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char *argv_0;
	char input_buffer[ 512 ];
	char grace_begin_date_string[ 16 ];
	char grace_end_date_string[ 16 ];
	char graph_identifier[ 128 ];
	char *chart_email_command_line;
	char *email_address = {0};
	char *units_converted = {0};
	char units_converted_process[ 128 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	int datatype_entity_piece = 0;
	int datatype_piece = 0;
	int date_piece = 0;
	int value_piece = 0;
	char *heading_string;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
"Usage: %s process_set process login_name parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	
	argv_0 = argv[ 0 ];
	process_set_name = argv[ 1 ];
	process_name = argv[ 2 ];
	if ( ( login_name = argv[ 3 ] ) ){};
	post_dictionary = dictionary_string2dictionary( argv[ 4 ] );

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
			&begin_date_string /* in/out */,
			&end_date_string /* in/out */,
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
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		printf(
	"<h1>%s</h1><h3>ERROR: no data available for these dates.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	if ( aggregate_level == real_time )
	{
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		printf(
	"<h1>%s</h1><h3>ERROR: please select an aggregate level.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	aggregate_statistic = average;

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date_string,
			&end_date_string,
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
			DELIMITER,
			aggregate_level,
			aggregate_statistic,
			1 /* append_low_high */,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */,
			process_generic_output->accumulate );

	units_converted =
		dictionary_get_index_zero(
			post_dictionary,
			"units_converted" );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "piece_replace.e '|' 0 \"%s\"",
			 units_converted );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(	title,
			"%s High/Avg/Low %s Values", 
			process_generic_output->
				value_folder->
				value_folder_name,
			aggregate_level_get_string(
				aggregate_level ) );

	format_initial_capital( title, title );

	strcpy( grace_begin_date_string,
		begin_date_string );
	strcpy( grace_end_date_string,
		end_date_string );

	sprintf(sub_title,
		"%s from: %s to: %s",
		list_display_delimited(
			process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
			'/' ),
		 grace_begin_date_string,
		 grace_end_date_string );

	format_initial_capital( sub_title, sub_title );

	grace = grace_new_unit_graph_grace(
			application_name,
			(char *)0 /* role_name */,
			(char *)0 /* infrastructure_process */,
			(char *)0 /* data_process */,
			argv_0,
			datatype_entity_piece,
			datatype_piece,
			date_piece,
			-1 /* time_piece */,
			value_piece,
			title,
			sub_title,
			1 /* datatype_type_xyhilo */,
			no_cycle_colors );

	if ( !grace_set_begin_end_date( 	grace,
						grace_begin_date_string,
						grace_end_date_string ) )
	{
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		printf(
	"<h1>%s</h1><h3>ERROR: Invalid date format format.</h3>",
			buffer );
		document_close();
		exit( 1 );
	}

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
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );

	grace_graph = grace_new_grace_graph();

	grace_graph->units = units_label;

	grace_datatype =
		grace_new_grace_datatype(
			(char *)0 /* datatype_entity_name */,
			process_generic_get_datatype_name(
				process_generic_output->
				value_folder->
				datatype->
				primary_attribute_data_list,
				' ' /* delimiter */ ) );

	/*
	grace_datatype->nodisplay_legend = 1;
	*/
	grace_datatype->datatype_type_bar_xy_xyhilo = "xyhilo";
	grace_datatype->symbol_size = GRACE_XYHILO_SYMBOL_SIZE;

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

	heading_string =
	 	process_generic_output_get_heading_string(
			process_generic_output->value_folder,
			DELIMITER,
			aggregate_level );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( timlib_strncmp( input_buffer, heading_string ) == 0 )
		{
			continue;
		}

		search_replace_character( input_buffer, DELIMITER, '|' );

		grace_set_string_to_point_list(
				grace->graph_list, 
				-1 /* grace->datatype_entity_piece */,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
	}
	pclose( input_pipe );

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
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		printf(
		"<h1>%s</h1><h3>Warning: no graphs to display.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	grace_set_world_min_y_based_on_grace_point_low( grace->graph_list );
	grace_set_world_max_y_based_on_grace_point_high( grace->graph_list );

	chart_email_command_line =
			application_chart_email_command_line(
				application_name );

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
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<h1>%s</h1><h3>Internal ERROR: could not output charts.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	email_address =
		dictionary_get_index_zero(
			post_dictionary,
			"email_address" );

	if ( !*chart_email_command_line
	||   *chart_email_command_line == '#'
	||   !email_address
	||   strcmp( email_address, "email_address" ) == 0 )
	{
		grace_output_graph_window(
				application_name,
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
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
	}
	else
	{
		grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				grace->output_filename,
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
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

