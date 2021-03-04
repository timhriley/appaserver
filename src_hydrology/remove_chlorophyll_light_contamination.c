/* ------------------------------------------------------------ */
/* src_hydrology/remove_chlorophyll_light_contamination.c	*/
/* ------------------------------------------------------------ */
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

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
#include "measurement_update_parameter.h"
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
/* #define TESTING_ONLY		1 */
#define ESTIMATION_METHOD	"block_delete"
#define INFRASTRUCTURE_PROCESS	"%s/%s/estimation_datatype_unit_record_list.sh %s %s %s"
#define PDF_PROMPT		"Press to view chart."
#define STATION_PIECE		 0
#define DATATYPE_PIECE		 1
#define DATE_PIECE		 2
#define TIME_PIECE		 3
#define VALUE_PIECE		 4

/* Prototypes */
/* ---------- */
int perform_execute(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			char *from_measurement_time,
			char *to_measurement_time,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			DICTIONARY *parameter_dictionary,
			char *login_name,
			char *process_name );

void output_table(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *from_measurement_time,
			char *to_measurement_date,
			char *to_measurement_time,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char contamination_seek_yn );

void output_gracechart(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char *argv_0 );

FILE *get_input_pipe(
			char *application_name,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			char *from_measurement_time,
			char *to_measurement_time );

int main( int argc, char **argv )
{
	char *login_name;
	char *process_name;
	char *application_name;
	char *station;
	char *datatype;
	char buffer[ 4096 ];
	char chart_yn;
	char really_yn;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	char *from_measurement_date, *to_measurement_date;
	char *from_measurement_time, *to_measurement_time;
	char title[ 256 ];
	char sub_title[ 256 ] = {0};
	char *database_string = {0};
	char contamination_seek_yn;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	if ( argc != 14 )
	{
		fprintf(stderr,
"Usage: %s application process login_name station datatype from_date from_time to_date to_time parameter_dictionary contamination_seek_yn chart_yn really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	from_measurement_date = argv[ 6 ];
	from_measurement_time = argv[ 7 ];
	to_measurement_date = argv[ 8 ];
	to_measurement_time = argv[ 9 ];
	parameter_dictionary_string = argv[ 10 ];
	contamination_seek_yn = *argv[ 11 ];
	chart_yn = *argv[ 12 ];
	really_yn = *argv[ 13 ];

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

	if ( really_yn == 'y' )
	{
		if ( !*from_measurement_time
		||   strcmp( from_measurement_time, "begin_time" ) == 0
		||   !*to_measurement_time
		||   strcmp( to_measurement_time, "end_time" ) == 0 )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
					appaserver_mount_point );

			printf( "<p>Please enter begin and end times.\n" );
			document_close();
			exit( 0 );
		}
	}
	else
	{
		if ( !*from_measurement_time
		||   strcmp( from_measurement_time, "begin_time" ) == 0 )
		{
			from_measurement_time = "0000";
		}
	
		if ( !*to_measurement_time
		||   strcmp( to_measurement_time, "end_time" ) == 0 )
		{
			to_measurement_time = "2359";
		}
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

	make_date_time_between_compatable(	&to_measurement_date,
						&from_measurement_time,
						&to_measurement_time,
						from_measurement_date );

	if (	chart_yn != 'y'
	&& 	really_yn != 'y'
	&&	contamination_seek_yn == 'y' )
	{
		from_measurement_time = "0000";
		to_measurement_time = "2359";
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


	if ( chart_yn == 'y' && really_yn != 'y' )
	{
		output_gracechart(
			application_name,
			title,
			sub_title,
			station,
			datatype,
			from_measurement_date,
			to_measurement_date,
			appaserver_parameter_file,
			argv[ 0 ] );
	}
	else
	if ( chart_yn != 'y' && really_yn != 'y' )
	{
		output_table(
			application_name,
			title,
			sub_title,
			station,
			datatype,
			from_measurement_date,
			from_measurement_time,
			to_measurement_date,
			to_measurement_time,
			appaserver_parameter_file,
			contamination_seek_yn );
	}
	else
	if ( really_yn == 'y' )
	{
		int record_count;

		record_count = perform_execute(
			application_name,
			title,
			sub_title,
			station,
			datatype,
			from_measurement_date,
			to_measurement_date,
			from_measurement_time,
			to_measurement_time,
			appaserver_parameter_file,
			parameter_dictionary,
			login_name,
			process_name );

		printf( "<p>Deleted %d measurements.\n", record_count );
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

FILE *get_input_pipe(
			char *application_name,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			char *from_measurement_time,
			char *to_measurement_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char measurement_time_where[ 128 ];
	char *select;

	select =
	"station,datatype,measurement_date,measurement_time,measurement_value";

	if ( from_measurement_time && *from_measurement_time )
	{
		sprintf( measurement_time_where,
			 "and measurement_time between '%s' and '%s'",
			 from_measurement_time,
			 to_measurement_time );
	}
	else
	{
		strcpy( measurement_time_where, "and 1 = 1" );
	}

	sprintf( where,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date between '%s' and '%s'	"
		 "%s						",
		 station,
		 datatype,
		 from_measurement_date,
		 to_measurement_date,
		 measurement_time_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=measurement	"
		 "			where=\"%s\"		"
		 "			order=select		",
		 application_name,
		 select,
		 where );

	return popen( sys_string, "r" );
} /* get_input_pipe() */

void output_gracechart(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char *argv_0 )
{
	GRACE *grace = {0};
	char graph_identifier[ 128 ];
	char infrastructure_process[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];

	sprintf(infrastructure_process,
		INFRASTRUCTURE_PROCESS,
		appaserver_parameter_file->appaserver_mount_point,
		application_relative_source_directory(
			application_name ),
		application_name,
		station,
		datatype );

	grace = grace_new_unit_graph_grace(
			application_name,
			(char *)0 /* role_name */,
			infrastructure_process,
			(char *)0 /* data_process */,
			argv_0,
			STATION_PIECE,
			DATATYPE_PIECE,
			DATE_PIECE,
			TIME_PIECE,
			VALUE_PIECE,
			strdup( title ),
			strdup( sub_title ),
			0 /* not datatype_type_xyhilo */,
			no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date( grace,
					from_measurement_date,
					to_measurement_date ) )
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
				application_relative_source_directory(
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
		application_grace_output( application_name );

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

	input_pipe = get_input_pipe(
			application_name,
			station,
			datatype,
			from_measurement_date,
			to_measurement_date,
			(char *)0 /* from_measurement_time */,
			(char *)0 /* to_measurement_time */ );

	/* Sample input: "BD^chlorophyll^1999-01-01^2.5" */
	/* --------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		grace_set_string_to_point_list(
				grace->graph_list, 
				STATION_PIECE,
				DATATYPE_PIECE,
				DATE_PIECE,
				TIME_PIECE,
				VALUE_PIECE,
				input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

	} /* while( get_line() */

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

} /* output_gracechart() */

void output_table(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *from_measurement_time,
			char *to_measurement_date,
			char *to_measurement_time,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char contamination_seek_yn )
{
	DOCUMENT *document;
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	HTML_TABLE *html_table = {0};
	LIST *heading_list;
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char value[ 128 ];
	char buffer[ 128 ];

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

	input_pipe = get_input_pipe(
			application_name,
			station,
			datatype,
			from_measurement_date,
			to_measurement_date,
			from_measurement_time,
			to_measurement_time );

	html_table = new_html_table( title, sub_title );
	
	heading_list = new_list();
	list_append_string( heading_list, "Station" );
	list_append_string( heading_list, "Datatype" );
	list_append_string( heading_list, "Date" );
	list_append_string( heading_list, "Time" );
	list_append_string( heading_list, "Value" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	/* Sample input: "BD^chlorophyll^1999-01-01^2.5" */
	/* --------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			TIME_PIECE );

		piece(	value,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			VALUE_PIECE );

		if ( atof( value ) < DRIFT_MEASUREMENT_DETECTION_LIMIT )
		{
			html_table_set_background_shaded( html_table );
		}
		else
		{
			if ( contamination_seek_yn == 'y' ) continue;
			html_table_set_background_unshaded( html_table );
		}

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
					strdup( value ) );
	
		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	} /* while( get_line() */

	pclose( input_pipe );

} /* output_table() */

int perform_execute(	char *application_name,
			char *title,
			char *sub_title,
			char *station,
			char *datatype,
			char *from_measurement_date,
			char *to_measurement_date,
			char *from_measurement_time,
			char *to_measurement_time,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			DICTIONARY *parameter_dictionary,
			char *login_name,
			char *process_name )
{
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	FILE *delete_pipe;
	char sys_string[ 1024 ];
	char *field;
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char *table;
	DOCUMENT *document;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	MEASUREMENT_BACKUP *measurement_backup;
	int record_count = 0;
	char *executable;

#ifdef TESTING_ONLY
	executable = "html_paragraph_wrapper.e";
#else
	executable = "sql.e";
#endif

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

	printf( "<h1>%s</h1><h2>%s</h2>\n", title, sub_title );

	table = "measurement";
	field = "station,datatype,measurement_date,measurement_time";

	sprintf( sys_string,
		 "delete_statement.e table=%s field=%s delimiter='^'	|"
		 "%s							 ",
		 table,
		 field,
		 executable );

	delete_pipe = popen( sys_string, "w" );

	input_pipe = get_input_pipe(
			application_name,
			station,
			datatype,
			from_measurement_date,
			to_measurement_date,
			from_measurement_time,
			to_measurement_time );

	measurement_update_parameter =
		measurement_update_parameter_new(
				application_name,
				station,
				datatype,
				ESTIMATION_METHOD,
				login_name,
				parameter_dictionary,
				(char *)0 /* notes */ );

	measurement_backup = 
		new_measurement_backup(
				application_name, 
				ESTIMATION_METHOD,
				login_name,
				measurement_update_parameter->now_date,
				measurement_update_parameter->now_time );

	measurement_backup->insert_pipe =
		measurement_backup_open_insert_pipe(
					application_name );

	/* Sample input: "BD^chlorophyll^1999-01-01^2.5" */
	/* --------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		record_count++;

		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			TIME_PIECE );

		fprintf( delete_pipe,
			 "%s^%s^%s^%s\n",
			 station,
			 datatype,
			 measurement_date,
			 measurement_time );

		measurement_backup_insert(
			measurement_backup->
				insert_pipe,
			measurement_backup->
				measurement_update_date,
			measurement_backup->
				measurement_update_time,
			measurement_backup->
			      measurement_update_method,
			measurement_backup->
				login_name,
			input_buffer,
			'y' /* really_yn */,
			FOLDER_DATA_DELIMITER );

	}

	pclose( input_pipe );
	pclose( delete_pipe );

	measurement_update_parameter_save(
				measurement_update_parameter );

	measurement_backup_close( measurement_backup->insert_pipe );

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	
	return record_count;
} /* perform_execute() */

