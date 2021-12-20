/* ----------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/cubic_feet_per_second_moving_sum.c	*/
/* ----------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "environ.h"
#include "process.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "station_datatype.h"
#include "grace.h"
#include "julian.h"
#include "session.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define UNITS				"cfs"
#define EXCEEDANCE_DELIMITER		'|'
#define DEFAULT_DAYS_TO_SUM		30
#define DEFAULT_OUTPUT_MEDIUM		"chart"
#define GRACE_DATATYPE_ENTITY_PIECE	0
#define GRACE_DATATYPE_PIECE		1
#define GRACE_DATE_PIECE		2
#define GRACE_TIME_PIECE		-1
#define GRACE_VALUE_PIECE		3
#define GRACE_TICKLABEL_ANGLE		90
#define KEY_DELIMITER			'/'

#define ROWS_BETWEEN_HEADING			20
#define SELECT_LIST			"measurement_date,measurement_value"

#define DATE_PIECE		 		0
#define TIME_PIECE		 		-1
#define VALUE_PIECE		 		1
#define INPUT_DELIMITER				','
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
char *cubic_title(	LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum );

char *cubic_where_clause(
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum );

void cubic_stdout(	char *input_system_string );

void cubic_table(	char *input_system_string,
			char *units_display,
			char *title,
			char *where_clause );

void cubic_text_file(	char *input_system_string,
			int days_to_sum,
			char *units_display,
			char *title,
			char *where_clause );

void cubic_spreadsheet(	char *application_name,
			char *input_system_string,
			char *units_display,
			char *title,
			char *where_clause,
			char *process_name,
			char *document_root_directory );

void cubic_chart(	char *argv_0,
			char *application_name,
			char *role_name,
			char *input_system_string,
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units_display,
			char *title,
			char *where_clause,
			char *document_root_directory );

void cubic_feet_per_second_moving_sum(
			char *argv_0,
			char *application_name,
			char *role_name,
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *output_medium,
			char *units,
			char *units_converted,
			char *units_display,
			char *document_root_directory,
			char *process_name );

char *cubic_input_system_string(
			char *application_name,
			char *where_clause,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units,
			char *units_converted );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *role_name;
	LIST *station_list;
	char datatype_name[ 128 ];
	char *begin_date_string;
	char *end_date_string;
	int days_to_sum;
	char *output_medium;
	char *units_converted;
	char *units_display;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s process role station datatype begin_date end_date days_to_sum output_medium units_converted\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	role_name = argv[ 2 ];
	station_list = list_string_list( argv[ 3 ], ',' );
	piece( datatype_name, ',', argv[ 4 ], 0 );
	begin_date_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	days_to_sum = atoi( argv[ 7 ] );
	output_medium = argv[ 8 ];
	units_converted = argv[ 9 ];

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !days_to_sum ) days_to_sum = DEFAULT_DAYS_TO_SUM;

	if (	!output_medium
	|| 	!*output_medium
	|| 	strcmp( output_medium, "select" ) == 0
	|| 	strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );
	}

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			UNITS,
			units_converted,
			sum /* aggregate_statistic */ );

	cubic_feet_per_second_moving_sum(
			argv[ 0 ],
			application_name,
			role_name,
			station_list,
			datatype_name,
			begin_date_string,
			end_date_string,
			days_to_sum,
			output_medium,
			UNITS,
			units_converted,
			units_display,
			appaserver_parameter_file->document_root,
			process_name );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;
}

void cubic_feet_per_second_moving_sum(
			char *argv_0,
			char *application_name,
			char *role_name,
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *output_medium,
			char *units,
			char *units_converted,
			char *units_display,
			char *document_root_directory,
			char *process_name )
{
	char *input_system_string;
	char *where_clause;
	char *title;

	title =
		cubic_title(
			station_list,
			datatype_name,
			begin_date_string,
			end_date_string,
			days_to_sum );

	where_clause =
		cubic_where_clause(
			station_list,
			datatype_name,
			begin_date_string,
			end_date_string,
			days_to_sum );

	input_system_string =
		cubic_input_system_string(
			application_name,
			where_clause,
			begin_date_string,
			end_date_string,
			days_to_sum,
			units,
			units_converted );

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		cubic_stdout( input_system_string );
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		cubic_table(
			input_system_string,
			units_display,
			title,
			where_clause );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		cubic_text_file(
			input_system_string,
			days_to_sum,
			units_display,
			title,
			where_clause );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		cubic_spreadsheet(
			application_name,
			input_system_string,
			units_display,
			title,
			where_clause,
			process_name,
			document_root_directory );
	}
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		cubic_chart(
			argv_0,
			application_name,
			role_name,
			input_system_string,
			station_list,
			datatype_name,
			begin_date_string,
			end_date_string,
			days_to_sum,
			units_display,
			title,
			where_clause,
			document_root_directory );
	}
}

char *cubic_where_clause(
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum )
{
	char where_clause[ 512 ];
	JULIAN *new_begin_date;

	new_begin_date = julian_new_yyyy_mm_dd( begin_date_string );
	julian_decrement_days( new_begin_date, days_to_sum - 1 );

	sprintf( where_clause,
 	"station in (%s) and 				      "
 	"datatype = '%s' and				      "
	"measurement_date between '%s' and '%s' 	      ",
		timlib_in_clause( station_list ),
		datatype_name,
		julian_display_yyyy_mm_dd( new_begin_date->current ),
		end_date_string );

	return strdup( where_clause );
}

char *cubic_input_system_string(
			char *application_name,
			char *where_clause,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units,
			char *units_converted )
{
	char aggregation_process[ 1024 ];
	char units_converted_process[ 128 ];
	JULIAN *new_begin_date;
	char system_string[ 1024 ];

	new_begin_date = julian_new_yyyy_mm_dd( begin_date_string );
	julian_decrement_days( new_begin_date, days_to_sum - 1 );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s 1 '%c'",
			 units,
			 units_converted,
			 INPUT_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(
			aggregation_process, 
	 "real_time2aggregate_value.e sum %d %d %d '%c' daily n %s	|"
	 "piece_inverse.e 3 '%c'					|"
	 "%s								|"
	 "pad_missing_times.e ',' 0,-1,1 daily %s 0000 %s 2359 0 '' 	|"
	 "moving_average.e %d '%c' %s %s				 ",
	 		DATE_PIECE,
	 		TIME_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			end_date_string,
			INPUT_DELIMITER,
			units_converted_process,
			julian_display_yyyy_mm_dd( new_begin_date->current ),
			end_date_string,
			days_to_sum,
			INPUT_DELIMITER,
			"sum",
			begin_date_string );

	sprintf( system_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		    "
	"			quick=yes		   |"
	"tr '%c' '%c' 					   |"
	"sort						   |"
	"%s						   |"
	"sort						    ",
		application_name,
		SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregation_process );

	return strdup( system_string );
}

void cubic_stdout( char *input_system_string )
{
	FILE *input_pipe = popen( input_system_string, "r" );
	char input_buffer[ 1024 ];

	while( get_line( input_buffer, input_pipe ) )
	{
		printf( "%s\n",
			input_buffer );
	}

	pclose( input_pipe );
}

void cubic_table(	char *input_system_string,
			char *units_display,
			char *title,
			char *where_clause )
{
	FILE *input_pipe = popen( input_system_string, "r" );
	char input_buffer[ 1024 ];
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char units_buffer[ 128 ];
	char title_buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	int count = 0;

	html_table =
		html_table_new(
			format_initial_capital(
				title_buffer,
				title ),
			where_clause /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	heading_list = new_list();

	list_set( heading_list, "Date" );

	list_set(
		heading_list,
		format_initial_capital(
			units_buffer,
			units_display ) );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 1;

	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
		html_table->title,
		html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );

		html_table_set_data(
			html_table->data_list,
			strdup( date_string ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_number_string(
					value_string ) ) );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();
	}

	pclose( input_pipe );
	html_table_close();
}

void cubic_text_file(	char *input_system_string,
			int days_to_sum,
			char *units_display,
			char *title,
			char *where_clause )
{
	char buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	FILE *input_pipe;
	FILE *output_pipe;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", where_clause );
	fflush( stdout );

	output_pipe = popen( "html_paragraph_wrapper.e", "w" );

	fprintf(	output_pipe,
			"Date|MovingSum(%d)(%s)\n",
			days_to_sum,
			units_display );

	input_pipe = popen( input_system_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, buffer, 0 );
		piece( value_string, INPUT_DELIMITER, buffer, 1 );

		fprintf( output_pipe,
			 "%s|%s\n",
			 date_string,
			 strdup( place_commas_in_number_string(
					value_string ) ) );
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

void cubic_spreadsheet(	char *application_name,
			char *input_system_string,
			char *units_display,
			char *title,
			char *where_clause,
			char *process_name,
			char *document_root_directory )
{
	char *ftp_filename;
	char *output_filename;
	pid_t process_id = getpid();
	FILE *input_pipe;
	FILE *output_pipe;
	FILE *output_file;
	char output_system_string[ 512 ];
	char input_buffer[ 1024 ];

	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( output_system_string,
	 	"tr '|' ',' > %s",
	 	output_filename );

	output_pipe = popen( output_system_string, "w" );

	fprintf( output_pipe, "Date,%s\n", units_display );

	input_pipe = popen( input_system_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf(output_pipe,
			"%s\n",
			input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	printf( "<h1>%s</h1><h2>%s</h2>\n",
		title,
		where_clause );

	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){};
	fflush( stdout );
	printf( "</h2>\n" );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		TRANSMIT_PROMPT,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );
}

char *cubic_title(	LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum )
{
	char title[ 1024 ];

	sprintf(	title,
	"%d Day Moving Sum<br>%s/%s<br>From: %s To: %s",
			days_to_sum,
			list_display_delimited( station_list, ',' ),
			datatype_name,
			begin_date_string,
			end_date_string );

	return strdup( title );
}

void cubic_chart(	char *argv_0,
			char *application_name,
			char *role_name,
			char *input_system_string,
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units_display,
			char *title,
			char *where_clause,
			char *document_root_directory )
{
	char input_buffer[ 512 ];
	char graph_title[ 512 ];
	char graph_sub_title[ 512 ];
	char buffer1[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	GRACE *grace;
	char graph_identifier[ 128 ];
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
	char grace_string[ 128 ];

	printf( "<h1>%s</h1><h2>%s</h2>\n",
		title,
		where_clause );

	sprintf(graph_title,
		"%d Day Moving Sum",
		days_to_sum );

	sprintf(graph_sub_title,
		"%s/%s From %s To %s",
			list_display_delimited( station_list, ',' ),
			format_initial_capital( buffer1, datatype_name ),
			begin_date_string,
			end_date_string );

	grace =
		grace_new_unit_graph_grace(
			application_name,
			role_name,
			(char *)0 /* infrastructure_process */,
			(char *)0 /* data_process */,
			argv_0,
			GRACE_DATATYPE_ENTITY_PIECE,
			GRACE_DATATYPE_PIECE,
			GRACE_DATE_PIECE,
			GRACE_TIME_PIECE,
			GRACE_VALUE_PIECE,
			graph_title,
			graph_sub_title,
			0 /* not datatype_type_xyhilo */,
			no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
			begin_date_string,
			end_date_string ) )
	{
		printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph_set_scale_to_zero( grace_graph, 1 );

	grace_graph->units = units_display;

	grace_datatype =
		grace_new_grace_datatype(
			(char *)0 /* station_name */,
			datatype_name );

	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;

	if ( hydrology_library_get_bar_graph(
					application_name,
					datatype_name ) )
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		grace_datatype->line_linestyle = 0;
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

	list_set(	grace_graph->datatype_list,
			grace_datatype );

	list_set( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output(
			application_name );

	sprintf(	graph_identifier,
			"%s_%d",
			datatype_name,
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

	input_pipe = popen( input_system_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );

		/* -------------------------------------------- */
		/* Sample grace_string:				*/
		/* BA|bottom_temperature|2000-06-01|29.334|24	*/
		/* -------------------------------------------- */
		sprintf( grace_string,
			 "|%s|%s|%s|1",
			 datatype_name,
			 date_string,
			 value_string );

		grace_set_string_to_point_list(
			grace->graph_list, 
			GRACE_DATATYPE_ENTITY_PIECE,
			GRACE_DATATYPE_PIECE,
			GRACE_DATE_PIECE,
			GRACE_TIME_PIECE,
			GRACE_VALUE_PIECE,
			grace_string,
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
		printf( "<h2>Warning: no graphs to display.</h2>\n" );
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
			(char *)0 /* appaserver_mount_point */,
			0 /* not with_document_output */,
			(char *)0 /* where_clause */ );
	}
}
