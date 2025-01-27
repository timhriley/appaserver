/* --------------------------------------------------- 	*/
/* src_appaserver/generic_overlay_output.c	      	*/
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
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "easycharts.h"
#include "process_generic_output.h"
#include "application_constants.h"
#include "scatterplot.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define DEFAULT_COMPARE_OUTPUT			"table"
#define DELIMITER_BETWEEN_ENTITY_AND_DATATYPE	'/'

#define PROMPT_PREFIX			"compare_station|compare_datatype"
#define PDF_PROMPT			"Press to view chart."

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
boolean populate_scatter_input_chart_list_datatypes(
			LIST *input_chart_list,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			char *anchor_datatype_entity,
			char *anchor_datatype_name );

boolean populate_scatter_input_chart_list_data(
			LIST *input_chart_list,
			char *input_sys_string );

int build_sys_string(
				char **begin_date,
				char **end_date,
				char **anchor_where_clause,
				int *date_piece,
				int *time_piece,
				int *value_piece,
				char *sys_string,
				char *application_name,
				enum aggregate_level aggregate_level,
				PROCESS_GENERIC_OUTPUT *process_generic_output,
				DICTIONARY *post_dictionary );

boolean output_scatter(
			char *application_name,
			char *input_sys_string,
			char *sub_title,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			enum aggregate_level aggregate_level );

void output_covariance(	char *application_name,
			char *input_sys_string,
			enum aggregate_level aggregate_level,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char *anchor_where_clause,
			PROCESS_GENERIC_OUTPUT *process_generic_output );

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *get_grace_datatype_overlay_input_group(
			PROCESS_GENERIC_OUTPUT *process_generic_output );

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	char *text_output_filename = {0};
	char *text_ftp_filename = {0};
	FILE *text_output_pipe = {0};
	char ftp_output_sys_string[ 128 ];
	char end_date_suffix[ 128 ];
	DICTIONARY *post_dictionary;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *process_set_name;
	char *process_name;
	enum aggregate_level aggregate_level;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *begin_date = {0};
	char *end_date = {0};
	char sys_string[ 65536 ];
	char *compare_output = {0};
	char title[ 256 ];
	int process_id = getpid();
	char sub_title[ 128 ];
	HTML_TABLE *html_table = {0};
	char title_buffer[ 128 ];
	LIST *heading_list;
	char *anchor_where_clause;
	FILE *input_pipe;
	char input_buffer[ 4096 ];
	char shade_compare_buffer[ 512 ];
	char *foreign_attribute_data_list_string;
	int str_len;
	char buffer[ 4096 ];
	char *ftp_agr_filename = {0};
	char *ftp_output_filename = {0};
	char *output_filename = {0};
	char *agr_filename = {0};
	char *postscript_filename = {0};
	GRACE *grace = {0};
	char graph_identifier[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	int date_piece = 0;
	int time_piece = 0;
	int value_piece = 0;
	int chart_value_piece = 0;
	char folder_data_delimiter_string[ 2 ];
	APPASERVER_LINK *appaserver_link;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s ignored role process_set dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	role_name = argv[ 2 ];
	process_set_name = argv[ 3 ];

	sprintf( folder_data_delimiter_string, "%c", FOLDER_DATA_DELIMITER );

	post_dictionary =
		dictionary_string2dictionary( argv[ 4 ] );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	dictionary_get_index_data(
				&process_name,
				post_dictionary,
				"process",
				0 );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	if ( !*process_set_name
	||   strcmp( process_set_name, "process_set" ) == 0
	||   strcmp( process_set_name, "$process_set" ) == 0 )
	{
		process_generic_output =
			process_generic_output_new(
				application_name,
				process_name,
				(char *)0 /* process_set_name */,
				0 /* accumulate_flag */ );
	}
	else
	{
		process_generic_output =
			process_generic_output_new(
				application_name,
				(char *)0 /* process_name */,
				process_set_name,
				0 /* accumulate_flag */ );
	}

	appaserver_parameter_file = new_appaserver_parameter_file();

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
			post_dictionary,
			0 /* dictionary_index */ );

	if ( !process_generic_output->value_folder->datatype )
	{
		document_basic_output_body(
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

	foreign_attribute_data_list_string =
		list2string_delimited(	process_generic_output->
					value_folder->
					datatype->
					foreign_attribute_data_list,
					' ' );

	str_len = strlen( foreign_attribute_data_list_string );

	process_generic_output->value_folder->compare_datatype_list =
		process_generic_get_compare_datatype_list(
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
			post_dictionary,
			PROMPT_PREFIX,
			0 /* not with_select_data */,
			aggregate_level,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* value_folder_name */,
			(char *)0 /* date_attribute_name */,
			(char *)0 /* time_attribute_name */,
			(char *)0 /* value_attribute_name */,
			process_generic_output->accumulate );

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
			post_dictionary
				/* query_removed_post_dictionary */ ) )
	{
		document_basic_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	dictionary_get_index_data( 	&compare_output, 
					post_dictionary, 
					"compare_output", 
					0 );

	if ( !compare_output
	||   !*compare_output
	||   strcmp( compare_output, "compare_output" ) == 0 )
	{
		compare_output = DEFAULT_COMPARE_OUTPUT;
	}

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		sprintf(title, "Compare %s",
			list_display_delimited(
				process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
				' ' ) );
	}
	else
	{
		sprintf(title, "Compare %s %s values",
			list_display_delimited(
				process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
				' ' ),
			aggregate_level_get_string( aggregate_level ) );
	}

	if ( !list_length(	process_generic_output->
				value_folder->
				compare_datatype_list ) )
	{
		document_basic_output_body(
					application_name,
					appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<h3>ERROR: There are no datatypes selected.</h3>" );
		document_close();
		exit( 1 );
	}

	if ( !build_sys_string(
		&begin_date,
		&end_date,
		&anchor_where_clause,
		&date_piece,
		&time_piece,
		&value_piece,
		sys_string,
		application_name,
		aggregate_level,
		process_generic_output,
		post_dictionary ) )
	{
		document_basic_output_body(
					application_name,
					appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<h3>ERROR: Insufficient input.</h3>" );
		document_close();
		exit( 1 );
	}

	sprintf(sub_title,
		"%s Begin: %s End: %s",
		process_generic_output->value_folder->value_folder_name,
	 	begin_date,
	 	end_date );

	format_initial_capital( sub_title, sub_title );

	if ( strcmp( compare_output, "covariance" ) == 0
	||   strcmp( compare_output, "correlation" ) == 0 )
	{
		output_covariance(	application_name,
					sys_string,
					aggregate_level,
					appaserver_parameter_file,
					anchor_where_clause,
					process_generic_output );
		exit( 0 );
	}
	else
	if ( strcmp( compare_output, "scatter" ) == 0 )
	{
		if ( !output_scatter(
					application_name,
					sys_string,
					sub_title,
					appaserver_parameter_file,
					process_generic_output,
					aggregate_level ) )
		{
			document_basic_output_body(
					application_name,
					appaserver_parameter_file->
					appaserver_mount_point );

			printf( "<h3>ERROR: Insufficient input.</h3>" );
			document_close();
			exit( 1 );
		}
		exit( 0 );
	}
	else
	if ( strcmp( compare_output, "chart" ) == 0 )
	{
		grace = grace_new_datatype_overlay_grace(
				application_name,
				role_name );

		if ( ! grace_set_begin_end_date( 	grace,
							begin_date,
							end_date ) )
		{
			document_basic_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
			document_close();
			exit( 1 );
		}

		grace->title = 
			strdup( format_initial_capital( buffer, title ) );

		grace->sub_title = strdup( sub_title );

		if ( ! ( grace->grace_datatype_overlay_input_group =
				get_grace_datatype_overlay_input_group(
					process_generic_output ) ) )
		{
			document_basic_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<h3>Insufficient input</h3>\n" );
			document_close();
			exit( 1 );
		}

		grace_populate_datatype_overlay_graph_list(
				grace->graph_list,
				grace->anchor_graph_list,
				grace->grace_datatype_overlay_input_group->
					anchor_datatype_overlay_input,
				grace->grace_datatype_overlay_input_group->
					compare_datatype_overlay_input_list );

		grace->grace_output =
			application_grace_output(
				application_name );

		sprintf( graph_identifier, "%d", getpid() );

		grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			chart_value_piece = value_piece - 2;
		}
		else
		{
			chart_value_piece = value_piece - 1;
		}
	}
	else
	if ( strcmp( compare_output, "table" ) == 0 )
	{
		document_basic_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

	 	html_table = new_html_table( 
				format_initial_capital(
					title_buffer, title ),
				(char *)0 /* sub_title */ );
	
		heading_list = process_generic_output_get_concat_heading_list(
			process_generic_output->value_folder,
			aggregate_level );

		html_table->number_left_justified_columns = 4;
		html_table->number_right_justified_columns = 1;

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			list_append_pointer( heading_list, "Count" );
			html_table->number_right_justified_columns++;
		}
	
		html_table_set_heading_list( html_table, heading_list );
		html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );

		printf( "<br>Search criteria: %s",
			 query_get_display_where_clause(
				anchor_where_clause,
				application_name,
				process_generic_output->
					value_folder->value_folder_name,
				application_is_primary_application(
					application_name ) ) );

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}
	else
	if ( strcmp( compare_output, "spreadsheet" ) == 0 )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
	 			process_generic_output->
					value_folder->
					value_folder_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" );

		if ( end_date && *end_date )
			sprintf( end_date_suffix, "_%s", end_date );
		else
			*end_date_suffix = '\0';

		appaserver_link->begin_date_string = begin_date;
		appaserver_link->end_date_string = end_date_suffix;

		text_output_filename =
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

		text_ftp_filename =
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

		sprintf(ftp_output_sys_string, "cat > %s",
			text_output_filename );

		text_output_pipe = popen( ftp_output_sys_string, "w" );

		heading_list = process_generic_output_get_concat_heading_list(
			process_generic_output->value_folder,
			aggregate_level );

		fprintf(text_output_pipe,
			"%s",
			list_display( heading_list ) );

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			fprintf(text_output_pipe, ",count" );
		}
	
		fprintf(text_output_pipe, "\n" );
	}
	else
	if ( strcmp( compare_output, "text_file" ) == 0 )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
	 			process_generic_output->
					value_folder->
					value_folder_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"txt" );

		if ( end_date && *end_date )
			sprintf( end_date_suffix, "_%s", end_date );
		else
			*end_date_suffix = '\0';

		appaserver_link->begin_date_string = begin_date;
		appaserver_link->end_date_string = end_date_suffix;

		text_output_filename =
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

		text_ftp_filename =
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

/*
		sprintf(ftp_output_sys_string,
		 	"tr ',' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	text_output_filename );
*/
		sprintf(ftp_output_sys_string,
		 	"tr '%c' '%c' > %s",
			FOLDER_DATA_DELIMITER,
			OUTPUT_TEXT_FILE_DELIMITER,
		 	text_output_filename );

		text_output_pipe = popen( ftp_output_sys_string, "w" );

		heading_list = process_generic_output_get_concat_heading_list(
			process_generic_output->value_folder,
			aggregate_level );

		fprintf(text_output_pipe,
			"%s",
			list_display_delimited(
				heading_list,
				FOLDER_DATA_DELIMITER ) );

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			fprintf(text_output_pipe, ",count" );
		}
	
		fprintf(text_output_pipe, "\n" );
	}
	else
	if ( strcmp( compare_output, "stdout" ) == 0 )
	{
		strcpy(ftp_output_sys_string, "cat " );

		text_output_pipe = popen( ftp_output_sys_string, "w" );

		heading_list = process_generic_output_get_concat_heading_list(
			process_generic_output->value_folder,
			aggregate_level );

		fprintf(text_output_pipe,
			"%s",
			list_display( heading_list ) );

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			fprintf(text_output_pipe, ",count" );
		}
	
		fprintf(text_output_pipe, "\n" );
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid compare_output = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
		      compare_output );
		exit( 1 );
	}

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( strcmp( compare_output, "chart" ) == 0 )
		{
			if ( !grace_set_string_to_point_list(
				grace->anchor_graph_list,
				0 /* datatype_entity_piece */,
				1 /* datatype_piece */,
				2 /* date_piece */,
				-1,
				chart_value_piece,
				input_buffer,
				grace->grace_graph_type,
				0 /* not datatype_type_xyhilo */,
				1 /* dataset_no_cycle_color */,
				(char *)0 /* optional_label */ ) )
			{
				grace_set_string_to_point_list(
					grace->graph_list,
					0 /* datatype_entity_piece */,
					1 /* datatype_piece */,
					2 /* date_piece */,
					-1,
					chart_value_piece,
					input_buffer,
					grace->grace_graph_type,
					0 /* not datatype_type_xyhilo */,
					1 /* dataset_no_cycle_color */,
					(char *)0 /* optional_label */ );
			}
		}
		else
		if ( strcmp( compare_output, "table" ) == 0 )
		{
			strcpy( shade_compare_buffer, input_buffer );
			search_replace_string(
				shade_compare_buffer,
				folder_data_delimiter_string,
				" " );

			if ( strncmp(
				shade_compare_buffer,
				foreign_attribute_data_list_string,
				str_len ) == 0 )
			{
				html_table_set_background_shaded(
							html_table );
			}
			else
			{
				html_table_set_background_unshaded(
							html_table );
			}

			html_table->data_list =
				list_string2list(
					input_buffer,
					FOLDER_DATA_DELIMITER );

			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new_list();
		}
		else
		if ( strcmp( compare_output, "spreadsheet" ) == 0 )
		{
			search_replace_string(
				input_buffer,
				"^",
				"\",\"" );
			fprintf( text_output_pipe, "\"%s\"\n", input_buffer );
		}
		else
		if ( strcmp( compare_output, "text_file" ) == 0 )
		{
/*
			search_replace_string(
				input_buffer,
				"^",
				"," );
*/
			fprintf( text_output_pipe, "%s\n", input_buffer );
		}
		else
		if ( strcmp( compare_output, "stdout" ) == 0 )
		{
			fprintf( text_output_pipe, "%s\n", input_buffer );
		}

	} /* while( get_line() ) */

	pclose( input_pipe );

	if ( strcmp( compare_output, "spreadsheet" ) == 0
	||   strcmp( compare_output, "text_file" ) == 0
	||   strcmp( compare_output, "stdout" ) == 0 )
	{
		pclose( text_output_pipe );
	}

	if ( strcmp( compare_output, "chart" ) == 0 )
	{
		grace_graph_set_no_yaxis_grid_lines( grace->graph_list );
		grace->dataset_no_cycle_color = 1;

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
			document_basic_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<p>Warning: no graphs to display.\n" );
			document_close();
			exit( 0 );
		}

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
					appaserver_parameter_file->
						appaserver_mount_point );

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
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
			 	query_get_display_where_clause(
					anchor_where_clause,
					application_name,
					process_generic_output->
						value_folder->
							value_folder_name,
					1 ) );
		}
	}
	else
	if ( strcmp( compare_output, "table" ) == 0 )
	{
		html_table_close();
		document_close();
	}
	else
	if ( strcmp( compare_output, "spreadsheet" ) == 0
	||   strcmp( compare_output, "text_file" ) == 0 )
	{
		char buffer[ 1024 ];

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
		
		printf( "<BR><p>Search criteria: %s<hr>\n",
			search_replace_string(	anchor_where_clause,
						"1 = 1 and ",
						"" ) );

		appaserver_library_output_ftp_prompt(
				text_ftp_filename, 
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );

		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );

} /* main() */

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *get_grace_datatype_overlay_input_group(
		PROCESS_GENERIC_OUTPUT *process_generic_output )
{
	GRACE_DATATYPE_OVERLAY_INPUT_GROUP *g = {0};
	char *datatype_entity;
	char *datatype_name;
	PROCESS_GENERIC_DATATYPE *compare_datatype;

	/* Set anchor data */
	/* --------------- */
	datatype_entity =
		process_generic_get_datatype_entity(
			process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
			process_generic_output->
				value_folder->
				datatype_folder->
				primary_key_list,
			' ' /* delimiter */ );

	datatype_name =
		process_generic_get_datatype_name(
			process_generic_output->
				value_folder->
				datatype->
				primary_attribute_data_list,
			' ' /* delimiter */ );

	g = grace_new_grace_datatype_overlay_input_group(
			datatype_entity,
			datatype_name,
			process_generic_output->
				value_folder->
				datatype->
				units,
			process_generic_output->
				value_folder->
				datatype->
				bar_graph,
			process_generic_output->
				value_folder->
				datatype->
				scale_graph_zero );

	/* Set compare data */
	/* ---------------- */
	if ( !list_rewind(	process_generic_output->
				value_folder->
				compare_datatype_list ) )
	{
		return g;
	}

	do {
		compare_datatype =
			list_get_pointer(
				process_generic_output->
				value_folder->
				compare_datatype_list );

		datatype_entity =
			process_generic_get_datatype_entity(
				compare_datatype->
					foreign_attribute_data_list,
				process_generic_output->
					value_folder->
					datatype_folder->
					primary_key_list,
			' ' /* delimiter */ );

		datatype_name =
			process_generic_get_datatype_name(
				compare_datatype->
					primary_attribute_data_list,
				' ' /* delimiter */ );

		grace_set_compare_datatype_overlay_input(
			g->compare_datatype_overlay_input_list,
			datatype_entity,
			datatype_name,
			compare_datatype->units,
			compare_datatype->bar_graph,
			compare_datatype->scale_graph_zero );

	} while( list_next( process_generic_output->
				value_folder->
				compare_datatype_list ) );

	return g;

} /* get_grace_datatype_overlay_input_group() */

void output_covariance(	char *application_name,
			char *input_sys_string,
			enum aggregate_level aggregate_level,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file,
			char *anchor_where_clause,
			PROCESS_GENERIC_OUTPUT *process_generic_output )
{
	char sys_string[ 4096 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	LIST *input_list;
	char *input_record;
	char label_buffer[ 1024 ];
	char covariance_buffer[ 128 ];
	char correlation_buffer[ 128 ];
	char determination_buffer[ 128 ];
	char aggregate_level_buffer[ 128 ];
	char count_buffer[ 128 ];
	char average_buffer[ 128 ];
	char standard_deviation_buffer[ 128 ];
	HTML_TABLE *html_table = {0};
	LIST *heading_list;
	char hourly_time_round_process[ 128 ];
	char merge_real_time_with_date_process[ 128 ];
	DOCUMENT *document;
	char *anchor_datatype_entity;
	char *anchor_datatype;

	anchor_datatype_entity =
		process_generic_get_datatype_entity(
			process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
			process_generic_output->
				value_folder->
				datatype_folder->
				primary_key_list,
			' ' /* delimiter */ );

	anchor_datatype =
		process_generic_get_datatype_name(
			process_generic_output->
				value_folder->
				datatype->
				primary_attribute_data_list,
			' ' /* delimiter */ );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	/* Adjust because hourly aggregation rounds 10 minute intervals to 50 */
	/* ------------------------------------------------------------------ */
/*
	strcpy( hourly_time_round_process,
		"sed 's|\\(/[0-9][0-9]\\)\\([0-9][0-9]\\),|\\100,|'" );
*/
	strcpy( hourly_time_round_process, "cat" );

	if ( process_generic_output->
		value_folder->
		time_attribute_name
	&&   *process_generic_output->
		value_folder->
		time_attribute_name
	&& ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time ) )
	{
		sprintf( merge_real_time_with_date_process,
			 "sed 's/\\%c/:/2'",
		 	 FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( merge_real_time_with_date_process, "cat" );
	}

	/* ------------------------------------------------------------ */
	/* covariance.e receives:				        */
	/* ------------------------------------------------------------ */
	/* NSM_v462 tim C17_R27/depth^1970-01-01^1.9500			*/
	/* NSM_v462 Nsm C17_R27/depth^1970-01-01^1.7500			*/
	/* ------------------------------------------------------------ */
	sprintf( sys_string,
		 "%s							|"
		 "%s							|"
		 "search_replace_character_limited.e '%c' '%c' 1	|"
		 "%s							|"
		 "covariance.e '%s%c%s' 1 0 2 '%c'			|"
		 "cat							 ",
		 input_sys_string,
		 hourly_time_round_process,
		 FOLDER_DATA_DELIMITER,
		 DELIMITER_BETWEEN_ENTITY_AND_DATATYPE,
		 merge_real_time_with_date_process,
		 anchor_datatype_entity,
		 DELIMITER_BETWEEN_ENTITY_AND_DATATYPE,
		 anchor_datatype,
		 FOLDER_DATA_DELIMITER );

	input_list = pipe2list( sys_string );

	if ( !list_rewind( input_list ) )
	{
		printf( "<h3>ERROR: input error for sys_string = (%s)\n",
			sys_string );
		html_table_close();
		return;
	}

	sprintf( title,
		 "%s Correlation %s/%s",
		 format_initial_capital(
			aggregate_level_buffer,
			aggregate_level_get_string(
				aggregate_level ) ),
		 anchor_datatype_entity,
		 anchor_datatype );

 	html_table = new_html_table(
			format_initial_capital(
				title_buffer, title ),
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

	printf( "<br>Search criteria: %s\n",
		 query_get_display_where_clause(
			anchor_where_clause,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			1 ) );

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
					strdup( label_buffer ) );

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
	document_close();

} /* output_covariance() */

int build_sys_string(
		char **begin_date,
		char **end_date,
		char **anchor_where_clause,
		int *date_piece,
		int *time_piece,
		int *value_piece,
		char *sys_string,
		char *application_name,
		enum aggregate_level aggregate_level,
		PROCESS_GENERIC_OUTPUT *process_generic_output,
		DICTIONARY *post_dictionary )
{
	char *where_clause;
	int terminating_character;
	int first_time = 1;
	char aggregation_process[ 1024 ];
	char *optional_sort_descending_process = {0};
	PROCESS_GENERIC_DATATYPE *compare_datatype;
	static char local_where_clause[ 1024 ];
	LIST *select_list;
	LIST *follows_select_list;
	int length_select_list;

	if ( !list_rewind(	process_generic_output->
				value_folder->
				compare_datatype_list ) )
	{
		return 0;
	}

	select_list =
		process_generic_output_get_select_list(
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			date_piece,
			time_piece,
			value_piece,
			&length_select_list,
			process_generic_output->value_folder,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */ );

	list_set_position_string(
			select_list,
			"'aaaa'",
			list_length( select_list ) - 1 /* position */ );

	follows_select_list =
		process_generic_output_get_select_list(
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			date_piece,
			time_piece,
			value_piece,
			&length_select_list,
			process_generic_output->value_folder,
			1 /* concat_datatype_entity */,
			1 /* concat_datatype */ );

	list_set_position_string(
			follows_select_list,
			"'zzzz'",
			list_length( follows_select_list ) - 1 /* position */ );

	(*value_piece)++;

	if ( ! ( where_clause =
		   process_generic_output_get_foreign_folder_where_clause(
			begin_date,
			end_date,
			process_generic_output->
				value_folder->
				foreign_folder->
				foreign_attribute_name_list,
			process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
			post_dictionary,
	       		application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name ) ) )
	{
		return 0;
	}

	/* process_generic_output_get_where_clause() returns static memory */
	/* --------------------------------------------------------------- */
	strcpy( local_where_clause, where_clause );
	*anchor_where_clause = local_where_clause;

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		strcpy( aggregation_process, "cat" );
	}
	else
	{
		sprintf( aggregation_process, 
			 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
			 aggregate_statistic_get_string(
				process_generic_output_get_aggregate_statistic(
					process_generic_output->
					value_folder->
					datatype->
					aggregation_sum ) ),
			 *date_piece,
			 *time_piece,
			 *value_piece,
			 FOLDER_DATA_DELIMITER,
			 aggregate_level_get_string( aggregate_level ),
			 *end_date );
	}

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		optional_sort_descending_process = "cat";
	}
	else
	{
		optional_sort_descending_process = "sort -r";
	}

/*
	"tr ',' '/'							 |"
*/
	sys_string += sprintf( sys_string,
	"(								  "
	"get_folder_data	application=%s				  "
	"			folder=%s				  "
	"			select=\"%s\"				  "
	"			where=\"%s\"				 |"
	"%s								 |"
	"%s								 |"
	"sort								 ;",
		application_name,
		process_generic_output->value_folder->value_folder_name,
		list_display_delimited( select_list, ',' ),
		where_clause,
		optional_sort_descending_process,
		aggregation_process );

	do {
		compare_datatype =
			list_get_pointer( 
				process_generic_output->
					value_folder->
					compare_datatype_list );

		if ( ! ( where_clause =
		   process_generic_output_get_foreign_folder_where_clause(
			begin_date,
			end_date,
			process_generic_output->
				value_folder->
				foreign_folder->
				foreign_attribute_name_list,
			compare_datatype->
				foreign_attribute_data_list,
			post_dictionary,
	       		application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
		 	process_generic_output->
				value_folder->
				date_attribute_name ) ) )
		{
			return 0;
		}

		if ( first_time )
		{
			first_time = 0;
			terminating_character = ' ';
		}
		else
		{
			terminating_character = ';';
		}

		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time )
		{
			strcpy( aggregation_process, "cat" );
		}
		else
		{
			sprintf(
			aggregation_process, 
		 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
		 	aggregate_statistic_get_string(
			process_generic_output_get_aggregate_statistic(
				compare_datatype->aggregation_sum ) ),
		 	*date_piece,
		 	*time_piece,
		 	*value_piece,
			FOLDER_DATA_DELIMITER,
		 	aggregate_level_get_string( aggregate_level ),
			*end_date );
		}

/*
		"tr ',' '/'					   |"
*/
		sys_string += sprintf( sys_string,
		"%cget_folder_data	application=%s		    "
		"			folder=%s		    "
		"			select=\"%s\"		    "
		"			where=\"%s\"		   |"
		"%s	 					   |"
		"%s	 					   |"
		"sort						    ",
			terminating_character,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			list_display_delimited( follows_select_list, ',' ),
			where_clause,
			optional_sort_descending_process,
			aggregation_process );

	} while( list_next( process_generic_output->
				value_folder->
				compare_datatype_list ) );

	if ( process_generic_output->
		value_folder->
		time_attribute_name
	&&    *process_generic_output->
		value_folder->
		time_attribute_name
	&& ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time ) )
	{
		sprintf(	sys_string,
			") | sort --key=%d,%d --field-separator='%c' |"
			"piece_inverse.e %d '%c'",
				*date_piece + 1 /* date */,
				*date_piece + 3 /* sort_label */,
				FOLDER_DATA_DELIMITER,
				*date_piece + 2 /* sort_label */,
				FOLDER_DATA_DELIMITER );
	}
	else
	if ( process_generic_output->
		value_folder->
		time_attribute_name
	&&    *process_generic_output->
		value_folder->
		time_attribute_name
	&&   aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf(	sys_string,
				") | sort --key=%d,%d --field-separator='%c' |"
				"piece_inverse.e %d '%c'		     |"
				"piece_inverse.e %d '%c'		      ",
				*date_piece + 1 /* date */,
				*date_piece + 3 /* sort_label */,
				FOLDER_DATA_DELIMITER,
				*date_piece + 2 /* sort_label */,
				FOLDER_DATA_DELIMITER,
				*date_piece + 1 /* time */,
				FOLDER_DATA_DELIMITER );
	}
	else
	{
		sprintf(	sys_string,
				") | sort --key=%d,%d --field-separator='%c' |"
				"piece_inverse.e %d '%c'		      ",
				*date_piece + 1 /* date */,
				*date_piece + 2 /* sort_label */,
				FOLDER_DATA_DELIMITER,
				*date_piece + 1 /* sort_label */,
				FOLDER_DATA_DELIMITER );
	}

	return 1;

} /* build_sys_string() */

boolean output_scatter(	char *application_name,
			char *input_sys_string,
			char *sub_title,
			APPASERVER_PARAMETER_FILE
				*appaserver_parameter_file,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			enum aggregate_level aggregate_level )
{
	char *anchor_datatype_entity;
	char *anchor_datatype_name;
	char *anchor_units;
	LIST *compare_entity_name_list;
	LIST *compare_datatype_name_list;
	LIST *compare_datatype_units_list;
	char local_sub_title[ 256 ];
	char sys_string[ 65536 ];

	anchor_datatype_entity =
		process_generic_get_datatype_entity(
			process_generic_output->
				value_folder->
				datatype->
				foreign_attribute_data_list,
			process_generic_output->
				value_folder->
				datatype_folder->
				primary_key_list,
			' ' /* delimiter */ );

	anchor_datatype_name =
		process_generic_get_datatype_name(
			process_generic_output->
				value_folder->
				datatype->
				primary_attribute_data_list,
			' ' /* delimiter */ );

	anchor_units =
		process_generic_output->
			value_folder->
			datatype->
			units;

	format_initial_capital( local_sub_title, sub_title );

	compare_entity_name_list =
		process_generic_output_get_compare_entity_name_list(
			process_generic_output->
				value_folder->
					compare_datatype_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_key_list );

	compare_datatype_name_list =
		process_generic_output_get_compare_datatype_name_list(
			process_generic_output->
				value_folder->
					compare_datatype_list );

	compare_datatype_units_list =
		process_generic_output_get_compare_datatype_units_list(
			process_generic_output->
				value_folder->
					compare_datatype_list );

	if ( process_generic_output->
		value_folder->
		time_attribute_name
	&&   *process_generic_output->
		value_folder->
		time_attribute_name
	&& ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time ) )
	{
		sprintf( sys_string,
			 "%s			|"
			 "sed 's/\\%c/:/3'	|"
			 "sed 's|\\^|/|'	 ",
			 input_sys_string,
		 	 FOLDER_DATA_DELIMITER );
	}
	else
	{
		sprintf( sys_string,
			 "%s			|"
			 "sed 's|\\^|/|'	 ",
			 input_sys_string );
	}

	scatterplot_output_scatter_plot(
				application_name,
				sys_string,
				anchor_datatype_entity,
				anchor_datatype_name,
				local_sub_title,
				appaserver_parameter_file->
					appaserver_mount_point,
				appaserver_parameter_file->
					appaserver_data_directory,
				compare_entity_name_list,
				compare_datatype_name_list,
				aggregate_level,
				anchor_units,
				compare_datatype_units_list,
				1 /* measurement_date_time_piece */,
				0 /* entity_datatype_piece */,
				2 /* value_piece */,
				'^' /* record_delimiter */,
				'/' /* entity_datatype_delimiter */ );

	return 1;
} /* output_scatter() */

