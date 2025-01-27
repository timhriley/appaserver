/* --------------------------------------------------------- 	*/
/* src_appaserver/generic_output_multi_datatype_exceedance.c	*/
/* --------------------------------------------------------- 	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"
#include "query.h"
#include "application.h"
#include "piece.h"
#include "column.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "grace.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "process.h"
#include "session.h"
#include "process_generic_output.h"
#include "dictionary_appaserver.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM	"table"
#define PDF_PROMPT		"Press to view chart."
#define HEADING_DELIMITER	'/'

/* Prototypes */
/* ---------- */
char *get_heading_string(	enum aggregate_level aggregate_level,
				char *time_attribute_name );

char *get_report_sub_title(	char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level );

void output_exceedance_spreadsheet(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *document_root_directory,
			char *login_name,
			char *process_name );

void output_exceedance_text_file(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *document_root_directory,
			char *login_name,
			char *process_name );

void output_exceedance_table(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *login_name,
			char *process_name );

void output_exceedance_gracechart(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *document_root_directory,
			char *login_name,
			char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *process_name;
	char *login_name;
	char *begin_date = {0};
	char *end_date = {0};
	DICTIONARY_APPASERVER *dictionary_appaserver;
	int highest_index;
	char *output_medium;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s ignored login_name process_name output_medium dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 2 ];
	process_name = argv[ 3 ];
	output_medium = argv[ 4 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	original_post_dictionary =
		dictionary_string2dictionary( argv[ 5 ] );

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

	process_generic_output =
		process_generic_output_new(
			application_name,
			process_name,
			(char *)0 /* process_set_name */,
			0 /* accumulate_flag */ );

	highest_index =
		dictionary_get_key_highest_index(
			dictionary_appaserver->
				non_prefixed_dictionary );

	if ( !highest_index )
	{
		document_basic_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h3>ERROR: please select a datatype.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date /* in/out */,
			&end_date /* in/out */,
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
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h3>ERROR: invalid date.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_exceedance_table(
			application_name,
			process_generic_output,
			highest_index,
			begin_date,
			end_date,
			dictionary_appaserver->non_prefixed_dictionary,
			appaserver_parameter_file->
				appaserver_mount_point,
			login_name,
			process_name );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_exceedance_text_file(
			application_name,
			process_generic_output,
			highest_index,
			begin_date,
			end_date,
			dictionary_appaserver->non_prefixed_dictionary,
			appaserver_parameter_file->
				appaserver_mount_point,
			appaserver_parameter_file->
				document_root,
			login_name,
			process_name );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_exceedance_spreadsheet(
			application_name,
			process_generic_output,
			highest_index,
			begin_date,
			end_date,
			dictionary_appaserver->non_prefixed_dictionary,
			appaserver_parameter_file->
				appaserver_mount_point,
			appaserver_parameter_file->
				document_root,
			login_name,
			process_name );
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		output_exceedance_gracechart(
			application_name,
			process_generic_output,
			highest_index,
			begin_date,
			end_date,
			dictionary_appaserver->non_prefixed_dictionary,
			appaserver_parameter_file->
				document_root,
			login_name,
			process_name );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		char *sys_string;
		int percent_below_piece = 0;
		char *where_clause = "";

		sys_string =
			process_generic_output_get_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary_appaserver->
				non_prefixed_dictionary /* post_dictionary */,
			0 /* not with_html_table */,
			login_name );
		system( sys_string );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

void output_exceedance_table(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *login_name,
			char *process_name )
{
	int dictionary_index;
	enum aggregate_level aggregate_level;
	int percent_below_piece = 0;
	char *where_clause = {0};
	char *title_heading_string;
	char title[ 128 ];
	char *sub_title;
	char sys_string[ 1024 ];
	char *input_sys_string;
	FILE *output_pipe;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *table_heading_string;

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				dictionary,
				"aggregate_level" ) );

	document_basic_output_body(	application_name,
					appaserver_mount_point );

	format_initial_capital( title, process_name );

	sub_title = get_report_sub_title(
				begin_date,
				end_date,
				aggregate_level );

	for(	dictionary_index = 1;
		dictionary_index <= highest_index;
		dictionary_index++ )
	{
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
						primary_key_list,
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
			dictionary,
			dictionary_index );

		if ( !process_generic_output->value_folder->datatype )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get value_folder->datatype.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

		title_heading_string =
			process_generic_output_get_datatype_heading_string(
				process_generic_output->
					value_folder->foreign_folder,
				process_generic_output->
					value_folder->datatype,
				HEADING_DELIMITER );

		table_heading_string =
			get_heading_string(
				aggregate_level,
				process_generic_output->
					value_folder->
					time_attribute_name );

		sprintf(sys_string,
			"html_table.e \"%s^%s<br>%s\" \"%s\" '%c' %s",
		 	title,
		 	title_heading_string,
		 	sub_title,
			table_heading_string,
			FOLDER_DATA_DELIMITER,
		 	"right" );

		output_pipe = popen( sys_string, "w" );

		input_sys_string =
		process_generic_output_get_row_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary,
			login_name,
			dictionary_index );

		input_pipe = popen( input_sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( output_pipe );
		pclose( input_pipe );
	}

	document_close();

} /* output_exceedance_table() */

char *get_report_sub_title(	char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level )
{
	static char sub_title[ 1024 ];
	char buffer[ 128 ];

	sprintf(	sub_title,
			"Begin: %s End: %s Aggregation: %s",
			begin_date,
			end_date,
			format_initial_capital(
				buffer,
				aggregate_level_get_string(
					aggregate_level ) ) );

	return sub_title;

} /* get_report_sub_title() */

void output_exceedance_gracechart(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *document_root_directory,
			char *login_name,
			char *process_name )
{
	int dictionary_index;
	enum aggregate_level aggregate_level;
	int percent_below_piece = 0;
	char *where_clause = {0};
	char *title_heading_string;
	char title[ 128 ];
	char *sub_title;
	char *input_sys_string;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	GRACE_GRAPH *grace_graph;
	char value[ 128 ];
	char percent_below[ 128 ];
	int page_width_pixels = 0;
	int page_length_pixels = 0;
	char *distill_landscape_flag = "";
	double legend_char_size_increase_amount;

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				dictionary,
				"aggregate_level" ) );

	document_basic_output_body(
		application_name,
		(char *)0 /* appaserver_mount_point */ );

	format_initial_capital( title, process_name ),

	sub_title = get_report_sub_title(
				begin_date,
				end_date,
				aggregate_level );

	grace = grace_new_xy_grace(
		(char *)0 /* application_name */,
		(char *)0 /* role_name */,
		title,
		sub_title,
		(char *)0 /* units_label */,
		(char *)0 /* legend */ );

	grace_graph = list_get_first_pointer( grace->graph_list );

	sprintf( graph_identifier, "%d", getpid() );

	grace->grace_output =
		application_grace_output(
			application_name );

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

	for(	dictionary_index = 1;
		dictionary_index <= highest_index;
		dictionary_index++ )
	{
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
						primary_key_list,
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
			dictionary,
			dictionary_index );

		if ( !process_generic_output->value_folder->datatype )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get value_folder->datatype.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

		title_heading_string =
			process_generic_output_get_datatype_heading_string(
				process_generic_output->
					value_folder->foreign_folder,
				process_generic_output->
					value_folder->datatype,
				HEADING_DELIMITER );

		grace_datatype_set_legend(
			grace_graph->datatype_list,
			dictionary_index - 1 /* datatype_number */,
			strdup( title_heading_string ) );

		input_sys_string =
		process_generic_output_get_row_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary,
			login_name,
			dictionary_index );

		input_pipe = popen( input_sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			piece(	value,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				0 );

			if ( ! piece(	percent_below,
					FOLDER_DATA_DELIMITER,
					input_buffer,
					percent_below_piece ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece (%c) (%d) in (%s)\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	FOLDER_DATA_DELIMITER,
				 	percent_below_piece,
				 	input_buffer );
				exit( 1 );
			}

#ifdef NOT_DEFINED
			grace_set_datatype_number_point(
				grace_graph,
				atof( value ) /* x */,
				strdup( percent_below ) /* y_string*/,
				dictionary_index - 1 /* datatype_number */ );
#endif
			grace_set_datatype_number_point(
				grace_graph,
				atof( percent_below ) /* x*/,
				strdup( value ) /* y_string */,
				dictionary_index - 1 /* datatype_number */ );
		}

		pclose( input_pipe );
	}

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

	grace_move_legend_bottom_left(
				grace_graph,
				grace->landscape_mode );

	if ( list_length( grace_graph->datatype_list ) > 9 )
		legend_char_size_increase_amount = 0.0;
	else
	if ( list_length( grace_graph->datatype_list ) > 5 )
		legend_char_size_increase_amount = 0.3;
	else
		legend_char_size_increase_amount = 0.5;

	grace_increase_legend_char_size(
				grace_graph,
				legend_char_size_increase_amount );

	grace_move_legend_up(	&grace_graph->legend_view_y,
				0.1 /* up_increment */ );

	grace->xaxis_ticklabel_precision = 0;

/*
	grace_set_world_max_x_based_on_grace_point_high(
				&grace->world_max_x,
				grace->graph_list );

	grace_set_world_min_x_based_on_grace_point_low(
				&grace->world_min_x,
				grace->graph_list );
*/
	grace->world_min_x = 0;
	grace->world_max_x = 100;

	grace_graph->x_tick_major = 10;
	grace_graph->x_tick_minor = 4;

	grace_datatype_list_set_datatype_type_bar_xy_xyhilo(
		grace->graph_list,
		"xy" );

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
		document_basic_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<h3>No data for selected parameters.</h3>\n" );
		document_close();
		exit( 0 );
	}

	grace_output_graph_window(
		application_name,
		ftp_output_filename,
		ftp_agr_filename,
		(char *)0 /* appaserver_mount_point */,
		0 /* not with_document_output */,
		"" /* where_clause */ );

	document_close();

} /* output_exceedance_gracechart() */

void output_exceedance_spreadsheet(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *document_root_directory,
			char *login_name,
			char *process_name )
{
	int dictionary_index;
	enum aggregate_level aggregate_level;
	int percent_below_piece = 0;
	char *where_clause = {0};
	char *title_heading_string;
	char title[ 128 ];
	char *sub_title;
	char *input_sys_string;
	FILE *output_file;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *table_heading_string;
	char *output_filename;
	char *ftp_filename;
	char buffer[ 1024 ];
	pid_t process_id = getpid();
	char dictionary_index_string[ 8 ];
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
	 		process_generic_output->
				value_folder->
				value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				dictionary,
				"aggregate_level" ) );

	document_basic_output_body(	application_name,
					appaserver_mount_point );

	format_initial_capital( title, process_name ),

	sub_title = get_report_sub_title(
				begin_date,
				end_date,
				aggregate_level );

	printf( "<h1>%s Transmission<br></h1>\n",
		format_initial_capital(
			buffer,
			process_generic_output->
			value_folder->value_folder_name ) );

	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	for(	dictionary_index = 1;
		dictionary_index <= highest_index;
		dictionary_index++ )
	{
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
						primary_key_list,
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
			dictionary,
			dictionary_index );

		if ( !process_generic_output->value_folder->datatype )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get value_folder->datatype.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

		sprintf( dictionary_index_string, "%d", dictionary_index );
		appaserver_link->end_date_string = dictionary_index_string;

		output_filename =
			appaserver_link__output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		ftp_filename =
			appaserver_link_prompt_filename(
				appaserver_link_prompt_link_half(
					appaserver_link->prepend_http,
					appaserver_link->http_prefix,
					appaserver_link->server_address ),
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension );

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot open %s for write.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 output_filename );
			exit( 1 );
		}

		title_heading_string =
			process_generic_output_get_datatype_heading_string(
				process_generic_output->
					value_folder->foreign_folder,
				process_generic_output->
					value_folder->datatype,
				HEADING_DELIMITER );

		fprintf( output_file, "%s\n", title_heading_string );

		fprintf( output_file, "%s\n", sub_title );

		table_heading_string =
			get_heading_string(
				aggregate_level,
				process_generic_output->
					value_folder->
					time_attribute_name );

		fprintf( output_file, "%s\n", table_heading_string );

		input_sys_string =
		process_generic_output_get_row_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary,
			login_name,
			dictionary_index );

		input_pipe = popen( input_sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_character(
				input_buffer,
				FOLDER_DATA_DELIMITER,
				',' );

			fprintf( output_file, "%s\n", input_buffer );
		}

		fclose( output_file );
		pclose( input_pipe );

		printf( "<p>%s\n", title_heading_string );
		appaserver_library_output_ftp_prompt(
				ftp_filename, 
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	document_close();

} /* output_exceedance_spreadsheet() */

void output_exceedance_text_file(
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			int highest_index,
			char *begin_date,
			char *end_date,
			DICTIONARY *dictionary,
			char *appaserver_mount_point,
			char *document_root_directory,
			char *login_name,
			char *process_name )
{
	int dictionary_index;
	enum aggregate_level aggregate_level;
	int percent_below_piece = 0;
	char *where_clause = {0};
	char *title_heading_string;
	char title[ 128 ];
	char *sub_title;
	char *input_sys_string;
	FILE *output_file;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *table_heading_string;
	char *output_filename;
	char *ftp_filename;
	char buffer[ 1024 ];
	pid_t process_id = getpid();
	char dictionary_index_string[ 8 ];
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
	 		process_generic_output->
				value_folder->
				value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				dictionary,
				"aggregate_level" ) );

	document_basic_output_body(	application_name,
					appaserver_mount_point );

	format_initial_capital( title, process_name ),

	sub_title = get_report_sub_title(
				begin_date,
				end_date,
				aggregate_level );

	printf( "<h1>%s Transmission<br></h1>\n",
		format_initial_capital(
			buffer,
			process_generic_output->
			value_folder->value_folder_name ) );

	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	for(	dictionary_index = 1;
		dictionary_index <= highest_index;
		dictionary_index++ )
	{
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
						primary_key_list,
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
			dictionary,
			dictionary_index );

		if ( !process_generic_output->value_folder->datatype )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get value_folder->datatype.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

		sprintf( dictionary_index_string, "%d", dictionary_index );
		appaserver_link->end_date_string = dictionary_index_string;

		output_filename =
			appaserver_link_output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		ftp_filename =
			appaserver_link_prompt_filename(
				appaserver_link_prompt_link_half(
					appaserver_link->prepend_http,
					appaserver_link->http_prefix,
					appaserver_link->server_address ),
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension );

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot open %s for write.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 output_filename );
			exit( 1 );
		}

		title_heading_string =
			process_generic_output_get_datatype_heading_string(
				process_generic_output->
					value_folder->foreign_folder,
				process_generic_output->
					value_folder->datatype,
				HEADING_DELIMITER );

		fprintf( output_file, "%s\n", title_heading_string );

		fprintf( output_file, "%s\n", sub_title );

		table_heading_string =
			get_heading_string(
				aggregate_level,
				process_generic_output->
					value_folder->
					time_attribute_name );

		fprintf( output_file, "%s\n", table_heading_string );

		input_sys_string =
		process_generic_output_get_row_exceedance_stdout_sys_string(
			&begin_date,
			&end_date,
			&percent_below_piece,
			&where_clause,
			application_name,
			process_generic_output,
			dictionary,
			login_name,
			dictionary_index );

		input_pipe = popen( input_sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_character(
				input_buffer,
				FOLDER_DATA_DELIMITER,
				',' );

			fprintf( output_file, "%s\n", input_buffer );
		}

		fclose( output_file );
		pclose( input_pipe );

		printf( "<p>%s\n", title_heading_string );
		appaserver_library_output_ftp_prompt(
				ftp_filename, 
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	document_close();

} /* output_exceedance_text_file() */

char *get_heading_string(	enum aggregate_level aggregate_level,
				char *time_attribute_name )
{
	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		if ( time_attribute_name && *time_attribute_name )
			return "Value,Date,Time,CountBelow,PercentBelow";
		else
			return "Value,Date,CountBelow,PercentBelow";
	}
	else
	{
		return "Value,Date,GroupCount,CountBelow,PercentBelow";
	}

} /* get_heading_string() */

