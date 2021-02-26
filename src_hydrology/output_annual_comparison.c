/* --------------------------------------------------- 	*/
/* src_hydrology/output_annual_comparison.c	      	*/
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
#include "julian.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM	"table"

#define ROWS_BETWEEN_HEADING			20

#define GRACE_DATATYPE_ENTITY_PIECE		-1
#define GRACE_DATATYPE_PIECE			0
#define GRACE_DATE_PIECE			1
#define GRACE_TIME_PIECE			-1
#define GRACE_VALUE_PIECE			2

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void get_begin_end_date(		char **begin_date_string,
					char **end_date_string,
					char *begin_month_day,
					char *begin_year,
					char *end_month_day );

boolean begin_before_end(		char *begin_month_day,
					char *begin_year,
					char *end_month_day );

void get_report_sub_title(
					char *sub_title,
					char *begin_month_day,
					char *end_month_day,
					char *begin_year,
					char *end_year,
					char *aggregate_statistic_string,
					char *application_name,
					char *datatype_name );

void output_annual_comparison_output_spreadsheet(
					FILE *output_pipe,
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *aggregate_statistic_string );

void output_annual_comparison_output_text_file(
					FILE *output_pipe,
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *aggregate_statistic_string );

void output_annual_comparison_output_table(
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *begin_year,
					char *end_year,
					char *process_name,
					char *aggregate_statistic_string );

void get_report_title(		char *title,
				char *process_name,
				char *station_name,
				char *datatype_name );

boolean output_annual_comparison_output_gracechart(
				char *application_name,
				char *document_root_directory,
				char *argv_0,
				char *station_name,
				char *datatype_name,
				char *begin_month_day,
				char *end_month_day,
				char *year_expression,
				char *begin_year,
				char *end_year,
				char *process_name,
				char *aggregate_statistic_string );

void build_sys_string(
				char *sys_string,
				char *application_name,
				char *station_name,
				char *datatype_name,
				char *begin_month_day,
				char *end_month_day,
				char *year_expression,
				char *aggregate_statistic_string );

int main( int argc, char **argv )
{
	char *application_name;
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *process_name;
	char *session;
	char *station_name;
	char *datatype_name;
	char *begin_month_day;
	char *end_month_day;
	char *year_expression;
	char *begin_year = {0};
	char *end_year = {0};
	char *aggregate_statistic_string;
	char *output_medium;
	char *ftp_filename;
	char *output_pipename;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s application session process station datatype begin_day_month end_day_month year_expression aggregate_statistic output_medium\n",
			argv[ 0 ] );
		fprintf(stderr,
"Note: output_medium = {chart,table,text_file,spreadsheet,stdout}\n" );
		exit( 1 );
	}


	application_name = argv[ 1 ];
	session = argv[ 2 ];
	process_name = argv[ 3 ];
	station_name = argv[ 4 ];
	datatype_name = argv[ 5 ];
	begin_month_day = argv[ 6 ];
	end_month_day = argv[ 7 ];
	year_expression = argv[ 8 ];
	aggregate_statistic_string = argv[ 9 ];
	output_medium = argv[ 10 ];

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

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			(char *)0 /* extension */ );

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

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

	if ( !hydrology_library_get_begin_end_year(
			&begin_year,
			&end_year,
			year_expression ) )
	{
		printf(
"<h3>Error: invalid year expression.</h3>\n" );
		document_close();
		exit( 0 );
	}
	if ( !character_exists( begin_month_day, '-' )
	||   !character_exists( end_month_day, '-' ) )
	{
		printf(
"<h3>Error: the dates must contain a dash.</h3>\n" );
		document_close();
		exit( 0 );
	}
	else
	if ( !begin_before_end(	begin_month_day,
				begin_year,
				end_month_day ) )
	{
		printf(
"<h3>Error: the begin month day must be earlier than the end month day.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_annual_comparison_output_table(
					application_name,
					station_name,
					datatype_name,
					begin_month_day,
					end_month_day,
					year_expression,
					begin_year,
					end_year,
					process_name,
					aggregate_statistic_string );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		get_report_title(	title,
					process_name,
					station_name,
					datatype_name );

		get_report_sub_title(	sub_title,
					begin_month_day,
					end_month_day,
					begin_year,
					end_year,
					aggregate_statistic_string,
					application_name,
					datatype_name );

		appaserver_link_file->begin_date_string = begin_month_day;
		appaserver_link_file->end_date_string = end_month_day;
		appaserver_link_file->extension = "csv";

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

/*
		sprintf( output_pipename,
			 OUTPUT_SPREADSHEET_TEMPLATE,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 begin_month_day,
			 end_month_day,
			 session );
*/

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

		hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

		sprintf( sys_string,
			 "cat >> %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "%s %s\n",
			 title,
			 sub_title );

		output_annual_comparison_output_spreadsheet(
					output_pipe,
					application_name,
					station_name,
					datatype_name,
					begin_month_day,
					end_month_day,
					year_expression,
					aggregate_statistic_string );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];
		char title[ 512 ];

		get_report_title(	title,
					process_name,
					station_name,
					datatype_name );

		get_report_sub_title(	sub_title,
					begin_month_day,
					end_month_day,
					begin_year,
					end_year,
					aggregate_statistic_string,
					application_name,
					datatype_name );

		appaserver_link_file->begin_date_string = begin_month_day;
		appaserver_link_file->end_date_string = end_month_day;
		appaserver_link_file->extension = "txt";

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

		hydrology_library_output_station_text_filename(
				output_pipename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

		sprintf( sys_string,
			 "cat >> %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "%s %s\n",
			 title,
			 sub_title );

		output_annual_comparison_output_text_file(
					output_pipe,
					application_name,
					station_name,
					datatype_name,
					begin_month_day,
					end_month_day,
					year_expression,
					aggregate_statistic_string );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
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

		get_report_title(	title,
					process_name,
					station_name,
					datatype_name );

		sprintf( sys_string,
			 "cat" );

		output_pipe = popen( sys_string, "w" );

		fprintf( output_pipe,
			 "%s\n",
			 title );

		output_annual_comparison_output_text_file(
					output_pipe,
					application_name,
					station_name,
					datatype_name,
					begin_month_day,
					end_month_day,
					year_expression,
					aggregate_statistic_string );

		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		if ( !output_annual_comparison_output_gracechart(
				application_name,
				appaserver_parameter_file->
					document_root,
				argv[ 0 ],
				station_name,
				datatype_name,
				begin_month_day,
				end_month_day,
				year_expression,
				begin_year,
				end_year,
				process_name,
				aggregate_statistic_string ) )
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
}

void build_sys_string(
		char *sys_string,
		char *application_name,
		char *station_name,
		char *datatype_name,
		char *begin_month_day,
		char *end_month_day,
		char *year_expression,
		char *aggregate_statistic_string )
{
	sprintf( sys_string,
		 "annual_comparison %s %s %s %s %s '%s' %s",
		 application_name,
		 station_name,
		 datatype_name,
		 begin_month_day,
		 end_month_day,
		 year_expression,
		 aggregate_statistic_string );

}

boolean output_annual_comparison_output_gracechart(
				char *application_name,
				char *document_root_directory,
				char *argv_0,
				char *station_name,
				char *datatype_name,
				char *begin_month_day,
				char *end_month_day,
				char *year_expression,
				char *begin_year,
				char *end_year,
				char *process_name,
				char *aggregate_statistic_string )
{
	GRACE *grace;
	char graph_identifier[ 128 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char *begin_date_string;
	char *end_date_string;
	char point_list_string[ 1024 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	char input_string[ 128 ];
	char sys_string[ 1024 ] = {0};
	FILE *input_pipe;
	int year;
	int begin_year_integer;
	int end_year_integer;
	char datatype_name_string[ 16 ];
	char day_string[ 16 ];
	char year_value_string[ 32 ];
	char year_string[ 16 ];
	char value_string[ 24 ];

	build_sys_string(
		sys_string,
		application_name,
		station_name,
		datatype_name,
		begin_month_day,
		end_month_day,
		year_expression,
		aggregate_statistic_string );

	get_report_title(	title,
				process_name,
				station_name,
				datatype_name );

	get_report_sub_title(	sub_title,
				begin_month_day,
				end_month_day,
				begin_year,
				end_year,
				aggregate_statistic_string,
				application_name,
				datatype_name );

	grace = grace_new_unit_graph_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				strdup( title ),
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	get_begin_end_date(	&begin_date_string,
				&end_date_string,
				begin_month_day,
				begin_year,
				end_month_day );

	if ( !grace_set_begin_end_date(	grace,
					begin_date_string,
					end_date_string ) )
	{
		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

/*
	grace_graph->units = "count";
	grace_graph->world_min_y = -10;
	grace_graph->y_tick_major = 30;
*/

	begin_year_integer = atoi( begin_year );
	end_year_integer = atoi( end_year );

	for( year = begin_year_integer; year <= end_year_integer; year++ )
	{
		sprintf( datatype_name_string, "%d", year );

		grace_datatype =
			grace_new_grace_datatype(
				(char *)0 /* datatype_entity */,
				strdup( datatype_name_string ) );

		grace_datatype->legend = strdup( datatype_name_string );

		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";

		list_append_pointer(	grace_graph->datatype_list,
					grace_datatype );
	}

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

	build_sys_string(
		sys_string,
		application_name,
		station_name,
		datatype_name,
		begin_month_day,
		end_month_day,
		year_expression,
		aggregate_statistic_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
		{
			strcpy( day_string, input_string + 5 );
		}
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
		{
			strcpy( year_value_string, input_string + 6 );

			piece(	year_string,
				',',
				year_value_string,
				0 );

			piece(	value_string,
				',',
				year_value_string,
				1 );

			sprintf(point_list_string,
				"%s|%s-%s|%s",
				year_string,
		 		begin_year,
				day_string,
		 		value_string );

			grace_set_string_to_point_list(
				grace->graph_list, 
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				point_list_string,
				unit_graph,
				grace->datatype_type_xyhilo,
				1 /* grace->dataset_no_cycle_color */,
				(char *)0 /* optional_label */ );
		}
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
		printf( "<h3>Warning: no graphs to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

#ifdef NOT_DEFINED
	grace_move_legend_bottom_left(
			list_get_first_pointer( grace->graph_list ),
			grace->landscape_mode );

	grace_increase_legend_char_size(
			list_get_first_pointer(
				grace->graph_list ),
			0.15 );

	/* Make the graph wider -- 97% of the page */
	/* --------------------------------------- */
	grace_set_view_maximum_x(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.97 );

	/* Move the legend down a little */
	/* ----------------------------- */
	grace_lower_legend(	grace->graph_list,
				0.04 );
#endif

	grace->dataset_no_cycle_color = 1;

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
		printf( "<h3>No data for selected parameters.</h3>\n" );
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

	return 1;

}

void get_report_sub_title(
			char *sub_title,
			char *begin_month_day,
			char *end_month_day,
			char *begin_year,
			char *end_year,
			char *aggregate_statistic_string,
			char *application_name,
			char *datatype_name )
{
	enum aggregate_statistic aggregate_statistic;

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			daily );

	aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
			application_name,
			datatype_name,
			aggregate_statistic );

	sprintf(sub_title,
		"daily %s values from %s to %s year %s to %s",
		aggregate_statistic_get_string( aggregate_statistic ),
		begin_month_day,
		end_month_day,
		begin_year,
		end_year );
	format_initial_capital( sub_title, sub_title );
}

void get_report_title(	char *title,
			char *process_name,
			char *station_name,
			char *datatype_name )
{
	sprintf(	title,
			"%s for %s/%s",
			process_name,
			station_name,
			datatype_name );
	format_initial_capital( title, title );
}

double get_ground_surface_elevation(
				boolean *missing_ground_surface_elevation,
				char *application_name,
				char *station_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results_string;

	sprintf( where, "station = '%s'", station_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			   "
		 "			select=ground_surface_elevation_ft "
		 "			folder=station			   "
		 "			where=\"%s\"			   ",
		 application_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string || !*results_string )
	{
		*missing_ground_surface_elevation = 1;
		return 0.0;
	}

	*missing_ground_surface_elevation = 0;
	return atof( results_string );
}

void output_annual_comparison_output_table(
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *begin_year,
					char *end_year,
					char *process_name,
					char *aggregate_statistic_string )
{
	char title[ 512 ];
	char sys_string[ 1024 ];
	char sub_title[ 512 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_string[ 1024 ];

	hydrology_library_output_station_table(
		application_name,
		station_name );

	get_report_title(	title,
				process_name,
				station_name,
				datatype_name );

	get_report_sub_title(	sub_title,
				begin_month_day,
				end_month_day,
				begin_year,
				end_year,
				aggregate_statistic_string,
				application_name,
				datatype_name );

	sprintf( sys_string,
		 "html_table.e \"%s^%s\" \"%s\" ',' %s",
		 title,
		 sub_title,
		 "Day,Year,Value",
		 "left,right,right" );

	output_pipe = popen( sys_string, "w" );

	build_sys_string(
		sys_string,
		application_name,
		station_name,
		datatype_name,
		begin_month_day,
		end_month_day,
		year_expression,
		aggregate_statistic_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
			fprintf( output_pipe, "%s", input_string + 5 );
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
			fprintf( output_pipe, ",%s\n", input_string + 6 );
	}

	pclose( input_pipe );
	pclose( output_pipe );

}

void output_annual_comparison_output_text_file(
					FILE *output_pipe,
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *aggregate_statistic_string )
{

	char sys_string[ 1024 ] = {0};
	char input_string[ 1024 ];
	FILE *input_pipe;

	build_sys_string(
		sys_string,
		application_name,
		station_name,
		datatype_name,
		begin_month_day,
		end_month_day,
		year_expression,
		aggregate_statistic_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
		{
			fprintf( output_pipe, "%s\n", input_string + 5 );
		}
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
		{
			search_replace_character( input_string, ',', ' ' );
			fprintf( output_pipe, "%s\n", input_string + 6 );
		}
	}

	pclose( input_pipe );

}

void output_annual_comparison_output_spreadsheet(
					FILE *output_pipe,
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_month_day,
					char *end_month_day,
					char *year_expression,
					char *aggregate_statistic_string )
{

	char sys_string[ 1024 ] = {0};
	char input_string[ 1024 ];
	FILE *input_pipe;

	build_sys_string(
		sys_string,
		application_name,
		station_name,
		datatype_name,
		begin_month_day,
		end_month_day,
		year_expression,
		aggregate_statistic_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_string, input_pipe ) )
	{
		if ( strncmp( input_string, ".day ", 5 ) == 0 )
			fprintf( output_pipe, "%s", input_string + 5 );
		else
		if ( strncmp( input_string, ".year ", 6 ) == 0 )
			fprintf( output_pipe, ",%s\n", input_string + 6 );
	}

	pclose( input_pipe );

}

boolean begin_before_end(	char *begin_month_day,
				char *begin_year,
				char *end_month_day )
{
	JULIAN *begin_julian;
	JULIAN *end_julian;
	char *begin_date_string;
	char *end_date_string;
	boolean return_value;

	get_begin_end_date(	&begin_date_string,
				&end_date_string,
				begin_month_day,
				begin_year,
				end_month_day );

	begin_julian = julian_yyyy_mm_dd_new( begin_date_string );
	end_julian = julian_yyyy_mm_dd_new( end_date_string );

	return_value = (begin_julian->current <= end_julian->current );
	julian_free( begin_julian );
	julian_free( end_julian );
	return return_value;
}

void get_begin_end_date(	char **begin_date_string,
				char **end_date_string,
				char *begin_month_day,
				char *begin_year,
				char *end_month_day )
{
	static char local_begin_date_string[ 32 ];
	static char local_end_date_string[ 32 ];

	sprintf( local_begin_date_string,
		 "%s-%s",
		 begin_year,
		 begin_month_day );

	sprintf( local_end_date_string,
		 "%s-%s",
		 begin_year,
		 end_month_day );

	*begin_date_string = local_begin_date_string;
	*end_date_string = local_end_date_string;
}

