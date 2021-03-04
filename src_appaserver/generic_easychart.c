/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_easychart.c	*/
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
#include "environ.h"
#include "aggregate_level.h"
#include "validation_level.h"
#include "aggregate_statistic.h"
#include "process_generic_output.h"
#include "document.h"
#include "easycharts.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define DELIMITER				'^'
#define DATE_TIME_DELIMITER			':'

/* Prototypes */
/* ---------- */
char *get_title(	char *value_folder_name,
			LIST *foreign_attribute_data_list,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string,
			char *end_date_string,
			char *accumulate_label );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	char *where_clause = {0};
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	enum aggregate_statistic aggregate_statistic;
	EASYCHARTS *easycharts;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	char *title;
	char y_axis_label[ 256 ];
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	char *units_label;
	char *process_set_name;
	DICTIONARY *original_post_dictionary;
	char *process_name;
	int easycharts_width;
	int easycharts_height;
	int date_piece = -1;
	int time_piece = -1;
	int value_piece = -1;
	char *accumulate_yn;
	boolean accumulate_flag;
	char accumulate_label[ 32 ];
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char heading_buffer[ 1024 ];
	char search_string[ 16 ];
	char replace_string[ 16 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s ignored process_set parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_set_name = argv[ 2 ];
	original_post_dictionary =
		dictionary_string2dictionary( argv[ 3 ] );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	accumulate_yn =
		dictionary_get_index_zero(
			dictionary_appaserver->non_prefixed_dictionary,
			"accumulate_yn" );

	accumulate_flag = (accumulate_yn && *accumulate_yn == 'y' );

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			accumulate_flag );

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
			dictionary_appaserver->non_prefixed_dictionary,
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
			dictionary_appaserver->non_prefixed_dictionary );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				dictionary_appaserver->non_prefixed_dictionary,
				"aggregate_level" ) );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				dictionary_appaserver->non_prefixed_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date_string /* in/out */,
			&end_date_string /* in/out */,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name,
			dictionary_appaserver->
				non_prefixed_dictionary
				/* query_removed_post_dictionary */ ) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date_string,
			&end_date_string,
			&where_clause,
			&units_label,
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			&date_piece,
			&time_piece,
			&value_piece,
			(int *)0 /* length_select_list */,
			application_name,
			process_generic_output,
			dictionary_appaserver->non_prefixed_dictionary,
			DELIMITER,
			aggregate_level,
			aggregate_statistic,
			0 /* append_low_high */,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */,
			process_generic_output->accumulate );

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
		application_name,
		EASYCHARTS_JAR_FILE );

	easycharts->applet_library_archive = applet_library_archive;

	if ( accumulate_flag )
	{
		value_piece++;
		strcpy( accumulate_label, "(Accumulated)" );
	}
	else
	{
		*accumulate_label = '\0';
	}

	title = get_title(	process_generic_output->
					value_folder->value_folder_name,
				process_generic_output->
					value_folder->
					datatype->
					foreign_attribute_data_list,
				aggregate_level,
				aggregate_statistic,
				begin_date_string,
				end_date_string,
				accumulate_label );

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
	input_chart->bar_chart =
			process_generic_output->
				value_folder->
			 		datatype->bar_graph;

	sprintf(y_axis_label,
		"%s %s",
		process_generic_get_datatype_name(
			process_generic_output->
				value_folder->
				datatype->
				primary_attribute_data_list,
			' ' /* delimiter */ ),
		units_label );

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
	input_datatype =
		easycharts_new_input_datatype(
			process_generic_get_datatype_name(
				process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list,
				' ' /* delimiter */ ),
			units_label );

	list_append_pointer( input_chart->datatype_list, input_datatype );

	strcpy( heading_buffer, 
	 	process_generic_output_get_heading_string(
			process_generic_output->value_folder,
			DELIMITER,
			aggregate_level ) );

	sprintf( search_string,
		 "%c",
		 DELIMITER );

	sprintf( replace_string,
		 "\\%c",
		 DELIMITER );

	search_replace_string(
		heading_buffer,
		search_string,
		replace_string );
			
	sprintf( sys_string + strlen( sys_string ),
	 	 " | grep -v '%s'",
		 heading_buffer );

	if ( time_piece != -1 )
	{
		sprintf( sys_string + strlen( sys_string ),
			 " | sed 's|\\%c|%c|%d'",
			 DELIMITER,
			 DATE_TIME_DELIMITER,
			 time_piece );
		value_piece--;
	}

	if ( !easycharts_set_all_input_values(
		easycharts->input_chart_list,
		sys_string,
		-1 /* datatype_piece */,
		date_piece,
		value_piece,
		DELIMITER ) )
	{
		printf(
		"<h3>Warning: nothing was selected to display.</h3>\n" );
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
				process_name,
				prompt_filename,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					process_generic_output->
						value_folder->
							value_folder_name,
					1 ) );

	document_close();
	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

char *get_title(		char *value_folder_name,
				LIST *foreign_attribute_data_list,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *begin_date_string,
				char *end_date_string,
				char *accumulate_label )
{
	static char title[ 512 ];
	char buffer[ 512 ];

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf(title,
		 	"Chart %s %s %s",
			aggregate_level_get_string( aggregate_level ),
			aggregate_statistic_get_string( aggregate_statistic ),
			value_folder_name );
	}
	else
	{
		sprintf(title,
		 	"Chart %s",
			value_folder_name );
	}


	sprintf(title + strlen( title ),
		"\\n%s%s From: %s To: %s",
		format_initial_capital( buffer,
			list_display_delimited(
				foreign_attribute_data_list,
				'/' ) ),
		accumulate_label,
		begin_date_string,
		end_date_string );

	format_initial_capital( title, title );

	return title;

} /* get_title() */

