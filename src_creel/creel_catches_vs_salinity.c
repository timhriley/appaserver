/* --------------------------------------------------- 	*/
/* src_creel/creel_catches_per_salinity.c	      	*/
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
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "html_table.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "application.h"
#include "environ.h"
#include "process.h"
#include "easycharts.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium {	output_medium_stdout,
			text_file,
			table,
			correlation,
			scatter };

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM		table
#define REPORT_TITLE			"Catches Vs. Salinity Report"
#define ROWS_BETWEEN_HEADING	20


/* Prototypes */
/* ---------- */
boolean output_scatter_populate_input_chart_list_data(
				LIST *input_chart_list,
				char *sys_string );

void output_scatter_populate_input_chart_list_datatypes(
				LIST *input_chart_list,
				char *horizontal_datatype_name,
				char *horizontal_units,
				char *vertical_datatype_name );

char *get_sys_string(		char *creel_application_name,
				char *everglades_application_name,
				char *fishing_area_string,
				char *begin_date_string,
				char *end_date_string );

void piece_input_buffer(
				char *date_string,
				char *label_string,
				char *value_string,
				char *count_string,
				char *input_buffer );

void output_table(		char *fishing_area,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string );

void output_scatter(		char *everglades_application_name,
				char *creel_application_name,
				char *fishing_area,
				char *begin_date_string,
				char *end_date_string,
				char *appaserver_mount_point,
				char *process_name,
				char *sys_string );

void output_correlation(	char *fishing_area,
				char *begin_date_string,
				char *end_date_string,
				char *input_sys_string );

void output_text_file(		char *creel_application_name,
				char *fishing_area,
				char *begin_date_string,
				char *end_date_string,
				enum output_medium,
				char *document_root_directory,
				char *sys_string,
				char *process_name );

enum output_medium get_output_medium(
				char *output_medium_string );


int main( int argc, char **argv )
{
	char *creel_application_name;
	char *everglades_application_name;
	char *process_name;
	char *fishing_area;
	char *begin_date_string;
	char *end_date_string;
	char *database_string;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum output_medium output_medium;
	char *output_medium_string;
	char *sys_string;

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s creel_application everglades_application process_name fishing_area begin_date end_date output_medium\n",
			argv[ 0 ] );
		exit( 1 );
	}

	creel_application_name = argv[ 1 ];
	everglades_application_name = argv[ 2 ];
	process_name = argv[ 3 ];
	fishing_area = argv[ 4 ];
	begin_date_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	output_medium_string = argv[ 7 ];

	output_medium = get_output_medium( output_medium_string );

	if ( timlib_parse_database_string(	&database_string,
						creel_application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				creel_application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( creel_application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( output_medium != output_medium_stdout )
	{
		document_quick_output_body(
				creel_application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
	}

	if ( !*fishing_area ) )
	{
		printf( "<p>ERROR: Please select a location area/zone.</p>\n" );
		document_close();
		exit( 0 );
	}

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<p>ERROR: invalid begin and end dates.\n" );

		if ( output_medium != output_medium_stdout )
			document_close();

		exit( 0 );
	}

	sys_string = get_sys_string(
			creel_application_name,
			everglades_application_name,
			fishing_area,
			begin_date_string,
			end_date_string );

	if ( output_medium == table )
	{
		output_table(	fishing_area,
				begin_date_string,
				end_date_string,
				sys_string );
	}
	else
	if ( output_medium == scatter )
	{
		output_scatter(	everglades_application_name,
				creel_application_name,
				fishing_area,
				begin_date_string,
				end_date_string,
				appaserver_parameter_file->
					appaserver_mount_point,
				process_name,
				sys_string );
	}
	else
	if ( output_medium == correlation )
	{
		output_correlation(
				fishing_area,
				begin_date_string,
				end_date_string,
				sys_string );
	}
	else
	if ( output_medium == text_file
	||   output_medium == output_medium_stdout )
	{
		output_text_file(
				creel_application_name,
				fishing_area,
				begin_date_string,
				end_date_string,
				output_medium,
				appaserver_parameter_file->
					document_root,
				sys_string,
				process_name );
	}

	if ( output_medium != output_medium_stdout ) document_close();

	process_increment_execution_count(
				creel_application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

void piece_input_buffer(
			char *date_string,
			char *label_string,
			char *value_string,
			char *count_string,
			char *input_buffer )
{
	/* -------------------------------------------- */
	/* Sample input: 1999-01-01,minutes_between,54	*/
	/* Sample input: 1999-01-01,salinity,5.00	*/
	/* -------------------------------------------- */
	piece(	date_string,
		',',
		input_buffer,
		0 );

	piece(	label_string,
		',',
		input_buffer,
		1 );

	piece(	value_string,
		',',
		input_buffer,
		2 );

	piece(	count_string,
		',',
		input_buffer,
		3 );

} /* piece_input_buffer() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium_string" ) == 0
	||   !*output_medium_string )
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
	else
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "transmit" ) == 0 )
	{
		return text_file;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		return table;
	}
	else
	if ( strcmp( output_medium_string, "stdout" ) == 0 )
	{
		return output_medium_stdout;
	}
	else
	if ( strcmp( output_medium_string, "scatter" ) == 0 )
	{
		return scatter;
	}
	else
	if ( strcmp( output_medium_string, "correlation" ) == 0 )
	{
		return correlation;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium_string() */

void output_scatter(	char *everglades_application_name,
			char *creel_application_name,
			char *fishing_area,
			char *begin_date_string,
			char *end_date_string,
			char *appaserver_mount_point,
			char *process_name,
			char *sys_string )
{
	EASYCHARTS *easycharts;
	char applet_library_archive[ 128 ];
	FILE *chart_file;
	char horizontal_datatype_name[ 128 ];
	char vertical_datatype_name[ 128 ];
	boolean bar_graph;
	int easycharts_width, easycharts_height;
	char complete_title[ 128 ];
	char title[ 128 ];
	char sub_title[ 128 ];

	sprintf(title,
		"%s For Location Area/Zone %s",
		REPORT_TITLE,
		fishing_area );

	sprintf(sub_title,
		"Begin: %s End: %s",
	 	begin_date_string,
	 	end_date_string );

	sprintf(complete_title,
		"%s\\n%s",
		title,
		sub_title );

	format_initial_capital( complete_title, complete_title );

	strcpy( horizontal_datatype_name, "salinity" );
	strcpy( vertical_datatype_name, "minutes_between" );

	application_constants_get_easycharts_width_height(
			&easycharts_width,
			&easycharts_height,
			creel_application_name,
			appaserver_parameter_file_get_dbms() );

	easycharts =
		easycharts_new_scatter_easycharts(
			easycharts_width, easycharts_height );

	easycharts_get_chart_filename(
			&easycharts->chart_filename,
			&easycharts->prompt_filename,
			creel_application_name,
			appaserver_mount_point,
			getpid(),
			( application_prepend_http_protocol_yn(
				creel_application_name ) == 'y' ) );

	chart_file = fopen( easycharts->chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			easycharts->chart_filename );
		exit( 1 );
	}

	sprintf(applet_library_archive,
		"/appaserver/%s/%s",
		creel_application_name,
		EASYCHARTS_JAR_FILE );

	easycharts->applet_library_archive = applet_library_archive;

	easycharts->title = complete_title;
	easycharts->legend_on = 0;
	easycharts->bold_labels = 0;
	easycharts->bold_legends = 0;
	easycharts->set_y_lower_range = 1;
	easycharts->sample_scroller_on = 1;
	easycharts->range_scroller_on = 1;

	application_reset();

	output_scatter_populate_input_chart_list_datatypes(
			easycharts->input_chart_list,
			horizontal_datatype_name,
			hydrology_library_get_units_string(
				&bar_graph,
				everglades_application_name,
				horizontal_datatype_name ),
			vertical_datatype_name );

	if ( !output_scatter_populate_input_chart_list_data(
			easycharts->input_chart_list,
			sys_string ) )
	{
		printf( "<p>There are no charts to display\n" );
		document_close();
		exit( 0 );
	}

	easycharts->output_chart_list =
		easycharts_scatter_get_output_chart_list(
			easycharts->input_chart_list );

	easycharts->range_step =
		easycharts_get_range_step( easycharts->output_chart_list );

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
				creel_application_name,
				appaserver_mount_point,
				0 /* not with_document_output */,
				process_name,
				easycharts->prompt_filename,
				(char *)0 /* where_clause */ );

} /* output_scatter() */

boolean output_scatter_populate_input_chart_list_data(
			LIST *input_chart_list,
			char *sys_string )
{
	/* ---------------------------------------------------- */
	/* Sample input: "1999-01-01,salinity,5.00"		*/
	/* Sample input: "1999-01-01,minutes_between,4.00"	*/
	/* ---------------------------------------------------- */
	return easycharts_set_all_input_values(
				input_chart_list,
				sys_string,
				1 /* datatype_piece */,
				0 /* date_time_piece */,
				2 /* value_piece */,
				',' /* delimiter */ );

} /* output_scatter_populate_input_chart_list_data() */

void output_scatter_populate_input_chart_list_datatypes(
			LIST *input_chart_list,
			char *horizontal_datatype_name,
			char *horizontal_units,
			char *vertical_datatype_name )
{
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	char x_axis_label[ 128 ];
	char y_axis_label[ 128 ];

	/* Create the chart */
	/* ---------------- */
	input_chart = easycharts_new_input_chart();
	list_append_pointer( input_chart_list, input_chart );

	/* Create the vertical datatype */
	/* ---------------------------- */
	input_datatype =
		easycharts_new_input_datatype(
			vertical_datatype_name,
			(char *)0 /* units */ );

	list_append_pointer(	input_chart->datatype_list,
				input_datatype );

	strcpy( y_axis_label, vertical_datatype_name );

	/* Create the horizontal datatype */
	/* ------------------------------ */
	input_datatype =
		easycharts_new_input_datatype(
			horizontal_datatype_name,
			horizontal_units );

	list_append_pointer(	input_chart->datatype_list,
				input_datatype );

	if ( horizontal_units && *horizontal_units )
	{
		sprintf(	x_axis_label,
				"%s (%s)",
				horizontal_datatype_name,
				horizontal_units );
	}
	else
	{
		strcpy( x_axis_label, horizontal_datatype_name );
	}

	input_chart->y_axis_label = strdup( y_axis_label );
	input_chart->x_axis_label = strdup( x_axis_label );

	input_chart->applet_library_code =
		EASYCHARTS_APPLET_LIBRARY_LINE_CHART;

} /* output_scatter_populate_input_chart_list_datatypes() */

void output_correlation(char *fishing_area,
			char *begin_date_string,
			char *end_date_string,
			char *input_sys_string )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	LIST *input_list;
	char *input_record;
	char sys_string[ 1024 ];
	char label_buffer[ 1024 ];
	char covariance_buffer[ 128 ];
	char correlation_buffer[ 128 ];
	char determination_buffer[ 128 ];
	char count_buffer[ 128 ];
	char average_buffer[ 128 ];
	char standard_deviation_buffer[ 128 ];
	HTML_TABLE *html_table = {0};
	LIST *heading_list;

	/* ---------------------------------------------------- */
	/* Sample input: "1999-01-01,salinity,5.00"		*/
	/* Sample input: "1999-01-01,minutes_between,4.00"	*/
	/* ---------------------------------------------------- */
	sprintf( sys_string,
		 "%s							|"
		 "covariance.e 'Salinity' 0 1 2 ','			|"
		 "cat							 ",
		 input_sys_string );

	input_list = pipe2list( sys_string );

	if ( !list_rewind( input_list ) )
	{
		printf( "<h2>ERROR: input error for sys_string = (%s)\n",
			sys_string );
		html_table_close();
		return;
	}

	sprintf(sub_title,
		"Begin: %s End: %s",
	 	begin_date_string,
	 	end_date_string );

	sprintf( title,
		 "%s Correlation for Location Area/Zone %s<br>%s",
		 REPORT_TITLE,
		 fishing_area,
		 sub_title );

 	html_table = new_html_table(
			format_initial_capital(
				title, title ),
			(char *)0 /* sub_title */ );

	heading_list = new_list();

	list_append_pointer( heading_list, "Datatype" );
	list_append_pointer( heading_list, "Correlation (r)" );
	list_append_pointer( heading_list, "Determination (r^2)" );
	list_append_pointer( heading_list, "Covariance" );
	list_append_pointer( heading_list, "Average" );
	list_append_pointer( heading_list, "Count" );
	list_append_pointer( heading_list, "Standard Deviation" );

	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 6;

	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	do {
		input_record = list_get_pointer( input_list );

		piece( label_buffer, ':', input_record, 0 );
		piece( covariance_buffer, ':', input_record, 1 );
		piece( count_buffer, ':', input_record, 2 );
		piece( average_buffer, ':', input_record, 3 );
		piece( standard_deviation_buffer, ':', input_record, 4 );
		piece( correlation_buffer, ':', input_record, 5 );

		html_table_set_data(	html_table->data_list,
					strdup( format_initial_capital(
							label_buffer,
							label_buffer ) ) );

		html_table_set_data(	html_table->data_list,
					strdup( correlation_buffer ) );

		sprintf(	determination_buffer,
				"%lf",
				atof( correlation_buffer ) *
				atof( correlation_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( determination_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( covariance_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( average_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( count_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( standard_deviation_buffer ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();
	} while( list_next( input_list ) );
	html_table_close();

} /* output_correlation() */

void output_text_file(	char *creel_application_name,
			char *fishing_area,
			char *begin_date_string,
			char *end_date_string,
			enum output_medium output_medium,
			char *document_root_directory,
			char *sys_string,
			char *process_name )
{
	char *text_output_filename;
	char *text_ftp_filename;
	int right_justified_columns_from_right = 4;
	char date_string[ 16 ];
	char label_string[ 16 ];
	char value_string[ 16 ];
	char count_string[ 16 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	FILE *text_output_pipe;
	char title[ 128 ];
	char sub_title[ 128 ];
	boolean got_salinity;
	char salinity_count[ 16 ];
	char old_date_string[ 16 ];
	char ftp_output_sys_string[ 256 ];
	int process_id = getpid();

	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	appaserver_link_file->begin_date_string = begin_date_string;
	appaserver_link_file->end_date_string = end_date_string;

	text_output_filename =
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

	text_ftp_filename =
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

	application_reset();

	if ( output_medium == text_file )
	{
/*
		sprintf(ftp_output_sys_string,
			"tr ',' '\\t' | delimiter2padded_columns.e tab %d > %s",
	 		right_justified_columns_from_right,
			text_output_filename );
*/
		sprintf(ftp_output_sys_string,
		 	"tr ',' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	text_output_filename );
	}
	else
	{
/*
		sprintf(ftp_output_sys_string,
			"tr ',' '\\t' | delimiter2padded_columns.e tab %d",
	 		right_justified_columns_from_right );
*/
		sprintf(ftp_output_sys_string,
		 	"tr ',' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );
	}

	text_output_pipe = popen( ftp_output_sys_string, "w" );

	sprintf(title,
		"%s For Location Area/Zone %s",
		REPORT_TITLE,
		fishing_area );

	sprintf(sub_title,
		"Begin: %s End: %s",
	 	begin_date_string,
	 	end_date_string );

	fprintf(text_output_pipe,
"#%s %s\n#date,salinity,minuts_between,salinity_count,catch_records\n",
		title,
		sub_title );
	
	input_pipe = popen( sys_string, "r" );
	got_salinity = 0;

	/* ---------------------------------------------------- */
	/* Sample input: "1999-01-01,salinity,5.00"		*/
	/* Sample input: "1999-01-01,minutes_between,4.00"	*/
	/* ---------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				date_string,
				label_string,
				value_string,
				count_string,
				input_buffer );

		if ( strcmp( label_string, "salinity" ) == 0 )
		{
			fprintf(text_output_pipe,
				"%s,%s",
				date_string,
				value_string );

			strcpy( salinity_count, count_string );
			strcpy( old_date_string, date_string );
			got_salinity = 1;
		}
		else
		/* ----------------------- */
		/* Must be minutes_between */
		/* ----------------------- */
		{
			if ( got_salinity
			&&   strcmp( old_date_string, date_string ) == 0 )
			{
				fprintf(text_output_pipe,
					",%s,%s,%s\n",
					value_string,
					salinity_count,
					count_string );
			}
			else
			{
				fprintf(text_output_pipe, "\n" );
			}
			got_salinity = 0;

		} /* If minutes_between */

	} /* while( get_line() ) */

	pclose( text_output_pipe );

	if ( output_medium == text_file )
	{
		printf( "<h1>%s Transmission<br></h1>\n", REPORT_TITLE );
	
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
			
		appaserver_library_output_ftp_prompt(
				text_ftp_filename, 
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

} /* output_text_file() */

void output_table(	char *fishing_area,
			char *begin_date_string,
			char *end_date_string,
			char *sys_string )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	char date_string[ 16 ];
	char label_string[ 16 ];
	char value_string[ 16 ];
	char count_string[ 16 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char title[ 128 ];
	char sub_title[ 128 ];
	boolean got_salinity;
	char salinity_count[ 16 ];
	char old_date_string[ 16 ];
	int count = 0;

	sprintf(title,
		"%s For Location Area/Zone %s",
		REPORT_TITLE,
		fishing_area );

	sprintf(sub_title,
		"Begin: %s End: %s",
	 	begin_date_string,
	 	end_date_string );

	html_table = new_html_table( 
			title,
			sub_title );
	
	heading_list = new_list();

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 4;

	list_append_pointer( heading_list, "Date" );
	list_append_pointer( heading_list, "Salinity" );
	list_append_pointer( heading_list, "Minutes Between" );
	list_append_pointer( heading_list, "Salinity Count" );
	list_append_pointer( heading_list, "Catch Records" );
	
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
	got_salinity = 0;

	/* ---------------------------------------------------- */
	/* Sample input: "1999-01-01,salinity,5.00"		*/
	/* Sample input: "1999-01-01,minutes_between,4.00"	*/
	/* ---------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !(++count % ( ROWS_BETWEEN_HEADING * 2 ) ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		piece_input_buffer(
				date_string,
				label_string,
				value_string,
				count_string,
				input_buffer );

		if ( strcmp( label_string, "salinity" ) == 0 )
		{
			html_table_set_data(	html_table->data_list,
						strdup( date_string ) );

			html_table_set_data(	html_table->data_list,
						strdup( value_string ) );

			strcpy( salinity_count, count_string );
			strcpy( old_date_string, date_string );
			got_salinity = 1;
		}
		else
		/* ----------------------- */
		/* Must be minutes_between */
		/* ----------------------- */
		{
			if ( got_salinity
			&&   strcmp( old_date_string, date_string ) == 0 )
			{
				html_table_set_data(
						html_table->data_list,
						strdup( value_string ) );
	
				html_table_set_data(
						html_table->data_list,
						strdup( salinity_count ) );
	
				html_table_set_data(
						html_table->data_list,
						strdup( count_string ) );
	
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
			else
			{
				if ( list_length( html_table->data_list ) )
				{
					list_free_string_list(
						html_table->data_list );
				}
				html_table->data_list = list_new();
			}
			got_salinity = 0;

		} /* If minutes_between */

	} /* while( get_line() ) */

	pclose( input_pipe) ;
	html_table_close();

} /* output_table() */

char *get_sys_string(	char *creel_application_name,
			char *everglades_application_name,
			char *fishing_area_string,
			char *begin_date_string,
			char *end_date_string )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "catches_salinity %s %s %s %s %s",
		 creel_application_name,
		 everglades_application_name,
		 fishing_area_string,
		 begin_date_string,
		 end_date_string );

	return strdup( sys_string );

} /* get_sys_string() */

