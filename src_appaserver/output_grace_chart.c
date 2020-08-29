/* $APPASERVER_HOME/src_appaserver/output_grace_chart.c			*/
/* -------------------------------------------------------------	*/
/* This process is triggered if you select the grace chart radio	*/
/* button in some of the lookup forms.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "grace.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_chart(		char *application_name,
				char *login_name,
				char *role_name,
				char *document_root_directory,
				char *appaserver_mount_point,
				char *folder_name,
				DICTIONARY *query_dictionary,
				char *time_attribute_name,
				LIST *float_integer_attribute_name_list,
				LIST *select_attribute_name_list );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FOLDER *folder;
	LIST *date_primary_attribute_name_list;
	LIST *time_primary_attribute_name_list;
	LIST *float_integer_attribute_name_list;
	char *date_attribute_name = {0};
	char *time_attribute_name = {0};
	LIST *select_attribute_name_list;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored [dictionary_stdin]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* state = argv[ 6 ]; */

	if ( argc == 8 && strcmp( argv[ 7 ], "dictionary_stdin" ) == 0 )
	{
		get_line( dictionary_string, stdin );

		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		original_post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					original_post_dictionary,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: exiting early.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}
	}
	else
	{
		dictionary_appaserver =
				dictionary_appaserver_new(
					(DICTIONARY *)0,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */ );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	date_primary_attribute_name_list =
	    attribute_list_get_primary_datatype_attribute_string_list(
		folder->attribute_list,
		"current_date,current_date_time,date,date_time,current_date" );

	if ( !list_length( date_primary_attribute_name_list ) )
		return 0;
	else
		date_attribute_name =
			list_get_first_pointer(
				date_primary_attribute_name_list );

	time_primary_attribute_name_list =
		attribute_list_get_primary_datatype_attribute_string_list(
						folder->attribute_list,
						"time,current_time" );

	if ( list_length( time_primary_attribute_name_list ) )
	{
		time_attribute_name =
			list_get_first_pointer(
				time_primary_attribute_name_list );
	}

	float_integer_attribute_name_list =
		attribute_get_time_chart_attribute_name_list(
			folder->attribute_list );

	if ( !list_length( float_integer_attribute_name_list ) )
	{
		return 0;
	}

	select_attribute_name_list = list_new();
	list_append_pointer(
			select_attribute_name_list,
			date_attribute_name );

	if ( time_attribute_name )
	{
		list_append_pointer(
				select_attribute_name_list,
				time_attribute_name );
	}

	list_append_list(
			select_attribute_name_list,
			float_integer_attribute_name_list );

	output_chart(	application_name,
			login_name,
			role_name,
			appaserver_parameter_file->document_root,
			appaserver_parameter_file->appaserver_mount_point,
			folder_name,
			dictionary_appaserver->query_dictionary,
			time_attribute_name,
			float_integer_attribute_name_list,
			select_attribute_name_list );
	return 0;
} /* main() */

void output_chart(	char *application_name,
			char *login_name,
			char *role_name,
			char *document_root_directory,
			char *appaserver_mount_point,
			char *folder_name,
			DICTIONARY *query_dictionary,
			char *time_attribute_name,
			LIST *float_integer_attribute_name_list,
			LIST *select_attribute_name_list )
{
	char *output_filename;
	char *ftp_output_filename;
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char title[ 256 ];
	char buffer[ 256 ];
	char sub_title[ 1024 ];
	char initcap_buffer[ 256 ];
	DOCUMENT *document;
	LIST *query_record_list;
	char graph_identifier[ 128 ];
	char begin_date_string[ 128 ];
	char date_data[ 128 ];
	char time_data[ 128 ];
	char value_data[ 128 ];
	char *float_integer_attribute_name;
	GRACE *grace;
	QUERY *query;
	int first_time = 1;
	int data_piece_offset;
	char *query_record;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	int value_piece_offset = 0;

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	query = query_folder_new(
			application_name,
			login_name,
			folder_name,
			query_dictionary,
			role_new( application_name, role_name ),
			(LIST *)0,
			(LIST *)0 );

	query_record_list =
		query_get_record_list(
			application_name,
			query->query_output,
			list_display( select_attribute_name_list ),
			query->query_output->order_clause );

	if ( !query_record_list || !list_length( query_record_list ) )
	{
		printf( "<h2>No data for selected parameters.</h2>\n" );
		document_close();
		exit( 0 );
	}

	sprintf(	title,
			"Chart %s", 
			format_initial_capital( initcap_buffer,
				format_plural( buffer, folder_name ) ) );

	strcpy( sub_title,
		query_get_display_where_clause(
				query->query_output->where_clause,
				application_name,
				folder_name,
				application_get_is_primary_application(
					application_name ) ) );

	if ( strcmp( sub_title, "1 = 1" ) == 0 )
		*sub_title = '\0';

	strcpy( buffer, "'" );
	search_replace_string( sub_title, buffer, "" );

	strcpy( sub_title, format_initial_capital( buffer, sub_title ) );

	grace = grace_new_date_time_grace(
				application_name,
				role_name,
				title,
				sub_title );

	sprintf( graph_identifier, "%d", getpid() );

	grace->grace_output = application_get_grace_output( application_name );

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

	list_rewind( float_integer_attribute_name_list );

	if ( time_attribute_name )
		data_piece_offset = 2;
	else
		data_piece_offset = 1;

	do {
		float_integer_attribute_name =
			list_get_pointer(
				float_integer_attribute_name_list );

		grace_date_time_set_attribute_name(
					grace->graph_list,
					float_integer_attribute_name );

		if ( !list_rewind( query_record_list ) ) break;

		do {
			query_record = list_get_pointer( query_record_list );

			piece(	date_data,
				FOLDER_DATA_DELIMITER,
				query_record,
				0 );

			if ( strlen( date_data ) == 4 )
				strcat( date_data, "-12-31" );

			if ( first_time )
			{
				strcpy( begin_date_string, date_data );
				first_time = 0;

			}

			if ( time_attribute_name )
			{
				piece(	time_data,
					FOLDER_DATA_DELIMITER,
					query_record,
					1 );
			}
			else
			{
				strcpy( time_data, "2359" );
			}

			piece(	value_data,
				FOLDER_DATA_DELIMITER,
				query_record,
				data_piece_offset + value_piece_offset );

			if ( *value_data )
			{
				grace_date_time_set_data(
						grace->graph_list,
						date_data,
						time_data,
						strdup( value_data ),
						grace->dataset_no_cycle_color );
			}

		} while( list_next( query_record_list ) );

		value_piece_offset++;

	} while( list_next( float_integer_attribute_name_list ) );

	if ( !grace_set_begin_end_date( 	grace,
						begin_date_string,
						date_data ) )
	{
		DOCUMENT *document;

		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf(
	"<h2>ERROR: Invalid date format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace_set_xaxis_ticklabel_angle(
					grace->graph_list,
					15 );

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
		DOCUMENT *document;

		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf( "<p>Warning: no graphs to display.\n" );
		document_close();
		exit( 0 );
	}

	if ( list_length( grace->graph_list ) == 1 )
	{
		grace_move_legend_bottom_left(
				list_get_first_pointer( grace->graph_list ),
				grace->landscape_mode );
/*
		grace_lower_legend( grace->graph_list, 0.08 );
*/
	}
	else
	{
		grace_lower_legend( grace->graph_list, -0.08 );
	}

	if ( grace_contains_multiple_datatypes( grace->graph_list ) )
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
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_get_ghost_script_directory(
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
					appaserver_mount_point );

		printf( "<h2>No data for selected parameters.</h2>\n" );
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
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );
	}

	document_close();

} /* output_chart() */

