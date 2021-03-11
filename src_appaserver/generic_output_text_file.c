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
void generic_output_text_file(
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

	process_generic = process_generic_calloc();

	process_generic->process_set_name = process_set_name;
	process_generic->output_medium_string = output_medium_string;
	process_generic->post_dictionary = post_dictionary;

	if ( ! ( process_generic->process_name =
			process_generic_process_name(
				process_generic->process_set_name,
				process_generic->post_dictionary ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: process_generic_process_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( process_generic->value_folder_name =
			process_generic_value_folder_name(
				process_generic->process_name,
				process_generic->process_set_name ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: process_generic_value_folder_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( process_generic->value_folder =
			process_generic_value_folder_fetch(
				process_generic->value_folder_name,
				process_generic->post_dictionary ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: process_generic_value_folder_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic->parameter =
		/* ----------- */
		/* Never fails */
		/* ----------- */
		process_generic_parameter_parse(
			output_medium_string,
			post_dictionary,
			process_generic->
				value_folder->
				datatype->
				aggregation_sum,
				argv[ 0 ] );

	process_generic->process_generic_date_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_date_where(
			process_generic->
				value_folder->
				date_attribute_name,
			process_generic->
				parameter->
				begin_date,
			process_generic->
				parameter->
				end_date );

	process_generic->process_generic_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_where(
			process_generic->
				value_folder->
				foreign_folder->
				foreign_attribute_name_list,
			process_generic->
				value_folder->
				foreign_folder->
				foreign_attribute_data_list,
			process_generic->
				process_generic_date_where );

	if ( !process_generic->process_generic_where )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: process_generic_where() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic->process_generic_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_system_string(
			process_generic_select(
				process_generic->
					value_folder->
					primary_attribute_name_list,
				process_generic->
					value_folder->
					value_attribute_name ),
			process_generic->value_folder_name,
			process_generic->process_generic_where,
			process_generic->
				parameter->
				aggregate_level,
			process_generic->
				parameter->
				aggregate_statistic,
			process_generic->
				parameter->
				accumulate,
			process_generic->
				value_folder->
				date_attribute_name,
			process_generic->
				value_folder->
				time_attribute_name,
			process_generic->
				parameter->
				end_date,
			process_generic->
				value_folder->
				primary_attribute_name_list );

	if ( !process_generic->process_generic_system_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: process_generic_system_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic->
		process_generic_heading =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_generic_heading(
				process_generic->
					value_folder->
					primary_attribute_name_list,
				process_generic->
					value_folder->
					value_attribute_name,
				process_generic->
					value_folder->
					time_attribute_name,
				process_generic->
					value_folder->
					datatype->
					unit,
				process_generic->
					parameter->
					aggregate_level,
				process_generic->
					parameter->
					aggregate_statistic,
				process_generic->
					parameter->
					accumulate );

	process_generic->
		process_generic_subtitle =
			/* Returns static memory */
			/* --------------------- */
			process_generic_subtitle(
				process_generic->value_folder_name,
				process_generic->
					parameter->
					begin_date,
				process_generic->
					parameter->
					end_date,
				process_generic->
					parameter->
					aggregate_level,
				process_generic->
					parameter->
					aggregate_statistic );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if (	process_generic->parameter->output_medium !=
		output_medium_stdout )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );

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

	if (	process_generic->parameter->output_medium !=
		output_medium_stdout )
	{
		document_close();
	}

#ifdef NOT_DEFINED
	APPASERVER_LINK_FILE *appaserver_link_file;
	char delimiter;
	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
	 		process_generic->
				value_folder->
				value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->application_name = application_name;
	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date_suffix;

	if ( output_medium == table )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		char justify_column_list_string[ 128 ];
		char title[ 512 ];
		int i;
		int left_column_count;

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

		fflush( stdout );

		input_pipe = popen( sys_string, "r" );
		get_line( input_buffer, input_pipe );

		/* Headings need to be comma delimited */
		/* ----------------------------------- */
		search_replace_character( input_buffer, '^', ',' );

		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time )
		{
			left_column_count = length_select_list - 1;
		}
		else
		{
			left_column_count = length_select_list - 2;
		}

		for ( i = 0; i < left_column_count; i++ )
		{
			if ( i == 0 )
				strcpy(	justify_column_list_string,
					"left" );
			else
				strcat(	justify_column_list_string,
					",left" );
		}

		strcat( justify_column_list_string, ",right" );

		/* The frequency per period */
		/* ------------------------ */
		if ( aggregate_level != real_time )
		{
			strcat( justify_column_list_string, ",right" );
		}

		sprintf(title,
			"%s: %s",
			process_generic->
				value_folder->value_folder_name,
			where_clause );

		format_initial_capital( title, title );

		sprintf(sys_string,
			"queue_top_bottom_lines.e %d		|"
			"html_table.e \"%s\" \"%s\" '^' %s	 ",
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			title,
		        input_buffer,
			justify_column_list_string );	

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( process_generic->
				value_folder->
				time_attribute_name
			&&   *process_generic->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, '^', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
		document_close();
	}
	else
	if ( output_medium == text_file )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		char *output_filename;
		char *link_prompt;
		FILE *output_pipe;
		boolean first_time = 1;

		appaserver_link_file->extension = "txt";

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

		input_pipe = popen( sys_string, "r" );

		/* The frequency per period */
		/* ------------------------ */
		sprintf(sys_string,
			"tr '%c' '%c' > %s",
			FOLDER_DATA_DELIMITER,
			OUTPUT_TEXT_FILE_DELIMITER,
			output_filename );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( first_time )
			{
				search_replace_character(
					input_buffer,
					' ',
					'_' );
				fprintf( output_pipe, "%s\n", input_buffer );
				first_time = 0;
				continue;
			}

			if ( process_generic->
				value_folder->
				time_attribute_name
			&&   *process_generic->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, '^', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

		email_address =
			dictionary_get_index_zero(
				post_dictionary,
				"email_address" );

		if ( email_address
		&&   *email_address
		&&   strcmp( email_address, "email_address" ) != 0 ) 
		{
			char sys_string[ 1024 ];
	
			search_replace_string(	where_clause,
						"1 = 1 and ",
						"" );

			sprintf( sys_string,
				 "cat %s				|"
				 "mailx -s \"%s %s\" %s	 		 ",
				 output_filename,
				 where_clause,
				 process_generic->
					value_folder->value_folder_name,
				 email_address );
			system( sys_string );

			printf( "<h1>%s Transmission<br></h1>\n",
				process_generic->
					value_folder->value_folder_name );
			printf( "<BR><p>Search criteria: %s<hr>\n",
				where_clause );

			printf( "<BR><p>Sent to %s<hr>\n", email_address );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		}
		else
		{
			char buffer[ 1024 ];

			printf( "<h1>%s Transmission<br></h1>\n",
				format_initial_capital(
					buffer,
					process_generic->
					value_folder->value_folder_name ) );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s<hr>\n",
				search_replace_string(	where_clause,
							"1 = 1 and ",
							"" ) );

			appaserver_library_output_ftp_prompt(
					link_prompt, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
		document_close();
	}
	else
	if ( output_medium == spreadsheet )
	{
		FILE *input_pipe;
		char *output_filename;
		char *link_prompt;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		boolean first_time = 1;

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

		input_pipe = popen( sys_string, "r" );

		sprintf(sys_string,
			"cat > %s",
			output_filename );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_character(
				input_buffer,
				'^',
				',' );

			if ( first_time )
			{
				fprintf( output_pipe, "%s\n", input_buffer );
				first_time = 0;
				continue;
			}

			if ( process_generic->
				value_folder->
				time_attribute_name
			&&   *process_generic->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, ',', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

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

		email_address =
			dictionary_get_index_zero(
				post_dictionary,
				"email_address" );

		if ( email_address
		&&   *email_address
		&&   strcmp( email_address, "email_address" ) != 0 ) 
		{
			char sys_string[ 1024 ];
	
			search_replace_string(	where_clause,
						"1 = 1 and ",
						"" );

			sprintf( sys_string,
				 "cat %s				|"
				 "mailx -s \"%s %s\" %s	 		 ",
				 output_filename,
				 where_clause,
				 process_generic->
					value_folder->value_folder_name,
				 email_address );
			system( sys_string );

			printf( "<h1>%s Transmission<br></h1>\n",
				process_generic->
					value_folder->value_folder_name );
			printf( "<BR><p>Search criteria: %s<hr>\n",
				where_clause );

			printf( "<BR><p>Sent to %s<hr>\n", email_address );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		}
		else
		{
			char buffer[ 1024 ];

			printf( "<h1>%s Transmission<br></h1>\n",
				format_initial_capital(
					buffer,
					process_generic->
					value_folder->value_folder_name ) );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s<hr>\n",
				search_replace_string(	where_clause,
							"1 = 1 and ",
							"" ) );

			appaserver_library_output_ftp_prompt(
					link_prompt, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		boolean first_time = 1;

		input_pipe = popen( sys_string, "r" );

		/* The frequency per period */
		/* ------------------------ */
		sprintf(sys_string,
			"tr '^' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( first_time )
			{
				search_replace_character(
					input_buffer,
					' ',
					'_' );
				fprintf( output_pipe, "#" );
				first_time = 0;
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
	}
#endif

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;
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

