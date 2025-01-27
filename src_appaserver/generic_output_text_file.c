/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_output_text_file.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "application.h"
#include "sql.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver.h"
#include "appaserver_link.h"
#include "post_dictionary.h"
#include "process_generic.h"

void generic_output_stdout(
			char *system_string,
			char *heading,
			char *sub_title );

void generic_output_text(
			char *system_string,
			char *heading,
			char *sub_title );

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *sub_title,
			int primary_key_list_length );

void generic_output_spreadsheet(
			char *input_system_string,
			char *heading,
			char *sub_title,
			APPASERVER_LINK *appaserver_link );

void generic_output_spreadsheet_create(
			char *input_system_string,
			char *output_filename,
			char *heading,
			char *sub_title );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_set_name;
	char *process_name;
	char *output_medium_string;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	char *generic_process_set_name;
	char *generic_process_name;
	PROCESS_GENERIC *process_generic;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s login_name process_set process_name output_medium dictionary\n",
			 argv[ 0 ] );

		fprintf( stderr,
	"Note: output_medium = {stdout,table,spreadsheet}\n" );

		exit ( 1 );
	}

	login_name = argv[ 1 ];
	process_set_name = argv[ 2 ];
	process_name = argv[ 3 ];
	output_medium_string = argv[ 4 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 5 ] /* dictionary_string */ );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary->original_post_dictionary
				/* post_dictionary */,
			generic_process_set_name );

	process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			generic_process_name,
			generic_process_set_name,
			output_medium_string,
			appaserver_parameter->data_directory,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	if ( process_generic->
		process_generic_input->
		output_medium != output_medium_stdout )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_generic->process_name );
	}

	if (	process_generic->process_generic_input->output_medium ==
		output_medium_stdout )
	{
		generic_output_stdout(
			process_generic->
				system_string,
			process_generic->
				heading,
			process_generic->
				sub_title );
	}
	else
	if (	process_generic->process_generic_input->output_medium ==
		spreadsheet )
	{
		generic_output_spreadsheet(
			process_generic->
				system_string
				/* input_system_string */,
			process_generic->
				heading,
			process_generic->
				sub_title,
			process_generic->
				process_generic_input->
				process_generic_link->
				appaserver_link );
	}
	else
	if (	process_generic->process_generic_input->output_medium ==
		text_file )
	{
		generic_output_text(
			process_generic->
				system_string,
			process_generic->
				heading,
			process_generic->
				sub_title );
	}
	else
	if (	process_generic->process_generic_input->output_medium ==
		table )
	{
		generic_output_table(
			process_generic->
				system_string
				/* input_system_string */,
			process_generic->
				heading,
			process_generic->
				sub_title,
			process_generic->
				process_generic_value->
				primary_key_list_length );
	}

	if (	process_generic->process_generic_input->output_medium !=
		output_medium_stdout )
	{
		document_close();
	}

	process_execution_count_increment( process_generic->process_name );

	return 0;
}

void generic_output_stdout(
			char *system_string,
			char *heading,
			char *sub_title )
{
	FILE *input_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( system_string, "r" );

	printf( "%s\n%s\n",
		sub_title,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		printf( "%s\n",
			string_search_replace_character(
				input_buffer /* source_destination */,
				SQL_DELIMITER /* search_character */,
				',' /* replace_character */ ) );
	}
	pclose( input_pipe );
}

void generic_output_text(
			char *system_string,
			char *heading,
			char *sub_title )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];

	input_pipe = popen( system_string, "r" );
	output_pipe = popen( "html_paragraph_wrapper", "w" );

	fprintf(output_pipe,
		"%s\n%s\n",
		sub_title,
		heading );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		fprintf(output_pipe,
			"%s\n",
			string_search_replace_character(
				input_buffer /* source_destination */,
				SQL_DELIMITER /* search_character */,
				',' /* replace_character */ ) );

	}
	pclose( input_pipe );
	pclose( output_pipe );
}

void generic_output_table(
			char *input_system_string,
			char *heading,
			char *sub_title,
			int primary_key_list_length )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 2048 ];
	char output_system_string[ 1024 ];

	sprintf(output_system_string,
		"html_table \"^%s\" \"%s\" '%c' %sright",
		sub_title,
		heading,
		SQL_DELIMITER,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_repeat(
			"left,",
			primary_key_list_length
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
			char *input_system_string,
			char *heading,
			char *sub_title,
			APPASERVER_LINK *appaserver_link )
{
	char *output_filename;
	char *prompt_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_filename =
		appaserver_link->
			appaserver_link_output->
			filename;

	prompt_filename =
		appaserver_link->
			appaserver_link_prompt->
			filename;

	generic_output_spreadsheet_create(
		input_system_string,
		output_filename,
		heading,
		sub_title );

	printf( "%s\n",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			prompt_filename,
			(char *)0 /* target_window */,
			APPLICATION_TRANSMIT_PROMPT
				/* prompt_message */ ) );
}

void generic_output_spreadsheet_create(
			char *input_system_string,
			char *output_filename,
			char *heading,
			char *sub_title )
{
	FILE *input_pipe;
	FILE *output_file;
	FILE *output_pipe;
	char input_buffer[ 2048 ];
	char output_buffer[ 2048 ];

	printf( "<h2>%s</h2>\n", sub_title );

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			output_filename );

	fprintf(output_file,
		"%s\n",
		string_double_quote_comma(
			output_buffer /* destination */,
			heading /* source */,
			',' /* delimiter */ ) );

	fclose( output_file );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_spreadsheet_output_system_string(
				SQL_DELIMITER,
				output_filename
				/* spreadsheet_filename */ ) );

	input_pipe = popen( input_system_string, "r" );

	while ( string_input( input_buffer, input_pipe, 2048 ) )
	{
		fprintf(output_pipe,
			"%s\n",
			string_double_quote_comma(
				output_buffer /* destination */,
				input_buffer /* source */,
				SQL_DELIMITER ) );
				
	}
	pclose( input_pipe );
	pclose( output_pipe );
}

