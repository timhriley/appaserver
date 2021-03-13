/* -------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_daily_moving_average.c      	*/
/* -------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "list_usage.h"
#include "aggregate_statistic.h"
#include "grace.h"
#include "julian.h"
#include "session.h"
#include "process_generic.h"
#include "moving_average.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define EXCEEDANCE_DELIMITER		'|'
#define DEFAULT_DAYS_TO_AVERAGE		30
#define GRACE_DATATYPE_ENTITY_PIECE	0
#define GRACE_DATATYPE_PIECE		1
#define GRACE_DATE_PIECE		2
#define GRACE_TIME_PIECE		-1
#define GRACE_VALUE_PIECE		3
#define KEY_DELIMITER			'/'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
char *moving_average_system_string(
			int date_piece,
			int value_piece,
			int days_to_average );

void generic_output_spreadsheet_create(
			char *output_filename,
			char *input_system_string,
			char *output_system_string,
			char *heading,
			char *subtitle );

void generic_output_text_file(
			char *input_system_string,
			char *heading,
			char *subtitle );

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *subtitle,
			int primary_attribute_name_list_length );

void generic_output_spreadsheet(
			char *application_name,
			char *input_system_string,
			char *heading,
			char *subtitle,
			char *document_root_directory,
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			pid_t process_id,
			char *email_address );

void generic_output_stdout(
			char *input_system_string,
			char *heading,
			char *subtitle );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	int days_to_average;
	boolean exceedance_format;
	char *output_medium_string;
	char title[ 128 ];
	DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	PROCESS_GENERIC *process_generic;
	char input_system_string[ 1024 ];
	char additional_message[ 128 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s process_set process days_to_average exceedance_format_yn output_medium dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 1 ];
	process_name = argv[ 2 ];
	days_to_average = atoi( argv[ 3 ] );
	exceedance_format = ( *argv[ 4 ] == 'y' );
	output_medium_string = argv[ 5 ];

	post_dictionary =
		dictionary_string2dictionary(
			argv[ 6 ] );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );
	}

	/* Have process_generic_system_string() set real_time2aggregate_value */
	/* ------------------------------------------------------------------ */
	dictionary_set_pointer(
		post_dictionary,
		"aggregate_level_0",
		"daily" );

	dictionary_set_pointer(
		post_dictionary,
		"aggregate_statistic_0",
		"average" );

	process_generic =
		process_generic_fetch(
			process_set_name,
			process_name,
			output_medium_string,
			post_dictionary );

	if ( !process_generic
	||   !process_generic->parameter->begin_date
	||   !process_generic->parameter->end_date )
	{
		printf( "<h3>Insufficient input</h3>\n" );
		document_close();
		exit( 0 );
	}

	/* Reset the subtitle */
	/* ------------------ */
	process_generic->parameter->aggregate_level = moving;

	sprintf(additional_message,
		"(%d days)",
		(days_to_average)
			? days_to_average
			: MOVING_AVERAGE_DEFAULT );

	process_generic->process_generic_subtitle =
		process_generic_subtitle(
			process_generic->value_folder_name,
			process_generic->parameter->begin_date,
			process_generic->parameter->end_date,
			process_generic->parameter->aggregate_level,
			process_generic->parameter->aggregate_statistic,
			additional_message );

	if ( process_generic->parameter->output_medium != output_medium_stdout )
	{
		printf(	"<h1>%s</h1>\n",
			format_initial_capital(
				title,
				process_generic->process_name ) );
	}
	else
	{
		printf(	"%s\n",
			format_initial_capital(
				title,
				process_generic->process_name ) );
	}

	fflush( stdout );

	sprintf(input_system_string,
		"%s | %s",
		process_generic->process_generic_system_string,
		moving_average_system_string(
			process_generic->
				parameter->
				date_piece,
			process_generic->
				parameter->
				value_piece,
			days_to_average ) );

	if (	process_generic->parameter->output_medium ==
		output_medium_stdout )
	{
		generic_output_stdout(
			input_system_string,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle );
	}
	else
	if (	process_generic->parameter->output_medium ==
		spreadsheet )
	{
		generic_output_spreadsheet(
			environment_application(),
			process_generic->
				process_generic_system_string
				/* input_system_string */,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle,
			appaserver_parameter_file->document_root,
			process_generic->
				value_folder->
				value_folder_name,
			process_generic->
				parameter->
				begin_date,
			process_generic->
				parameter->
				end_date,
			process_generic->
				parameter->
				process_id,
			process_generic->
				parameter->
				email_address );
	}
	else
	if (	process_generic->parameter->output_medium ==
		text_file )
	{
		generic_output_text_file(
			process_generic->
				process_generic_system_string,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle );
	}
	else
	if (	process_generic->parameter->output_medium ==
		table
	||      process_generic->parameter->output_medium ==
		output_medium_unknown )
	{
		generic_output_table(
			input_system_string,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle,
			list_length(
				process_generic->
					value_folder->
					primary_attribute_name_list ) );
	}

#ifdef NOT_DEFINED
	JULIAN *moving_begin_date;
	moving_begin_date = julian_new_yyyy_mm_dd( begin_date_string );
	julian_decrement_days( moving_begin_date, days_to_average - 1 );
	moving_begin_date_string =
		julian_get_yyyy_mm_dd_string( moving_begin_date->current );

	where_clause = process_generic_output_get_dictionary_where_clause(
			&moving_begin_date_string,
			&end_date_string,
			application_name,
			process_generic_output,
			post_dictionary,
			1 /* with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name );

	build_sys_string(
				sys_string,
				application_name,
				aggregate_statistic,
				exceedance_format_yn,
				days_to_average,
				process_generic_output->
					value_folder->
					datatype->
					units,
				units_converted,
				process_generic_output,
				where_clause,
				begin_date_string,
				end_date_string );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		units = units_converted;
	}
	else
	{
		units = process_generic_output->value_folder->datatype->units;
	}

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( output_medium, "chart" ) != 0 )
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
		if ( *exceedance_format_yn == 'y' )
		{
			daily_moving_average_output_table_exceedance_format(
					application_name,
					sys_string,
					days_to_average,
					units,
					where_clause,
					process_generic_output->
						value_folder->
							value_folder_name );
		}
		else
		{
			daily_moving_average_output_table(
					application_name,
					sys_string,
					days_to_average,
					units,
					where_clause,
					process_generic_output->
						value_folder->
							value_folder_name );
		}
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		int results;

		if ( *exceedance_format_yn == 'y' )
		{
			results =
			daily_moving_average_output_chart_exceedance_format(
					application_name,
					process_generic_get_datatype_name(
					   process_generic_output->
					   value_folder->
					   datatype->
					   primary_attribute_data_list,
					   ' ' /* delimiter */ ),
					begin_date_string,
					end_date_string,
					sys_string,
					appaserver_parameter_file->
						document_root,
					appaserver_parameter_file->
						appaserver_mount_point,
					days_to_average,
					units,
					where_clause,
					process_generic_output->
						value_folder->
							value_folder_name );
		}
		else
		{
			results = daily_moving_average_output_chart(
					application_name,
					role_name,
					process_generic_get_datatype_name(
					   process_generic_output->
					   value_folder->
					   datatype->
					   primary_attribute_data_list,
					   ' ' /* delimiter */ ),
					begin_date_string,
					end_date_string,
					sys_string,
					appaserver_parameter_file->
						document_root,
					appaserver_parameter_file->
						appaserver_mount_point,
					argv[ 0 ],
					days_to_average,
					units,
					where_clause,
					process_generic_output->
						value_folder->
							value_folder_name,
					process_generic_output->
						value_folder->
						datatype->
						foreign_attribute_data_list );
		}

		if ( !results )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( output_medium, "transmit" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char output_sys_string[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->document_root,
				(char *)0 /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" );

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

/*
		sprintf( output_filename, 
			 OUTPUT_FILE_TEMPLATE,
			 appaserver_parameter_file->appaserver_mount_point,
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
		else
		{
			fclose( output_file );
		}

/*
		sprintf( output_sys_string,
			 "delimiter2padded_columns.e '|' 99 > %s",
			 output_filename );
*/

		sprintf(sys_string,
		 	"tr '|' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			daily_moving_average_output_transmit_exceedance_format(
				output_pipe,
				sys_string,
				days_to_average,
				units );
		}
		else
		{
			daily_moving_average_output_transmit(
				output_pipe,
				sys_string,
				days_to_average,
				units );
		}

		pclose( output_pipe );

		printf( "<h1>%d Day Moving Average Transmission<br></h1>\n",
			days_to_average );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );
	
		printf( "<br>Search Criteria: %s\n",
			 query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->
						value_folder_name,
				1 ) );

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
		char output_sys_string[ 512 ];

		strcpy( output_sys_string, "cat" );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			daily_moving_average_output_transmit_exceedance_format(
				output_pipe,
				sys_string,
				days_to_average,
				units );
		}
		else
		{
			daily_moving_average_output_transmit(
				output_pipe,
				sys_string,
				days_to_average,
				units );
		}
		pclose( output_pipe );
	}
#endif

	if (	process_generic->parameter->output_medium !=
		output_medium_stdout )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_generic->process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

}

#ifdef NOT_DEFINED
void build_sys_string(
				char *sys_string,
				char *application_name,
				enum aggregate_statistic aggregate_statistic,
				char *exceedance_format_yn,
				int days_to_average,
				char *units,
				char *units_converted,
				PROCESS_GENERIC_OUTPUT *process_generic_output,
				char *where_clause,
				char *begin_date_string,
				char *end_date_string )
{
	char aggregation_process[ 1024 ];
	char exceedance_process[ 256 ];
	char units_converted_process[ 128 ];

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

	if ( *exceedance_format_yn == 'y' )
	{
		sprintf(	exceedance_process,
				"piece_shift_left.e '%c'	|"
				"piece_exceedance.e '%c'	|"
				"sed 's/%c/%c/g'		|"
				"cat				 ",
				INPUT_DELIMITER,
				INPUT_DELIMITER,
				INPUT_DELIMITER,
				EXCEEDANCE_DELIMITER );
	}
	else
	{
		strcpy( exceedance_process, "cat" );
	}

	sprintf(
	 aggregation_process,
	 "real_time2aggregate_value.e %s %d %d %d '%c' daily n %s	|"
	 "piece_inverse.e 2 '%c'					|"
	 "%s								|"
	 "moving_average.e %d '%c' average %s				 ",
	 aggregate_statistic_get_string( aggregate_statistic ),
	 DATE_PIECE,
	 TIME_PIECE,
	 VALUE_PIECE,
	 INPUT_DELIMITER,
	 end_date_string,
	 INPUT_DELIMITER,
	 units_converted_process,
	 days_to_average,
	 INPUT_DELIMITER,
	 begin_date_string );

	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=%s		    "
	"			select=\"%s,%s\"	    "
	"			where=\"%s\"		   |"
	"tr '%c' '%c' 					   |"
	"%s						   |"
	"sort						   |"
	"%s						   |"
	"cat						    ",
		application_name,
		process_generic_output->value_folder->value_folder_name,
		process_generic_output->value_folder->date_attribute_name,
		process_generic_output->value_folder->value_attribute_name,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregation_process,
		exceedance_process );

}

int daily_moving_average_output_chart(
				char *application_name,
				char *role_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *appaserver_mount_point,
				char *argv_0,
				int days_to_average,
				char *units,
				char *where_clause,
				char *value_folder_name,
				LIST *primary_attribute_data_list )
{
	char input_buffer[ 512 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	char buffer1[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
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
	boolean bar_graph = 0;
	char legend[ 128 ];
	char grace_string[ 128 ];

	sprintf(title,
		"%s %d Day Moving Average",
		value_folder_name,
		days_to_average );

	format_initial_capital( title, title );

	sprintf(	sub_title,
			"%s From: %s To: %s",
			list_display_delimited(
				primary_attribute_data_list,
				'/' ),
			begin_date_string,
			end_date_string );

	format_initial_capital( sub_title, sub_title );

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
				title,
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		document_quick_output_body(
					application_name,
					appaserver_mount_point );

		printf(
		"<h3>ERROR: invalid date format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph->units = units;

	grace_datatype = grace_new_grace_datatype(
					(char *)0 /* datatype_entity_name */,
					strdup( datatype_name ) );

	grace_graph->xaxis_ticklabel_angle = 45;

	strcpy( legend, datatype_name );

	strcpy(	legend,
		format_initial_capital( buffer1, legend ) );

	grace_datatype->legend = strdup( legend );

	if ( bar_graph )
	{
		grace_datatype->line_linestyle = 0;
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

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
		document_quick_output_body(
					application_name,
					appaserver_mount_point );

		printf( "<h3>Warning: no graphs to display.</h3>\n" );
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
				appaserver_mount_point,
				1 /* with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
					1 ) );
	}
	return 1;
}

void daily_moving_average_output_transmit(
				FILE *output_pipe,
				char *sys_string,
				int days_to_average,
				char *units )
{
	char buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	FILE *input_pipe;

	fprintf(	output_pipe,
			"#Date|MovingAverage(%d)(%s)\n",
			days_to_average,
			units );

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
					char *application_name,
					char *sys_string,
					int days_to_average,
					char *units,
					char *where_clause,
					char *value_folder_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char units_buffer[ 128 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	int count = 0;

	sprintf(	title,
	"%d Day Moving Average<br><p>Search Criteria: %s",
			days_to_average,
			query_get_display_where_clause(
				where_clause,
				application_name,
				value_folder_name,
				1 ) );

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
				units ) );

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
				char *application_name,
				char *sys_string,
				int days_to_average,
				char *units,
				char *where_clause,
				char *value_folder_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	char units_buffer[ 128 ];
	int count = 0;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;

	sprintf(	title,
"%d Day Moving Average Exceedance Format<br><p>Search criteria: %s",
			days_to_average,
			 query_get_display_where_clause(
				where_clause,
				application_name,
				value_folder_name,
				1 ) );

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
				units ) );

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
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *appaserver_mount_point,
				int days_to_average,
				char *units,
				char *where_clause,
				char *value_folder_name )
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

	sprintf( title, "%d Day Moving Average", days_to_average );

	sprintf(sub_title,
		"%s from %s to %s Exceedance Format",
			format_initial_capital( buffer1, datatype_name ),
			begin_date_string,
			end_date_string );

	grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				title,
				sub_title,
				units,
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
				appaserver_mount_point,
				1 /* with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
					1 ) );
	return 1;
}

void daily_moving_average_output_transmit_exceedance_format(
				FILE *output_pipe,
				char *sys_string,
				int days_to_average,
				char *units )
{
	char buffer[ 512 ];
	FILE *input_pipe;

	fprintf(	output_pipe,
			"#MovingAverage(%d)(%s)|Date|CountBelow|%cBelow\n",
			days_to_average,
			units,
			'%' );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", buffer );
	}

	pclose( input_pipe );

}
#endif

void generic_output_stdout(
			char *input_system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( input_system_string, "r" );

	printf( "%s\n%s\n",
		subtitle,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		printf( "%s\n", input_buffer );
	}
	pclose( input_pipe );
}

void generic_output_text_file(
			char *system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( system_string, "r" );
	output_pipe = popen( "html_paragraph_wrapper", "w" );

	fprintf(output_pipe,
		"%s\n%s\n",
		subtitle,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *subtitle,
			int primary_attribute_name_list_length )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];
	char output_system_string[ 1024 ];

	sprintf(output_system_string,
		"html_table \"^%s\" \"%s\" '%c' %sright",
		subtitle,
		heading,
		SQL_DELIMITER,
		string_repeat(
			"left,",
			primary_attribute_name_list_length
				/* number_times */ ) );

	input_pipe = popen( input_system_string, "r" );
	output_pipe = popen( output_system_string, "w" );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

void generic_output_spreadsheet(
			char *application_name,
			char *input_system_string,
			char *heading,
			char *subtitle,
			char *document_root_directory,
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			pid_t process_id,
			char *email_address )
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *output_filename;
	char *link_prompt;
	char output_system_string[ 1024 ];

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->application_name = application_name;
	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	appaserver_link_file->extension = "csv";

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

	link_prompt =
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

	generic_output_spreadsheet_create(
		output_filename,
		input_system_string,
		output_system_string,
		heading,
		subtitle );

	if ( email_address && *email_address )
	{
		char sys_string[ 1024 ];
	
		sprintf( sys_string,
			 "cat %s				|"
			 "mailx -s \"%s\" %s	 		 ",
			 output_filename,
			 subtitle,
			 email_address );

		if ( system( sys_string ) ){};

		printf( "<h3>Sent to %s<hr></h3>\n", email_address );
	}
	else
	{
		appaserver_library_output_ftp_prompt(
			link_prompt, 
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}
}

void generic_output_spreadsheet_create(
			char *output_filename,
			char *input_system_string,
			char *output_system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( input_system_string, "r" );

	sprintf(output_system_string,
		"cat > %s",
		output_filename );

	output_pipe = popen( output_system_string, "w" );

	printf( "<h2>%s</h2>\n", subtitle );

	fprintf(output_pipe,
		"%s\n",
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

char *moving_average_system_string(
			int date_piece,
			int value_piece,
			int days_to_average )
{
	char system_string[ 1024 ];

/*
	sprintf(system_string,
	"pad_missing_times.e '%c' %d,-1,%d daily %s 0000 %s 2359 0 1 	|"
	"moving_average_piece.e %d %d %d '%c'				 ",
		SQL_DELIMITER,
		date_piece,
		value_piece,
		begin_date,
		end_date,
		days_to_average,
		date_piece,
		value_piece,
		SQL_DELIMITER );
*/

	sprintf(system_string,
	"moving_average_piece.e %d %d %d '%c'				 ",
		days_to_average,
		date_piece,
		value_piece,
		SQL_DELIMITER );

	return strdup( system_string );
}

