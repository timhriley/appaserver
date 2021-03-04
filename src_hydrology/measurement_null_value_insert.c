/* --------------------------------------------------- 	*/
/* measurement_null_value_insert.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "application.h"
#include "julian.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "query.h"
#include "measurement_update_parameter.h"
#include "session.h"
#include "hydrology_library.h"
#include "expected_count.h"

/* Constants */
/* --------- */
/* #define PROCESS_NAME		"measurement_null_value_insert" */
#define PROCESS_TITLE		"Measurement Null Value Insert"
#define ESTIMATION_METHOD	"null_value_insert"

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
	char *reason_value_missing;
	char really_yn;
	char sys_string[ 4096 ];
	DOCUMENT *document;
	LIST *expected_count_list;
	int with_delete;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	int count;
	int minutes_offset;
	char *session;
	char *role_name;
	char *database_string = {0};

	if ( argc != 16 )
	{
		fprintf(stderr,
"Usage: %s login_name application session role station datatype begin_date begin_time end_date end_time with_delete_yn reason_value_missing minutes_offset parameter_dictionary really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	begin_time = argv[ 8 ];
	end_date = argv[ 9 ];
	end_time = argv[ 10 ];
	with_delete = (*argv[ 11 ] == 'y');
	reason_value_missing = argv[ 12 ];
	minutes_offset = atoi( argv[ 13 ] );
	parameter_dictionary_string = argv[ 14 ];
	really_yn = *argv[ 15 ];

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

	expected_count_list =
		expected_count_get_expected_count_list(
			application_name,
			station,
			datatype );

	if ( !expected_count_list || !list_length( expected_count_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( 
	"<p>ERROR: There is no entry in DATA_COLLECTION_FREQUENCY for %s/%s.\n",
			station,
			datatype );
		document_close();
		exit( 1 );
	}

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

	if ( strcmp( end_date, "end_date" ) == 0 )
		end_date = begin_date;

	if ( strcmp( begin_time, "begin_time" ) == 0 )
		begin_time = "0000";

	if ( strcmp( end_time, "end_time" ) == 0 )
		end_time = "2359";
	
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

	if ( with_delete )
	{
		sprintf( sys_string, 
"measurement_block_delete %s %s %s %s %s %s %s %s %s %s '%s' '' %c nohtml",
			 login_name,
			 application_name,
			 session,
			 role_name,
			 station,
			 datatype,
			 begin_date,
			 begin_time,
			 end_date,
			 end_time,
			 parameter_dictionary_string,
			 really_yn );
		system( sys_string );
	}

	count = hydrology_library_insert_null_measurements(
			station,
			datatype,
			begin_date,
			begin_time,
			end_date,
			end_time,
			reason_value_missing,
			minutes_offset,
			really_yn,
			PROCESS_TITLE,
			1 /* with_table_output */,
			application_name );

	if ( really_yn == 'y' )
	{
		char count_string[ 128 ];
		MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;

		if ( !with_delete )
		{
			dictionary_set_pointer(
				parameter_dictionary,
				"delete_yn",
				"n" );
		}

		measurement_update_parameter =
			measurement_update_parameter_new(
					application_name,
					station,
					datatype,
					ESTIMATION_METHOD,
					login_name,
					parameter_dictionary,
					(char *)0 /* notes */ );
	
		sprintf( count_string, "%d", count );
	
		dictionary_add_string( 
				measurement_update_parameter->
					parameter_dictionary,
				"count",
				count_string );
	
		measurement_update_parameter_save(
				measurement_update_parameter );
		process_increment_execution_count(
				application_name,
				dictionary_get_string(
					parameter_dictionary, "process" ),
				appaserver_parameter_file_get_dbms() );
	}

	document_close();
	exit( 0 );
} /* main() */

