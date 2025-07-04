/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_table.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "application.h"
#include "folder_attribute.h"
#include "sql.h"
#include "insert.h"
#include "date.h"
#include "shell_script.h"
#include "html.h"
#include "export_table.h"

EXPORT_TABLE *export_table_new(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *where )
{
	EXPORT_TABLE *export_table;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	export_table = export_table_calloc();

	export_table->application_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_table_name(
			folder_name );

	export_table->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			folder_attribute_list(
				folder_name,
				(LIST *)0 /* exclude_attribute_name_list */,
				0 /* not fetch_attribute */,
				0 /* not cache_boolean */ ) );

	if ( !list_length( export_table->folder_attribute_name_list ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: folder_attribute_name_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );

		free( export_table );
		return (EXPORT_TABLE *)0;
	}

	export_table->appaserver_select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		appaserver_select(
			export_table->folder_attribute_name_list );

	export_table->appaserver_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			export_table->appaserver_select,
			export_table->application_table_name,
			where );

	if ( data_directory )
	{
		export_table->date_now_yyyy_mm_dd =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_yyyy_mm_dd(
				date_utc_offset() );

		export_table->shell_filespecification =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			export_table_shell_filespecification(
				application_name,
				folder_name,
				data_directory,
				export_table->date_now_yyyy_mm_dd );

		export_table->execute_bit_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			export_table_execute_bit_system_string(
				export_table->shell_filespecification );

		export_table->appaserver_spreadsheet_heading_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_spreadsheet_heading_string(
				export_table->folder_attribute_name_list );

		if ( !export_table->appaserver_spreadsheet_heading_string )
		{
			char message[ 128 ];
	
			sprintf(message,
		"appaserver_spreadsheet_heading_string() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	export_table->html_table_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		html_table_system_string(
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			SQL_DELIMITER,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			export_table_html_title(
				folder_name ),
			export_table->appaserver_select
				/* heading_string */ );

	export_table->insert_folder_statement_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_statement_system_string(
			SQL_DELIMITER,
			export_table->application_table_name,
			export_table->appaserver_select
				/* field_list_string */ );

	if ( application_name
	&&   data_directory )
	{
		export_table->appaserver_link =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_link_new(
				application_http_prefix(
					application_ssl_support_boolean(
						application_name ) ),
				application_server_address(),
				data_directory,
				folder_name /* filename_stem */,
				application_name,
				getpid() /* process_id */,
				(char *)0 /* session_key */,
				export_table->date_now_yyyy_mm_dd
					/* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" /* extension */ );

		export_table->appaserver_link_anchor_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_link_anchor_html(
				export_table->
					appaserver_link->
					appaserver_link_prompt->
					filename,
				"_new" /* target_window */,
				"Link to csv file" /* prompt_message */ );

		export_table->spreadsheet_filespecification =
			/* ----------------- */
			/* Returns parameter */
			/* ----------------- */
			export_table_spreadsheet_filespecification(
				export_table->
					appaserver_link->
					appaserver_link_output->
					filename );

		export_table->appaserver_spreadsheet_output_system_string =
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_spreadsheet_output_system_string(
					SQL_DELIMITER,
					export_table->
					     spreadsheet_filespecification ) );
	}

	return export_table;
}

EXPORT_TABLE *export_table_calloc( void )
{
	EXPORT_TABLE *export_table;

	if ( ! ( export_table = calloc( 1, sizeof ( EXPORT_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_table;
}

char *export_table_execute_bit_system_string( char *shell_filespecification )
{
	char system_string[ 256 ];

	if ( !shell_filespecification )
	{
		char message[ 128 ];

		sprintf(message, "shell_filespecification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"chmod +x %s",
		shell_filespecification );

	return strdup( system_string );
}

char *export_table_shell_filespecification(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd )
{
	char filespecification[ 128 ];

	if ( !application_name
	||   !folder_name
	||   !data_directory
	||   !date_now_yyyy_mm_dd )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/%s/export_table_%s_%s.sh",
		data_directory,
		application_name,
		folder_name,
		date_now_yyyy_mm_dd );

	return strdup( filespecification );
}

void export_table_shell_output(
		char *application_name,
		char *application_table_name,
		char *appaserver_select,
		char *appaserver_system_string,
		char *shell_filespecification,
		char *execute_bit_system_string,
		boolean output_prompt_boolean )
{
	FILE *input_pipe;
	FILE *output_file;
	char input[ 65536 ];
	char *tmp;

	if ( !application_table_name
	||   !appaserver_select
	||   !appaserver_system_string
	||   !shell_filespecification
	||   !execute_bit_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			appaserver_system_string );

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			shell_filespecification );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL ) );

	while ( string_input( input, input_pipe, 65536 ) )
	{
		fprintf(output_file,
			"%s\n",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = insert_folder_string_sql_statement(
					SQL_DELIMITER,
					application_table_name,
					appaserver_select
					/* attribute_name_list_string */,
					input /* delimited_string */ ) ) );

		free( tmp );
	}

	pclose( input_pipe );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			0 /* not tee_appaserver_boolean */,
			0 /* not html_paragraph_wrapper_boolean */ ) );

	fprintf(output_file,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	fclose( output_file );

	if ( system( execute_bit_system_string ) ) {}

	if ( output_prompt_boolean )
	{
		printf(	"<h3>Created: %s</h3>\n",
			shell_filespecification );
	}
}

void export_table_spreadsheet_output(
		char *appaserver_system_string,
		char *filespecification,
		char *heading_string,
		char *output_system_string,
		char *appaserver_link_anchor_html )
{
	FILE *output_file;
	FILE *output_pipe;
	FILE *input_pipe;
	char input[ 65536 ];

	if ( !appaserver_system_string
	||   !filespecification
	||   !heading_string
	||   !output_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			filespecification );

	fprintf(output_file,
		"%s\n",
		heading_string );

	fclose( output_file );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			output_system_string );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			appaserver_system_string );

	while ( string_input( input, input_pipe, 65536 ) )
	{
		fprintf(output_pipe,
			"%s\n",
			input );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	if ( appaserver_link_anchor_html )
	{
		printf( "%s\n",
			appaserver_link_anchor_html );
	}
}

void export_table_html_output(
		char *appaserver_system_string,
		char *html_table_system_string )
{
	FILE *output_pipe;
	FILE *input_pipe;
	char input[ 65536 ];

	if ( !appaserver_system_string
	||   !html_table_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			html_table_system_string );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			appaserver_system_string );

	while ( string_input( input, input_pipe, 65536 ) )
	{
		fprintf(output_pipe,
			"%s\n",
			input );
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

LIST *export_table_shell_filename_list( LIST *export_table_list )
{
	LIST *filename_list = list_new();
	EXPORT_TABLE *export_table;

	if ( list_rewind( export_table_list ) )
	do {
		export_table = list_get( export_table_list );

		if ( !export_table->shell_filespecification )
		{
			char message[ 128 ];

			sprintf(message,
				"shell_filespecification is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			filename_list,
			export_table->shell_filespecification );

	} while ( list_next( export_table_list ) );

	return filename_list;
}

LIST *export_table_spreadsheet_filename_list( LIST *export_table_list )
{
	LIST *filename_list = list_new();
	EXPORT_TABLE *export_table;

	if ( list_rewind( export_table_list ) )
	do {
		export_table = list_get( export_table_list );

		if ( !export_table->spreadsheet_filespecification )
		{
			char message[ 128 ];

			sprintf(message,
			"spreadsheet_filespecification is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			filename_list,
			export_table->spreadsheet_filespecification );

	} while ( list_next( export_table_list ) );

	return filename_list;
}

char *export_table_html_title( char *folder_name )
{
	static char title[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_initial_capital(
		title,
		folder_name );
}

char *export_table_spreadsheet_filespecification(
		char *appaserver_link_output_filename )
{
	return appaserver_link_output_filename;
}

void export_table_insert_statement_output(
		char *appaserver_system_string,
		char *insert_folder_statement_system_string,
		char *output_pipe_string )
{
	char *output_system_string;
	FILE *output_pipe;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !appaserver_system_string
	||   !insert_folder_statement_system_string
	||   !output_pipe_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_table_output_system_string(
			insert_folder_statement_system_string,
			output_pipe_string );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			output_system_string );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			appaserver_system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		fprintf(output_pipe,
			"%s\n",
			input );
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

char *export_table_output_system_string(
		char *insert_folder_statement_system_string,
		char *output_pipe_string )
{
	char output_system_string[ 1024 ];

	if ( !insert_folder_statement_system_string
	||   !output_pipe_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		output_system_string,
		sizeof ( output_system_string ),
		"%s %s",
		insert_folder_statement_system_string,
		output_pipe_string );

	return strdup( output_system_string );
}

