/* --------------------------------------------------- 	*/
/* src_hydrology/sensor_exposed_null_output.c   	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "application.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "boolean.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "make_date_time_between_compatable.h"
#include "hydrology_library.h"
#include "measurement_validation.h"

/* Constants */
/* --------- */
#define DATE_EXPAND_PLUS_MINUS	2
#define INFRASTRUCTURE_PROCESS	"%s/%s/estimation_datatype_unit_record_list.sh %s %s %s"
#define DATATYPE_ENTITY_PIECE	 0
#define DATATYPE_PIECE		 1
#define DATE_PIECE		 2
#define TIME_PIECE		 3
#define VALUE_PIECE		 4

/* Prototypes */
/* ---------- */
void sensor_exposed_null_output(
			char *application_name,
			char *argv_0,
			char *login_name,
			char *process_name,
			char *appaserver_mount_point,
			char *role_name,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *from_measurement_time,
			char *to_measurement_date,
			char *to_measurement_time,
			double threshold_value,
			char *above_below,
			char chart_yn,
			char *notes,
			char *parameter_dictionary_string,
			char *document_root_directory,
			boolean execute );

char *get_where_clause(	char *station,
			char *datatype,
			char *expanded_from_date_string,
			char *expanded_to_date_string,
			char *from_measurement_time,
			char *to_measurement_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *role_name;
	char *station;
	char *datatype;
	char *process_name;
	char *from_measurement_date, *to_measurement_date;
	char *from_measurement_time, *to_measurement_time;
	char chart_yn;
	boolean execute;
	char *notes;
	char *threshold_value_string;
	double threshold_value;
	char *above_below;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	char *override_destination_validation_requirement_yn;
	boolean override_destination_validation_requirement;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int number_measurements_validated;

	if ( argc != 16 )
	{
		fprintf(stderr,
"Usage: %s login_name process role station datatype from_date to_date from_time to_time threshold_value above_below chart_yn parameter_dictionary notes execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	role_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	from_measurement_date = argv[ 6 ];
	to_measurement_date = argv[ 7 ];
	from_measurement_time = argv[ 8 ];
	to_measurement_time = argv[ 9 ];
	threshold_value_string = argv[ 10 ];
	above_below = argv[ 11 ];
	chart_yn = *argv[ 12 ];
	parameter_dictionary_string = argv[ 13 ];
	notes = argv[ 14 ];
	execute = (*argv[ 15 ] == 'y');

	application_name = environ_get_application_name( argv[ 0 ] );

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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	make_date_time_between_compatable(	&to_measurement_date,
						&from_measurement_time,
						&to_measurement_time,
						from_measurement_date );

	appaserver_parameter_file = appaserver_parameter_file_new();

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
					from_measurement_date,
					to_measurement_date ) ) )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf(
	"<h3>ERROR: %d of the destination measurements are validated.</h3>\n",
			number_measurements_validated );
		document_close();
		exit( 0 );
	}

	if ( !*above_below
	||   strcmp( above_below, "above_below" ) == 0
	||   strcmp( above_below, "select" ) == 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h3>ERROR: above or below is not selected.</h3>\n" );

		document_close();
		exit( 0 );
	}

	if ( !*threshold_value_string
	||   strcmp( threshold_value_string, "threshold_value" ) == 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf(
		"<h3>ERROR: please enter in a threshold value.</h3>\n" );

		document_close();
		exit( 0 );
	}

	threshold_value = atof( threshold_value_string );

	if ( !*datatype
	||   strcmp( datatype, "datatype" ) == 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf(
		"<h3>ERROR: please choose a station/datatype.</h3>\n" );

		document_close();
		exit( 0 );
	}

	if ( !*from_measurement_date
	||   strcmp( from_measurement_date, "from_measurement_date" ) == 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf(
		"<h3>ERROR: please isolate a date range.</h3>\n" );

		document_close();
		exit( 0 );
	}

	if ( !appaserver_library_validate_begin_end_date(
					&from_measurement_date,
					&to_measurement_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h3>ERROR: invalid date(s).</h3>\n" );

		document_close();
		exit( 0 );
	}

	sensor_exposed_null_output(
		application_name,
		argv[ 0 ],
		login_name,
		process_name,
		appaserver_parameter_file->
			appaserver_mount_point,
		role_name,
		station,
		datatype,
		from_measurement_date,
		from_measurement_time,
		to_measurement_date,
		to_measurement_time,
		threshold_value,
		above_below,
		chart_yn,
		notes,
		parameter_dictionary_string,
		appaserver_parameter_file->
			document_root,
		execute );

	document_close();

	return 0;

} /* main() */

void sensor_exposed_null_output(	char *application_name,
					char *argv_0,
					char *login_name,
					char *process_name,
					char *appaserver_mount_point,
					char *role_name,
					char *station,
					char *datatype,
					char *from_measurement_date,
					char *from_measurement_time,
					char *to_measurement_date,
					char *to_measurement_time,
					double threshold_value,
					char *above_below,
					char chart_yn,
					char *notes,
					char *parameter_dictionary_string,
					char *document_root_directory,
					boolean execute )
{
	char input_buffer[ 4096 ];
	char buffer[ 4096 ];
	DOCUMENT *document;
	FILE *input_pipe;
	HTML_TABLE *html_table = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char old_value[ 128 ];
	char new_value[ 128 ];
	LIST *heading_list;
	char infrastructure_process[ 1024 ];
	char *ftp_output_filename;
	char *output_filename;
	char *ftp_agr_filename;
	char *agr_filename;
	char *postscript_filename;
	char title[ 256 ];
	char sub_title[ 256 ] = {0};
	GRACE *grace = {0};
	char *expanded_from_date_string, *expanded_to_date_string;
	DATE *expanded_date;
	char graph_identifier[ 128 ];
	char *where_clause;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;

	if ( chart_yn == 'y' && !execute )
	{
		expanded_date =
			date_new_yyyy_mm_dd_date(
				from_measurement_date );

		date_decrement_days(
			expanded_date,
			DATE_EXPAND_PLUS_MINUS );

		expanded_from_date_string =
			date_get_yyyy_mm_dd_string(
				expanded_date );

		date_free( expanded_date );

		expanded_date =
			date_new_yyyy_mm_dd_date(
				to_measurement_date );

		date_increment_days(
			expanded_date,
			DATE_EXPAND_PLUS_MINUS );

		expanded_to_date_string =
			date_get_yyyy_mm_dd_string(
				expanded_date );

		date_free( expanded_date );
	}
	else
	{
		expanded_from_date_string = from_measurement_date;
		expanded_to_date_string = to_measurement_date;
	}

	where_clause =
		get_where_clause(	station,
					datatype,
					expanded_from_date_string,
					expanded_to_date_string,
					from_measurement_time,
					to_measurement_time );

	search_replace_string( notes, "\"", "'" );

	sprintf(buffer,
"%s/%s/sensor_exposed_null %s %s \"%s\" %s %lf %s %s %s %s %s \"%s\" \"%s\" \"%s\" %c 2>>%s",
		appaserver_mount_point,
		application_get_relative_source_directory( application_name ),
		login_name,
		application_name,
		station,
		datatype,
		threshold_value,
		above_below,
		from_measurement_date,
		from_measurement_time,
		to_measurement_date,
		to_measurement_time,
		parameter_dictionary_string,
		where_clause,
		notes,
		(execute) ? 'y' : 'n',
		appaserver_error_get_filename(
			application_name ) );

	input_pipe = popen( buffer, "r" );

	if ( chart_yn == 'y' && !execute )
	{
		sprintf(infrastructure_process,
			INFRASTRUCTURE_PROCESS,
			appaserver_mount_point,
			application_get_relative_source_directory(
				application_name ),
			application_name,
			station,
			datatype );

		format_initial_capital( title, process_name );

		sprintf(sub_title,
			"%s/%s from %s:%s to %s:%s",
			station,
			format_initial_capital( buffer, datatype ),
			from_measurement_date,
			from_measurement_time,
			to_measurement_date,
			to_measurement_time );

		grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				infrastructure_process,
				(char *)0 /* data_process */,
				argv_0,
				DATATYPE_ENTITY_PIECE,
				DATATYPE_PIECE,
				DATE_PIECE,
				TIME_PIECE,
				VALUE_PIECE,
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

		if ( !grace_set_begin_end_date(grace,
						expanded_from_date_string,
						expanded_to_date_string ) )
		{
			document_quick_output_body(
					application_name,
					appaserver_mount_point );

			printf(
			"<h2>ERROR: Invalid date format format.</h2>" );
			document_close();
			exit( 1 );
		}

		grace->grace_output =
			application_get_grace_output( application_name );

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
			document_root_directory,
			graph_identifier,
			grace->grace_output );
	}
	else
	/* ------------------------------------------------ */
	/* Must not be chart_yn == 'y' or execute_yn == 'y' */
	/* ------------------------------------------------ */
	{
		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

	 	html_table = new_html_table(
				format_initial_capital(
					buffer,
					process_name ),
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

	/* Sample input: "BD,bottom_temperature,1999-01-01,1000,4.00|null" */
	/* --------------------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		search_replace_string( input_buffer, "|", "," );

		/* The count is the last to arrive */
		/* ------------------------------- */
		if ( !piece( measurement_date, ',', input_buffer, 2 ) )
		{
			break;
		}

		if ( chart_yn == 'y' && !execute )
		{
			search_replace_string( input_buffer, ",", "|" );

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
		if ( !execute )
		{
			piece(	measurement_time,
				',',
				input_buffer,
				TIME_PIECE );

			piece( old_value, ',', input_buffer, VALUE_PIECE );
			piece( new_value, ',', input_buffer, VALUE_PIECE + 1 );

			html_table_set_data(	html_table->data_list,
						strdup( station ) );
	
			html_table_set_data(	html_table->data_list,
						strdup( 
						format_initial_capital( 
							buffer, datatype ) ) );
	
			html_table_set_data(	html_table->data_list,
						strdup( measurement_date ) );
			html_table_set_data(	html_table->data_list,
						strdup( measurement_time ) );
			html_table_set_data(	html_table->data_list,
						strdup( old_value ) );
			html_table_set_data(	html_table->data_list,
						strdup( new_value ) );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		}
	} /* while( get_line() */

	pclose( input_pipe );

	if ( chart_yn == 'y' && !execute )
	{
		GRACE_GRAPH *grace_graph;

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

		grace_move_legend_bottom_left(	grace_graph,
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
				grace->grace_output,
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
				appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
	}
	else
	/* ------------------------------------------- */
	/* if ( chart_yn != 'y' || execute_yn == 'y' ) */
	/* ------------------------------------------- */
	{
		/* Output the count */
		/* ---------------- */
		if ( !execute )
			sprintf( buffer, "Did not process %s", input_buffer );
		else
			sprintf( buffer, "Processed %s", input_buffer );
	
		html_table_set_data( html_table->data_list, buffer );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

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
					from_measurement_date,
					to_measurement_date );
	}

} /* sensor_exposed_null_output() */

char *get_where_clause(	char *station,
			char *datatype,
			char *expanded_from_date_string,
			char *expanded_to_date_string,
			char *from_measurement_time,
			char *to_measurement_time )
{
	char where_clause[ 4096 ];

	sprintf( where_clause,
		 "station = '%s' and datatype = '%s' and %s",
		 station,
		 datatype,
		 query_get_between_date_time_where(
					"measurement_date",
					"measurement_time",
					expanded_from_date_string,
					from_measurement_time,
					expanded_to_date_string,
					to_measurement_time,
					(char *)0 /* application */,
					(char *)0 /* folder_name */ ) );
	return strdup( where_clause );
} /* get_where_clause() */

