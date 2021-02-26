/* ---------------------------------------------------	*/
/* src_hydrology/output_measurement_easycharts.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "validation_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "document.h"
#include "datatype.h"
#include "easycharts.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application_constants.h"

/* Constants */
/* --------- */
#define PROCESS_NAME				"easychart_measurements"
#define SOURCE_FOLDER				"measurement"
#define DATE_PIECE				0
#define SELECT_LIST				"measurement_date,measurement_time,measurement_value,station"

/* Prototypes */
/* ---------- */
char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date, *end_date;
	char *aggregate_level_string;
	char *validation_level_string;
	char *aggregate_statistic_string;
	char *station_name;
	char *datatype_name;
	char where_clause[ 4096 ];
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	enum validation_level validation_level;
	enum aggregate_statistic aggregate_statistic;
	EASYCHARTS *easycharts;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	char title[ 256 ];
	char sub_title[ 256 ];
	char y_axis_label[ 256 ];
	char *units_converted;
	char *units;
	char *accumulate_yn;
	char *database_string = {0};
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	char *units_display;
	int easycharts_width;
	int easycharts_height;
	boolean bypass_data_collection_frequency;

	if ( argc != 13 )
	{
		fprintf( stderr, 
	"Usage: %s ignored ignored application station datatype begin_date end_date aggregate_level validation_level aggregate_statistic units_converted accumulate_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* person = argv[ 1 ]; */
	/* session = argv[ 2 ]; */
	application_name = argv[ 3 ];
	station_name = argv[ 4 ];
	datatype_name = argv[ 5 ];
	begin_date = argv[ 6 ];
	end_date = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];
	validation_level_string = argv[ 9 ];
	aggregate_statistic_string = argv[ 10 ];
	units_converted = argv[ 11 ];
	accumulate_yn = argv[ 12 ];

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

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_name,
					datatype_name );

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

	if ( !*station_name || strcmp( station_name, "station" ) == 0
	||   !*datatype_name || strcmp( datatype_name, "datatype" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<p>ERROR: Please select a station/datatype\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	validation_level =
		validation_level_get_validation_level( validation_level_string);

	bypass_data_collection_frequency =
		datatype_get_bypass_data_collection_frequency(
				application_name,
				station_name,
				datatype_name );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string,
				aggregate_level );

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date between '%s' and '%s'	",
			station_name,
			datatype_name,
			begin_date,
			end_date );
	}
	else
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date = '%s'		",
			station_name,
			datatype_name,
			begin_date );
	}

	strcat( where_clause,
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level ) );

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

	easycharts_get_chart_filename(
			&chart_filename,
			&prompt_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			getpid() );

	chart_file = fopen( chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			chart_filename );
		exit( 1 );
	}

	application_constants_get_easycharts_width_height(
			&easycharts_width,
			&easycharts_height,
			application_name );

	easycharts =
		easycharts_new_timeline_easycharts(
			easycharts_width, easycharts_height );

	easycharts->point_highlight_size = 0;

	sprintf(applet_library_archive,
		"/appaserver/%s/%s",
		application_relative_source_directory(
			application_name ),
		EASYCHARTS_JAR_FILE );

	easycharts->applet_library_archive = applet_library_archive;

	hydrology_library_get_title(
				title,
				sub_title,
				validation_level,
				aggregate_statistic,
				aggregate_level,
				station_name,
				(char *)0 /* datatype */,
				begin_date,
				end_date,
				*accumulate_yn );

	sprintf( title + strlen( title ), "\\n%s", sub_title );

	easycharts->title = title;
	easycharts->legend_on = 0;
	easycharts->bold_labels = 0;
	easycharts->bold_legends = 0;
	easycharts->set_y_lower_range = 1;
	easycharts->sample_scroller_on = 1;
	easycharts->range_scroller_on = 1;

	/* Build the single chart */
	/* ---------------------- */
	input_chart = easycharts_new_input_chart();
	easycharts->input_chart_list = list_new_list();
	list_append_pointer( easycharts->input_chart_list, input_chart );

	units = hydrology_library_get_units_string(
				&input_chart->bar_chart,
				application_name,
				datatype_name );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype_name,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	sys_string = get_sys_string(	
			application_name,
			where_clause,
			aggregate_level,
			aggregate_statistic,
			station_name,
			datatype_name,
			DATE_PIECE,
			begin_date,
			end_date,
			units,
			units_converted,
			end_date,
			*accumulate_yn,
			bypass_data_collection_frequency );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		units = units_converted;
	}

	sprintf(y_axis_label,
		"%s (%s)",
		datatype_name,
		units_display );

	input_chart->y_axis_label = strdup( y_axis_label );

	if ( input_chart->bar_chart )
	{
		input_chart->applet_library_code =
			EASYCHARTS_APPLET_LIBRARY_BAR_CHART;
		input_chart->bar_labels_on = 1;
	}
	else
	{
		input_chart->applet_library_code =
			EASYCHARTS_APPLET_LIBRARY_LINE_CHART;
	}

	/* Build the single datatype */
	/* ------------------------- */
	input_datatype = easycharts_new_input_datatype( datatype_name, units );
	list_append_pointer( input_chart->datatype_list, input_datatype );

	if ( !easycharts_set_all_input_values(
			easycharts->input_chart_list,
			sys_string,
			-1 /* datatype_piece */,
			0  /* date_time_piece */,
			1  /* value_piece */,
			'^'/* delimiter */ ) )
	{
		printf(
		"<h2>Warning: nothing was selected to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	easycharts->output_chart_list =
		easycharts_timeline_get_output_chart_list(
			easycharts->input_chart_list );

	easycharts->yaxis_decimal_count =
		easycharts_get_yaxis_decimal_count(
			easycharts->output_chart_list );

	easycharts_output_all_charts(
			chart_file,
			easycharts->output_chart_list,
			easycharts->highlight_on,
			easycharts->highlight_style,
			easycharts->point_highlight_size,
			easycharts->series_labels,
			easycharts->series_line_off,
			easycharts->applet_library_archive,
			easycharts->width,
			easycharts->height,
			easycharts->title,
			easycharts->set_y_lower_range,
			easycharts->legend_on,
			easycharts->value_labels_on,
			easycharts->sample_scroller_on,
			easycharts->range_scroller_on,
			easycharts->xaxis_decimal_count,
			easycharts->yaxis_decimal_count,
			easycharts->range_labels_off,
			easycharts->value_lines_off,
			easycharts->range_step,
			easycharts->sample_label_angle,
			easycharts->bold_labels,
			easycharts->bold_legends,
			easycharts->font_size,
			easycharts->label_parameter_name,
			1 /* include_sample_series_output */ );

	easycharts_output_html( chart_file );

	fclose( chart_file );

	easycharts_output_graph_window(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				0 /* not with_document_output */,
				PROCESS_NAME,
				prompt_filename,
				(char *)0 /* where_clause */ );

	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency )
{
	char sys_string[ 4096 ];
	char real_time_aggregation_process[ 1024 ];
	char accumulate_process[ 1024 ];
	char trim_time_process[ 1024 ];
	char *select_list_string;
	char units_converted_process[ 128 ];

	select_list_string = SELECT_LIST;

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic );
	}

	if ( aggregate_level == real_time )
	{
		strcpy( real_time_aggregation_process, "cat" );
	}
	else
	{
		sprintf( real_time_aggregation_process, 
		"real_time2aggregate_value.e %s %d %d %d '^' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 date_piece + 1,
			 date_piece + 2,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );

		if ( aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			sprintf( trim_time_process,
			 	"piece_inverse.e %d ','",
			 	date_piece + 1 );
		}
		else
		{
			strcpy( trim_time_process, "cat" );
		}
	}

	if ( accumulate_yn != 'y' )
	{
		strcpy( accumulate_process, "cat" );
	}
	else
	{
		sprintf( accumulate_process, 
			 "accumulate.e %d '^' replace",
			 date_piece + 2 );
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s 2 '^' 3",
			 application_name,
			 units,
			 units_converted );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	if ( !bypass_data_collection_frequency )
	{
		sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "pad_missing_times.e '^' 0,1,2 %s %s 0000 %s 2359 0 '%s'|"
		 "sed 's/\\^/:/1'					 |"
		 "sed 's/:null//1'					 |"
		 "cat							  ",
		 	application_name,
		 	SOURCE_FOLDER,
		 	select_list_string,
		 	where_clause,
		 	real_time_aggregation_process,
		 	accumulate_process,
		 	units_converted_process,
		 	aggregate_level_get_string( aggregate_level ),
		 	begin_date_string,
		 	end_date_string,
		 	hydrology_library_get_expected_count_list_string(
				application_name,
				station_name,
				datatype_name,
				'|' ) );
	}
	else
	{
		sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "sed 's/\\^/:/1'					 |"
		 "sed 's/:null//1'					 |"
		 "cat							  ",
		 	application_name,
		 	SOURCE_FOLDER,
		 	select_list_string,
		 	where_clause,
		 	real_time_aggregation_process,
		 	accumulate_process,
		 	units_converted_process );
	}

	return strdup( sys_string );

} /* get_sys_string() */

