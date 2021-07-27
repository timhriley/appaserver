/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/post_measurement_generic_load.c	*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "query.h"
#include "piece.h"
#include "column.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "attribute.h"
#include "environ.h"
#include "application.h"
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "related_folder.h"
#include "session.h"
#include "role.h"
#include "date.h"
#include "date_convert.h"
#include "hydrology_library.h"
#include "generic_load.h"

/* Enumerated types */
/* ---------------- */
enum event_parameter_return_value {
				multiple_station_datatype,
				event_parameter_invalid_date,
				event_parameter_invalid_datatype,
				event_parameter_okay };

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
boolean generic_measurement_load_datatype_okay(
				char *datatype_name );

void execute_measurement_block_delete(
				char *application_name,
				char *station,
				char *datatype,
		 		char *begin_measurement_date_string,
		 		char *begin_measurement_time_string,
		 		char *end_measurement_date_string,
		 		char *end_measurement_time_string,
				char *login_name,
				char *role_name,
				char *session,
				char *process_name );

int get_measurements_to_delete(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_measurement_date_string,
				char *begin_measurement_time_string,
				char *end_measurement_date_string,
				char *end_measurement_time_string );

enum event_parameter_return_value get_measurement_update_event_parameters(
			char **station,
			char **datatype,
			char **begin_measurement_date_string,
			char **begin_measurement_time_string,
			char **end_measurement_date_string,
			char **end_measurement_time_string,
			char *application_name,
			GENERIC_LOAD_FOLDER *measurement_generic_load_folder,
			char delimiter,
			char *load_filename,
			int skip_header_rows );

void post_generic_measurement_load(
				char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *appaserver_data_directory,
				char *login_name,
				char *process_name );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 256 ];
	char buffer[ 256 ];
	char *database_string = {0};
	DOCUMENT *document;
	char *process_name;

	if ( argc != 6 )
	{
		fprintf( stderr, 
		"Usage: %s application session role folder process_name \n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	process_name = argv[ 5 ];

	folder_name = "measurement";

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	login_name =
		session_get_login_name(
			application_name,
			session );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !login_name || !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				"insert" ) )
	{
		session_access_failed_message_and_exit(
			application_name,
			session,
			login_name );
	}

	if ( !appaserver_user_exists_role(
			login_name,
			role_name ) )
	{
		session_access_failed_message_and_exit(
			application_name,
			session,
			login_name );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new(
				application_title_string(
					application_name ),
				application_name );

	document->output_content_type = 1;

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

	sprintf( title,
		 "%s Load %s",
		 application_title_string( application_name ),
		 format_initial_capital( buffer,
					 folder_name ) );

	printf( "<h1>%s</h1>\n", title );

	fflush( stdout );

	post_generic_measurement_load(
			application_name,
			session,
			role_name,
			folder_name,
			appaserver_parameter_file->
				appaserver_data_directory,
			login_name,
			process_name );

	document_close();
	return 0;
} /* main() */

void post_generic_measurement_load(
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			char *appaserver_data_directory,
			char *login_name,
			char *process_name )
{
	DICTIONARY *post_dictionary;
	DICTIONARY *position_dictionary;
	DICTIONARY *constant_dictionary;
	GENERIC_LOAD *generic_load;
	char *load_filename;
	char delimiter = {0};
	char *really_yn;
	char *skip_header_rows;
	int skip_header_rows_integer = 0;
	FILE *input_file;
	boolean with_sort_unique;
	char *station = {0};
	char *datatype = {0};
	char *begin_measurement_date_string = {0};
	char *begin_measurement_time_string = {0};
	char *end_measurement_date_string = {0};
	char *end_measurement_time_string = {0};
	enum event_parameter_return_value event_parameter_return_value;
	int measurements_to_delete = 0;

	post_dictionary =
		post2dictionary(stdin,
				appaserver_data_directory,
				session );

	really_yn =
		dictionary_get_pointer(
			post_dictionary,
			"really_yn" );

	load_filename =
		dictionary_get_pointer(
			post_dictionary,
			"load_filename" );

	if ( !load_filename
	||   !( input_file = fopen( load_filename, "r" ) ) )
	{
		printf( "<p>ERROR: please transmit a file.\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	if ( ( skip_header_rows =
		dictionary_get_pointer(
			post_dictionary,
			HYDROLOGY_LIBRARY_SKIP_HEADER_ROWS ) ) )
	{
		skip_header_rows_integer = atoi( skip_header_rows );
	}

	generic_load = generic_load_new();

	generic_load->folder =
		generic_load_get_database_folder(
			application_name,
			session,
			folder_name,
			role_name,
			1 /* with_mto1_related_folders */ );

	position_dictionary =
		dictionary_get_without_prefix(
			post_dictionary,
			"position_" );

	constant_dictionary =
		dictionary_get_without_prefix(
			post_dictionary,
			"constant_" );

	if ( ! ( generic_load->generic_load_folder_list =
			generic_load_get_folder_list(
				generic_load->folder,
				position_dictionary,
				constant_dictionary,
				login_name ) )
	||   !list_length( generic_load->generic_load_folder_list ) )
	{
		printf( "<h4>ERROR: Invalid input.</h4>\n" );
		document_close();
		exit( 0 );
	}

	generic_load_attribute_set_is_participating(
		generic_load->generic_load_folder_list,
		&delimiter,
		application_name,
		load_filename,
		skip_header_rows_integer );

	event_parameter_return_value =
		get_measurement_update_event_parameters(
			&station,
			&datatype,
			&begin_measurement_date_string,
			&begin_measurement_time_string,
			&end_measurement_date_string,
			&end_measurement_time_string,
			application_name,
			list_get_first_pointer(
				generic_load->generic_load_folder_list ),
			delimiter,
			load_filename,
			skip_header_rows_integer );

	printf( "<h2>From %s:%s to %s:%s</h2>\n",
		begin_measurement_date_string,
		begin_measurement_time_string,
		end_measurement_date_string,
		end_measurement_time_string );

	if ( event_parameter_return_value == event_parameter_invalid_date )
	{
		printf( "<h3>Error: invalid date.</h3>\n" );
	}
	else
	if ( event_parameter_return_value == event_parameter_invalid_datatype )
	{
		printf( "<h3>Error: datatypes cannot contain spaces.</h3>\n" );
	}

	if ( event_parameter_return_value == event_parameter_okay )
	{
		measurements_to_delete =
			get_measurements_to_delete(
				application_name,
				station,
				datatype,
				begin_measurement_date_string,
				begin_measurement_time_string,
				end_measurement_date_string,
				end_measurement_time_string );

		if ( measurements_to_delete )
		{
			if ( *really_yn == 'y' )
			{
				printf(
			"<h3>Backing up and deleting %d measurements.</h3>\n",
				measurements_to_delete );
			}
			else
			{
				printf(
			"<h3>Will backup and delete %d measurements.</h3>\n",
				measurements_to_delete );
			}
		}
	}

	fflush( stdout );

	if ( *really_yn != 'y'
	||   event_parameter_return_value == event_parameter_invalid_date
	||   event_parameter_return_value == event_parameter_invalid_datatype )
	{
		GENERIC_LOAD_FOLDER *generic_load_folder;
		int input_record_count = 0;

		list_rewind( generic_load->generic_load_folder_list );

		do {
			generic_load_folder =
				list_get_pointer(
					generic_load->
						generic_load_folder_list );

			if ( !generic_load_has_participating(
				generic_load_folder->
					generic_load_attribute_list ) )
			{
				continue;
			}

			with_sort_unique =
				!list_at_head(
					generic_load->
						generic_load_folder_list );

			input_record_count =
				generic_load_output_test_only_report(
					load_filename,
					delimiter,
					generic_load_folder->
						generic_load_attribute_list,
					generic_load_folder->
						primary_key_date_offset,
					generic_load_folder->
						primary_key_time_offset,
					application_name,
					list_at_head(
						generic_load->
						generic_load_folder_list )
					/* display_errors */,
					generic_load_folder->folder_name,
					skip_header_rows_integer,
					with_sort_unique );

		} while( list_next( generic_load->
					generic_load_folder_list ) );

		printf( "<p>Test complete -- Input count: %d\n",
			input_record_count );
	}
	else
	{
		if ( measurements_to_delete )
		{
			execute_measurement_block_delete(
				application_name,
				station,
				datatype,
		 		begin_measurement_date_string,
		 		begin_measurement_time_string,
		 		end_measurement_date_string,
		 		end_measurement_time_string,
				login_name,
				role_name,
				session,
				process_name );
		}

		generic_load_output_to_database(
				application_name,
				load_filename,
				delimiter,
				generic_load->generic_load_folder_list,
				skip_header_rows_integer );
	}
}

enum event_parameter_return_value
	get_measurement_update_event_parameters(
		char **station,
		char **datatype,
		char **begin_measurement_date_string,
		char **begin_measurement_time_string,
		char **end_measurement_date_string,
		char **end_measurement_time_string,
		char *application_name,
		GENERIC_LOAD_FOLDER *measurement_generic_load_folder,
		char delimiter,
		char *load_filename,
		int skip_header_rows )
{
	char input_buffer[ 4096 ];
	char *piece_buffer;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	FILE *input_file;
	LIST *generic_load_attribute_list;
	static char local_station[ 32 ] = {0};
	static char local_datatype[ 32 ] = {0};
	char measurement_date_string[ 16 ];
	DATE *measurement_date;
	DATE *begin_measurement_date;
	DATE *end_measurement_date;
	enum event_parameter_return_value event_parameter_return_value =
		event_parameter_okay;

	if ( !load_filename
	||   !( input_file = fopen( load_filename, "r" ) ) )
	{
		printf( "<p>ERROR: please transmit a file.\n" );
		document_close();
		exit( 0 );
	}

	measurement_date = date_calloc();
	begin_measurement_date = date_calloc();
	end_measurement_date = date_calloc();

	generic_load_reset_row_count();

	while( get_line( input_buffer, input_file ) )
	{
		trim( input_buffer );

		if ( generic_load_skip_header_rows(
			skip_header_rows ) )
		{
			continue;
		}

		if ( !*input_buffer ) continue;

		if ( *input_buffer == '#' ) continue;

		generic_load_set_international_date(
				input_buffer,
				application_name,
				delimiter,
				measurement_generic_load_folder->
					primary_key_date_offset );

		generic_load_replace_time_2400_with_0000(
				input_buffer,
				delimiter,
				measurement_generic_load_folder->
					primary_key_date_offset,
				measurement_generic_load_folder->
					primary_key_time_offset );

		if ( !generic_load_fix_time(
				input_buffer,
				delimiter,
				measurement_generic_load_folder->
					primary_key_time_offset ) )
		{
/*
			event_parameter_return_value =
				event_parameter_invalid_date;
*/
			continue;
		}

		generic_load_attribute_list =
			measurement_generic_load_folder->
				generic_load_attribute_list;

		if ( !list_rewind( generic_load_attribute_list ) )
		{
			fclose( input_file );
			return 0;
		}

		list_rewind( generic_load_attribute_list );
		do {
			generic_load_attribute =
				list_get_pointer(
					generic_load_attribute_list );

			piece_buffer =
			generic_load_get_piece_buffer(
					input_buffer,
					&delimiter,
					application_name,
					generic_load_attribute->
						datatype,
					generic_load_attribute->
						constant,
					generic_load_attribute->
						position - 1,
					generic_load_attribute->
						is_primary_key );

			if ( !*piece_buffer ) continue;

			if ( strcmp(	generic_load_attribute->attribute_name,
					"station" ) == 0 )
			{
				if ( !*local_station )
					strcpy( local_station, piece_buffer );
				else
				{
					if ( strcmp(	local_station,
							piece_buffer ) != 0 )
					{
						event_parameter_return_value =
						multiple_station_datatype;
					}
				}
			}
			else
			if ( strcmp(	generic_load_attribute->attribute_name,
					"datatype" ) == 0 )
			{
				if ( !*local_datatype )
					strcpy( local_datatype, piece_buffer );
				else
				{
					if ( strcmp(	local_datatype,
							piece_buffer ) != 0 )
					{
						event_parameter_return_value =
						multiple_station_datatype;
					}
				}

				if ( !generic_measurement_load_datatype_okay(
						piece_buffer ) )
				{
					event_parameter_return_value =
					event_parameter_invalid_datatype;
					continue;
				}

			}
			else
			if ( strcmp(	generic_load_attribute->attribute_name,
					"measurement_date" ) == 0 )
			{
				strcpy( measurement_date_string, piece_buffer );
			}
			else
			if ( strcmp(	generic_load_attribute->attribute_name,
					"measurement_time" ) == 0 )
			{
				date_set_yyyy_mm_dd(
						measurement_date,
						measurement_date_string );

				date_set_time_hhmm(
						measurement_date,
						piece_buffer );

				if ( begin_measurement_date->current == 0 )
				{
					date_copy(	begin_measurement_date,
							measurement_date );
					date_copy(	end_measurement_date,
							measurement_date );
				}
				else
				if ( measurement_date->current <
				     begin_measurement_date->current )
				{
					date_copy(	begin_measurement_date,
							measurement_date );
				}
				else
				if ( measurement_date->current >
				     end_measurement_date->current )
				{
					date_copy(	end_measurement_date,
							measurement_date );
				}
			}
				
		} while( list_next( generic_load_attribute_list ) );
	}

	fclose( input_file );

	*station = local_station;
	*datatype = local_datatype;

	*begin_measurement_date_string =
		strdup( date_yyyy_mm_dd( begin_measurement_date ) );

	*begin_measurement_time_string =
		strdup( date_hhmm( begin_measurement_date ) );

	*end_measurement_date_string =
		strdup( date_yyyy_mm_dd( end_measurement_date ) );

	*end_measurement_time_string =
		strdup( date_hhmm( end_measurement_date ) );

	date_free( measurement_date );
	date_free( begin_measurement_date );
	date_free( end_measurement_date );

	if ( timlib_strncmp( *begin_measurement_date_string, "1900" ) == 0 )
		event_parameter_return_value = event_parameter_invalid_date;

	return event_parameter_return_value;

} /* get_measurement_update_event_parameters() */

int get_measurements_to_delete(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_measurement_date_string,
				char *begin_measurement_time_string,
				char *end_measurement_date_string,
				char *end_measurement_time_string )
{
	char sys_string[ 2048 ];
	char where[ 1024 ];
	char *date_time_where_clause;

	date_time_where_clause =
		query_get_between_date_time_where(	
				"measurement_date",
				"measurement_time",
		 		begin_measurement_date_string,
		 		begin_measurement_time_string,
		 		end_measurement_date_string,
		 		end_measurement_time_string,
				application_name,
				"measurement" /* folder_name */ );

	sprintf( where,
		 "station = '%s' and		"
		 "datatype = '%s' 		"
		 "and %s			",
		 station,
		 datatype,
		 date_time_where_clause );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=measurement	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* get_measurements_to_delete() */

void execute_measurement_block_delete(
				char *application_name,
				char *station,
				char *datatype,
		 		char *begin_measurement_date_string,
		 		char *begin_measurement_time_string,
		 		char *end_measurement_date_string,
		 		char *end_measurement_time_string,
				char *login_name,
				char *role_name,
				char *session,
				char *process_name )
{
	char sys_string[ 1024 ];
	char notes[ 128 ];

	sprintf( notes, "From %s", process_name );

	sprintf( sys_string,
"measurement_block_delete %s %s %s %s %s %s %s %s %s %s parameter_dictionary \"%s\" y nohtml",
		 login_name,
		 application_name,
		 session,
		 role_name,
		 station,
		 datatype,
		 begin_measurement_date_string,
		 begin_measurement_time_string,
		 end_measurement_date_string,
		 end_measurement_time_string,
		 notes );

	if ( system( sys_string ) ){};
}

boolean generic_measurement_load_datatype_okay(
				char *datatype_name )
{
	return ( !character_exists( datatype_name, ' ' ) );

}

