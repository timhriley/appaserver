/* ---------------------------------------------------	*/
/* src_waterquality/output_results_exception_free.c	*/
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
#include "query.h"
#include "date_convert.h"
#include "application.h"
#include "process_generic_output.h"
#include "appaserver_link.h" 

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES	500

#define FILENAME_STEM		"results_exception_free"

/* Prototypes */
/* ---------- */
void output_results_exception_table(
				char *select,
				char *from_clause,
				char *parameter_where_clause,
				char *subquery_where );

char *exception_free_subquery_where(
				char *application_name,
				char *value_folder_name );

/* Returns count. */
/* -------------- */
int output_results_exception_spreadsheet(
				char *application_name,
				char *select,
				char *from_clause,
				char *parameter_where_clause,
				char *subquery_where,
				char *document_root_directory,
				char *begin_date_string,
				char *end_date_string );

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
	char buffer[ 256 ];
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	DICTIONARY *post_dictionary;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_set process_name begin_date end_date post_dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

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
				    	APPASERVER_QUERY_STARTING_LABEL );

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

}

void output_results_exception(
		char *application_name,
		char *begin_date_string,
		char *end_date_string,
		PROCESS_GENERIC_OUTPUT *process_generic_output,
		DICTIONARY *post_dictionary,
		char *document_root_directory )
{
	char *parameter_where_clause;
	char *subquery_where;
	char *from_clause;
	char *select;
	int count;

	select =
"station,collection_date,collection_time,parameter,units,concentration";

	from_clause = "results";

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

	subquery_where =
		exception_free_subquery_where(
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name );

	count = output_results_exception_spreadsheet(
			application_name,
			select,
			from_clause,
			parameter_where_clause,
			subquery_where,
			document_root_directory,
			begin_date_string,
			end_date_string );

	output_results_exception_table(
		select,
		from_clause,
		parameter_where_clause,
		subquery_where );

	printf( "<br><br>Selected %d rows.\n", count );

}

int output_results_exception_spreadsheet(
					char *application_name,
					char *select,
					char *from_clause,
					char *parameter_where_clause,
					char *subquery_where,
					char *document_root_directory,
					char *begin_date_string,
					char *end_date_string )
{
	char sys_string[ 3072 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char quote_buffer[ 1024 ];
	char *ftp_filename;
	char *output_pipename; pid_t process_id = getpid();
	APPASERVER_LINK *appaserver_link;
	LIST *list;
	int count = 0;

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
			(char *)0 /* session */,
			begin_date_string,
			end_date_string,
			"csv" );

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

	sprintf(sys_string,
	 	"cat > %s",
	 	output_pipename );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe, "%s\n", select );

	sprintf( sys_string,
	"echo \"select %s from %s where %s and %s order by %s;\"	|"
		 "sql.e '%c'						 ",
		 select,
		 from_clause,
		 parameter_where_clause,
		 subquery_where,
		 select,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !*input_buffer ) continue;

		list = list_string2list( input_buffer, FOLDER_DATA_DELIMITER );

		fprintf(output_pipe,
			"%s\n",
			list_display_double_quote_comma_delimited(
				quote_buffer,
				list ) );

		list_free_string_list( list );
		count++;
	}

	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );
	printf( "<br>\n" );
	fflush( stdout );

	return count;

}

void output_results_exception_table(	char *select,
					char *from_clause,
					char *parameter_where_clause,
					char *subquery_where )
{
	char sys_string[ 3072 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];

	sprintf(sys_string,
		"queue_top_bottom_lines.e %d	|"
	 	"html_table.e '' '%s' '%c'	 ",
		QUEUE_TOP_BOTTOM_LINES,
	 	select,
		FOLDER_DATA_DELIMITER );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
	"echo \"select %s from %s where %s and %s order by %s;\"	|"
		 "sql.e '%c'						 ",
		 select,
		 from_clause,
		 parameter_where_clause,
		 subquery_where,
		 select,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !*input_buffer ) continue;

		fprintf(output_pipe,
			"%s\n",
			input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

}

char *exception_free_subquery_where(
				char *application_name,
				char *value_folder_name )
{
	char subquery_where[ 2048 ];
	LIST *primary_key_list;
	LIST *foreign_attribute_name_list;

	primary_key_list =
		folder_get_primary_key_list(
			folder_get_attribute_list(
				application_name,
				value_folder_name ) );

	foreign_attribute_name_list = list_copy( primary_key_list );

	sprintf(subquery_where,
		"not exists ( select 1 from %s where %s)",
			"results_exception",
			query_append_where_clause_related_join(
				application_name,
				(char *)0 /* source_where_clause */,
				primary_key_list,
				foreign_attribute_name_list,
				value_folder_name,
				"results_exception"
					/* related_folder_name */ ) );

	return strdup( subquery_where );

}

