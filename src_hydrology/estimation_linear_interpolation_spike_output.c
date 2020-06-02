/* ------------------------------------------------------------ */
/* src_hydrology/estimation_linear_interpolation_spike_output.c	*/
/* ------------------------------------------------------------	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "process.h"
#include "measurement_validation.h"
#include "query.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"estimation_linear_interpolation"
#define INFRASTRUCTURE_PROCESS	"estimation_datatype_unit_record_list.sh %s %s %s"
#define PDF_PROMPT		"Press to view chart."
#define DATATYPE_ENTITY_PIECE	 0
#define DATATYPE_PIECE		 1
#define DATE_PIECE		 2
#define TIME_PIECE		 3
#define VALUE_PIECE		 4

void duplicate_value_to_spike_if_null( char *input_buffer, int value_piece );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *role_name;
	char *station;
	char *datatype;
	char input_buffer[ 4096 ];
	char buffer[ 4096 ];
	DOCUMENT *document;
	boolean execute;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *input_pipe;
	double minimum_spike;
	HTML_TABLE *html_table = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char old_value[ 128 ];
	char new_value[ 128 ];
	LIST *heading_list;
	char infrastructure_process[ 1024 ];
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;
	char title[ 256 ];
	char sub_title[ 256 ] = {0};
	GRACE *grace = {0};
	int chart_yn;
	char graph_identifier[ 128 ];
	GRACE_GRAPH *grace_graph;
	char *notes;
	char *ftp_output_filename;
	char *output_filename;
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char *grace_output;
	int number_measurements_validated;
	char *override_destination_validation_requirement_yn;
	boolean override_destination_validation_requirement;
	boolean trim_negative_drop;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 15 )
	{
		fprintf(stderr,
"Usage: %s person role station datatype minimum_spike parameter_dictionary begin_date begin_time end_date end_time trim_negative_drop_yn char_yn notes execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	role_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	minimum_spike = atof( argv[ 5 ] );
	parameter_dictionary_string = argv[ 6 ];
	begin_date = argv[ 7 ];
	begin_time = argv[ 8 ];
	end_date = argv[ 9 ];
	end_time = argv[ 10 ];
	trim_negative_drop = (*argv[ 11 ] == 'y');
	chart_yn = *argv[ 12 ];
	notes = argv[ 13 ];
	execute = (*argv[ 14 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	grace_output = application_get_grace_output( application_name );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	parameter_dictionary = 
		dictionary_index_string2dictionary(
				parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_index(
						parameter_dictionary );

	override_destination_validation_requirement_yn =
		dictionary_get_pointer(
			parameter_dictionary,
			"override_destination_validation_requirement_yn" );

	override_destination_validation_requirement =
		( override_destination_validation_requirement_yn
		&& *override_destination_validation_requirement_yn  == 'y' );

	if ( !override_destination_validation_requirement
	&&	( number_measurements_validated =
			hydrology_library_some_measurements_validated(
					application_name,
					(char *)0 /* where_clause */,
					station,
					datatype,
					begin_date,
					end_date ) ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<p>ERROR: %d of the destination measurements are validated.\n",
			number_measurements_validated );
		document_close();
		exit( 0 );
	}

	search_replace_string( notes, "\"", "'" );

	sprintf(buffer,
"%s/%s/estimation_linear_interpolation_spike %s %s %s %s %s %s %s %lf \"%s\" \"%s\" %c %c",
		appaserver_parameter_file->appaserver_mount_point,
		application_get_relative_source_directory( application_name ),
		login_name,
		station,
		datatype,
		begin_date,
		begin_time,
		end_date,
		end_time,
		minimum_spike,
		parameter_dictionary_string,
		notes,
		(trim_negative_drop) ? 'y' : 'n',
		(execute) ? 'y' : 'n' );

	input_pipe = popen( buffer, "r" );

	if ( chart_yn == 'y' && !execute )
	{
		sprintf(infrastructure_process,
			INFRASTRUCTURE_PROCESS,
			application_name,
			station,
			datatype );
			
		strcpy(	title, 
			"Estimation Linear Interpolation Spike" );

		sprintf(sub_title,
			"%s/%s; From %s:%s To %s:%s; Minimum Spike: %.1lf",
			station,
			format_initial_capital( buffer, datatype ),
			begin_date,
			begin_time,
			end_date,
			end_time,
			minimum_spike );

		grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				infrastructure_process,
				(char *)0 /* data_process */,
				argv[ 0 ],
				DATATYPE_ENTITY_PIECE,
				DATATYPE_PIECE,
				DATE_PIECE,
				TIME_PIECE,
				VALUE_PIECE,
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

		if ( !grace_set_begin_end_date( 	grace,
							begin_date,
							end_date ) )
		{
			DOCUMENT *document;

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
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );

			document_output_body(
				document->application_name,
				document->onload_control_string );

			printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
			document_close();
			exit( 1 );
		}

		sprintf(	graph_identifier,
				"%s_%s_%d",
				station,
				datatype,
				getpid() );

		grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace_output );
	}
	else
	/* --------------------------------- */
	/* if ( chart_yn != 'y' || execute ) */
	/* --------------------------------- */
	{
		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

	 	html_table = 
		     new_html_table( 
			"Estimation Linear Interpolation Spike",
			(char *)0 /* sub_title */ );
	
		heading_list = new_list();
		list_append_string( heading_list, "Station" );
		list_append_string( heading_list, "Datatype" );
		list_append_string( heading_list, "Measurement Date" );
		list_append_string( heading_list, "Measurement Time" );
		list_append_string( heading_list, "Old Value" );
		list_append_string( heading_list, "New Value" );
	
		html_table_set_heading_list( html_table, heading_list );
		html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );
		html_table->number_left_justified_columns = 4;
		html_table->number_right_justified_columns = 2;
		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}

	/* Sample input: "BD,bottom_temperature,1999-01-01,1000,4.00,5.00" */
	/* --------------------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		/* The count is the last to arrive */
		/* ------------------------------- */
		if ( !piece( measurement_date, ',', input_buffer, 2 ) )
		{
			break;
		}

		if ( chart_yn == 'y' && !execute )
		{
			search_replace_string( input_buffer, ",", "|" );

			duplicate_value_to_spike_if_null(
					input_buffer, VALUE_PIECE );

			grace_set_string_to_point_list(
				grace->graph_list, 
				DATATYPE_ENTITY_PIECE,
				DATATYPE_PIECE,
				DATE_PIECE,
				TIME_PIECE,
				VALUE_PIECE,
				input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

			piece_replace(	input_buffer,
					'|',
					"new_value",
					DATATYPE_PIECE );

			grace_set_string_to_point_list(
				grace->graph_list, 
				DATATYPE_ENTITY_PIECE,
				DATATYPE_PIECE,
				DATE_PIECE,
				TIME_PIECE,
				VALUE_PIECE + 1,
				input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
		}
		else
		{
			if ( !execute )
			{
				piece(	measurement_time,
					',',
					input_buffer,
					TIME_PIECE );
	
				piece( 	old_value,
					',', 
					input_buffer, 
					VALUE_PIECE );

				piece( 	new_value, 
					',', 
					input_buffer, 
					VALUE_PIECE + 1 );
	
				html_table_set_data(	html_table->data_list,
							strdup( station ) );
		
				html_table_set_data(	html_table->data_list,
							strdup( 
							format_initial_capital( 
								buffer,
								datatype ) ) );
		
				html_table_set_data(	html_table->data_list,
							strdup(
							measurement_date ) );

				html_table_set_data(	html_table->data_list,
							strdup(
							measurement_time ) );

				html_table_set_data(	html_table->data_list,
							strdup( old_value ) );
				html_table_set_data(	html_table->data_list,
							strdup( new_value ) );
		
				html_table_output_data(
					html_table->data_list,
					html_table->
					number_left_justified_columns,
					html_table->
					number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
	
				list_free_string_list( html_table->data_list );
				html_table->data_list = list_new();
			}
		}
	} /* while( get_line() */

	if ( chart_yn == 'y' && !execute )
	{
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
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
			document_output_body(
				document->application_name,
				document->onload_control_string );
	
			printf( "<p>Warning: no graphs to display.\n" );
			document_close();
			exit( 0 );
		}

		grace_graph = list_get_first_pointer( grace->graph_list );

		grace_move_legend_bottom_left(
			grace_graph,
			grace->landscape_mode );

		grace_move_legend_up(	&grace_graph->legend_view_y,
					0.08 /* up_increment */ );

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
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				application_get_grace_output(
					application_name ),
				application_get_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_get_ghost_script_directory(
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
						appaserver_parameter_file->
						appaserver_mount_point );

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
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
	}
	else
	/* --------------------------------- */
	/* if ( chart_yn != 'y' || execute ) */
	/* --------------------------------- */
	{
		/* Output the count */
		/* ---------------- */
		if ( !execute )
		{
			sprintf( buffer, "Did not process %s", input_buffer );
		}
		else
		{
			sprintf( buffer, "Processed %s", input_buffer );

			process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
		}
	
		html_table_set_data( html_table->data_list, buffer );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		pclose( input_pipe );
		html_table_close();
	}

	if ( execute )
	{
		measurement_validation_update_measurement(
					application_name,
					login_name,
					station,
					datatype,
					(char *)0 /* validation_date */,
					begin_date,
					end_date );
	}

	document_close();

	return 0;

} /* main() */

/* Sample input: "BD|bottom_temperature|1999-01-01|1000|4.00|5.00" */
/* --------------------------------------------------------------- */
void duplicate_value_to_spike_if_null( char *input_buffer, int value_piece )
{
	char buffer[ 128 ];

	if ( !piece( buffer, '|', input_buffer, value_piece + 1 ) )
	{
		piece( buffer, '|', input_buffer, value_piece );
		sprintf( input_buffer + strlen( input_buffer ), "|%s", buffer );
	}
}

