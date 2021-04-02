/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_gracechart.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "hash_table.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "environ.h"
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
boolean output_gracechart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *sys_string,
			char *document_root_directory,
			char *argv_0,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			LIST *datatype_primary_attribute_data_list,
			DICTIONARY *post_dictionary,
			char *where_clause,
			char *units_label,
			LIST *foreign_attribute_data_list,
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *login_name;
	DICTIONARY *post_dictionary;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *sys_string;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *where_clause = {0};
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *units_label;
	int datatype_entity_piece = 0;
	int datatype_piece = 0;
	int date_piece = 0;
	int time_piece = -1;
	int value_piece = 0;

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
			&datatype_entity_piece,
			&datatype_piece,
			&date_piece,
			&time_piece,
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

	if ( output_gracechart(
				application_name,
				begin_date_string,
				end_date_string,
				sys_string,
				appaserver_parameter_file->
					document_root,
				argv[ 0 ],
				process_generic_output,
				process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list,
				post_dictionary,
				where_clause,
				units_label,
				process_generic_output->
					value_folder->
					datatype->
					foreign_attribute_data_list,
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece ) )
	{
		process_increment_execution_count(
					application_name,
					process_name,
					appaserver_parameter_file_get_dbms() );
	}

	document_close();
	exit( 0 );
}

boolean output_gracechart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *sys_string,
			char *document_root_directory,
			char *argv_0,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			LIST *datatype_primary_attribute_data_list,
			DICTIONARY *post_dictionary,
			char *where_clause,
			char *units_label,
			LIST *foreign_attribute_data_list,
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece )
{
	char input_buffer[ 512 ];
	char title[ 512 ] = {0};
	char sub_title[ 512 ] = {0};
	GRACE *grace;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	FILE *input_pipe;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char legend[ 128 ];
	char delimiter_string[ 2 ] = {0};
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *chart_email_command_line;
	char *email_address;
	char *accumulate_yn;
	boolean accumulate_flag;
	char accumulate_label[ 32 ];

	*delimiter_string = DELIMITER;

	accumulate_yn =
		dictionary_fetch(
			post_dictionary,
			"accumulate_yn_0" );

	accumulate_flag = (accumulate_yn && *accumulate_yn == 'y' );

	if ( accumulate_flag )
	{
		value_piece++;
		strcpy( accumulate_label, "(Accumulate)" );
	}
	else
	{
		*accumulate_label = '\0';
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

#ifdef NOT_DEFINED
	/* Temporary assignment until the time attribute is implemented. */
	/* ------------------------------------------------------------- */
	if ( aggregate_level == daily
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		aggregate_level = real_time;
	}
#endif

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
				aggregate_level );

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf( title,
		 	"Chart %s %s",
			aggregate_level_get_string( aggregate_level ),
			process_generic_output->
				value_folder->
				value_folder_name );
	}
	else
	{
		sprintf( title,
		 	"Chart %s",
			process_generic_output->
				value_folder->
				value_folder_name );
	}

	format_initial_capital( title, title );

	if ( aggregate_statistic != aggregate_statistic_none )
	{
		sprintf(sub_title,
			"%s %s%s from %s to %s",
			list_display_delimited(
				foreign_attribute_data_list,
				'/' ),
			aggregate_statistic_get_string(
				aggregate_statistic ),
			accumulate_label,
			begin_date_string,
			end_date_string );
	}
	else
	{
		sprintf(sub_title,
			"%s%s from %s to %s",
			list_display_delimited(
				foreign_attribute_data_list,
				'/' ),
			accumulate_label,
			begin_date_string,
			end_date_string );
	}

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
		time_piece,
		value_piece,
		title,
		sub_title,
		0 /* not datatype_type_xyhilo */,
		no_cycle_colors_if_multiple_datatypes );

	grace->dataset_no_cycle_color = 1;

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		printf(
"<h3>ERROR: Invalid date format format or no data for datatype.</h3>\n" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph->units = units_label;

	grace_datatype =
		grace_new_grace_datatype(
			(char *)0 /* datatype_entity_name */,
			process_generic_get_datatype_name(
				datatype_primary_attribute_data_list,
				' ' /* delimiter */ ) );

	grace_datatype->nodisplay_legend = 1;

	grace_graph_set_scale_to_zero(
			grace_graph,
			process_generic_output->
				value_folder->
				datatype->scale_graph_zero );

	grace_graph->xaxis_ticklabel_angle = 45;

	strcpy(	legend,
		process_generic_get_datatype_name(
			datatype_primary_attribute_data_list,
			' ' /* delimiter */ ) );

	format_initial_capital( legend, legend );

	grace_datatype->legend = strdup( legend );

	if ( process_generic_output->
				value_folder->
			 		datatype->bar_graph )
	{
		grace_datatype->line_linestyle = 0;
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

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

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( *input_buffer == '#' ) continue;
		search_replace_string( input_buffer, delimiter_string, "|" );

		grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
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
		printf( "<h3>Warning: no graphs to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

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
				0 /* not with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					process_generic_output->
						value_folder->
						value_folder_name,
				application_is_primary_application(
					application_name ) ) );
	}
	else
	{
		grace_email_graph(
			application_name,
			email_address,
			chart_email_command_line,
			output_filename,
			(char *)0 /* appaserver_mount_point */,
			0 /* not with_document_output */,
			query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->
					value_folder_name,
				1 ) );
	}

	return 1;

}

