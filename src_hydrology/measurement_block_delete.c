/* --------------------------------------------------- 	*/
/* measurement_block_delete.c			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "measurement.h"
#include "query.h"
#include "measurement_update_parameter.h"
#include "measurement_backup.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "application.h"
#include "julian.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */
#define MAX_DISPLAY_ROWS	1000
#define BACKUP_RECORD_DELIMITER ','
#define PROCESS_NAME		"measurement_block_delete"

/* Prototypes */
/* ---------- */
char *get_where_clause_string( 	char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *begin_time,
				char *end_date,
				char *end_time,
				DICTIONARY *parameter_dictionary );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;
	char *where_clause;
	int really_yn;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	MEASUREMENT_STRUCTURE *measurement_structure;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	DOCUMENT *document = {0};
	int html_document_ok;
	LIST *heading_list;
	int counter = 0;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	HTML_TABLE *html_table = {0};
	MEASUREMENT_BACKUP *measurement_backup = {0};
	char *notes;
	char *database_string = {0};

	if ( argc < 14 )
	{
		fprintf(stderr,
"Usage: %s person application ignored ignored station datatype begin_date begin_time end_date end_time parameter_dictionary notes really_yn [nohtml]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	/* session = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	begin_time = argv[ 8 ];
	end_date = argv[ 9 ];
	end_time = argv[ 10 ];
	parameter_dictionary_string = argv[ 11 ];
	notes = argv[ 12 ];
	really_yn = *argv[ 13 ];

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

	html_document_ok = ( argc != 15 || strcmp( argv[ 14 ], "nohtml" ) != 0);

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	if ( strcmp( begin_time, "begin_time" ) == 0 )
		begin_time = "0000";
	if ( strcmp( end_time, "end_time" ) == 0 )
		end_time = "2359";
	
	parameter_dictionary = 
		dictionary_index_string2dictionary(
				parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_index(
						parameter_dictionary );

	where_clause = get_where_clause_string( 	
				application_name,
				station,
				datatype,
				begin_date,
				begin_time,
				end_date,
				end_time,
				parameter_dictionary );

	measurement_structure =
		measurement_structure_new(
			application_name );

	measurement_set_argv_0( measurement_structure, argv[ 0 ] );

	measurement_structure->input_pipe =
		measurement_open_input_pipe(
					application_name,
					where_clause,
					BACKUP_RECORD_DELIMITER );
		
	measurement_update_parameter =
		measurement_update_parameter_new(
			application_name,
			station,
			datatype,
			DELETE_MEASUREMENT_UPDATE_METHOD,
			login_name,
			parameter_dictionary,
			notes );

	if ( html_document_ok )
	{
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
	}

	if ( html_document_ok )
	{
		html_table = new_html_table(
				"Measurement Block Delete",
				(char *)0 /* sub_title */ );
	}
	
	if ( really_yn == 'y' )
	{
		if ( html_document_ok )
			html_table_output_table_heading(
							html_table->title,
							html_table->sub_title );

		measurement_structure->delete_pipe =
			measurement_open_delete_pipe(
						application_name );

		measurement_backup = 
			new_measurement_backup(
				application_name,
				DELETE_MEASUREMENT_UPDATE_METHOD,
				login_name,
				measurement_update_parameter->now_date,
				measurement_update_parameter->now_time );

		measurement_backup->insert_pipe =
			measurement_backup_open_insert_pipe(
						application_name );
	}
	else
	{
		if ( html_document_ok )
		{
			heading_list = new_list();
			list_append_string( heading_list, "Station" );
			list_append_string( heading_list, "Datatype" );
			list_append_string( heading_list, "Measurement Date" );
			list_append_string( heading_list, "Measurement Time" );
			list_append_string( heading_list, "Measurment Value" );
	
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
	}

	while( measurement_structure_fetch(
					measurement_structure,
					measurement_structure->input_pipe ) )
	{
		counter++;

		if ( really_yn == 'y' )
		{
			measurement_backup_insert(
					measurement_backup->insert_pipe,
					measurement_backup->
						measurement_update_date,
					measurement_backup->
						measurement_update_time,
					measurement_backup->
						measurement_update_method,
					measurement_backup->login_name,
					measurement_display(
					      measurement_structure->
					      	measurement ),
					really_yn,
					BACKUP_RECORD_DELIMITER );

			measurement_delete(
					measurement_structure->delete_pipe,
					measurement_structure->measurement );
		}
		else
		if ( html_document_ok )
		{
			char buffer[ 128 ];

			if ( counter < ( MAX_DISPLAY_ROWS + 1 ) )
			{
				html_table_set_data(
					html_table->data_list,
					measurement_structure->
						measurement->
						station_name );

				html_table_set_data(
					html_table->data_list,
					measurement_structure->
						measurement->
						datatype );

				html_table_set_data(
					html_table->data_list,
					measurement_structure->
						measurement->
						measurement_date );

				html_table_set_data(
					html_table->data_list,
					measurement_structure->
						measurement->
						measurement_time );

				sprintf( buffer,
					 "%lf",
					 measurement_structure->
						measurement->
						measurement_value );

				html_table_set_data(
					html_table->data_list,
					strdup( buffer ) );

				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				html_table->data_list = list_new();
			}
			else
			if ( counter == ( MAX_DISPLAY_ROWS + 1 ) )
			{
				html_table_set_data(
					html_table->data_list,
					strdup( "Skipping" ) );

				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				html_table->data_list = list_new();
			}
		}
	}

	if ( html_document_ok ) html_table_close();

	if ( really_yn == 'y' )
	{
		char counter_string[ 128 ];

		sprintf( counter_string, "%d", counter );

		dictionary_add_string( 
			measurement_update_parameter->parameter_dictionary,
			"count",
			strdup( counter_string ) );

		measurement_update_parameter_save(
			       measurement_update_parameter );

		if ( html_document_ok )
		{
			printf( "<BR><p>Deleted %d %s measurements</p>\n",
				counter, datatype );
		}
	}
	else
	{
		if ( html_document_ok )
		{
			printf( "<BR><p>Did not delete %d measurements</p>\n",
				counter );
		}
	}

	if ( html_document_ok ) document_close();

	pclose( measurement_structure->input_pipe );

	if ( really_yn == 'y' )
	{
		pclose( measurement_structure->delete_pipe );
		pclose( measurement_backup->insert_pipe );

		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file->
					database_management_system );
	}

	return 0;
}

char *get_where_clause_string( 	char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *begin_time,
				char *end_date,
				char *end_time,
				DICTIONARY *parameter_dictionary )
{
	char buffer[ 65536 ];
	char *date_time_where_clause;
	DICTIONARY *query_dictionary;
	QUERY *query;

	query_dictionary = dictionary_copy( parameter_dictionary );

	/* Parameter dictionary contains the source station/datatype. */
	/* ---------------------------------------------------------- */
	dictionary_set_pointer(
		query_dictionary,
		"station",
		"" );

	dictionary_set_pointer(
		query_dictionary,
		"datatype",
		"" );

	/* Parameter dictionary may contain reason_value_missing. */
	/* ------------------------------------------------------ */
	dictionary_set_pointer(
		query_dictionary,
		"reason_value_missing",
		"" );

	role =
		role_new(
			application_name,
			role_name );

	folder =
		folder_load_new( 	
			application_name,
			session,
			"measurement",
			role );

	if ( !folder )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_load_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query =
		query_simple_new(
			query_dictionary,
			(char *)0 /* login_name_only */,
			(char *)0 /* full_name_only */,
			(char *)0 /* street_address_only */,
			folder,
			(LIST *)0 /* ignore_attribute_name_list */ );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query->query_output )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_output is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* If executed from the user interface. */
	/* ------------------------------------ */
	date_time_where_clause =
		query_get_between_date_time_where(	
			"measurement_date",
			"measurement_time",
			begin_date,
			begin_time,
			end_date,
			end_time,
			(char *)0 /* application */,
			(char *)0 /* folder_name */ );

	sprintf( buffer,
		 "station = '%s' 				"
		 "and datatype = '%s'				"
		 "and %s					"
		 "and %s					",
		 station,
		 datatype,
		 date_time_where_clause,
		 query->query_output->where_clause );

	return strdup( buffer );
}

