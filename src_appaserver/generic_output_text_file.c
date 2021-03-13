/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_output_text_file.c	*/
/* ----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "sql.h"
#include "timlib.h"
#include "piece.h"
#include "application.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "process_generic.h"
#include "appaserver.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void generic_output_spreadsheet_create(
			char *output_filename,
			char *input_system_string,
			char *output_system_string,
			char *heading,
			char *subtitle );

void generic_output_text_file(
			char *system_string,
			char *heading,
			char *subtitle );

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *subtitle,
			int primary_attribute_name_list_length );

void generic_output_spreadsheet(
			char *application_name,
			char *input_system_string,
			char *heading,
			char *subtitle,
			char *document_root_directory,
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			pid_t process_id,
			char *email_address );

void generic_output_stdout(
			char *system_string,
			char *heading,
			char *subtitle );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *output_medium_string;
	DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	PROCESS_GENERIC *process_generic;
	char title[ 128 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s process_set process_name output_medium dictionary\n",
			 argv[ 0 ] );

		fprintf( stderr,
	"Note: output_medium = {text_file,stdout,table,spreadsheet}\n" );

		exit ( 1 );
	}

	process_set_name = argv[ 1 ];
	process_name = argv[ 2 ];
	output_medium_string = argv[ 3 ];

	post_dictionary =
		dictionary_string2dictionary(
			argv[ 4 ] );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		post_dictionary,
		QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium_string, "stdout" ) != 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );
	}

	process_generic =
		process_generic_fetch(
			process_set_name,
			process_name,
			output_medium_string,
			post_dictionary );

	if ( !process_generic )
	{
		printf( "<h3>Insufficient input</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( process_generic->parameter->output_medium != output_medium_stdout )
	{
		printf(	"<h1>%s</h1>\n",
			format_initial_capital(
				title,
				process_generic->process_name ) );
	}
	else
	{
		printf(	"%s\n",
			format_initial_capital(
				title,
				process_generic->process_name ) );
	}

	fflush( stdout );

	if (	process_generic->parameter->output_medium ==
		output_medium_stdout )
	{
		generic_output_stdout(
			process_generic->
				process_generic_system_string,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle );
	}
	else
	if (	process_generic->parameter->output_medium ==
		spreadsheet )
	{
		generic_output_spreadsheet(
			environment_application(),
			process_generic->
				process_generic_system_string
				/* input_system_string */,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle,
			appaserver_parameter_file->document_root,
			process_generic->
				value_folder->
				value_folder_name,
			process_generic->
				parameter->
				begin_date,
			process_generic->
				parameter->
				end_date,
			process_generic->
				parameter->
				process_id,
			process_generic->
				parameter->
				email_address );
	}
	else
	if (	process_generic->parameter->output_medium ==
		text_file )
	{
		generic_output_text_file(
			process_generic->
				process_generic_system_string,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle );
	}
	else
	if (	process_generic->parameter->output_medium ==
		table )
	{
		generic_output_table(
			process_generic->
				process_generic_system_string
				/* input_system_string */,
			process_generic->
				process_generic_heading,
			process_generic->
				process_generic_subtitle,
			list_length(
				process_generic->
					value_folder->
					primary_attribute_name_list ) );
	}

	if (	process_generic->parameter->output_medium !=
		output_medium_stdout )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;
}

void generic_output_stdout(
			char *system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( system_string, "r" );

	printf( "%s\n%s\n",
		subtitle,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		printf( "%s\n", input_buffer );
	}
	pclose( input_pipe );
}

void generic_output_text_file(
			char *system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( system_string, "r" );
	output_pipe = popen( "html_paragraph_wrapper", "w" );

	fprintf(output_pipe,
		"%s\n%s\n",
		subtitle,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *subtitle,
			int primary_attribute_name_list_length )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];
	char output_system_string[ 1024 ];

	sprintf(output_system_string,
		"html_table \"^%s\" \"%s\" '%c' %sright",
		subtitle,
		heading,
		SQL_DELIMITER,
		string_repeat(
			"left,",
			primary_attribute_name_list_length
				/* number_times */ ) );

	input_pipe = popen( input_system_string, "r" );
	output_pipe = popen( output_system_string, "w" );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

void generic_output_spreadsheet(
			char *application_name,
			char *input_system_string,
			char *heading,
			char *subtitle,
			char *document_root_directory,
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			pid_t process_id,
			char *email_address )
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *output_filename;
	char *link_prompt;
	char output_system_string[ 1024 ];

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->application_name = application_name;
	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

	appaserver_link_file->extension = "csv";

	output_filename =
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

	link_prompt =
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

	generic_output_spreadsheet_create(
		output_filename,
		input_system_string,
		output_system_string,
		heading,
		subtitle );

	if ( email_address && *email_address )
	{
		char sys_string[ 1024 ];
	
		sprintf( sys_string,
			 "cat %s				|"
			 "mailx -s \"%s\" %s	 		 ",
			 output_filename,
			 subtitle,
			 email_address );

		if ( system( sys_string ) ){};

		printf( "<h3>Sent to %s<hr></h3>\n", email_address );
	}
	else
	{
		appaserver_library_output_ftp_prompt(
			link_prompt, 
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}
}

void generic_output_spreadsheet_create(
			char *output_filename,
			char *input_system_string,
			char *output_system_string,
			char *heading,
			char *subtitle )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( input_system_string, "r" );

	sprintf(output_system_string,
		"cat > %s",
		output_filename );

	output_pipe = popen( output_system_string, "w" );

	printf( "<h2>%s</h2>\n", subtitle );

	fprintf(output_pipe,
		"%s\n",
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		search_replace_character( input_buffer, SQL_DELIMITER, ',' );
		fprintf(output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

