/* --------------------------------------------------- 	*/
/* datatype_overlay_output.c			      	*/
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
#include "column.h"
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "station_datatype.h"
#include "scatterplot.h"
#include "session.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define DEFAULT_COMPARE_OUTPUT	"table"

#define PROCESS_NAME		"output_datatype_overlay"
#define PDF_PROMPT		"Press to view chart."

#define DATE_PIECE		 		0
#define TIME_PIECE		 		1
#define DATATYPE_ENTITY_PIECE	 		2
#define DATATYPE_PIECE		 		3
#define VALUE_PIECE		 		4

/* Prototypes */
/* ---------- */
void piece_input_buffer(
			char *input_station,
			char *input_datatype,
			char *measurement_date,
			char *measurement_time,
			char *value_string,
			char *count_string,
			char *input_buffer,
			enum aggregate_level aggregate_level,
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece,
			int aggregate_count_piece );

LIST *get_compare_datatype_units_list(
			char *application_name,
			LIST *compare_datatype_name_list );

LIST *get_station_datatype_group_members_datatype_name_list(
			LIST **station_name_list,
			char **anchor_station,
			char **anchor_datatype,
			char *application_name,
			char *station_datatype_group,
			char *login_name,
			char *appaserver_mount_point );

LIST *get_compare_datatype_name_list(
			LIST **compare_station_name_list,
			char *compare_station,
			char *application_name,
			DICTIONARY *parameter_dictionary,
			char *appaserver_mount_point );

void output_scatter_plot(
			char *application_name,
			char *input_sys_string,
			char *anchor_station,
			char *anchor_datatype,
			char *sub_title,
			char *appaserver_mount_point,
			char *appaserver_data_directory,
			LIST *compare_station_name_list,
			LIST *compare_datatype_name_list,
			enum aggregate_level aggregate_level );

void output_covariance(	char *application_name,
			char *input_sys_string,
			char *anchor_station,
			char *anchor_datatype,
			char *sub_title,
			enum aggregate_level aggregate_level,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file );

LIST *get_datatype4station_list(char *application_name,
				char *login_name,
				char *station,
				char *appaserver_mount_point,
				char *relative_source_directory,
				char *compare_output );

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *get_grace_datatype_overlay_input_group(
				char *application_name,
				char *anchor_station,
				char *anchor_datatype,
				LIST *compare_station_name_list,
				LIST *compare_datatype_name_list );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *role_name;
	char *anchor_station = {0};
	char *anchor_datatype = {0};
	char *parameter_dictionary_string;
	char *compare_station = {0};
	LIST *compare_station_name_list = {0};
	LIST *compare_datatype_name_list = {0};
	char *begin_date, *end_date;
	char input_station[ 1024 ];
	char input_datatype[ 1024 ];
	char input_buffer[ 4096 ];
	char *text_output_filename;
	char ftp_output_sys_string[ 256 ];
	FILE *text_output_pipe = {0};
	char *text_ftp_filename = {0};
	char end_date_suffix[ 128 ];
	char buffer[ 4096 ];
	char sys_string[ 1024 ];
	DOCUMENT *document;
	char *compare_output = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *input_pipe;
	HTML_TABLE *html_table = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	LIST *heading_list;
	char *ftp_agr_filename;
	char *pdf_output_filename;
	char *output_filename;
	char *agr_filename;
	char *postscript_filename;
	GRACE *grace = {0};
	DICTIONARY *parameter_dictionary;
	char *aggregate_level_string;
	int datatype_entity_piece;
	int datatype_piece;
	int date_piece;
	int time_piece;
	int value_piece;
	int aggregate_count_piece;
	char graph_identifier[ 128 ];
	enum aggregate_level aggregate_level;
	char title[ 128 ], sub_title[ 128 ];
	char title_buffer[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char *new_station_datatype_group = {0};
	char *station_datatype_group = {0};
	int process_id = getpid();

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s login_name ignored application role aggregate_level parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	application_name = argv[ 3 ];
	role_name = argv[ 4 ];
	aggregate_level_string = argv[ 5 ];
	parameter_dictionary_string = argv[ 6 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_FROM_STARTING_LABEL );

	if ( dictionary_get_index_data(	&station_datatype_group, 
					parameter_dictionary, 
					"station_datatype_group", 
					0 ) != -1 )
	{
		char buffer[ 1024 ];

		/* Piece out the login name */
		/* ------------------------ */
		piece( buffer, '|', station_datatype_group, 0 );

		compare_datatype_name_list =
			get_station_datatype_group_members_datatype_name_list(
				&compare_station_name_list,
				&anchor_station,
				&anchor_datatype,
				application_name,
				buffer,
				login_name,
				appaserver_parameter_file->
					appaserver_mount_point );
	}
	else
	if ( dictionary_get_index_data(	&new_station_datatype_group, 
					parameter_dictionary, 
					"new_station_datatype_group", 
					0 ) != -1 )
	{
		dictionary_get_index_data(	&anchor_station, 
						parameter_dictionary, 
						"station", 
						0 );
		dictionary_get_index_data( 	&anchor_datatype, 
						parameter_dictionary, 
						"datatype", 
						0 );

		if ( !anchor_station || !anchor_datatype )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			printf(
		"<p>ERROR: you must select a station|datatype to save.\n" );
			document_close();
			exit( 0 );
		}
	}

	if ( !anchor_station )
	{
		dictionary_get_index_data(	&anchor_station, 
						parameter_dictionary, 
						"station", 
						0 );
		dictionary_get_index_data( 	&anchor_datatype, 
						parameter_dictionary, 
						"datatype", 
						0 );
	}

	if ( !anchor_station || !*anchor_station )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		printf( "<h3>Error: please select a Station Datatype.</h3>\n" );
		document_close();
		exit( 0 );
	}

	dictionary_get_index_data( 	&begin_date, 
					parameter_dictionary, 
					"begin_date", 
					0 );
	dictionary_get_index_data( 	&end_date, 
					parameter_dictionary, 
					"end_date", 
					0 );
	dictionary_get_index_data( 	&compare_output, 
					parameter_dictionary, 
					"compare_output", 
					0 );

	if ( !compare_output
	||   !*compare_output)
	{
		compare_output = DEFAULT_COMPARE_OUTPUT;
	}

	dictionary_get_index_data( 	&compare_station, 
					parameter_dictionary, 
					"compare_station", 
					0 );

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					anchor_station,
					anchor_datatype );

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

	if ( !compare_datatype_name_list )
	{
		compare_datatype_name_list =
			get_compare_datatype_name_list(
				&compare_station_name_list,
				compare_station,
				application_name,
				parameter_dictionary,
				appaserver_parameter_file->
					appaserver_mount_point );
	}

	aggregate_level = 
		aggregate_level_get_aggregate_level( aggregate_level_string );

	sprintf(title, "Compare %s/%s %s values",
		anchor_station,
		anchor_datatype,
		aggregate_level_get_string( aggregate_level ) );

	if ( !list_length( compare_datatype_name_list ) )
	{
		char *compare_station;

		compare_station =
			list_get_first_string(
				compare_station_name_list );

		compare_datatype_name_list =
			get_datatype4station_list(
				application_name,
				login_name,
				compare_station,
				appaserver_parameter_file->
					appaserver_mount_point,
				application_relative_source_directory(
					application_name ),
				compare_output );

		if ( !list_length( compare_datatype_name_list ) )
		{
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

			printf(
"<h2>ERROR: There are no datatypes for this station marked for charting.</h2>" );
			document_close();
			exit( 1 );
		}

		/* Prevent comparing station/datatype with itself */
		/* ---------------------------------------------- */
		if ( strcmp( compare_station, anchor_station ) == 0 )
		{
			list_remove_string(	compare_datatype_name_list,
						anchor_datatype );
		}
	
		compare_station_name_list =
			list_replicate_string_list(
				list_get_first_string(
					compare_station_name_list ),
				list_length(
					compare_datatype_name_list ) );
	}
		
	date_piece = DATE_PIECE;
	time_piece = TIME_PIECE;
	datatype_entity_piece = DATATYPE_ENTITY_PIECE;
	datatype_piece = DATATYPE_PIECE;
	value_piece = VALUE_PIECE;
	aggregate_count_piece = VALUE_PIECE + 1;

	sprintf(sub_title,
		"Begin: %s End: %s",
	 	begin_date,
	 	end_date );

	sprintf(sys_string,
"datatype_overlay %s %s %s %s %s '%s' '%s' %s",
		application_name,
		anchor_station,
		anchor_datatype,
		begin_date,
		end_date,
		list_display( compare_station_name_list ),
		list_display( compare_datatype_name_list ),
		aggregate_level_get_string( aggregate_level ) );

	if ( end_date && *end_date )
		sprintf( end_date_suffix, "_%s", end_date );
	else
		*end_date_suffix = '\0';

	if ( strcmp( compare_output, "covariance" ) == 0
	||   strcmp( compare_output, "correlation" ) == 0 )
	{
		output_covariance(	application_name,
					sys_string,
					anchor_station,
					anchor_datatype,
					sub_title,
					aggregate_level,
					appaserver_parameter_file );
		exit( 0 );
	}
	else
	if ( strcmp( compare_output, "scatter" ) == 0 )
	{
		output_scatter_plot(
					application_name,
					sys_string,
					anchor_station,
					anchor_datatype,
					sub_title,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						appaserver_data_directory,
					compare_station_name_list,
					compare_datatype_name_list,
					aggregate_level );
		exit( 0 );
	}
	else
	if ( strcmp( compare_output, "chart" ) == 0 )
	{
		grace = grace_new_datatype_overlay_grace(
				application_name,
				role_name );

		grace->dataset_no_cycle_color = 1;

		if ( ! grace_set_begin_end_date( 	grace,
							begin_date,
							end_date ) )
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

			printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
			document_close();
			exit( 1 );
		}

		grace->title = 
			strdup( format_initial_capital( buffer, title ) );

		grace->sub_title = strdup( sub_title );

		if ( ! ( grace->grace_datatype_overlay_input_group =
				get_grace_datatype_overlay_input_group(
					application_name,
					anchor_station,
					anchor_datatype,
					compare_station_name_list,
					compare_datatype_name_list ) ) )
		{
			fprintf( stderr,
			"ERROR %s/%s(): cannot extract from database\n",
				 __FILE__,
				 __FUNCTION__ );
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
			application_grace_output( application_name );

		sprintf(	graph_identifier,
				"%s_%s_%d",
				anchor_station,
				anchor_datatype,
				process_id );

		grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&pdf_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );
	}
	else
	if ( strcmp( compare_output, "table" ) == 0 )
	{
		char *station_name;
		LIST *done_station_name_list = list_new();

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

		hydrology_library_output_station_table(
			application_name,
			anchor_station );

		list_rewind( compare_station_name_list );

		list_append_pointer(	done_station_name_list,
					anchor_station );

		do {
			station_name =
				list_get_pointer(
					compare_station_name_list );

			if ( list_exists_string(
				station_name,
				done_station_name_list ) )
			{
				continue;
			}

			hydrology_library_output_station_table(
				application_name,
				station_name );

			list_append_pointer(
				done_station_name_list,
				station_name );

		} while( list_next( compare_station_name_list ) );

		sprintf( title + strlen( title ), "<br>%s", sub_title );
	 	html_table = new_html_table( 
				format_initial_capital(
					title_buffer, title ),
				(char *)0 /* sub_title */ );
	
		heading_list = new_list();

		html_table->number_left_justified_columns = 3;
		html_table->number_right_justified_columns = 2;

		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			list_append_pointer( heading_list, "Station" );
			list_append_pointer( heading_list, "Datatype" );
			list_append_pointer( heading_list, "Date" );
			list_append_pointer( heading_list, "Time" );
			list_append_pointer( heading_list, "Value" );
			html_table->number_left_justified_columns++;

			if ( aggregate_level == half_hour
			||   aggregate_level == hourly )
			{
				list_append_pointer( heading_list, "Count" );
			}
		}
		else
		{
			list_append_pointer( heading_list, "Station" );
			list_append_pointer( heading_list, "Datatype" );
			list_append_pointer( heading_list, "Date" );
			list_append_pointer( heading_list, "Value" );
			list_append_pointer( heading_list, "Count" );
		}
	
		html_table_set_heading_list( html_table, heading_list );
		html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}
	else
	if ( strcmp( compare_output, "text_file" ) == 0
	||   strcmp( compare_output, "spreadsheet" ) == 0
	||   strcmp( compare_output, "stdout" ) == 0 )
	{
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				PROCESS_NAME,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* extension */ );

		appaserver_link_file->begin_date_string = begin_date;
		appaserver_link_file->end_date_string = end_date_suffix;

/*
		if ( aggregate_level == real_time )
		{
			right_justified_columns_from_right = 1;
		}
		else
		{
			right_justified_columns_from_right = 2;
		}
*/

		if ( strcmp( compare_output, "text_file" ) == 0 )
		{
			FILE *output_file;
			char *station_name;
			LIST *done_station_name_list = list_new();

			document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

			appaserver_link_file->extension = "txt";

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

			if ( ! ( output_file =
					fopen( text_output_filename, "w" ) ) )
			{
				printf(
				"<H2>ERROR: Cannot open output file %s\n",
					text_output_filename );
				document_close();
				exit( 1 );
			}
			else
			{
				fclose( output_file );
			}

			hydrology_library_output_station_text_filename(
				text_output_filename,
				application_name,
				anchor_station,
				0 /* not with_zap_file */ );

			list_rewind( compare_station_name_list );

			list_append_pointer(	done_station_name_list,
						anchor_station );

			do {
				station_name =
					list_get_pointer(
					compare_station_name_list );

				if ( list_exists_string(
					station_name,
					done_station_name_list ) )
				{
					continue;
				}

				hydrology_library_output_station_text_filename(
						text_output_filename,
						application_name,
						station_name,
						0 /* not with_zap_file */ );

				list_append_pointer(
					done_station_name_list,
						station_name );

			} while( list_next( compare_station_name_list ) );

/*
			sprintf(ftp_output_sys_string,
		"tr ',' '\\t' | delimiter2padded_columns.e tab %d >> %s",
		 		right_justified_columns_from_right,
				text_output_filename );
*/
			sprintf(ftp_output_sys_string,
		 		"tr ',' '%c' >> %s",
				OUTPUT_TEXT_FILE_DELIMITER,
		 		text_output_filename );

		}
		else
		if ( strcmp( compare_output, "spreadsheet" ) == 0 )
		{
			FILE *output_file;
			char *station_name;
			LIST *done_station_name_list = list_new();

			document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

			appaserver_link_file->extension = "csv";

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

			if ( ! ( output_file =
					fopen( text_output_filename, "w" ) ) )
			{
				printf(
				"<H2>ERROR: Cannot open output file %s\n",
					text_output_filename );
				document_close();
				exit( 1 );
			}
			else
			{
				fclose( output_file );
			}

			hydrology_library_output_station_text_filename(
				text_output_filename,
				application_name,
				anchor_station,
				0 /* not with_zap_file */ );

			list_rewind( compare_station_name_list );

			list_append_pointer(	done_station_name_list,
						anchor_station );

			do {
				station_name =
					list_get_pointer(
					compare_station_name_list );

				if ( list_exists_string(
					station_name,
					done_station_name_list ) )
				{
					continue;
				}

				hydrology_library_output_station_text_filename(
						text_output_filename,
						application_name,
						station_name,
						0 /* not with_zap_file */ );

				list_append_pointer(
					done_station_name_list,
						station_name );

			} while( list_next( compare_station_name_list ) );

			sprintf(ftp_output_sys_string,
				"cat >> %s",
				text_output_filename );
		}
		else
		/* -------------- */
		/* Must be stdout */
		/* -------------- */
		{
			char *station_name;
			LIST *done_station_name_list = list_new();

			hydrology_library_output_station_text_file(
				stdout,
				application_name,
				anchor_station );

			list_rewind( compare_station_name_list );

			list_append_pointer(	done_station_name_list,
						anchor_station );

			do {
				station_name =
					list_get_pointer(
					compare_station_name_list );

				if ( list_exists_string(
					station_name,
					done_station_name_list ) )
				{
					continue;
				}

				hydrology_library_output_station_text_file(
						stdout,
						application_name,
						station_name );

				list_append_pointer(
					done_station_name_list,
						station_name );

			} while( list_next( compare_station_name_list ) );

/*
			sprintf(ftp_output_sys_string,
			"tr ',' '\\t' | delimiter2padded_columns.e tab %d",
		 		right_justified_columns_from_right );
*/
			sprintf(sys_string,
		 		"tr ',' '%c'",
				OUTPUT_TEXT_FILE_DELIMITER );
		}

		text_output_pipe = popen( ftp_output_sys_string, "w" );

		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			fprintf(text_output_pipe,
				"#station,datatype,date,time,value" );
		}
		else
		{
			fprintf(text_output_pipe,
				"#station,datatype,date,value,count" );
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

	/* ----------------------------------------------------------- */
	/* Sample input: "1999-01-01,1000,BD,bottom_temperature,4.00"  */
	/* Sample input: "1999-01-01,1000,BD,surface_temperature,5.00" */
	/* ----------------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( strcmp( compare_output, "chart" ) == 0 )
		{
			search_replace_string( input_buffer, ",", "|" );

			grace_set_string_to_point_list(
				grace->graph_list,
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece,
				input_buffer,
				grace->grace_graph_type,
				0 /* not datatype_type_xyhilo */,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

			grace_set_string_to_point_list(
				grace->anchor_graph_list,
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece,
				input_buffer,
				grace->grace_graph_type,
				0 /* not datatype_type_xyhilo */,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
		}
		else
		if ( strcmp( compare_output, "table" ) == 0 )
		{
			piece_input_buffer(
					input_station,
					input_datatype,
					measurement_date,
					measurement_time,
					value_string,
					count_string,
					input_buffer,
					aggregate_level,
					datatype_entity_piece,
					datatype_piece,
					date_piece,
					time_piece,
					value_piece,
					aggregate_count_piece );

			if ( strcmp( anchor_station, input_station ) == 0
			&&   strcmp( anchor_datatype, input_datatype ) == 0 )
			{
				html_table_set_background_shaded(
							html_table );
			}
			else
			{
				html_table_set_background_unshaded(
							html_table );
			}

			html_table_set_data(	html_table->data_list,
						strdup( input_station ) );

			html_table_set_data(	html_table->data_list,
						strdup( 
						format_initial_capital( 
							buffer,
							input_datatype ) ) );
	
			html_table_set_data(	html_table->data_list,
						strdup( measurement_date ) );

			if ( aggregate_level == real_time
			||   aggregate_level == half_hour
			||   aggregate_level == hourly )
			{
				html_table_set_data(
						html_table->data_list,
						strdup( measurement_time ) );
			}

			html_table_set_data(	html_table->data_list,
						strdup( value_string ) );

			if ( aggregate_level != real_time )
			{
				html_table_set_data(
						html_table->data_list,
						strdup( count_string ) );
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
		else
		{
			piece_input_buffer(
					input_station,
					input_datatype,
					measurement_date,
					measurement_time,
					value_string,
					count_string,
					input_buffer,
					aggregate_level,
					datatype_entity_piece,
					datatype_piece,
					date_piece,
					time_piece,
					value_piece,
					aggregate_count_piece );

			if ( aggregate_level == real_time
			||   aggregate_level == half_hour
			||   aggregate_level == hourly )
			{
				fprintf(text_output_pipe,
					"%s,%s,%s,%s,%s\n",
					input_station,
					input_datatype,
					measurement_date,
					measurement_time,
					value_string );
			}
			else
			{
				fprintf(text_output_pipe,
					"%s,%s,%s,%s,%s\n",
					input_station,
					input_datatype,
					measurement_date,
					value_string,
					count_string );
			}
		}

	} /* while( get_line() ) */

	pclose( input_pipe );

	if ( strcmp( compare_output, "text_file" ) == 0
	||   strcmp( compare_output, "spreadsheet" ) == 0
	||   strcmp( compare_output, "stdout" ) == 0 )
	{
		pclose( text_output_pipe );
	}

	if ( strcmp( compare_output, "chart" ) == 0 )
	{
		grace_graph_set_no_yaxis_grid_lines(
				grace->graph_list );

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
					appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<p>Warning: no graphs to display.\n" );
			document_close();
			exit( 0 );
		}

#ifdef NOT_DEFINED
		/* Move the legend down a little */
		/* ----------------------------- */
		grace_lower_legend(	grace->graph_list,
					0.04 );

		grace_lower_legend(	grace->anchor_graph_list,
					0.04 );
#endif

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
		else
		{
			grace_output_graph_window(
				application_name,
				pdf_output_filename,
				ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
	}
	else
	if ( strcmp( compare_output, "table" ) == 0 )
	{
		html_table_close();
		document_close();
	}
	else
	if ( strcmp( compare_output, "text_file" ) == 0
	||   strcmp( compare_output, "spreadsheet" ) == 0 )
	{
		printf( "<h1>Measurement Transmission<br></h1>\n" );

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

		document_close();
	}

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
}

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *get_grace_datatype_overlay_input_group(
				char *application_name,
				char *anchor_station,
				char *anchor_datatype,
				LIST *compare_station_name_list,
				LIST *compare_datatype_name_list )
{
	GRACE_DATATYPE_OVERLAY_INPUT_GROUP *g;
	STATION_DATATYPE *station_datatype;
	LIST *station_datatype_list;

	/* Set anchor data */
	/* --------------- */
	station_datatype =
		station_datatype_get_station_datatype(
				application_name,
				anchor_station,
				anchor_datatype );

	if ( ! station_datatype )
	{
		fprintf( stderr,
		"ERROR %s/%s(): cannot get anchor station datatype (%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 anchor_station,
			 anchor_datatype );
		return (GRACE_DATATYPE_OVERLAY_INPUT_GROUP *)0;
	}

	g = grace_new_grace_datatype_overlay_input_group(
			anchor_station,
			anchor_datatype,
			station_datatype->units,
			(station_datatype->bar_graph_yn == 'y'),
			(station_datatype->scale_graph_zero_yn == 'y') );

	/* Set compare data */
	/* ---------------- */
	if ( !list_length( compare_datatype_name_list ) ) return g;

	if ( ! ( station_datatype_list =
		station_datatype_get_station_datatype_list(
					application_name,
					compare_station_name_list,
					compare_datatype_name_list ) ) )
	{
		fprintf( stderr,
		"ERROR %s/%s(): cannot get station datatype (%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 list_display( compare_station_name_list ),
			 list_display( compare_datatype_name_list ) );
		return (GRACE_DATATYPE_OVERLAY_INPUT_GROUP *)0;
	}

	list_rewind( station_datatype_list );

	do {
		station_datatype = (STATION_DATATYPE *)
			list_get_pointer( station_datatype_list );

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: empty datatype for station = %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name );
			continue;
		}

		grace_set_compare_datatype_overlay_input(
			g->compare_datatype_overlay_input_list,
			station_datatype->station_name,
			station_datatype->datatype->datatype_name,
			station_datatype->units,
			(station_datatype->bar_graph_yn == 'y'),
			(station_datatype->scale_graph_zero_yn == 'y') );

	} while( list_next( station_datatype_list ) );

	return g;
}

LIST *get_datatype4station_list(char *application_name,
				char *login_name,
				char *station,
				char *appaserver_mount_point,
				char *relative_source_directory,
				char *compare_output )
{
	char sys_string[ 1024 ];

	if ( strcmp( compare_output, "chart" ) == 0 )
	{
		sprintf( sys_string,
		 	"%s/%s/datatype_chart4station_list.sh %s %s %s",
		 	appaserver_mount_point,
		 	relative_source_directory,
		 	application_name,
		 	login_name,
		 	station );
	}
	else
	{
		sprintf( sys_string,
		 	"%s/%s/datatype4station_list.sh %s %s %s",
		 	appaserver_mount_point,
		 	relative_source_directory,
		 	application_name,
		 	login_name,
		 	station );
	}
	return pipe2list( sys_string );
}

void output_covariance(	char *application_name,
			char *input_sys_string,
			char *anchor_station,
			char *anchor_datatype,
			char *sub_title,
			enum aggregate_level aggregate_level,
			APPASERVER_PARAMETER_FILE *appaserver_parameter_file )
{
	char sys_string[ 1024 ];
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
	DOCUMENT *document;
	char hourly_time_round_process[ 128 ];

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

	/* ----------------------------------------------------------- */
	/* covariance.e receives:				       */
	/* ----------------------------------------------------------- */
	/* Sample input: "1999-01-01/1000,BD/bottom_temperature,4.00"  */
	/* Sample input: "1999-01-01/1000,BD/surface_temperature,5.00" */
	/* ----------------------------------------------------------- */
	sprintf( sys_string,
		 "%s							|"
		 "sed 's|,|/|1'						|"
		 "sed 's|,|/|2'						|"
		 "%s							|"
		 "covariance.e '%s/%s' 0 1 2 ','			|"
		 "cat							 ",
		 input_sys_string,
		 hourly_time_round_process,
		 anchor_station,
		 anchor_datatype );

/*
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
m2( "hydrology", msg );
}
*/
	input_list = pipe2list( sys_string );

	if ( !list_rewind( input_list ) )
	{
		printf( "<h2>ERROR: input error for sys_string = (%s)\n",
			sys_string );
		html_table_close();
		return;
	}

	sprintf( title,
		 "%s Correlation %s/%s<br>%s",
		 format_initial_capital(
			aggregate_level_buffer,
			aggregate_level_get_string(
				aggregate_level ) ),
		 anchor_station,
		 anchor_datatype,
		 sub_title );

 	html_table = new_html_table(
			format_initial_capital(
				title_buffer, title ),
			(char *)0 /* sub_title */ );

	heading_list = new_list();

	list_append_pointer( heading_list, "Station/Datatype" );
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

}

LIST *get_compare_datatype_name_list(
				LIST **compare_station_name_list,
				char *compare_station,
				char *application_name,
				DICTIONARY *parameter_dictionary,
				char *appaserver_mount_point )
{
	LIST *compare_datatype_name_list;

	compare_datatype_name_list =
		dictionary_get_positive_index_list( 
					parameter_dictionary,
					"compare_datatype" );

	if ( !list_length( compare_datatype_name_list ) )
	{
		if ( !compare_station || !*compare_station )
		{
			document_quick_output_body(
						application_name,
						appaserver_mount_point );

			printf(
"<h2>ERROR: you must select a station or station|datatype to compare with.</h2>" );
			document_close();
			exit( 0 );
		}

		*compare_station_name_list = list_new_list();
		list_append_pointer(
			*compare_station_name_list,
			compare_station );
	}
	else
	{
		*compare_station_name_list = 
			dictionary_get_positive_index_list(
						parameter_dictionary,
						"compare_station" );
	}

	return appaserver_library_trim_carrot_number(
			compare_datatype_name_list );

}

LIST *get_station_datatype_group_members_datatype_name_list(
				LIST **station_name_list,
				char **anchor_station,
				char **anchor_datatype,
				char *application_name,
				char *station_datatype_group,
				char *login_name,
				char *appaserver_mount_point )
{
	LIST *datatype_name_list;
	char sys_string[ 1024 ];
	char piece_buffer[ 512 ];
	char grep_process[ 512 ];
	LIST *station_datatype_record_list;
	char where_clause[ 1024 ];
	char *select = "station,datatype";
	char local_station_datatype_group[ 512 ];
	char *station_datatype_record;

	strcpy( local_station_datatype_group, station_datatype_group );
	make_single_quotes_double_single_quotes( local_station_datatype_group);

	/* First, get the anchor station|datatype */
	/* -------------------------------------- */
	sprintf( where_clause,
		 "station_datatype_group = '%s' and login_name = '%s'",
		 local_station_datatype_group,
		 login_name );

	sprintf( sys_string,
		 "get_folder_data					"
		 "		application=%s				"
		 "		select=%s				"
		 "		folder=station_datatype_group_anchors	"
		 "		where=\"%s\"				",
		 application_name,
		 select,
		 where_clause );

	if ( ! ( station_datatype_record = pipe2string( sys_string ) ) )
	{
		document_quick_output_body(
			application_name,
			appaserver_mount_point );

		printf(
"<p>ERROR: there is no anchor station|datatype for this group. Manual maintenance is required.\n" );
		document_close();
		exit( 0 );
	}

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_record,
		0 );
	*anchor_station = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_record,
		1 );
	*anchor_datatype = strdup( piece_buffer );

	/* Second, get the group members */
	/* ----------------------------- */
	sprintf( grep_process,
		 "grep -v '%s\\%c%s'",
		 *anchor_station,
		 FOLDER_DATA_DELIMITER,
		 *anchor_datatype );

	sprintf( where_clause,
		 "station_datatype_group = '%s' and			"
		 "login_name = '%s'					",
		 local_station_datatype_group,
		 login_name );

	sprintf( sys_string,
		 "get_folder_data					 "
		 "		application=%s				 "
		 "		select=%s				 "
		 "		folder=station_datatype_group_members	 "
		 "		where=\"%s\"				|"
		 "%s							 ",
		 application_name,
		 select,
		 where_clause,
		 grep_process );

	station_datatype_record_list = pipe2list( sys_string );

	if ( !list_length( station_datatype_record_list ) )
		return (LIST *)0;

	*station_name_list =
		list_usage_piece_list(	station_datatype_record_list,
					FOLDER_DATA_DELIMITER,
					0 );
	datatype_name_list =
		list_usage_piece_list(	station_datatype_record_list,
					FOLDER_DATA_DELIMITER,
					1 );

	return datatype_name_list;

}

LIST *get_compare_datatype_units_list(
			char *application_name,
			LIST *compare_datatype_name_list )
{
	LIST *compare_datatype_units_list = list_new_list();
	char sys_string[ 1024 ];
	LIST *datatype_units_list;
	char *datatype_units;
	char *compare_datatype_name;
	char units[ 128 ];
	char datatype[ 128 ];
	boolean found_it;

	if ( !list_length( compare_datatype_name_list ) )
		return compare_datatype_units_list;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=datatype,units		"
		 "			folder=datatype			",
		 application_name );

	datatype_units_list = pipe2list( sys_string );

	list_rewind( compare_datatype_name_list );
	do {
		compare_datatype_name =
			list_get_pointer(
				compare_datatype_name_list );

		found_it = 0;
		list_rewind( datatype_units_list );

		do {
			datatype_units =
				list_get_pointer(
					datatype_units_list );

			piece( datatype, '^', datatype_units, 0 );

			if ( strcmp(
				datatype,
				compare_datatype_name ) == 0 )
			{
				piece( units, '^', datatype_units, 1 );
				list_append_pointer(
					compare_datatype_units_list,
					strdup( units ) );
				found_it = 1;
				break;
			}
		} while( list_next( datatype_units_list ) );

		if ( !found_it )
		{
			list_append_pointer(
				compare_datatype_units_list,
				"" );
		}
	} while( list_next( compare_datatype_name_list ) );
	return compare_datatype_units_list;
}

void piece_input_buffer(
			char *input_station,
			char *input_datatype,
			char *measurement_date,
			char *measurement_time,
			char *value_string,
			char *count_string,
			char *input_buffer,
			enum aggregate_level aggregate_level,
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece,
			int aggregate_count_piece )
{

	piece(	measurement_date,
		',',
		input_buffer,
		date_piece );

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		piece(	measurement_time,
			',',
			input_buffer,
			time_piece );
	}

	if ( aggregate_level != real_time )
	{
		piece(	count_string,
			',',
			input_buffer,
			aggregate_count_piece );
	}

	piece(	input_station,
		',',
		input_buffer,
		datatype_entity_piece );

	piece(	input_datatype,
		',',
		input_buffer,
		datatype_piece );

	piece(	value_string,
		',',
		input_buffer,
		value_piece );

}

void output_scatter_plot(	char *application_name,
				char *input_sys_string,
				char *anchor_station,
				char *anchor_datatype,
				char *sub_title,
				char *appaserver_mount_point,
				char *appaserver_data_directory,
				LIST *compare_station_name_list,
				LIST *compare_datatype_name_list,
				enum aggregate_level aggregate_level )
{
	LIST *compare_datatype_units_list;
	char sys_string[ 1024 ];
	char *anchor_units;
	boolean bar_graph;
	char hourly_time_round_process[ 128 ];

	/* Adjust because hourly aggregation rounds 10 minute intervals to 50 */
	/* ------------------------------------------------------------------ */
	strcpy( hourly_time_round_process,
		"sed 's|\\(/[0-9][0-9]\\)\\([0-9][0-9]\\),|\\100,|'" );

	/* ----------------------------------------------------------- */
	/* Sample input: "1999-01-01/1000,BD/bottom_temperature,4.00"  */
	/* Sample input: "1999-01-01/1000,BD/surface_temperature,5.00" */
	/* ----------------------------------------------------------- */
	sprintf( sys_string,
		 "%s							|"
		 "sed 's|,|/|1'						|"
		 "sed 's|,|/|2'						|"
		 "%s							|"
		 "cat							 ",
		 input_sys_string,
		 hourly_time_round_process );

	anchor_units = hydrology_library_get_units_string(
			&bar_graph,
			application_name,
			anchor_datatype );

	compare_datatype_units_list =
		get_compare_datatype_units_list(
			application_name,
			compare_datatype_name_list );

/*
fprintf( stderr, "%s(): got sys_string = (%s)\n", __FUNCTION__, sys_string );
*/

	scatterplot_output_scatter_plot(
				application_name,
				sys_string,
				anchor_station,
				anchor_datatype,
				sub_title,
				appaserver_mount_point,
				appaserver_data_directory,
				compare_station_name_list,
				compare_datatype_name_list,
				aggregate_level,
				anchor_units,
				compare_datatype_units_list,
				0 /* measurement_date_time_piece */,
				1 /* entity_datatype_piece */,
				2 /* value_piece */,
				',' /* record_delimiter */,
				'/' /* entity_datatype_delimiter */ );

}

