/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/export_process_set.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

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
#include "export_process_set.h"

EXPORT_PROCESS_SET *export_process_set_new(
		char *application_name,
		char *export_process_set_name,
		boolean exclude_roles_boolean,
		char *data_directory )
{
	EXPORT_PROCESS_SET *export_process_set;
	FILE *process_set_file;
	char *process_set_name;

	if ( !application_name
	||   !export_process_set_name
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

	export_process_set = export_process_set_calloc();

	export_process_set->name_list =
		export_process_set_name_list(
			export_process_set_name );

	if ( !list_rewind( export_process_set->name_list ) ) return NULL;

	export_process_set->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_process_set_filename(
			application_name,
			data_directory,
			date_now_yyyy_mm_dd(
				date_utc_offset() ) );

	process_set_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		export_process_set_file(
			export_process_set->
				filename );

	export_process_set->shell_script_open_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name );

	fprintf(process_set_file,
		"%s\n",
		export_process_set->
			shell_script_open_string );

	export_process_set->shell_script_here_open_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL );

	fprintf(process_set_file,
		"%s\n",
		export_process_set->
			shell_script_here_open_string );

	do {
		process_set_name =
			list_get(
				export_process_set->
					name_list );

		export_process_set_output_process_set(
			process_set_file,
			process_set_name );

		if ( !exclude_roles_boolean )
		{
			export_process_set_output_standard(
				ROLE_PROCESS_SET_MEMBER_TABLE,
				process_set_file,
				process_set_name );
		}

		export_process_set_output_standard(
			PROCESS_SET_PARAMETER_TABLE,
			process_set_file,
			process_set_name );

		export_process_set_output_prompt(
			process_set_file,
			process_set_name );

		export_process_set_output_drop_down_prompt(
			process_set_file,
			process_set_name );

		export_process_set_output_drop_down_prompt_data(
			process_set_file,
			process_set_name );

		export_process_set_output_process_group(
			process_set_file,
			process_set_name );

	} while ( list_next( export_process_set->name_list ) );

	export_process_set->shell_script_here_close_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			0 /* not tee_appaserver_boolean */,
			0 /* not html_paragraph_wrapper_boolean */ );

	fprintf(process_set_file,
		"%s\n",
		export_process_set->shell_script_here_close_string );

	export_process_set->shell_script_close_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string();

	fprintf(process_set_file,
		"%s\n",
		export_process_set->shell_script_close_string );

	fclose( process_set_file );

	export_process_set->execute_bit_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_process_set_execute_bit_system_string(
			export_process_set->filename );

	return export_process_set;
}

EXPORT_PROCESS_SET *export_process_set_calloc( void )
{
	EXPORT_PROCESS_SET *export_process_set;

	if ( ! ( export_process_set =
			calloc( 1,
				sizeof ( EXPORT_PROCESS_SET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_process_set;
}

LIST *export_process_set_name_list(
		char *export_process_set_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	list_string_list(
		export_process_set_name /* list_string */,
		',' /* delimiter */ );
}

char *export_process_set_filename(
		char *application_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd )
{
	static char filename[ 128 ];

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

	sprintf(filename,
		"%s/export_process_set_%s_%s.sh",
		data_directory,
		application_name,
		date_now_yyyy_mm_dd );

	return filename;
}

FILE *export_process_set_file( char *filename )
{
	FILE *file;

	if ( !filename )
	{
		char message[ 128 ];

		sprintf(message, "filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( file = fopen( filename, "w" ) ) )
	{
		char message[ 256 ];

		sprintf(message,
			"fopen(%s) for write returned empty.",
			filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return file;
}

void export_process_set_output_process_set(
		FILE *process_set_file,
		char *process_set_name )
{
	char *select;
	char *system_string;
	char *input;
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_set_select(
			PROCESS_SET_TABLE /* folder_name */ );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_set_select(%s) returned empty.",
			PROCESS_TABLE );

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
			PROCESS_SET_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_set_primary_where(
				process_set_name ) );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( input = string_pipe_input( system_string ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: string_pipe_input(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );

		return;
	}

	sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_string_sql_statement(
			SQL_DELIMITER,
			PROCESS_SET_TABLE,
			select /* attribute_name_list_string */,
			input /* delimited_string */ );

	fprintf(process_set_file,
		"%s\n",
		sql_statement );

	free( select );
	free( system_string );
	free( input );
	free( sql_statement );
}

void export_process_set_output_standard(
		const char *appaserver_table_name,
		FILE *process_set_file,
		char *process_set_name )
{
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_set_select(
			(char *)appaserver_table_name /* folder_name */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			(char *)appaserver_table_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_set_primary_where(
				process_set_name ) );

	input_pipe =
		export_process_set_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				(char *)appaserver_table_name,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_set_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( system_string );
}

FILE *export_process_set_input_pipe( char *appaserver_system_string )
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

char *export_process_set_select( char *folder_name )
{
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

	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	return folder_attribute_list_string( folder_name );
}

boolean export_process_set_exclude_roles_boolean(
		DICTIONARY *non_prefixed_dictionary )
{
	char *exclude_roles_yn =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			"exclude_roles_yn",
			non_prefixed_dictionary );

	if ( !exclude_roles_yn )
	{
		char message[ 128 ];

		sprintf(message,
			"dictionary_get(%s) returned empty.",
			"exclude_roles_yn" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return ( *exclude_roles_yn == 'y' );
}

void export_process_set_output_prompt(
		FILE *process_set_file,
		char *process_set_name )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_set_select(
			PROCESS_PARAMETER_PROMPT_TABLE );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_set_select(%s) returned empty.",
			PROCESS_PARAMETER_PROMPT_TABLE );

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
		export_process_set_prompt_where(
			process_set_name );

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
		export_process_set_input_pipe(
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

		fprintf(process_set_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_set_prompt_where( char *process_set_name )
{
	char where[ 1024 ];
	LIST *prompt_list;
	char *around_string;

	if ( !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "process_set_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_list =
		process_parameter_prompt_list(
			process_parameter_set_list(
				process_set_name ) );

	if ( !list_length( prompt_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			prompt_list );

	sprintf(where,
		"prompt in (%s)",
		around_string );

	free( around_string );

	return strdup( where );
}

void export_process_set_output_drop_down_prompt(
		FILE *process_set_file,
		char *process_set_name )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_set_select(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_set_select(%s) returned empty.",
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_TABLE );

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
		export_process_set_drop_down_prompt_where(
			process_set_name );

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
		export_process_set_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
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

		fprintf(process_set_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_set_drop_down_prompt_where( char *process_set_name )
{
	LIST *prompt_list;
	char *around_string;
	char where[ 1024 ];

	prompt_list =
		process_parameter_drop_down_prompt_list(
			process_parameter_set_list(
				process_set_name ) );

	if ( !list_length( prompt_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			prompt_list );

	sprintf(where,
		"drop_down_prompt in (%s)",
		around_string );

	free( around_string );

	return strdup( where );
}

void export_process_set_output_drop_down_prompt_data(
		FILE *process_set_file,
		char *process_set_name )
{
	FILE *input_pipe;
	char *select;
	char *where;
	char *system_string;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		export_process_set_select(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE );

	if ( !*select )
	{
		char message[ 128 ];

		sprintf(message,
			"export_process_set_select(%s) returned empty.",
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE );

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
		export_process_set_drop_down_prompt_data_where(
			process_set_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE,
			where );

	input_pipe =
		export_process_set_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
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

		fprintf(process_set_file,
			"%s\n",
			sql_statement );
	}

	pclose( input_pipe );
	free( select );
	free( where );
}

char *export_process_set_drop_down_prompt_data_where(
		char *process_set_name )
{
	LIST *prompt_list;
	char *around_string;
	LIST *in_clause_list;
	char where[ 1024 ];

	if ( !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "process_set_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_list =
		process_parameter_drop_down_prompt_list(
			process_parameter_set_list(
				process_set_name ) );

	if ( !list_length( prompt_list ) ) return (char *)0;

	in_clause_list =
		process_parameter_drop_down_prompt_data_in_clause_list(
			PROCESS_PARAMETER_DROP_DOWN_PROMPT_DATA_TABLE,
			prompt_list );

	if ( !list_length( in_clause_list ) ) return (char *)0;

	around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			in_clause_list );

	sprintf(where,
		"drop_down_prompt_data in (%s)",
		around_string );

	free( around_string );

	return strdup( where );
}

void export_process_set_output_process_group(
		FILE *process_set_file,
		char *process_set_name )
{
	char *system_string;
	char *input;
	char *sql_statement;

	if ( !process_set_file
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

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
			PROCESS_SET_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_set_primary_where(
				process_set_name ) );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( input = string_pipe_input( system_string ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: string_pipe_input(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );

		return;
	}

	sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_string_sql_statement(
			SQL_DELIMITER,
			PROCESS_PARAMETER_PROCESS_GROUP_TABLE,
			"process_group" /* attribute_name_list_string */,
			input /* delimited_string */ );

	fprintf(process_set_file,
		"%s\n",
		sql_statement );
}

char *export_process_set_execute_bit_system_string(
		char *export_process_set_filename )
{
	char system_string[ 1024 ];

	if ( !export_process_set_filename )
	{
		char message[ 128 ];

		sprintf(message, "export_process_set_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"chmod +x %s",
		export_process_set_filename );

	return strdup( system_string );
}

