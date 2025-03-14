/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/export_process.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "appaserver.h"
#include "folder.h"
#include "folder_attribute.h"
#include "dictionary.h"
#include "insert.h"
#include "sql.h"
#include "date.h"
#include "process_parameter.h"
#include "role.h"
#include "shell_script.h"
#include "export_process.h"

EXPORT_PROCESS_SHELL *export_process_shell_new(
		char *application_name,
		char *export_process_name,
		boolean exclude_roles_boolean,
		char *data_directory )
{
	EXPORT_PROCESS_SHELL *export_process_shell;
	FILE *process_stream;
	char *process_name;

	if ( !application_name
	||   !export_process_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	export_process_shell = export_process_shell_calloc();

	export_process_shell->name_list =
		export_process_shell_name_list(
			export_process_name,
			'^' );

	if ( !list_rewind( export_process_shell->name_list ) ) return NULL;

	export_process_shell->filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_process_shell_filespecification(
			application_name,
			data_directory,
			date_now_yyyy_mm_dd( date_utc_offset() ) );

	process_stream =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		export_process_stream(
			export_process_shell->filespecification,
			"w" /* mode */ );

	export_process_shell->shell_script_open_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name );

	fprintf(process_stream,
		"%s\n",
		export_process_shell->shell_script_open_string );

	export_process_shell->shell_script_here_open_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL );

	fprintf(process_stream,
		"%s\n",
		export_process_shell->shell_script_here_open_string );

	do {
		process_name =
			list_get(
				export_process_shell->name_list );

		export_process_output_process(
			process_stream,
			process_name );

		if ( !exclude_roles_boolean )
		{
			export_process_output_standard(
				ROLE_PROCESS_TABLE,
				process_stream,
				process_name );
		}

		export_process_output_standard(
			PROCESS_PARAMETER_TABLE,
			process_stream,
			process_name );

		export_process_shell->process_parameter_list =
			process_parameter_list(
				process_name,
				(char *)0 /* process_set_name */,
				0 /* not is_drillthru */ );

		export_process_output_prompt(
			process_stream,
			export_process_shell->process_parameter_list );

		export_process_output_drop_down_prompt(
			process_stream,
			export_process_shell->process_parameter_list );

		export_process_output_drop_down_prompt_data(
			process_stream,
			export_process_shell->process_parameter_list );

		export_process_output_process_group(
			process_stream,
			process_name );

		export_process_output_populate_name(
			process_stream,
			export_process_shell->process_parameter_list,
			0 /* not helper_name_boolean */ );

		export_process_output_populate_name(
			process_stream,
			export_process_shell->process_parameter_list,
			1 /* helper_name_boolean */ );

	} while ( list_next( export_process_shell->name_list ) );

	export_process_shell->shell_script_here_close_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			0 /* not tee_appaserver_boolean */,
			0 /* not html_paragraph_wrapper_boolean */ );

	fprintf(process_stream,
		"%s\n",
		export_process_shell->shell_script_here_close_string );

	export_process_shell->shell_script_close_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string();

	fprintf(process_stream,
		"%s\n",
		export_process_shell->shell_script_close_string );

	fclose( process_stream );

	export_process_shell->shell_script_execute_bit_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		shell_script_execute_bit_system_string(
			export_process_shell->filespecification );

	return export_process_shell;
}

EXPORT_PROCESS_SHELL *export_process_shell_calloc( void )
{
	EXPORT_PROCESS_SHELL *export_process_shell;

	if ( ! ( export_process_shell =
			calloc( 1,
				sizeof ( EXPORT_PROCESS_SHELL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_process_shell;
}

LIST *export_process_shell_name_list(
		char *export_process_name,
		const char delimiter )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	list_string_list(
		export_process_name /* list_string */,
		(char )delimiter );
}

char *export_process_shell_filespecification(
		char *application_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd )
{
	static char filespecification[ 128 ];

	if ( !application_name
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
		"%s/%s/export_process_%s_%s.sh",
		data_directory,
		application_name,
		application_name,
		date_now_yyyy_mm_dd );

	return filespecification;
}

FILE *export_process_stream(
		char *filespecification,
		const char *mode )
{
	FILE *stream;

	if ( !filespecification )
	{
		char message[ 128 ];

		sprintf(message, "filespecification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( stream = fopen( filespecification, mode ) ) )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
			"fopen(%s,%s) returned empty.",
			filespecification,
			mode );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return stream;
}

void export_process_output_process(
		FILE *process_stream,
		char *process_name )
{
	char *sql_statement;
	char *where_string;
	char *select;
	char *system_string;
	char *input;
	char *escape;

	if ( !process_stream
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( process_stream ) fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_primary_where(
			process_name );

	sql_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_process_delete_sql_statement(
			PROCESS_TABLE,
			where_string );

	fprintf(process_stream,
		"%s\n",
		sql_statement );

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_select(
			PROCESS_TABLE /* folder_name */ );

	if ( !*select )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"export_process_select(%s) returned empty.",
			PROCESS_TABLE );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			PROCESS_TABLE,
			where_string );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( input = string_pipe_input( system_string ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_input(%s) returned empty.",
			system_string );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	escape =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_process_escape(
			input );

	sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_string_sql_statement(
			SQL_DELIMITER,
			PROCESS_TABLE,
			select /* attribute_name_list_string */,
			escape /* delimited_string */ );

	fprintf(process_stream,
		"%s\n",
		sql_statement );

	free( select );
	free( system_string );
	free( input );
	free( sql_statement );
}

void export_process_output_standard(
		char *table_name,
		FILE *process_stream,
		char *process_name )
{
	char *where_string;
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	char *sql_statement;

	if ( !table_name
	||   !process_stream
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( process_stream ) fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_primary_where(
			process_name );

	sql_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_process_delete_sql_statement(
			table_name,
			where_string );

	fprintf(process_stream,
		"%s\n",
		sql_statement );

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_select(
			table_name /* folder_name */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			table_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_primary_where(
				process_name ) );

	input_pipe =
		export_process_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				table_name,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_stream,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );

	free( select );
	free( system_string );
}

FILE *export_process_input_pipe( char *appaserver_system_string )
{
	if ( !appaserver_system_string )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( appaserver_system_string, "r" );
}

char *export_process_select( char *table_name )
{
	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	return
	folder_attribute_list_string(
		table_name /* folder_name */ );
}

void export_process_output_prompt(
		FILE *process_stream,
		LIST *process_parameter_list )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_stream )
	{
		char message[ 128 ];

		sprintf(message, "process_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( process_parameter_list ) ) return;

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_select(
			PROCESS_PARAMETER_PROMPT_TABLE
				/* table_name */ );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_select(%s) returned empty.",
			PROCESS_PARAMETER_PROMPT_TABLE );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		export_process_prompt_where(
			process_parameter_list );

	if ( !where ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			PROCESS_PARAMETER_PROMPT_TABLE,
			where );

	input_pipe =
		export_process_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_PARAMETER_PROMPT_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_stream,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_prompt_where( LIST *process_parameter_list )
{
	char where[ 1024 ];
	LIST *prompt_list;
	char *around_string;

	if ( !list_length( process_parameter_list ) ) return (char *)0;

	prompt_list =
		process_parameter_prompt_list(
			process_parameter_list );

	if ( !list_length( prompt_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			prompt_list );

	snprintf(
		where,
		sizeof ( where ),
		"prompt in (%s)",
		around_string );

	free( around_string );

	return strdup( where );
}

void export_process_output_drop_down_prompt(
		FILE *process_stream,
		LIST *process_parameter_list )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_stream )
	{
		char message[ 128 ];

		sprintf(message, "process_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( process_parameter_list ) ) return;

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_select(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE
				/* table_name */ );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_select(%s) returned empty.",
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		export_process_drop_down_prompt_where(
			process_parameter_list );

	if ( !where ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE,
			where );

	input_pipe =
		export_process_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_stream,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_drop_down_prompt_where( LIST *process_parameter_list )
{
	LIST *prompt_list;
	char *around_string;
	char where[ 1024 ];

	prompt_list =
		process_parameter_drop_down_prompt_list(
			process_parameter_list );

	if ( !list_length( prompt_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			prompt_list );

	snprintf(
		where,
		sizeof ( where ),
		"drop_down_prompt in (%s)",
		around_string );

	free( around_string );

	return strdup( where );
}

void export_process_output_drop_down_prompt_data(
		FILE *process_stream,
		LIST *process_parameter_list )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_stream )
	{
		char message[ 128 ];

		sprintf(message, "process_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( process_parameter_list ) ) return;

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_select(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE
				/* table_name */ );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_select(%s) returned empty.",
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		export_process_drop_down_prompt_data_where(
			process_parameter_list );

	if ( !where ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE,
			where );

	input_pipe =
		export_process_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_stream,
			"%s\n",
			sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_drop_down_prompt_data_where(
		LIST *process_parameter_list )
{
	LIST *prompt_list;
	char *around_string;
	char subquery[ 512 ];
	char where[ 1024 ];

	if ( !list_length( process_parameter_list ) ) return (char *)0;

	prompt_list =
		process_parameter_drop_down_prompt_list(
			process_parameter_list );

	if ( !list_length( prompt_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			prompt_list );

	snprintf(
		subquery,
		sizeof ( subquery ),
		"select drop_down_prompt "
		"from drop_down_prompt "
		"where drop_down_prompt in (%s)",
		around_string );

	snprintf(
		where,
		sizeof ( where ),
		"drop_down_prompt in (%s)",
		subquery );

	free( around_string );

	return strdup( where );
}

void export_process_output_process_group(
		FILE *process_stream,
		char *process_name )
{
	char *system_string;
	char *input;
	char *sql_statement;

	if ( !process_stream
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( process_stream ) fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			"process_group" /* select */,
			PROCESS_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_primary_where(
				process_name ) );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( input = string_pipe_input( system_string ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_input(%s) returned empty.",
			system_string );

		fclose( process_stream );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( *input )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
				insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_PARAMETER_PROCESS_GROUP_TABLE,
				"process_group"
					/* attribute_name_list_string */,
				input	/* delimited_string */ );

		fprintf(process_stream,
			"%s\n",
			sql_statement );
	}
}

char *export_process_delete_sql_statement(
		char *table_name,
		char *where_string )
{
	static char sql_statement[ 128 ];

	if ( !table_name
	||   !where_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		sql_statement,
		sizeof ( sql_statement ),
		"delete from %s where %s;",
		table_name,
		where_string );

	return sql_statement;
}

char *export_process_escape( char *string_pipe_input )
{
	char destination[ 1024 ];

	if ( !string_pipe_input ) return NULL;

	return
	string_escape_character(
		destination,
		string_pipe_input /* data */,
		'$' /* character */ );
}

EXPORT_PROCESS_FILE *export_process_file_new(
		char *filespecification,
		LIST *process_name_list )
{
	EXPORT_PROCESS_FILE *export_process_file;
	char *process_name;
	FILE *process_stream;

	if ( !filespecification
	||   !list_rewind( process_name_list ) )
	{
		return NULL;
	}

	export_process_file = export_process_file_calloc();

	process_stream =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		export_process_stream(
			filespecification,
			"a" /* mode */ );

	do {
		process_name = list_get( process_name_list );

		export_process_output_process(
			process_stream,
			process_name );
	
		export_process_output_standard(
			PROCESS_PARAMETER_TABLE,
			process_stream,
			process_name );
	
		export_process_file->process_parameter_list =
			process_parameter_list(
				process_name,
				(char *)0 /* process_set_name */,
				0 /* not is_drillthru */ );
	
		export_process_output_prompt(
			process_stream,
			export_process_file->process_parameter_list );
	
		export_process_output_drop_down_prompt(
			process_stream,
			export_process_file->process_parameter_list );
	
		export_process_output_drop_down_prompt_data(
			process_stream,
			export_process_file->process_parameter_list );
	
		export_process_output_process_group(
			process_stream,
			process_name );

	} while ( list_next( process_name_list ) );

	fclose( process_stream );

	return export_process_file;
}

EXPORT_PROCESS_FILE *export_process_file_calloc( void )
{
	EXPORT_PROCESS_FILE *export_process_file;

	if ( ! ( export_process_file =
			calloc( 1,
				sizeof ( EXPORT_PROCESS_FILE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_process_file;
}

void export_process_output_populate_name(
		FILE *export_process_stream,
		LIST *process_parameter_list,
		boolean helper_name_boolean )
{
	PROCESS_PARAMETER *process_parameter;
	char *attribute_name_list_string =
		"process,command_line";
	char *populate_process;
	char *command_line_fetch;
	char delimited_string[ 2048 ];
	char *escape_dollar;
	char *sql_statement;

	if ( list_rewind( process_parameter_list ) )
	do {
		process_parameter =
			list_get(
				process_parameter_list );

		if ( helper_name_boolean )
		{
			populate_process =
				process_parameter->
					populate_helper_process_name;
		}
		else
		{
			populate_process =
				process_parameter->
					populate_drop_down_process_name;
		}

		if ( !populate_process ) continue;

		command_line_fetch =
			/* ---------------------------------------- */
			/* Returns component of static process_list */
			/* ---------------------------------------- */
			process_command_line_fetch(
				populate_process /* process_name */ );

		escape_dollar =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_escape_dollar(
				command_line_fetch );

		snprintf(
			delimited_string,
			sizeof ( delimited_string ),
			"%s%c%s",
			populate_process,
			SQL_DELIMITER,
			escape_dollar );

		free( escape_dollar );

		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_TABLE,
				attribute_name_list_string,
				delimited_string );

		fprintf(
			export_process_stream,
			"%s\n",
			sql_statement );

		free( sql_statement );

	} while ( list_next( process_parameter_list ) );
}

