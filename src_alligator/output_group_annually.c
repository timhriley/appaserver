/* ---------------------------------------------------	*/
/* src_alligator/output_group_annually.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "html_table.h"
#include "grace.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table, chart };

/* Constants */
/* --------- */
#define FILENAME_STEM		"output_group_annually"

#define ROWS_BETWEEN_HEADING		10
#define DEFAULT_OUTPUT_MEDIUM		table

/* Prototypes */
/* ---------- */
void build_sys_string(		char *sys_string,
				char *application_name,
				char *basin_list_string );

void output_group_annually_chart(
				char *application_name,
				char *document_root_directory,
				char *basin_list_string,
				char *process_name );

void output_group_annually_table(
				char *application_name,
				char *basin_list_string,
				char *process_name );

enum output_medium get_output_medium(
				char *output_medium_string );

void get_title(			char *title,
				char *sub_title,
				char *process_name,
				char *basin_list_string );

void output_group_annually_text_file(
				char *application_name,
				enum output_medium output_medium,
				char *document_root_directory,
				char *basin_list_string,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *basin_list_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *output_medium_string;
	enum output_medium output_medium;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name ignored basin output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	/* session = argv[ 3 ]; */
	basin_list_string = argv[ 4 ];
	output_medium_string = argv[ 5 ];

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

	output_medium = get_output_medium( output_medium_string );
	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( output_medium != output_medium_stdout )
	{
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
	}

	if ( output_medium == text_file
	||   output_medium == output_medium_stdout )
	{
		output_group_annually_text_file(
				application_name,
				output_medium,
				appaserver_parameter_file->
					document_root,
				basin_list_string,
				process_name );
	}
	else
	if ( output_medium == table )
	{
		output_group_annually_table(
				application_name,
				basin_list_string,
				process_name );
	}
	else
	if ( output_medium == chart )
	{
		output_group_annually_chart(
				application_name,
				appaserver_parameter_file->
					document_root,
				basin_list_string,
				process_name );
	}

	if ( output_medium != output_medium_stdout ) document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_group_annually_text_file(
				char *application_name,
				enum output_medium output_medium,
				char *document_root_directory,
				char *basin_list_string,
				char *process_name )
{
	char *ftp_filename = {0};
	char *output_filename = {0};
	FILE *output_file;
	char title[ 128 ];
	char sub_title[ 256 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_line[ 1024 ];
	int process_id = getpid();

	get_title(	title,
			sub_title,
			process_name,
			basin_list_string );

	if ( output_medium != output_medium_stdout )
	{
		APPASERVER_LINK_FILE *appaserver_link_file;

		printf( "<h1>%s</h1>\n", title );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			document_root_directory,
				FILENAME_STEM,
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

/*
		sprintf(output_filename, 
	 		OUTPUT_FILE,
	 		appaserver_mount_point,
	 		application_name,
	 		process_id );
*/

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
	}
	else
	{
		output_file = stdout;
	}

	search_replace_string( sub_title, ",", ";" );

	fprintf(output_file, "%s\n%s\n", title, sub_title );
	fprintf(output_file, "Year,Nests,Eggs,Average\n" );

	build_sys_string( sys_string, application_name, basin_list_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_line, input_pipe ) )
	{
		fprintf( output_file, "%s\n", input_line );
	}

	pclose( input_pipe );

	if ( output_medium != output_medium_stdout ) fclose( output_file );

	if ( output_medium != output_medium_stdout )
	{
		appaserver_library_output_ftp_prompt(
				ftp_filename,
"Left Click&gt; to view or &lt;Right Click&gt; to save.",
				(char *)0 /* target */,
				(char *)0 /* application_type */ );

		printf( "<br>\n" );
	}

} /* output_group_annually_text_file() */

void output_group_annually_table(
			char *application_name,
			char *basin_list_string,
			char *process_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char title[ 128 ];
	char sub_title[ 256 ];
	char piece_buffer[ 128 ];
	char input_buffer[ 128 ];
	int count = 0;
	char sys_string[ 1024 ];
	FILE *input_pipe;

	build_sys_string(
			sys_string,
			application_name,
			basin_list_string );

	input_pipe = popen( sys_string, "r" );

	get_title(	title,
			sub_title,
			process_name,
			basin_list_string );

	html_table = new_html_table( title, sub_title );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	
	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 9;

	heading_list = list_new();
	list_append_pointer( heading_list, "Year" );
	list_append_pointer( heading_list, "Nests" );
	list_append_pointer( heading_list, "Eggs" );
	list_append_pointer( heading_list, "Average" );

	html_table_set_heading_list( html_table, heading_list );
	
	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
					html_table->heading_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->justify_list );
		}

		/* Year */
		/* ---- */
		piece( piece_buffer, ',', input_buffer, 0 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Nests */
		/* ----- */
		piece( piece_buffer, ',', input_buffer, 1 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Eggs */
		/* ---- */
		piece( piece_buffer, ',', input_buffer, 2 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Average */
		/* ------- */
		piece( piece_buffer, ',', input_buffer, 3 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();
	}
	printf( "<tr><td colspan=99>\n" );
	pclose( input_pipe );
	html_table_close();

} /* output_group_annually_table() */

void get_title(	char *title,
		char *sub_title,
		char *process_name,
		char *basin_list_string )
{
	format_initial_capital( title, process_name );

	if (basin_list_string
	&&  *basin_list_string
	&&  strcmp( basin_list_string, "basin" ) != 0 )
	{
		sprintf( sub_title, "Basin: %s", basin_list_string );
		format_initial_capital( sub_title, sub_title );
	}
	else
	{
		*sub_title = '\0';
	}

} /* get_title() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium" ) == 0
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
	if ( strcmp( output_medium_string, "chart" ) == 0 )
	{
		return chart;
	}
	else
	if ( strcmp( output_medium_string, "stdout" ) == 0 )
	{
		return output_medium_stdout;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium() */

void output_group_annually_chart(
			char *application_name,
			char *document_root_directory,
			char *basin_list_string,
			char *process_name )
{
	char *ftp_agr_filename;
	char *ftp_output_filename;
	char *output_filename;
	char *agr_filename;
	char *postscript_filename;
	char title[ 128 ];
	char sub_title[ 256 ];
	GRACE *grace = {0};
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char graph_identifier[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char nest_count[ 16 ];
	char eggs_count[ 16 ];
	char average[ 16 ];
	char year[ 16 ];
	int minimum_year = 9999;
	int maximum_year = 0;
	int year_integer;

	get_title(	title,
			sub_title,
			process_name,
			basin_list_string );

	grace = grace_new_grace(	application_name,
					(char *)0 /* role_name */ );

	grace->xaxis_ticklabel_format = "decimal";
	grace->x_label_size = DEFAULT_X_LABEL_SIZE;
	grace->grace_graph_type = xy_graph;
	grace->title = title;
	grace->sub_title = sub_title;
	grace->graph_list = list_new_list();
	grace->dataset_no_cycle_color = 1;

	/* Make the Nest Count Graph */
	/* ------------------------- */
	grace_graph = grace_new_grace_graph();
	list_append_pointer( grace->graph_list, grace_graph );

	grace_graph->datatype_list = list_new_list();
	grace_datatype = grace_new_grace_datatype( 
				"" /* datatype_entity */,
				"" /* datatype_name */ );

	grace_datatype->datatype_number = 0;
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	grace_datatype->legend = "Nests";

	grace_datatype->dataset_list = list_new();
	list_append_pointer( grace_graph->datatype_list, grace_datatype );

	/* Make the Eggs Count Graph */
	/* ------------------------- */
	grace_graph = grace_new_grace_graph();
	list_append_pointer( grace->graph_list, grace_graph );

	grace_graph->datatype_list = list_new_list();
	grace_datatype = grace_new_grace_datatype( 
				"" /* datatype_entity */,
				"" /* datatype_name */ );

	grace_datatype->datatype_number = 0;
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	grace_datatype->legend = "Eggs";

	grace_datatype->dataset_list = list_new();
	list_append_pointer( grace_graph->datatype_list, grace_datatype );

	/* Make the Average Graph */
	/* ---------------------- */
	grace_graph = grace_new_grace_graph();
	list_append_pointer( grace->graph_list, grace_graph );

	grace_graph->datatype_list = list_new_list();
	grace_datatype = grace_new_grace_datatype( 
				"" /* datatype_entity */,
				"" /* datatype_name */ );

	grace_datatype->datatype_number = 0;
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	grace_datatype->legend = "Average";

	grace_datatype->dataset_list = list_new();
	list_append_pointer( grace_graph->datatype_list, grace_datatype );

	/* Set the remaining grace variables */
	/* --------------------------------- */
	grace->xaxis_ticklabel_precision = 0;
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

	build_sys_string(
			sys_string,
			application_name,
			basin_list_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( year, ',', input_buffer, 0 );

		year_integer = atoi( year );

		if ( year_integer < minimum_year )
			minimum_year = year_integer;
		if ( year_integer > maximum_year )
			maximum_year = year_integer;

		piece( nest_count, ',', input_buffer, 1 );
		piece( eggs_count, ',', input_buffer, 2 );
		piece( average, ',', input_buffer, 3 );

		/* Set the nest count */
		/* ------------------ */
		list_rewind( grace->graph_list );
		grace_graph = list_get_pointer( grace->graph_list );
		grace_set_to_point_list(
				list_get_first_pointer(
					grace_graph->datatype_list ), 
				atof( year ),
				strdup( nest_count ),
				strdup( year ),
				grace->dataset_no_cycle_color );

		/* Set the nest count */
		/* ------------------ */
		list_next( grace->graph_list );
		grace_graph = list_get_pointer( grace->graph_list );
		grace_set_to_point_list(
				list_get_first_pointer(
					grace_graph->datatype_list ), 
				atof( year ),
				strdup( eggs_count ),
				strdup( year ),
				grace->dataset_no_cycle_color );

		/* Set the average */
		/* --------------- */
		list_next( grace->graph_list );
		grace_graph = list_get_pointer( grace->graph_list );
		grace_set_to_point_list(
				list_get_first_pointer(
					grace_graph->datatype_list ), 
				atof( year ),
				strdup( average ),
				strdup( year ),
				grace->dataset_no_cycle_color );
	}

	pclose( input_pipe );

	grace->world_min_x = minimum_year - 1;
	grace->world_max_x = maximum_year + 1;

	grace_set_bar_graph( grace->graph_list );
	grace_set_scale_to_zero( grace->graph_list );
	grace_set_xaxis_ticklabel_angle( grace->graph_list, 45 );
	grace_set_yaxis_ticklabel_precision( grace->graph_list, 0 );

	grace_set_structures(
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
				0 /* not force_landscape_mode */ );

	grace_set_x_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_y_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_xaxis_tickmarks_off( grace->graph_list );
	grace_set_yaxis_tickmajor_integer_ceiling( grace->graph_list );

	grace_output_charts(
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
				grace_get_optional_label_list(
					grace->graph_list ),
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list );

	grace_output_graph_window(
			application_name,
			ftp_output_filename,
			ftp_agr_filename,
			(char *)0 /* appaserver_mount_point */,
			0 /* not with_document_output */,
			(char *)0 /* where_clause */ );
} /* output_group_annually_chart() */

void build_sys_string(	char *sys_string,
			char *application_name,
			char *basin_list_string )
{
	sprintf(sys_string,
		"group_annually.sh %s '%s'",
		application_name,
		basin_list_string );
} /* build_sys_string() */


