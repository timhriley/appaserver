/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_hydroperiod.c	*/
/* --------------------------------------------------- 	*/
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
#include "measurement_update_parameter.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define REPORT_TITLE				"Hydroperiod"
#define DEFAULT_OUTPUT_MEDIUM			"chart"

#define FILENAME_STEM				"hydroperiod"

#define ROWS_BETWEEN_HEADING			20

#define GRACE_TICKLABEL_ANGLE			90
#define DATE_PIECE		 		0
#define TIME_PIECE		 		1
#define VALUE_PIECE		 		2
#define COUNT_PIECE		 		3
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void get_report_sub_title(
					char *sub_title,
					char *begin_date_string,
					char *end_date_string );

void output_hydroperiod_output_transmit(
					FILE *output_pipe,
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *station_name,
					char *datatype_name,
					double elevation,
					char *output_medium );

void output_hydroperiod_output_table(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *station_name,
					char *datatype_name,
					double elevation,
					char *output_medium );

void get_report_title(		char *title,
				char *station_name,
				char *datatype_name,
				double elevation );

boolean output_hydroperiod_output_gracechart(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				char *argv_0,
				char *station_name,
				char *datatype_name,
				double elevation_offset,
				char *output_medium );

void build_sys_string(
				char *sys_string,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *station_name,
				char *datatype_name,
				double elevation,
				char *output_medium );

double get_ground_surface_elevation(
				boolean *missing_ground_surface_elevation,
				char *application_name,
				char *station_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date_string, *end_date_string;
	char *output_medium;
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *process_name;
	char *station_name;
	char *datatype_name;
	double elevation;
	boolean missing_ground_surface_elevation = 0;
	double elevation_offset;

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application process station datatype begin_date end_date elevation_offset output_medium\n",
			argv[ 0 ] );
		fprintf(stderr,
"Note: output_medium = {chart,table,transmit,stdout}\n" );
		exit( 1 );
	}


	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	datatype_name = argv[ 4 ];
	begin_date_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	elevation_offset = atof( argv[ 7 ] );
	output_medium = argv[ 8 ];

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

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	hydrology_library_get_clean_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					station_name,
					datatype_name );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	elevation = get_ground_surface_elevation(
				&missing_ground_surface_elevation,
				application_name,
				station_name );

	if ( missing_ground_surface_elevation )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		printf(
	"<p>ERROR: cannot get ground surface elevation for station %s\n",
			station_name );
		document_close();
		exit( 0 );
	}

	elevation += elevation_offset;

	if ( strcmp( output_medium, "stdout" ) != 0 )
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

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_hydroperiod_output_table(
					application_name,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					elevation,
					output_medium );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		char sub_title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		get_report_title(	title,
					station_name,
					datatype_name,
					elevation );

		get_report_sub_title(	sub_title,
					begin_date_string,
					end_date_string );

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" );

		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;

		output_pipename =
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

		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

		hydrology_library_output_station_text_filename(
			output_pipename,
			application_name,
			station_name,
			1 /* with_zap_file */ );

		sprintf( sys_string, "sed 's/\\^/,/g' >> %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "%s %s\nBegin^Count^Relative^End\n",
			 title,
			 sub_title );

		output_hydroperiod_output_transmit(
					output_pipe,
					application_name,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					elevation,
					output_medium );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "transmit" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		char sub_title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		get_report_title(	title,
					station_name,
					datatype_name,
					elevation );

		get_report_sub_title(	sub_title,
					begin_date_string,
					end_date_string );

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" );

		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;

		output_pipename =
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

		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

		hydrology_library_output_station_text_filename(
			output_pipename,
			application_name,
			station_name,
			1 /* with_zap_file */ );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '^' 3 >> %s",
			 output_pipename );
*/
		sprintf(sys_string,
		 	"tr '^' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "#%s %s\n#Begin^Count^Relative^End\n",
			 title,
			 sub_title );

		output_hydroperiod_output_transmit(
					output_pipe,
					application_name,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					elevation,
					output_medium );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];
		char title[ 512 ];

		hydrology_library_output_station_text_file(
			stdout,
			application_name,
			station_name );

		get_report_title(	title,
					station_name,
					datatype_name,
					elevation );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '^' 3" );
*/
		sprintf(sys_string,
		 	"tr '^' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "#%s\n#Begin^Count^Relative^End\n",
			 title );

		output_hydroperiod_output_transmit(
					output_pipe,
					application_name,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					elevation,
					output_medium );

		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		if ( !output_hydroperiod_output_gracechart(
				application_name,
				begin_date_string,
				end_date_string,
				appaserver_parameter_file->
					document_root,
				argv[ 0 ],
				station_name,
				datatype_name,
				elevation,
				output_medium ) )
		{
			printf( "<p>Warning: nothing selected to output\n" );
		}
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void build_sys_string(
		char *sys_string,
		char *application_name,
		char *begin_date_string,
		char *end_date_string,
		char *station_name,
		char *datatype_name,
		double elevation,
		char *output_medium )
{
	sprintf( sys_string,
		 "calculate_hydroperiod %s %s %s %s %s %lf %s",
		 application_name,
		 station_name,
		 datatype_name,
		 begin_date_string,
		 end_date_string,
		 elevation,
		 output_medium );

} /* build_sys_string() */

boolean output_hydroperiod_output_gracechart(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				char *argv_0,
				char *station_name,
				char *datatype_name,
				double elevation,
				char *output_medium )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	GRACE *grace;
	char graph_identifier[ 128 ];
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char input_buffer[ 128 ];
	char grace_set_buffer[ 128 ];
	char sys_string[ 1024 ] = {0};
	FILE *input_pipe;

	build_sys_string(
		sys_string,
		application_name,
		begin_date_string,
		end_date_string,
		station_name,
		datatype_name,
		elevation,
		output_medium );

	get_report_title(	title,
				station_name,
				datatype_name,
				elevation );

	get_report_sub_title( sub_title, begin_date_string, end_date_string );

	format_initial_capital( sub_title, sub_title );

	grace = grace_new_unit_graph_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				0 /* grace_datatype_entity_piece */,
				1 /* grace_datatype_piece */,
				2 /* grace_date_piece */,
				-1 /* grace_time_piece */,
				3 /* grace_value_piece */,
				strdup( title ),
				strdup( sub_title ),
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

	/* Create the elevation graph */
	/* -------------------------- */
	grace_graph = grace_new_grace_graph();
	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;
	grace_graph->yaxis_tickmarks_on_off = "off";
	list_append_pointer( grace->graph_list, grace_graph );

	grace_datatype =
		grace_new_grace_datatype( station_name, datatype_name );
	grace_datatype->legend = "Stage";
	grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
	grace_datatype->line_linestyle = 0;

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		sprintf(	grace_set_buffer,
				"%s^%s^%s",
				station_name,
				datatype_name,
				input_buffer );

		grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				grace_set_buffer,
				grace->grace_graph_type,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
	}

	pclose( input_pipe );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf(	graph_identifier,
			"%s_%s_%d",
			station_name,
			datatype_name,
			getpid() );

	grace_get_filenames(
			&grace->agr_filename,
			&grace->ftp_agr_filename,
			&grace->postscript_filename,
			&grace->output_filename,
			&grace->ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	grace_graph_set_no_yaxis_grid_lines_offset(
			grace->graph_list,
			0 /* graph_offset */ );

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
		return 0;
	}

	grace_graph->y_tick_major = 0.0;
	grace_graph->y_tick_minor = 0;

	if ( grace_contains_multiple_datatypes( grace->graph_list ) )
		grace->dataset_no_cycle_color = 1;

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
		return 0;
	}
	else
	{
		grace_output_graph_window(
				application_name,
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* with_document_output */,
				(char *)0 /* where_clause */ );
		return 1;
	}
} /* output_hydroperiod_output_gracechart() */

void get_report_sub_title(
			char *sub_title,
			char *begin_date_string,
			char *end_date_string )
{
	sprintf(sub_title,
		"From %s To %s",
		begin_date_string,
		end_date_string );
}

void get_report_title(	char *title,
			char *station_name,
			char *datatype_name,
			double elevation )
{
	sprintf(	title,
			"%s for %s/%s at %.2lf feet",
			REPORT_TITLE,
			station_name,
			datatype_name,
			elevation );
	format_initial_capital( title, title );
} /* get_report_title() */

double get_ground_surface_elevation(
				boolean *missing_ground_surface_elevation,
				char *application_name,
				char *station_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results_string;
	char *folder_name;

	sprintf( where, "station = '%s'", station_name );

/*
	if ( folder_exists_folder( application_name, "enp_station" ) )
		folder_name = "enp_station";
	else
		folder_name = "station";
*/
	folder_name = "station";

	sprintf( sys_string,
		 "get_folder_data	application=%s			   "
		 "			select=ground_surface_elevation_ft "
		 "			folder=%s			   "
		 "			where=\"%s\"			   ",
		 application_name,
		 folder_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string || !*results_string )
	{
		*missing_ground_surface_elevation = 1;
		return 0.0;
	}

	*missing_ground_surface_elevation = 0;
	return atof( results_string );
} /* get_ground_surface_elevation() */

void output_hydroperiod_output_table(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *station_name,
					char *datatype_name,
					double elevation,
					char *output_medium )
{

	char title[ 512 ];
	char sys_string[ 1024 ] = {0};
	char sub_title[ 512 ];

	hydrology_library_output_station_table(
		application_name,
		station_name );

	build_sys_string(
		sys_string,
		application_name,
		begin_date_string,
		end_date_string,
		station_name,
		datatype_name,
		elevation,
		output_medium );

	get_report_title(	title,
				station_name,
				datatype_name,
				elevation );

	get_report_sub_title( sub_title, begin_date_string, end_date_string );

	sprintf( sys_string + strlen( sys_string ),
		 "| html_table.e \"%s^%s\" \"%s\" '^' %s		|"
		 "cat							 ",
		 title,
		 sub_title,
		 "Begin,Count,Relative,End",
		 "left,right,left,left" );

	fflush( stdout );
	system( sys_string );
} /* output_hydroperiod_output_table() */

void output_hydroperiod_output_transmit(
					FILE *output_pipe,
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *station_name,
					char *datatype_name,
					double elevation,
					char *output_medium )
{

	char sys_string[ 1024 ] = {0};
	char input_string[ 1024 ];
	FILE *input_pipe;

	build_sys_string(
		sys_string,
		application_name,
		begin_date_string,
		end_date_string,
		station_name,
		datatype_name,
		elevation,
		output_medium );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
		fprintf( output_pipe, "%s\n", input_string );

	pclose( input_pipe );

} /* output_hydroperiod_output_transmit() */

