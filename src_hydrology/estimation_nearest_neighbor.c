/* --------------------------------------------------- 	*/
/* src_hydrology/estimation_nearest_neighbor.c	       	*/
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
#include "measurement_update_parameter.h"
#include "query.h"
#include "application.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"estimation_nearest_neighbor"
#define VALUE_OFFSET			4
#define MAX_DISPLAY_COUNT		50

#define ESTIMATION_METHOD		"nearest_neighbor"

#define MEASUREMENT_PRIMARY_LIST	 "station,measurement_date,measurement_time,datatype"

#define MEASUREMENT_SELECT_LIST		 "station,measurement_date,measurement_time,datatype,measurement_value"

#define MEASUREMENT_INSERT_LIST		 "station,measurement_date,measurement_time,datatype,measurement_value,measurement_update_method"

/* Prototypes */
/* ---------- */
int some_not_validated(
			char *application_name,
			char *where_clause );

int main( int argc, char **argv )
{
	register int counter = 0;
	char *login_name;
	char *application_name;
	char *source_datatype;
	char *source_station;
	char *where_clause;
	char destination_where_clause[ 1024 ];
	char buffer[ 4096 ];
	char *table_name;
	FILE *input_pipe;
	FILE *insert_pipe;
	char *destination_station;
	char *destination_datatype;
	double offset, multiplier, value;
	char value_string[ 64 ];
	DOCUMENT *document;
	char really_yn;
	char *parameter_dictionary_string;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *notes;
	char *session;
	char *role_name;
	char *database_string = {0};
	char *override_source_validation_requirement_yn;
	boolean override_source_validation_requirement;
	char *override_destination_validation_requirement_yn;
	boolean override_destination_validation_requirement;
	int not_validated_count;
	int validated_count;
	char dont_delete_yn;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( argc != 16 )
	{
		fprintf(stderr,
"Usage: %s person application session role source_station destination_station source_datatype destination_datatype offset multiplier dictionary \"where_clause\" dont_delete_yn notes really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];
	source_station = argv[ 5 ];
	destination_station = argv[ 6 ];
	source_datatype = argv[ 7 ];
	destination_datatype = argv[ 8 ];
	offset = atof( argv[ 9 ] );
	multiplier = atof( argv[ 10 ] );
	parameter_dictionary_string = argv[ 11 ];
	where_clause = argv[ 12 ];
	dont_delete_yn = *argv[ 13 ];
	notes = argv[ 14 ];
	really_yn = *argv[ 15 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(	"APPASERVER_DATABASE",
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

	if ( timlib_strcmp( source_station, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<h3>Please choose a station and datatype.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( timlib_strcmp( destination_station, "destination_station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<h3>Please choose a destination station and datatype.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( timlib_strcmp( destination_station, source_station ) == 0
	&&   timlib_strcmp( destination_datatype, source_datatype ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<h3>Please choose a different station and datatype.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				dictionary_index_string2dictionary(
					parameter_dictionary_string ),
				application_name,
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

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_remove_prefix(
			dictionary_appaserver->non_prefixed_dictionary,
			QUERY_FROM_STARTING_LABEL );

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_remove_index_zero(
			dictionary_appaserver->non_prefixed_dictionary );

	override_source_validation_requirement_yn =
		dictionary_get_pointer(
			dictionary_appaserver->non_prefixed_dictionary,
			"override_source_validation_requirement_yn" );

	override_source_validation_requirement =
		( override_source_validation_requirement_yn
		&& *override_source_validation_requirement_yn  == 'y' );

	override_destination_validation_requirement_yn =
		dictionary_get_pointer(
			dictionary_appaserver->non_prefixed_dictionary,
			"override_destination_validation_requirement_yn" );

	override_destination_validation_requirement =
		( override_destination_validation_requirement_yn
		&& *override_destination_validation_requirement_yn  == 'y' );

	if ( !override_source_validation_requirement
	&&   (not_validated_count =
		some_not_validated( application_name, where_clause ) ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<p>ERROR: %d source measurements are not validated.\n",
       			not_validated_count );
		document_close();
		exit( 0 );
	}

	strcpy( destination_where_clause, where_clause );

	search_replace_once(
			destination_where_clause,
			source_datatype,
			destination_datatype );

	search_replace_once(
			destination_where_clause,
			source_station,
			destination_station );

	if ( !override_destination_validation_requirement
	&&   (validated_count =
		hydrology_library_some_measurements_validated(
				application_name,
				destination_where_clause,
				(char *)0 /* station */,
				(char *)0 /* datatype */,
				(char *)0 /* begin_measurement_date */,
				(char *)0 /* end_measurement_date */ ) ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<p>ERROR: %d destination measurements are validated.\n",
       			validated_count );
		document_close();
		exit( 0 );
	}

	if ( !multiplier ) multiplier = 1.0;

	if ( really_yn == 'y' && dont_delete_yn != 'y' )
	{
		char sys_string[ 4096 ];
		char *begin_date;
		char *begin_time;
		char *end_date;
		char *end_time;

		if ( ! ( begin_date = dictionary_get_pointer(
				dictionary_appaserver->
					non_prefixed_dictionary,
				"measurement_date" ) ) )
		{
			begin_date = "";
		}

		if ( ! ( end_date = dictionary_get_pointer(
				dictionary_appaserver->
					non_prefixed_dictionary,
				"to_measurement_date" ) ) )
		{
			end_date = begin_date;
		}

		if ( ! ( begin_time = dictionary_get_pointer(
				dictionary_appaserver->
					non_prefixed_dictionary,
				"measurement_time" ) ) )
		{
			begin_time = "0000";
		}

		if ( ! ( end_time = dictionary_get_pointer(
				dictionary_appaserver->
					non_prefixed_dictionary,
				"to_measurement_time" ) ) )
		{
			end_time = "2359";
		}

		sprintf( sys_string, 
"measurement_block_delete %s %s %s %s %s %s %s %s %s %s \"%s\" \"%s\" %c nohtml",
		 	login_name,
		 	application_name,
		 	session,
		 	role_name,
		 	destination_station,
		 	destination_datatype,
		 	begin_date,
		 	begin_time,
		 	end_date,
		 	end_time,
		 	parameter_dictionary_string,
		 	notes,
		 	really_yn );

		system( sys_string );
	}

	table_name = get_table_name( application_name, "measurement" );

	/* Create the input pipe */
	/* --------------------- */
	sprintf(buffer, 
		"get_folder_data	application=%s			  "
		"			folder=measurement		  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"tr '%c' '|'						 |"
		"sed 's/^[0-z][0-z]*|/%s|/'				 |"
		"sed 's/|%s|/|%s|/'					 |"
		"sed 's/$/|%s/'						  ",
		application_name,
		MEASUREMENT_SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		destination_station,
		source_datatype,
		destination_datatype,
		ESTIMATION_METHOD );

	input_pipe = popen( buffer, "r" );

	/* Create the insert output pipe */
	/* ----------------------------- */
	if ( really_yn == 'y' )
	{
		sprintf(buffer,
			"insert_statement.e %s '%s'		|"
			"sql.e 2>&1				|"
			"html_paragraph_wrapper			 ",
			table_name,
			MEASUREMENT_INSERT_LIST );
	}
	else
	{
		sprintf(buffer,
			"insert_statement.e %s '%s'		|"
			"queue_top_bottom_lines.e %d		|"
			"html_paragraph_wrapper			",
			table_name,
			MEASUREMENT_INSERT_LIST,
			MAX_DISPLAY_COUNT );
	}

	insert_pipe = popen( buffer, "w" );

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

	/* Sample input: "BD|1999-01-01|1000|bottom_temperature|..." */
	/* --------------------------------------------------------- */
	while( get_line( buffer, input_pipe ) )
	{
		counter++;

/*
		if ( really_yn != 'y' )
		{
			if ( counter == MAX_DISPLAY_COUNT )
			{
				printf( "<p>Skipping output...\n" );
				continue;
			}
			else
			if ( counter > MAX_DISPLAY_COUNT )
			{
				continue;
			}
		}
*/

		piece( value_string, '|', buffer, VALUE_OFFSET );

		if ( *value_string )
		{
			value = ( atof( value_string ) + offset ) * multiplier;
			sprintf( value_string, "%lf", value );
		}

		replace_piece(	buffer,
				'|',
				value_string,
				VALUE_OFFSET );

		fprintf(insert_pipe, "%s\n", buffer );
	}

	pclose( insert_pipe );
	pclose( input_pipe );

	if ( really_yn == 'y' )
	{
		char counter_string[ 128 ];

		printf( "<br>Records inserted: %d\n", counter );

		sprintf( counter_string, "%d", counter );
		dictionary_add_string( 	dictionary_appaserver->
						non_prefixed_dictionary,
					"count",
					counter_string );

		measurement_update_parameter =
			measurement_update_parameter_new(
					application_name,
					destination_station,
					source_datatype,
					ESTIMATION_METHOD,
					login_name,
					dictionary_appaserver->
						non_prefixed_dictionary,
					notes );

		measurement_update_parameter_save(
			measurement_update_parameter );

		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( 
		"<br>Not processed. Records to process: %d\n", counter );
	}

	document_close();
	exit( 0 );
} /* main() */

boolean some_not_validated( char *application_name, char *where_clause )
{
	char sys_string[ 1024 ];
	char local_where_clause[ 1024 ];
	char *results;

	sprintf( local_where_clause,
		 "%s and last_validation_date is null",
		 where_clause );

	sprintf( sys_string,
		 "get_folder_data		application=%s		"
		 "				select=count		"
		 "				folder=measurement	"
		 "				where=\"%s\"		",
		 application_name,
		 local_where_clause );
	results = pipe2string( sys_string );
	return atoi( results );
} /* some_not_validated() */

