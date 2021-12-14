/* -------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/cubic_feet_per_second_sum.c		*/
/* -------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------- 	*/

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
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "station_datatype.h"
#include "grace.h"
#include "julian.h"
#include "hash_table.h"
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
void cubic_stdout(	char *input_system_string );

void cubic_feet_per_second_sum(
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
			char *units_display );

char *cubic_input_system_string(
			char *application_name,
			LIST *station_list,
			char *datatype_name,
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

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( output_medium, "chart" ) != 0 )
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

	cubic_feet_per_second_sum(
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
			units_display );

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

#ifdef NOT_DEFINED
int daily_moving_average_output_chart(
				char *application_name,
				char *role_name,
				char *station_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *argv_0,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic )
{
	char input_buffer[ 512 ];
	char title[ 512 ];
	char sub_title[ 512 ];
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
	boolean bar_graph;
	char legend[ 128 ];
	char grace_string[ 128 ];

	sprintf(title,
		"%d Day Moving %s",
		days_to_average,
		format_initial_capital(
			buffer1,
			aggregate_statistic_get_string(
				aggregate_statistic ) ) );

	sprintf(sub_title,
		"%s/%s from %s to %s",
			station_name,
			format_initial_capital( buffer1, datatype_name ),
			begin_date_string,
			end_date_string );

	grace = grace_new_unit_graph_grace(
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
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph->units = units_display;

	grace_datatype = grace_new_grace_datatype(
					strdup( station_name ),
					strdup( datatype_name ) );

	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;

	strcpy( legend, datatype_name );

	strcpy(	legend,
		format_initial_capital( buffer1, legend ) );

	grace_datatype->legend = strdup( legend );

	bar_graph =
		hydrology_library_get_bar_graph(
					application_name,
					datatype_name );

	if ( bar_graph )
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		grace_datatype->line_linestyle = 0;
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

/*
	grace_datatype->datatype_type_bar_xy_xyhilo =
				(bar_graph) ? "bar" : "xy";
*/

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf(	graph_identifier,
			"%s_%s_%d",
			station_name,
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

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );

		/* -------------------------------------------- */
		/* Sample grace_string:				*/
		/* BA|bottom_temperature|2000-06-01|29.334|24	*/
		/* -------------------------------------------- */
		sprintf( grace_string,
			 "%s|%s|%s|%s|1",
			 station_name,
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
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<h2>Warning: no graphs to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	/* grace->symbols = 1; */

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
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
	}
	return 1;
}

void daily_moving_average_output_transmit(
				FILE *output_pipe,
				char *station_name,
				char *datatype_name,
				char *sys_string,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic,
				char *begin_date_string,
				char *end_date_string,
				char *application_name )
{
	char buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	FILE *input_pipe;

	hydrology_library_output_station_text_file(
			output_pipe,
			application_name,
			station_name );

	fprintf(	output_pipe,
	"#%d Day Moving %s for: %s/%s from: %s to: %s\n",
			days_to_average,
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

	fprintf(	output_pipe,
			"#Date|Moving%s(%d)(%s)\n",
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			days_to_average,
			units_display );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, buffer, 0 );
		piece( value_string, INPUT_DELIMITER, buffer, 1 );

		fprintf( output_pipe,
			 "%s|%s\n",
			 date_string,
			 value_string );
	}

	pclose( input_pipe );

}

void daily_moving_average_output_table(
				char *station_name,
				char *datatype_name,
				char *sys_string,
				char *begin_date_string,
				char *end_date_string,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic,
				char *application_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char units_buffer[ 128 ];
	char buffer[ 128 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	int count = 0;

	hydrology_library_output_station_table(
		application_name,
		station_name );

	sprintf(	title,
	"%d Day Moving %s<br>%s/%s<br>from: %s to: %s",
			days_to_average,
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

	html_table = new_html_table(
			format_initial_capital(
				title_buffer,
				title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	list_append_pointer( heading_list, "Date" );

	list_append_pointer(
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

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );

		html_table_set_data(	html_table->data_list,
					strdup( date_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( value_string ) );

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

void daily_moving_average_output_table_exceedance_format(
				char *station_name,
				char *datatype_name,
				char *sys_string,
				char *begin_date_string,
				char *end_date_string,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic,
				char *application_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	char units_buffer[ 128 ];
	char buffer[ 128 ];
	int count = 0;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;

	hydrology_library_output_station_table(
		application_name,
		station_name );

	sprintf(	title,
"%d Day Moving %s<br>%s/%s<br>from: %s to: %s<br>Exceedance Format",
			days_to_average,
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

	html_table = new_html_table(
			format_initial_capital(
				title_buffer,
				title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	list_append_pointer(
			heading_list,
			format_initial_capital(
				units_buffer,
				units_display ) );

	list_append_pointer( heading_list, "Count Below" );
	list_append_pointer( heading_list, "Percent Below" );
	list_append_pointer( heading_list, "Date" );

	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 4;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece_exceedance_variables(
				&value_string,
				&date_string,
				&count_below_string,
				&percent_below_string,
				input_buffer );

		html_table_set_data(	html_table->data_list,
					strdup( value_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( count_below_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( percent_below_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( date_string ) );

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

void piece_exceedance_variables(
				char **value_string,
				char **date_string,
				char **count_below_string,
				char **percent_below_string,
				char *input_buffer )
{
	static char local_value_string[ 128 ];
	static char local_date_string[ 128 ];
	static char local_count_below_string[ 128 ];
	static char local_percent_below_string[ 128 ];

	piece(	local_value_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		0 );
	piece(	local_date_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		1 );
	piece(	local_count_below_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		2 );
	piece(	local_percent_below_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		3 );
	*value_string = local_value_string;
	*date_string = local_date_string;
	*count_below_string = local_count_below_string;
	*percent_below_string = local_percent_below_string;
}

int daily_moving_average_output_chart_exceedance_format(
				char *application_name,
				char *role_name,
				char *station_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic )
{
	GRACE *grace;
	char title[ 512 ];
	char sub_title[ 512 ];
	char input_buffer[ 512 ];
	char buffer1[ 512 ];
	char legend[ 128 ];
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	GRACE_GRAPH *grace_graph;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;
	FILE *input_pipe;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;

	sprintf(title,
		"%d Day Moving %s",
		days_to_average,
		format_initial_capital(
			buffer1,
			aggregate_statistic_get_string(
				aggregate_statistic ) ) );

	sprintf(sub_title,
		"%s/%s from %s to %s Exceedance Format",
			station_name,
			format_initial_capital( buffer1, datatype_name ),
			begin_date_string,
			end_date_string );

	grace = grace_new_xy_grace(
				application_name,
				role_name,
				title,
				sub_title,
				units_display,
				format_initial_capital(
					legend, datatype_name ) );

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
		piece_exceedance_variables(
				&value_string,
				&date_string,
				&count_below_string,
				&percent_below_string,
				input_buffer );

		grace_set_xy_to_point_list(
				grace->graph_list, 
				atof( percent_below_string ),
				strdup( value_string ),
				(char *)0 /* optional_label */,
				grace->dataset_no_cycle_color );
	}

	pclose( input_pipe );

	grace_graph =
			(GRACE_GRAPH *)
				list_get_first_pointer(
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
		return 0;
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
		return 0;
	}

	grace_output_graph_window(
		application_name,
		ftp_output_filename,
		ftp_agr_filename,
		(char *)0 /* appaserver_mount_point */,
		1 /* with_document_output */,
		(char *)0 /* where_clause */ );

	return 1;

}

void daily_moving_average_output_transmit_exceedance_format(
				FILE *output_pipe,
				char *station_name,
				char *datatype_name,
				char *sys_string,
				int days_to_average,
				char *units_display,
				enum aggregate_statistic aggregate_statistic,
				char *begin_date_string,
				char *end_date_string,
				char *application_name )
{
	char buffer[ 512 ];
	FILE *input_pipe;

	hydrology_library_output_station_text_file(
			output_pipe,
			application_name,
			station_name );

	fprintf(	output_pipe,
	"#%d Day Moving %s (exceedance format) for: %s/%s from: %s to: %s\n",
			days_to_average,
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

	fprintf(	output_pipe,
			"#Moving%s(%d)(%s)|Date|CountBelow|%cBelow\n",
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			days_to_average,
			units_display,
			'%' );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", buffer );
	}

	pclose( input_pipe );

}
#endif

void cubic_feet_per_second_sum(
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
			char *units_display )
{
	char *input_system_string;

	input_system_string =
		cubic_input_system_string(
			application_name,
			station_list,
			datatype_name,
			begin_date_string,
			end_date_string,
			days_to_sum,
			units,
			units_converted );

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		cubic_stdout( input_system_string );
	}
}

char *cubic_input_system_string(
			char *application_name,
			LIST *station_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units,
			char *units_converted )
{
	char where_clause[ 512 ];
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
			 "measurement_convert_units.e %s %s 1 '%c' 2",
			 units,
			 units_converted,
			 INPUT_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf( where_clause,
 	"station in (%s) and 				      "
 	"datatype = '%s' and				      "
	"measurement_date between '%s' and '%s' and	      "
	"measurement_value is not null 			      ",
		timlib_in_clause( station_list ),
		datatype_name,
		julian_display_yyyy_mm_dd( new_begin_date->current ),
		end_date_string );

	sprintf(
			aggregation_process, 
	 "real_time2aggregate_value.e sum %d %d %d '%c' daily n %s	|"
	 "piece_inverse.e 3 '%c'					|"
	 "sort								|"
	 "%s								|"
	 "moving_average.e %d '%c' %s %s				 ",
	 		DATE_PIECE,
	 		TIME_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			end_date_string,
			INPUT_DELIMITER,
			units_converted_process,
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
"tee /dev/tty |"
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
