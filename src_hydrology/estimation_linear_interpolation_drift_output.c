/* --------------------------------------------------- 	*/
/* estimation_linear_interpolation_drift_output.c      	*/
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
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "date.h"
#include "application.h"
#include "environ.h"
#include "make_date_time_between_compatable.h"
#include "measurement_validation.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define INFRASTRUCTURE_PROCESS	"%s/%s/estimation_datatype_unit_record_list.sh %s %s %s"
#define PDF_PROMPT		"Press to view chart."
#define DATATYPE_ENTITY_PIECE	 0
#define DATATYPE_PIECE		 1
#define DATE_PIECE		 2
#define TIME_PIECE		 3
#define VALUE_PIECE		 4
#define DATE_EXPAND_PLUS_MINUS	 2

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name;
	char *process_name;
	char *application_name;
	char *role_name;
	char *station;
	char *datatype;
	char input_buffer[ 4096 ];
	char buffer[ 4096 ];
	char sys_string[ 4096 ];
	DOCUMENT *document;
	char chart_yn;
	char really_yn;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	DICTIONARY *drift_parameter_dictionary = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *input_pipe;
	double first_value, last_value;
	double first_multiplier, last_multiplier;
	char *first_multiplier_string, *last_multiplier_string;
	HTML_TABLE *html_table = {0};
	char *from_measurement_date, *to_measurement_date;
	char *from_measurement_time, *to_measurement_time;
	char old_value[ 128 ];
	char new_value[ 128 ];
	LIST *heading_list;
	char infrastructure_process[ 1024 ];
	char title[ 256 ];
	char sub_title[ 256 ] = {0};
	GRACE *grace = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char graph_identifier[ 128 ];
	char *expanded_from_date_string = {0};
	char *expanded_to_date_string = {0};
	DATE *expanded_date;
	char *notes;
	char *database_string = {0};
	char *override_destination_validation_requirement_yn;
	boolean override_destination_validation_requirement = 0;
	int number_measurements_validated;

	if ( argc != 17 )
	{
		fprintf(stderr,
"Usage: %s person process application role station datatype from_date to_date from_time to_time first_value last_value parameter_dictionary chart_yn notes really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	application_name = argv[ 3 ];
	role_name = argv[ 4 ];
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	from_measurement_date = argv[ 7 ];
	to_measurement_date = argv[ 8 ];
	from_measurement_time = argv[ 9 ];
	to_measurement_time = argv[ 10 ];
	first_value = atof( argv[ 11 ] );
	last_value = atof( argv[ 12 ] );
	parameter_dictionary_string = argv[ 13 ];
	chart_yn = *argv[ 14 ];
	notes = argv[ 15 ];
	really_yn = *argv[ 16 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	make_date_time_between_compatable(	&to_measurement_date,
						&from_measurement_time,
						&to_measurement_time,
						from_measurement_date );

	/* First and last multiplier */
	/* ------------------------- */
	dictionary_get_index_data( 	&first_multiplier_string,
					parameter_dictionary,
					"first_multiplier",
					0 );

	if ( first_multiplier_string
	&&   *first_multiplier_string
	&&   strcmp( first_multiplier_string, "first_multiplier") != 0 )
	{
		first_multiplier = atof( first_multiplier_string );
	}
	else
	{
		first_multiplier = 0.0;
	}

	dictionary_get_index_data( 	&last_multiplier_string,
					parameter_dictionary,
					"last_multiplier",
					0 );

	if ( last_multiplier_string
	&&   *last_multiplier_string
	&&   strcmp( last_multiplier_string, "last_multiplier") != 0 )
	{
		last_multiplier = atof( last_multiplier_string );
	}
	else
	{
		last_multiplier = 0.0;
	}

	if ((!(first_value + last_value + first_multiplier + last_multiplier))
	|| ( ( first_value + last_value )
	&&   ( first_multiplier + last_multiplier ) ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
"<p>ERROR: either first value and last value are used or first multiplier and last multiplier are used.\n" );
		document_close();
		exit( 0 );
	}

	/* Get override changing measurements that are validated. */
	/* ------------------------------------------------------ */
	override_destination_validation_requirement_yn =
		dictionary_get_pointer(
			parameter_dictionary,
			"override_destination_validation_requirement_yn" );

	override_destination_validation_requirement =
		( override_destination_validation_requirement_yn
		&& *override_destination_validation_requirement_yn  == 'y' );

	if ( !override_destination_validation_requirement
	&& ( number_measurements_validated =
			hydrology_library_some_measurements_validated(
				application_name,
				(char *)0 /* where_clause */,
				station,
				datatype,
				from_measurement_date,
				to_measurement_date ) ) )
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

	if ( chart_yn == 'y' && really_yn != 'y' )
	{
		if ( ( expanded_date =
			date_new_yyyy_mm_dd_date(
				from_measurement_date ) ) )
		{
			date_decrement_days(
				expanded_date,
				DATE_EXPAND_PLUS_MINUS );

			expanded_from_date_string =
				date_get_yyyy_mm_dd_string(
					expanded_date );

			date_free( expanded_date );
		}

		if ( ( expanded_date =
			date_new_yyyy_mm_dd_date(
				to_measurement_date ) ) )
		{
			date_increment_days(
				expanded_date,
				DATE_EXPAND_PLUS_MINUS,
				date_utc_offset() );

			expanded_to_date_string =
				date_get_yyyy_mm_dd_string(
					expanded_date );

			date_free( expanded_date );
		}
	}
	else
	{
		expanded_from_date_string = from_measurement_date;
		expanded_to_date_string = to_measurement_date;
	}

	strcpy(	title,
		format_initial_capital(
			buffer,
			process_name ) );

	sprintf(	sub_title,
			"%s/%s from %s:%s to %s:%s",
			station,
			format_initial_capital( buffer, datatype ),
			from_measurement_date,
			from_measurement_time,
			to_measurement_date,
			to_measurement_time );

	if ( strcmp( process_name, DRIFT_RATIO_PROCESS_NAME ) == 0 )
	{
		double local_first_multiplier;
		double local_last_multiplier;

		drift_parameter_dictionary = dictionary_small_new();

		if ( !hydrology_library_get_drift_ratio_variables(
						drift_parameter_dictionary,
						&local_first_multiplier,
						&local_last_multiplier,
						application_name,
						first_value,
						last_value,
						station,
						datatype,
						from_measurement_date,
						from_measurement_time,
						to_measurement_date,
						to_measurement_time ) )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			printf(
			"<h3>Cannot get drift ratio variables.</h3>\n" );
			document_close();
			exit( 0 );
		}
	}

	if ( chart_yn == 'y' && really_yn != 'y' )
	{
		sprintf(infrastructure_process,
			INFRASTRUCTURE_PROCESS,
			appaserver_parameter_file->appaserver_mount_point,
			application_get_relative_source_directory(
				application_name ),
			application_name,
			station,
			datatype );

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

		if ( !grace_set_begin_end_date(grace,
						expanded_from_date_string,
						expanded_to_date_string ) )
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

		grace->grace_output =
			application_get_grace_output( application_name );

		sprintf(	graph_identifier,
				"%s_%s_%d",
				station,
				datatype,
				getpid() );

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
	}
	else
	/* ----------------------------------------------- */
	/* Must not be chart_yn == 'y' or really_yn == 'y' */
	/* ----------------------------------------------- */
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
		     new_html_table( title, sub_title );
	
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

		if ( strcmp( process_name, DRIFT_RATIO_PROCESS_NAME ) == 0 )
		{
			printf( "<table align=center>\n"
				"<tr><td>%s</td><td>%.3lf</td>\n"
				"<tr><td>%s</td><td>%.3lf</td>\n"
				"<tr><td>%s</td><td>%s</td>\n"
				"<tr><td>%s</td><td>%s</td>\n"
				"<tr><td>%s</td><td>%s</td>\n"
				"<tr><td>%s</td><td>%s</td>\n",
				"First Value",
				first_value,
				"Last Value",
				last_value,
				"Original First Value",
				dictionary_fetch( drift_parameter_dictionary,
						  "original_first_value" ),
				"Original Last Value",
				dictionary_fetch( drift_parameter_dictionary,
						  "original_last_value" ),
				"First Multiplier",
				dictionary_fetch( drift_parameter_dictionary,
						  "first_multiplier" ),
				"Last Multiplier",
				dictionary_fetch( drift_parameter_dictionary,
						  "last_multiplier" )
				);
			printf( "</table>\n" );
			printf( "<table border>\n" );
		}

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}

	sprintf(sys_string,
"%s/%s/estimation_linear_interpolation_drift %s %s %s %s %lf %lf %lf %lf %s %s %s %s %s %s \"%s\" \"%s\" %s %c 2>>%s",
		appaserver_parameter_file->appaserver_mount_point,
		application_get_relative_source_directory(
				application_name ),
		login_name,
		application_name,
		station,
		datatype,
		first_value,
		last_value,
		first_multiplier,
		last_multiplier,
		from_measurement_date,
		from_measurement_time,
		to_measurement_date,
		to_measurement_time,
		expanded_from_date_string,
		expanded_to_date_string,
		parameter_dictionary_string,
		notes,
		process_name,
		really_yn,
		appaserver_error_get_filename(
			application_name ) );

	input_pipe = popen( sys_string, "r" );

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

		if ( chart_yn == 'y' && really_yn != 'y' )
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

	if ( chart_yn == 'y' && really_yn != 'y' )
	{
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
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<p>Warning: no graphs to display.\n" );
			document_close();
			exit( 0 );
		}

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
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				grace->distill_landscape_flag,
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
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
	}
	else
	/* ------------------------------------------ */
	/* if ( chart_yn != 'y' || really_yn == 'y' ) */
	/* ------------------------------------------ */
	{
		/* Output the count */
		/* ---------------- */
		if ( !atoi( input_buffer ) )
		{
			strcpy( buffer, "No measurements selected." );
		}
		else
		if ( really_yn != 'y' )
		{
			sprintf( buffer, "Did not process %s", input_buffer );
		}
		else
		{
			sprintf( buffer, "Processed %s", input_buffer );
			process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
		}
	
		html_table_set_data( html_table->data_list, buffer );
		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table_close();
	}

	if ( really_yn == 'y' )
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

	document_close();

	exit( 0 );
} /* main() */

