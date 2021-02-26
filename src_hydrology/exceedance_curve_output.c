/* -------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/exceedance_curve_output.c    	*/
/* --------------------------------------------------------	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------- 	*/

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
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "environ.h"
#include "make_date_time_between_compatable.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"
#include "process.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM			"table"
#define PDF_PROMPT				"Press to view chart."
#define FILENAME_STEM				"exceedance_curve"

#define VALUE_PIECE			0
#define REAL_TIME_DATE_PIECE		1
#define REAL_TIME_TIME_PIECE		2
#define REAL_TIME_COUNT_BELOW_PIECE	4
#define REAL_TIME_PERCENT_BELOW_PIECE	5
#define AGGREGATION_COUNT_PIECE		2
#define AGGREGATION_DATE_PIECE		3
#define AGGREGATION_TIME_PIECE		4
#define AGGREGATION_COUNT_BELOW_PIECE	5
#define AGGREGATION_PERCENT_BELOW_PIECE	6

/* Prototypes */
/* ---------- */
void exceedance_curve_data_output(
				FILE *output_pipe,
				char *measurement_value,
				char *count_below,
				char *percent_below,
				char *measurement_date,
				char *measurement_time,
				char *aggregation_count,
				enum aggregate_level aggregate_level );

LIST *exceedance_curve_output_get_heading_list(
				char *units_display,
				enum aggregate_level aggregate_level,
				boolean with_html );

boolean exceedance_curve_spreadsheet_output(
				char **output_filename,
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display );

boolean exceedance_curve_text_file_output(
				char **output_filename,
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display );

void exceedance_curve_stdout_output(
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display );

void exceedance_curve_output_get_sys_string(
					char *sys_string,
					char *application_name,
					char *appaserver_mount_point,
					char *where_clause,
					enum aggregate_level
						aggregate_level,
					enum aggregate_statistic
						aggregate_statistic,
					char *units,
					char *units_converted );

void exceedance_curve_output_get_where_clause(
					char *where_clause,
					char *station,
					char *datatype,
					char *begin_date,
					char *end_date );

void exceedance_curve_output_get_sub_title(
					char *sub_title,
					enum aggregate_level aggregate_level,
					char *value_label,
					char *begin_date,
					char *end_date );

void exceedance_curve_output_get_title( char *title,
					char *process_name,
					char *station,
					char *datatype );

void exceedance_curve_html_output(
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display );

GRACE *exceedance_curve_output(
				char **agr_filename,
				char **ftp_agr_filename,
				char **postscript_filename,
				char **output_filename,
				char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level,
				enum aggregate_statistic,
				char *output_medium,
				char *units,
				char *units_converted,
				char *units_display,
				char *appaserver_mount_point,
				char *document_root_directory );

void piece_input_buffer(
				char *measurement_value,
				char *count_below,
				char *percent_below,
				char *measurement_date,
				char *measurement_time,
				char *aggregation_count,
				char *input_buffer,
				enum aggregate_level aggregate_level );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char *email_address;
	char *aggregate_level_string;
	char *aggregate_statistic_string;
	char *horizontal_line_at_point;
	char *units;
	char *units_converted;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_medium;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	boolean bar_chart;
	char *agr_filename = {0};
	char *ftp_agr_filename = {0};
	char *postscript_filename = {0};
	char *output_filename = {0};
	char *ftp_output_filename = {0};
	char *units_display;
	GRACE *grace = {0};

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf(stderr,
"Usage: %s process_name station datatype begin_date end_date aggregate_level aggregate_statistic output_medium email_address units_converted horizontal_line_at_point\n",
			argv[ 0 ] );

		fprintf(stderr,
"Note: output_medium = {chart, table, spreadsheet, text_file, stdout}\n" );

		exit( 1 );
	}

	process_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	aggregate_level_string = argv[ 6 ];
	aggregate_statistic_string = argv[ 7 ];
	output_medium = argv[ 8 ];
	email_address = argv[ 9 ];
	units_converted = argv[ 10 ];
	horizontal_line_at_point = argv[ 11 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station,
					datatype );

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

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			aggregate_level );

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
					application_name,
					datatype,
					aggregate_statistic_none );
	}

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		exceedance_curve_html_output(
					application_name,
					process_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					station,
					datatype,
					begin_date,
					end_date,
					aggregate_level,
					aggregate_statistic,
					units,
					units_converted,
					units_display );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *output_filename;

		if ( !exceedance_curve_spreadsheet_output(
					&output_filename,
					application_name,
					process_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						document_root,
					station,
					datatype,
					begin_date,
					end_date,
					aggregate_level,
					aggregate_statistic,
					units,
					units_converted,
					units_display ) )
		{
			document_quick_output_body(
				application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
			printf( "<H2>ERROR: Cannot open output file %s</H2>\n",
				output_filename );
			document_close();
			exit( 1 );
		}
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		char *output_filename;

		if ( !exceedance_curve_text_file_output(
					&output_filename,
					application_name,
					process_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						document_root,
					station,
					datatype,
					begin_date,
					end_date,
					aggregate_level,
					aggregate_statistic,
					units,
					units_converted,
					units_display ) )
		{
			document_quick_output_body(
				application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
			printf( "<H2>ERROR: Cannot open output file %s</H2>\n",
				output_filename );
			document_close();
			exit( 1 );
		}
	}
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		exceedance_curve_stdout_output(
					application_name,
					process_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						document_root,
					station,
					datatype,
					begin_date,
					end_date,
					aggregate_level,
					aggregate_statistic,
					units,
					units_converted,
					units_display );
	}
	else
	{
		grace = exceedance_curve_output(
					&agr_filename,
					&ftp_agr_filename,
					&postscript_filename,
					&output_filename,
					&ftp_output_filename,
					application_name,
					process_name,
					station,
					datatype,
					begin_date,
					end_date,
					aggregate_level,
					aggregate_statistic,
					output_medium,
					units,
					units_converted,
					units_display,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						document_root );
	}

	if ( strcmp( output_medium, "chart" ) == 0 && grace )
	{
		GRACE_GRAPH *grace_graph;
		int page_width_pixels;
		int page_length_pixels;
		char *distill_landscape_flag;
		char *chart_email_command_line;

		grace_graph =
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list );

		grace_graph->xaxis_label = "Percent Below";
		grace_graph->horizontal_line_at_point =
			atof( horizontal_line_at_point );

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
						appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<h2>No data for selected parameters.</h2>\n" );
			document_close();
			exit( 0 );
		}

		chart_email_command_line =
				application_chart_email_command_line(
					application_name );

		if ( !*chart_email_command_line
		||   *chart_email_command_line == '#'
		||   strcmp( email_address, "email_address" ) == 0 )
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
		else
		{
			grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				output_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}

		document_close();
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

void piece_input_buffer(
				char *measurement_value,
				char *count_below,
				char *percent_below,
				char *measurement_date,
				char *measurement_time,
				char *aggregation_count,
				char *input_buffer,
				enum aggregate_level aggregate_level )
{
	piece(	measurement_value,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		VALUE_PIECE );

	if ( aggregate_level == real_time )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_TIME_PIECE );

		piece(	count_below,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_COUNT_BELOW_PIECE );

		piece(	percent_below,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_PERCENT_BELOW_PIECE );
		return;
	}

	if ( aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			AGGREGATION_TIME_PIECE );

	}
	piece(	measurement_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_DATE_PIECE );

	piece(	aggregation_count,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_COUNT_PIECE );

	piece(	count_below,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_COUNT_BELOW_PIECE );

	piece(	percent_below,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_PERCENT_BELOW_PIECE );

} /* piece_input_buffer() */

GRACE *exceedance_curve_output(
				char **agr_filename,
				char **ftp_agr_filename,
				char **postscript_filename,
				char **output_filename,
				char **ftp_output_filename,
				char *application_name,
				char *process_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *output_medium,
				char *units,
				char *units_converted,
				char *units_display,
				char *appaserver_mount_point,
				char *document_root_directory )
{
	char where_clause[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	char buffer[ 4096 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	char title[ 256 ];
	char sub_title[ 1024 ] = {0};
	GRACE *grace = {0};
	char graph_identifier[ 128 ];
	char legend[ 128 ];
	char value_label[ 128 ];

	sprintf( title, "%s for %s/%s", process_name, station, datatype );
	format_initial_capital( title, title );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	sprintf( sub_title,
		 "%s %s Values Begin: %s End: %s",
		 format_initial_capital(
				buffer,
		 		aggregate_level_get_string( aggregate_level ) ),
		 value_label,
		 begin_date,
		 end_date );

	sprintf( where_clause,
"station = '%s' and datatype = '%s' and measurement_date between '%s' and '%s' and measurement_value is not null",
		 station,
		 datatype,
		 begin_date,
		 end_date );

	sprintf(buffer,
"%s/%s/exceedance_curve %s \"%s\" \"%s\" %s \"%s\" \"%s\" 2>>%s",
		appaserver_mount_point,
		application_relative_source_directory( application_name ),
		application_name,
		where_clause,
		aggregate_level_get_string( aggregate_level ),
		aggregate_statistic_get_string( aggregate_statistic ),
		units,
		units_converted,
		appaserver_error_get_filename( application_name ) );

	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				title,
				sub_title,
				units_display,
				format_initial_capital( legend, datatype ) );

		grace->xaxis_ticklabel_precision = 0;
		grace->world_min_x = 0.0;
		grace->world_max_x = 100.0;

		sprintf(	graph_identifier,
				"%s_%s_%d",
				station,
				datatype,
				getpid() );

		grace->grace_output =
			application_grace_output( application_name );

		grace_get_filenames(
			agr_filename,
			ftp_agr_filename,
			postscript_filename,
			output_filename,
			ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );
	}

	input_pipe = popen( buffer, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				measurement_value,
				count_below,
				percent_below,
				measurement_date,
				measurement_time,
				aggregation_count,
				input_buffer,
				aggregate_level );

		if ( strcmp( output_medium, "chart" ) == 0 )
		{
			grace_set_xy_to_point_list(
				grace->graph_list, 
				atof( percent_below ),
				strdup( measurement_value ),
				(char *)0 /* optional_label */,
				grace->dataset_no_cycle_color );
		}

	} /* while( get_line() */

	pclose( input_pipe );
	return grace;

} /* exceedance_curve_output() */

void exceedance_curve_output_get_title( char *title,
					char *process_name,
					char *station,
					char *datatype )
{
	sprintf( title, "%s for %s/%s", process_name, station, datatype );
	format_initial_capital( title, title );

} /* exceedance_curve_output_get_title() */

void exceedance_curve_output_get_sub_title(
					char *sub_title,
					enum aggregate_level aggregate_level,
					char *value_label,
					char *begin_date,
					char *end_date )
{
	char buffer[ 128 ];

	sprintf( sub_title,
		 "%s %s Values Begin: %s End: %s",
		 format_initial_capital(
				buffer,
		 		aggregate_level_get_string( aggregate_level ) ),
		 value_label,
		 begin_date,
		 end_date );
} /* exceedance_curve_output_get_sub_title() */

void exceedance_curve_output_get_where_clause(
					char *where_clause,
					char *station,
					char *datatype,
					char *begin_date,
					char *end_date )
{
	sprintf( where_clause,
"station = '%s' and datatype = '%s' and measurement_date between '%s' and '%s' and measurement_value is not null",
		 station,
		 datatype,
		 begin_date,
		 end_date );

} /* exceedance_curve_output_get_where_clause() */

void exceedance_curve_output_get_sys_string(
					char *sys_string,
					char *application_name,
					char *appaserver_mount_point,
					char *where_clause,
					enum aggregate_level
						aggregate_level,
					enum aggregate_statistic
						aggregate_statistic,
					char *units,
					char *units_converted )
{
	sprintf(sys_string,
"%s/%s/exceedance_curve %s \"%s\" \"%s\" %s \"%s\" \"%s\" 2>>%s",
		appaserver_mount_point,
		application_relative_source_directory( application_name ),
		application_name,
		where_clause,
		aggregate_level_get_string( aggregate_level ),
		aggregate_statistic_get_string( aggregate_statistic ),
		units,
		units_converted,
		appaserver_error_get_filename( application_name ) );

} /* exceedance_curve_output_get_sys_string() */

void exceedance_curve_html_output(
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	HTML_TABLE *html_table = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	LIST *heading_list;
	char title[ 256 ];
	char sub_title[ 1024 ] = {0};
	char value_label[ 128 ];
	int count = 0;
	char value_heading[ 128 ];
	DOCUMENT *document;

	exceedance_curve_output_get_title(
		title,
		process_name,
		station,
		datatype );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	exceedance_curve_output_get_sub_title(
		sub_title,
		aggregate_level,
		value_label,
		begin_date,
		end_date );

	exceedance_curve_output_get_where_clause(
		where_clause,
		station,
		datatype,
		begin_date,
		end_date );

	exceedance_curve_output_get_sys_string(
		sys_string,
		application_name,
		appaserver_mount_point,
		where_clause,
		aggregate_level,
		aggregate_statistic,
		units,
		units_converted );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	sprintf( title + strlen( title ), "<br>%s", sub_title );

 	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );
	
	heading_list = new_list();

	sprintf( value_heading,
		 "Value<br>(%s)",
		 units_display );

	list_append_string( heading_list, value_heading );

	list_append_string( heading_list, "Count Below" );
	list_append_string( heading_list, "Percent Below" );
	
	if ( aggregate_level == real_time )
	{
		list_append_string( heading_list, "Date" );
		list_append_string( heading_list, "Time" );
	}
	else
	if ( aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		list_append_string(	heading_list,
					"Date" );
		list_append_string( heading_list, "Time" );
		list_append_string( 	heading_list,
					"Aggregation Count" );
	}
	else
	if ( aggregate_level == daily )
	{
		list_append_string(	heading_list,
					"Date" );
		list_append_string(	heading_list,
					"Aggregation Count" );
	}
	else
	{
		list_append_string(	heading_list,
					"Representative Date" );
		list_append_string(	heading_list,
					"Aggregation Count" );
	}

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
				html_table->title,
					html_table->sub_title );

	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 99;

	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				measurement_value,
				count_below,
				percent_below,
				measurement_date,
				measurement_time,
				aggregation_count,
				input_buffer,
				aggregate_level );

		html_table_set_data(	html_table->data_list,
					strdup( measurement_value ) );
		html_table_set_data(	html_table->data_list,
					strdup( count_below ) );
		html_table_set_data(	html_table->data_list,
					strdup( percent_below ) );
		html_table_set_data(	html_table->data_list,
					strdup( measurement_date ) );
	
		if ( aggregate_level == real_time )
		{
			html_table_set_data(
					html_table->data_list,
					strdup( measurement_time ) );
		}
		else
		if ( aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			html_table_set_data(
					html_table->data_list,
					strdup( measurement_time ) );

			html_table_set_data(
					html_table->data_list,
					strdup( aggregation_count ) );
		}
		else
		{
			html_table_set_data(
					html_table->data_list,
					strdup( aggregation_count ) );
		}

		if ( ++count == 20 )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
				number_left_justified_columns,
				html_table->
				number_right_justified_columns,
				html_table->justify_list );
			count = 0;
		}

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

	html_table_close();
	document_close();

} /* exceedance_curve_html_output() */

LIST *exceedance_curve_output_get_heading_list(
				char *units_display,
				enum aggregate_level aggregate_level,
				boolean with_html )
{
	LIST *heading_list;
	char value_heading[ 128 ];

	heading_list = new_list();

	if ( with_html )
	{
		sprintf(value_heading,
		 	"Value<br>(%s)",
		 	units_display );
	}
	else
	{
		sprintf(value_heading,
		 	"Value (%s)",
		 	units_display );
	}

	list_append_string( heading_list, value_heading );

	list_append_string( heading_list, "Count Below" );
	list_append_string( heading_list, "Percent Below" );
	
	if ( aggregate_level == real_time )
	{
		list_append_string( heading_list, "Date" );
		list_append_string( heading_list, "Time" );
	}
	else
	if ( aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		list_append_string(	heading_list,
					"Date" );

		list_append_string( heading_list, "Time" );

		list_append_string( 	heading_list,
					"Aggregation Count" );
	}
	else
	if ( aggregate_level == daily )
	{
		list_append_string(	heading_list,
					"Date" );
		list_append_string(	heading_list,
					"Aggregation Count" );
	}
	else
	{
		list_append_string(	heading_list,
					"Representative Date" );
		list_append_string(	heading_list,
					"Aggregation Count" );
	}

	return heading_list;

} /* exceedance_curve_output_get_heading_list() */

void exceedance_curve_data_output(	FILE *output_pipe,
					char *measurement_value,
					char *count_below,
					char *percent_below,
					char *measurement_date,
					char *measurement_time,
					char *aggregation_count,
					enum aggregate_level aggregate_level )
{

	fprintf( output_pipe, "%s,", measurement_value );
	fprintf( output_pipe, "%s,", count_below );
	fprintf( output_pipe, "%s,", percent_below );
	fprintf( output_pipe, "%s,", measurement_date );

	if ( aggregate_level == real_time )
	{
		fprintf( output_pipe, "%s\n", measurement_time );
	}
	else
	if ( aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		fprintf( output_pipe, "%s,", measurement_time );
		fprintf( output_pipe, "%s\n", aggregation_count );
	}
	else
	{
		fprintf( output_pipe, "%s\n", aggregation_count );
	}

} /* exceedance_curve_data_output() */

boolean exceedance_curve_spreadsheet_output(
				char **output_filename,
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	LIST *heading_list;
	char title[ 256 ];
	char sub_title[ 1024 ];
	char value_label[ 128 ];
	DOCUMENT *document;
	APPASERVER_LINK_FILE *appaserver_link_file;
	pid_t process_id = getpid();
	char *ftp_filename;
	FILE *output_pipe;
	char output_sys_string[ 128 ];

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	*output_filename =
		/* ---------------- */
		/* Returns strdup() */
		/* ---------------- */
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
		/* ---------------- */
		/* Returns strdup() */
		/* ---------------- */
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

	if ( ! ( output_pipe = fopen( *output_filename, "w" ) ) )
		return 0;
	else
		fclose( output_pipe );

	*title = '\0';
	exceedance_curve_output_get_title(
		title,
		process_name,
		station,
		datatype );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	*sub_title = '\0';
	exceedance_curve_output_get_sub_title(
		sub_title,
		aggregate_level,
		value_label,
		begin_date,
		end_date );

	exceedance_curve_output_get_where_clause(
		where_clause,
		station,
		datatype,
		begin_date,
		end_date );

	exceedance_curve_output_get_sys_string(
		sys_string,
		application_name,
		appaserver_mount_point,
		where_clause,
		aggregate_level,
		aggregate_statistic,
		units,
		units_converted );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	printf( "<h1>%s<br></h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
	
	hydrology_library_output_station_text_filename(
			*output_filename,
			application_name,
			station,
			1 /* with_zap_file */ );

	sprintf(output_sys_string,
	 	"tr '^' ',' >> %s",
	 	*output_filename );

	output_pipe = popen( output_sys_string, "w" );

	/* Output the title and sub_title */
	/* ------------------------------ */
	fprintf( output_pipe, "%s %s\n", title, sub_title );

	/* Output the heading */
	/* ------------------ */
	heading_list =
		exceedance_curve_output_get_heading_list(
				units_display,
				aggregate_level,
				0 /* not with_html */ );

	fprintf( output_pipe, "%s\n", list_display( heading_list ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
			measurement_value,
			count_below,
			percent_below,
			measurement_date,
			measurement_time,
			aggregation_count,
			input_buffer,
			aggregate_level );

		exceedance_curve_data_output(
			output_pipe,
			measurement_value,
			count_below,
			percent_below,
			measurement_date,
			measurement_time,
			aggregation_count,
			aggregate_level );

	} /* while( get_line() */

	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	document_close();

	return 1;

} /* exceedance_curve_spreadsheet_output() */

boolean exceedance_curve_text_file_output(
				char **output_filename,
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	LIST *heading_list;
	char title[ 256 ];
	char sub_title[ 1024 ];
	char value_label[ 128 ];
	DOCUMENT *document;
	APPASERVER_LINK_FILE *appaserver_link_file;
	pid_t process_id = getpid();
	char *ftp_filename;
	FILE *output_pipe;
	char output_sys_string[ 128 ];

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	*output_filename =
		/* ---------------- */
		/* Returns strdup() */
		/* ---------------- */
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
		/* ---------------- */
		/* Returns strdup() */
		/* ---------------- */
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

	if ( ! ( output_pipe = fopen( *output_filename, "w" ) ) )
		return 0;
	else
		fclose( output_pipe );

	*title = '\0';
	exceedance_curve_output_get_title(
		title,
		process_name,
		station,
		datatype );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	*sub_title = '\0';
	exceedance_curve_output_get_sub_title(
		sub_title,
		aggregate_level,
		value_label,
		begin_date,
		end_date );

	exceedance_curve_output_get_where_clause(
		where_clause,
		station,
		datatype,
		begin_date,
		end_date );

	exceedance_curve_output_get_sys_string(
		sys_string,
		application_name,
		appaserver_mount_point,
		where_clause,
		aggregate_level,
		aggregate_statistic,
		units,
		units_converted );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	printf( "<h1>%s<br></h1>\n", title );
	printf( "<h2>%s<br></h2>\n", sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
	
	hydrology_library_output_station_text_filename(
			*output_filename,
			application_name,
			station,
			1 /* with_zap_file */ );

	sprintf(output_sys_string,
	 	"tr ',' '^' >> %s",
	 	*output_filename );

	output_pipe = popen( output_sys_string, "w" );

	/* Output the title and sub_title */
	/* ------------------------------ */
	fprintf( output_pipe, "#%s %s\n", title, sub_title );

	/* Output the heading */
	/* ------------------ */
	heading_list =
		exceedance_curve_output_get_heading_list(
				units_display,
				aggregate_level,
				0 /* not with_html */ );

	fprintf( output_pipe, "#%s\n", list_display( heading_list ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				measurement_value,
				count_below,
				percent_below,
				measurement_date,
				measurement_time,
				aggregation_count,
				input_buffer,
				aggregate_level );

		exceedance_curve_data_output(
			output_pipe,
			measurement_value,
			count_below,
			percent_below,
			measurement_date,
			measurement_time,
			aggregation_count,
			aggregate_level );

	} /* while( get_line() */

	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	document_close();

	return 1;

} /* exceedance_curve_text_file_output() */

void exceedance_curve_stdout_output(
				char *application_name,
				char *process_name,
				char *appaserver_mount_point,
				char *document_root_directory,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *units_converted,
				char *units_display )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	LIST *heading_list;
	char title[ 256 ];
	char sub_title[ 1024 ];
	char value_label[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;
	pid_t process_id = getpid();
	char *output_filename;
	FILE *output_pipe;
	char output_sys_string[ 128 ];

	*title = '\0';
	exceedance_curve_output_get_title(
		title,
		process_name,
		station,
		datatype );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	*sub_title = '\0';
	exceedance_curve_output_get_sub_title(
		sub_title,
		aggregate_level,
		value_label,
		begin_date,
		end_date );

	exceedance_curve_output_get_where_clause(
		where_clause,
		station,
		datatype,
		begin_date,
		end_date );

	exceedance_curve_output_get_sys_string(
		sys_string,
		application_name,
		appaserver_mount_point,
		where_clause,
		aggregate_level,
		aggregate_statistic,
		units,
		units_converted );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	output_filename =
		/* ---------------- */
		/* Returns strdup() */
		/* ---------------- */
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

	hydrology_library_output_station_text_filename(
			output_filename,
			application_name,
			station,
			1 /* with_zap_file */ );

	sprintf(output_sys_string,
	 	"tr ',' '^' >> %s",
	 	output_filename );

	output_pipe = popen( output_sys_string, "w" );

	/* Output the title and sub_title */
	/* ------------------------------ */
	fprintf( output_pipe, "#%s %s\n", title, sub_title );

	/* Output the heading */
	/* ------------------ */
	heading_list =
		exceedance_curve_output_get_heading_list(
				units_display,
				aggregate_level,
				0 /* not with_html */ );

	fprintf( output_pipe, "#%s\n", list_display( heading_list ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				measurement_value,
				count_below,
				percent_below,
				measurement_date,
				measurement_time,
				aggregation_count,
				input_buffer,
				aggregate_level );

		exceedance_curve_data_output(
			output_pipe,
			measurement_value,
			count_below,
			percent_below,
			measurement_date,
			measurement_time,
			aggregation_count,
			aggregate_level );

	} /* while( get_line() */

	pclose( input_pipe );
	pclose( output_pipe );

	sprintf( sys_string, "cat %s", output_filename );
	if ( system( sys_string ) ) {};

} /* exceedance_curve_stdout_output() */

