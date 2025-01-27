/* ------------------------------------------------------------------ 	*/
/* $APPASERVER_HOME/src_appaserver/generic_output_annual_comparison.c	*/
/* ------------------------------------------------------------------ 	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* ------------------------------------------------------------------ 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "grace.h"
#include "post_dictionary.h"
#include "application.h"
#include "environ.h"
#include "process_generic.h"

void generic_output_annual_comparison_gracechart(
			enum aggregate_statistic aggregate_statistic,
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date,
			char ***value_matrix,
			char *sub_title );

void generic_output_annual_comparison_table(
			enum aggregate_statistic aggregate_statistic,
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date,
			char ***value_matrix,
			char *sub_title );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_name;
	char *output_medium_string;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	PROCESS_GENERIC_ANNUAL *process_generic_annual;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s login_name process output_medium dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	output_medium_string = argv[ 3 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 4 ] /* post_dictionary_string */ );

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	process_generic_annual =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_annual_new(
			application_name,
			login_name,
			process_name,
			output_medium_string,
			appaserver_parameter->document_root,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	if ( !process_generic_annual->process_generic_annual_date )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name );

		printf( "<h3>Error: invalid input.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( process_generic_annual->
		process_generic->
		process_generic_input->
		output_medium != output_medium_stdout )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name );
	}

	if ( process_generic_annual->
		process_generic->
		process_generic_input->
		output_medium == table )
	{
		generic_output_annual_comparison_table(
			process_generic_annual->
				process_generic->
				process_generic_input->
				aggregate_statistic,
			process_generic_annual->
				process_generic_annual_date->
				day_range,
			process_generic_annual->
				process_generic_annual_date->
				year_range,
			process_generic_annual->
				process_generic_annual_date->
				begin_year_integer,
			process_generic_annual->
				process_generic_annual_date->
				begin_date,
			process_generic_annual->
				process_generic_annual_matrix->
				value_matrix,
			process_generic_annual->sub_title );
	}
	else
	if ( process_generic_annual->
		process_generic->
		process_generic_input->
		output_medium == gracechart )
	{
		generic_output_annual_comparison_gracechart(
			process_generic_annual->
				process_generic->
				process_generic_input->
				aggregate_statistic,
			process_generic_annual->
				process_generic_annual_date->
				day_range,
			process_generic_annual->
				process_generic_annual_date->
				year_range,
			process_generic_annual->
				process_generic_annual_date->
				begin_year_integer,
			process_generic_annual->
				process_generic_annual_date->
				begin_date,
			process_generic_annual->
				process_generic_annual_matrix->
				value_matrix,
			process_generic_annual->sub_title );
	}
	else
	{
		char message[ 256 ];

		sprintf(message,
			"invalid output_medium = %d.",
			process_generic_annual->
				process_generic->
				process_generic_input->
				output_medium  );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( process_generic_annual->
		process_generic->
		process_generic_input->
		output_medium != output_medium_stdout )
	{
		document_close();
	}

	process_execution_count_increment( process_name );

	return 0;
}

void generic_output_annual_comparison_table(
			enum aggregate_statistic aggregate_statistic,
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date,
			char ***value_matrix,
			char *sub_title )
{
	char *temp_filename;
	FILE *stream_file;
	FILE *output_pipe;
	char buffer[ 128 ];
	char system_string[ 1024 ];
	FILE *input_file;
	char input_string[ 128 ];

	if ( !day_range
	||   !year_range
	||   !begin_year_integer
	||   !begin_date
	||   !value_matrix
	||   !sub_title )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	temp_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		timlib_temp_filename(
			"annual_comparison" /* key */ );

	stream_file = 
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			temp_filename );

	process_generic_annual_matrix_output(
		day_range,
		year_range,
		begin_year_integer,
		begin_date /* in/out */,
		value_matrix,
		stream_file  );

	fclose( stream_file );
	input_file = fopen( temp_filename, "r" );

	sprintf(system_string,
		"html_table.e \"%s\" \"Day,Year,%s\" ',' %s",
		sub_title,
		string_initial_capital(
			buffer,
			aggregate_statistic_string(
				aggregate_statistic ) ),
		 "left,right,right" );

	output_pipe = popen( system_string, "w" );

	while( string_input( input_string, input_file, 128 ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
			fprintf( output_pipe, "%s", input_string + 5 );
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
			fprintf( output_pipe, ",%s\n", input_string + 6 );
	}

	fclose( input_file );
	pclose( output_pipe );
}

boolean output_annual_comparison_output_gracechart(
			enum aggregate_statistic aggregate_statistic,
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date,
			char ***value_matrix,
			char *sub_title )
{
	GRACE *grace;
	char graph_identifier[ 128 ];
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
	char *begin_date_string;
	char *end_date_string;
	char point_list_string[ 1024 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	char input_string[ 128 ];
	char sys_string[ 4096 ] = {0};
	FILE *input_pipe;
	int year;
	int begin_year_integer;
	int end_year_integer;
	char datatype_name_string[ 16 ];
	char day_string[ 16 ];
	char year_value_string[ 32 ];
	char year_string[ 16 ];
	char value_string[ 24 ];

	build_sys_string(
		sys_string,
		application_name,
		begin_month_day,
		end_month_day,
		begin_year,
		end_year,
		process_name,
		aggregate_level,
		aggregate_statistic,
		post_dictionary_string );

	get_report_title(	title,
				process_name,
				datatype_entity_name,
				datatype_name );

	get_report_sub_title(	sub_title,
				begin_month_day,
				end_month_day,
				begin_year,
				end_year,
				aggregate_level,
				aggregate_statistic );

	grace = grace_new_unit_graph_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				strdup( title ),
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	get_begin_end_date(	&begin_date_string,
				&end_date_string,
				begin_month_day,
				begin_year,
				end_month_day );

	if ( !grace_set_begin_end_date(	grace,
					begin_date_string,
					end_date_string ) )
	{
		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

/*
	grace_graph->units = "count";
	grace_graph->world_min_y = -10;
	grace_graph->y_tick_major = 30;
*/

	begin_year_integer = atoi( begin_year );
	end_year_integer = atoi( end_year );

	for( year = begin_year_integer; year <= end_year_integer; year++ )
	{
		sprintf( datatype_name_string, "%d", year );

		grace_datatype =
			grace_new_grace_datatype(
				(char *)0 /* datatype_entity */,
				strdup( datatype_name_string ) );

		grace_datatype->legend = strdup( datatype_name_string );

		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";

		list_append_pointer(	grace_graph->datatype_list,
					grace_datatype );
	}

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf(	graph_identifier,
			"%d",
			getpid() );

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

	build_sys_string(
		sys_string,
		application_name,
		begin_month_day,
		end_month_day,
		begin_year,
		end_year,
		process_name,
		aggregate_level,
		aggregate_statistic,
		post_dictionary_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
		{
			strcpy( day_string, input_string + 5 );
		}
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
		{
			strcpy( year_value_string, input_string + 6 );

			piece(	year_string,
				',',
				year_value_string,
				0 );

			piece(	value_string,
				',',
				year_value_string,
				1 );

			sprintf(point_list_string,
				"%s|%s-%s|%s",
				year_string,
		 		begin_year,
				day_string,
		 		value_string );

			grace_set_string_to_point_list(
				grace->graph_list, 
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				point_list_string,
				unit_graph,
				grace->datatype_type_xyhilo,
				1 /* grace->dataset_no_cycle_color */,
				(char *)0 /* optional_label */ );
		}
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

#ifdef NOT_DEFINED
	grace_move_legend_bottom_left(
			list_get_first_pointer( grace->graph_list ),
			grace->landscape_mode );

	grace_increase_legend_char_size(
			list_get_first_pointer(
				grace->graph_list ),
			0.15 );

	/* Make the graph wider -- 97% of the page */
	/* --------------------------------------- */
	grace_set_view_maximum_x(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.97 );

	/* Move the legend down a little */
	/* ----------------------------- */
	grace_lower_legend(	grace->graph_list,
				0.04 );
#endif

	grace->dataset_no_cycle_color = 1;

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
	else
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_mount_point,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );
	}
	return 1;
}

#ifdef NOT_DEFINED
void output_annual_comparison_output_transmit(
				FILE *output_pipe,
				char *application_name,
				char *begin_month_day,
				char *end_month_day,
				char *begin_year,
				char *end_year,
				char *process_name,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *post_dictionary_string )
{

	char sys_string[ 4096 ] = {0};
	char buffer[ 128 ];
	char input_string[ 1024 ];
	FILE *input_pipe;
	char day_string[ 16 ];

	build_sys_string(
		sys_string,
		application_name,
		begin_month_day,
		end_month_day,
		begin_year,
		end_year,
		process_name,
		aggregate_level,
		aggregate_statistic,
		post_dictionary_string );

	fprintf(output_pipe,
		"Day,Year,%s\n",
		format_initial_capital(
			buffer,
			aggregate_statistic_get_string(
				aggregate_statistic ) ) );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
		{
			strcpy( day_string, input_string + 5 );
		}
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
		{
			fprintf(	output_pipe,
					"%s,%s\n",
					day_string,
					input_string + 6 );
		}
	}
	pclose( input_pipe );
} /* output_annual_comparison_output_transmit() */
#endif

