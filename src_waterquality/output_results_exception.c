/* ---------------------------------------------------	*/
/* src_waterquality/output_results_exception.c		*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "appaserver.h"
#include "dictionary_separate.h"
#include "date_convert.h"
#include "application.h"
#include "process_generic_output.h"
#include "appaserver_link.h" 

/* Constants */
/* --------- */

#define FILENAME_STEM		"results_exception"

/* Prototypes */
/* ---------- */
void output_results_exception(
		char *application_name,
		char *begin_date_string,
		char *end_date_string,
		PROCESS_GENERIC_OUTPUT *process_generic_output,
		DICTIONARY *post_dictionary,
		char *document_root_directory );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *begin_date_string;
	char *end_date_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char buffer[ 256 ];
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	DICTIONARY *post_dictionary;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_set process_name begin_date end_date post_dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_set_name = argv[ 2 ];
	process_name = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	post_dictionary =
		dictionary_string2dictionary( argv[ 6 ] );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	APPASERVER_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	DICTIONARY_SEPARATE_QUERY_PREFIX );

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

	printf( "<h2>%s\n",
		 format_initial_capital(
			buffer,
			process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			0 /* accumulate_flag */ );

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date_string /* in/out */,
			&end_date_string /* in/out */,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name,
			(DICTIONARY *)0
				/* query_removed_post_dictionary */ ) )
	{
		printf( "<h3>Invalid date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	printf( "<h2>Beginning: %s Ending: %s</h2>\n",
		begin_date_string,
		end_date_string );
	fflush( stdout );

	output_results_exception(
		application_name,
		begin_date_string,
		end_date_string,
		process_generic_output,
		post_dictionary,
		appaserver_parameter_file->
			document_root );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );

} /* main() */

void output_results_exception(
		char *application_name,
		char *begin_date_string,
		char *end_date_string,
		PROCESS_GENERIC_OUTPUT *process_generic_output,
		DICTIONARY *post_dictionary,
		char *document_root_directory )
{
	char *parameter_where_clause;
	char join_where_clause[ 1024 ];
	char from_clause[ 1024 ];
	char sys_string[ 3072 ];
	char *select;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char heading[ 256 ];
	boolean first_time = 1;
	char *ftp_filename;
	char *output_pipename; pid_t process_id = getpid();
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			begin_date_string,
			end_date_string,
			(char *)0 /* session */,
			"txt" );

	output_pipename = appaserver_link->output->filename;

	ftp_filename = appaserver_link->prompt->filename;

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

	select =
"results.station,results.collection_date,results.collection_time,results.parameter,results.units,results.concentration,results_exception.exception";

	strcpy( heading, select );
	search_replace_string( heading, "results.", "" );
	search_replace_string( heading, "results_exception.", "" );
	search_replace_character( heading, ',', '^' );

/*
	sprintf( sys_string,
		 "delimiter2padded_columns.e '^' > %s",
		 output_pipename );
*/
		sprintf(sys_string,
		 	"tr '^' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );


	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe, "#%s\n", heading );

	parameter_where_clause =
		process_generic_output_get_dictionary_where_clause(
			&begin_date_string,
			&end_date_string,
			application_name,
			process_generic_output,
			post_dictionary,
			0 /* not with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name );

	sprintf( join_where_clause,
	"results.station = results_exception.station and		 "
	"results.collection_date = results_exception.collection_date and "
	"results.collection_time = results_exception.collection_time and "
	"results.parameter = results_exception.parameter and		 "
	"results.units = results_exception.units			 " );

	sprintf(	from_clause,
			"results left join results_exception on %s",
			join_where_clause );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql.e '%c'						|"
		 "separate_children_from_parents.e '%c' 5		 ",
		 select,
		 from_clause,
		 parameter_where_clause,
		 select,
		 FOLDER_DATA_DELIMITER,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( strcmp( input_buffer, ".end" ) == 0 )
		{
			fprintf( output_pipe, "\n" );
			first_time = 1;
		}
		else
		if ( timlib_strncmp( input_buffer, ".parent " ) == 0 )
		{
			search_replace_string(	input_buffer,
						".parent ",
						"" );
			fprintf( output_pipe, "%s", input_buffer );
		}
		else
		if ( timlib_strncmp( input_buffer, ".child " ) == 0 )
		{
			search_replace_string(	input_buffer,
						".child ",
						"" );

			if ( first_time )
			{
				fprintf( output_pipe, "^%s", input_buffer );
				first_time = 0;
			}
			else
			{
				fprintf( output_pipe, ",%s", input_buffer );
			}
		}
	}

	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_results_exception() */

