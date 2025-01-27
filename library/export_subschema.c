/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_subschema.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "appaserver.h"
#include "folder.h"
#include "relation.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "folder_operation.h"
#include "role_folder.h"
#include "row_security_role_update.h"
#include "folder_row_level_restriction.h"
#include "foreign_attribute.h"
#include "dictionary.h"
#include "insert.h"
#include "sql.h"
#include "date.h"
#include "shell_script.h"
#include "export_subschema.h"

EXPORT_SUBSCHEMA *export_subschema_new(
		char *application_name,
		char *folder_name,
		boolean exclude_roles_boolean,
		char *data_directory )
{
	EXPORT_SUBSCHEMA *export_subschema;
	FILE *process_file;

	if ( !application_name
	||   !folder_name
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

	export_subschema = export_subschema_calloc();

	export_subschema->folder_name_list =
		export_subschema_folder_name_list(
			SQL_DELIMITER,
			folder_name );

	if ( !list_rewind( export_subschema->folder_name_list ) )
	{
		free( export_subschema );
		return NULL;
	}

	export_subschema->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		export_subschema_filename(
			application_name,
			data_directory,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_yyyy_mm_dd(
				date_utc_offset() ) );

	process_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		export_subschema_process_file(
			export_subschema->filename );

	fprintf(process_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	fprintf(process_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL ) );

	do {
		folder_name =
			list_get(
				export_subschema->folder_name_list );

		export_subschema_output_folder(
			process_file,
			folder_name );

		export_subschema_output_relation(
			process_file,
			folder_name );

		export_subschema_output_standard(
			"table_column" /* appaserver_folder_name */,
			process_file,
			folder_name );

		export_subschema_output_attribute(
			process_file,
			folder_name );

		export_subschema_output_operation(
			process_file,
			folder_name );

		export_subschema_output_standard(
			"table_operation" /* appaserver_folder_name */,
			process_file,
			folder_name );

		if ( !exclude_roles_boolean )
		{
			export_subschema_output_standard(
				"role_table" /* appaserver_folder_name */,
				process_file,
				folder_name );
		}

		export_subschema_output_standard(
			"table_row_level_restriction"
				/* appaserver_folder_name */,
			process_file,
			folder_name );

		export_subschema_output_standard(
			"row_security_role_update"
				/* appaserver_folder_name */,
			process_file,
			folder_name );

		export_subschema_output_standard(
			"foreign_column" /* appaserver_folder_name */,
			process_file,
			folder_name );

		export_subschema_output_process(
			process_file,
			"post_change_process"
				/* process_column_name */,
			folder_name );

		export_subschema_output_process(
			process_file,
			"populate_drop_down_process"
				/* process_column_name */,
			folder_name );

	} while ( list_next( export_subschema->folder_name_list ) );

	fprintf(process_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			0 /* not tee_appaserver_boolean */,
			0 /* not html_paragraph_wrapper_boolean */ ) );

	fprintf(process_file,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	fclose( process_file );

	export_subschema->appaserver_execute_bit_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_execute_bit_system_string(
			export_subschema->filename
				/* shell_filename */ );

	return export_subschema;
}

EXPORT_SUBSCHEMA *export_subschema_calloc( void )
{
	EXPORT_SUBSCHEMA *export_subschema;

	if ( ! ( export_subschema = calloc( 1, sizeof ( EXPORT_SUBSCHEMA ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return export_subschema;
}

LIST *export_subschema_folder_name_list(
		const char sql_delimiter,
		char *folder_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	list_string_list(
		folder_name /* list_string */,
		sql_delimiter );
}

char *export_subschema_filename(
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
		"%s/export_subschema_%s_%s.sh",
		data_directory,
		application_name,
		date_now_yyyy_mm_dd );

	return filename;
}

FILE *export_subschema_process_file( char *filename )
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

void export_subschema_output_folder(
		FILE *process_file,
		char *folder_name )
{
	char *select;
	char *system_string;
	char *input;
	char *sql_statement;

	if ( !process_file
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		if ( process_file ) fclose( process_file );
	}

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_subschema_select(
			"table" /* appaserver_folder_name */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			FOLDER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_primary_where_string(
				FOLDER_PRIMARY_KEY,
				folder_name ) );

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
			FOLDER_TABLE,
			select /* attribute_name_list_string */,
			input /* delimited_string */ );

	fprintf(process_file,
		"%s\n",
		sql_statement );

	free( select );
	free( system_string );
	free( input );
	free( sql_statement );
}

void export_subschema_output_relation(
		FILE *process_file,
		char *folder_name )
{
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_file
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		if ( process_file ) fclose( process_file );
	}

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_subschema_select(
			"relation" /* appaserver_folder_name */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			RELATION_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			relation_mto1_where_string(
				FOLDER_PRIMARY_KEY,
				folder_name /* many_folder_name */ ) );

	input_pipe =
		export_subschema_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				RELATION_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( system_string );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			RELATION_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			relation_one2m_where_string(
				RELATION_FOREIGN_KEY
					/* probably related_table */,
				folder_name /* one_folder_name */ ) );

	input_pipe =
		export_subschema_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				RELATION_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );
	free( system_string );
	free( select );
}

char *export_subschema_select( char *appaserver_folder_name )
{
	if ( !appaserver_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( appaserver_folder_name, "folder" ) == 0 )
	{
		char message[ 128 ];

		sprintf(message, "invalid appaserver_folder_name=folder." );

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
		appaserver_folder_name );
}

FILE *export_subschema_input_pipe( char *appaserver_system_string )
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

char *export_subschema_attribute_where(
		const char *attribute_primary_key,
		char *folder_name )
{
	LIST *name_list;
	char *single_quotes_around_string;
	char where[ 1024 ];

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

	name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			folder_attribute_list(
				(char *)0 /* role_name */,
				folder_name,
				(LIST *)0 /* exclude_attribute_name_list */,
				0 /* not fetch_attribute */ ) );

	if ( !list_length( name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_list(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	single_quotes_around_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_single_quotes_around_string(
			name_list );

	sprintf(where,
		"%s in (%s)",
		attribute_primary_key,
		single_quotes_around_string );

	free( single_quotes_around_string );

	return strdup( where );
}

void export_subschema_output_attribute(
		FILE *process_file,
		char *folder_name )
{
	char *select;
	char *where;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	char *sql_statement;

	if ( !process_file
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		if ( process_file ) fclose( process_file );
	}

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_subschema_select(
			"column" /* appaserver_folder_name */ );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_subschema_attribute_where(
			ATTRIBUTE_PRIMARY_KEY,
			folder_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			ATTRIBUTE_TABLE,
			where );

	input_pipe =
		export_subschema_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				ATTRIBUTE_TABLE,
				select /* attribute_name_list_string */,
				input /* delimited_string */ );

		fprintf(process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );

	free( system_string );
	free( where );
	free( select );
}

void export_subschema_output_standard(
		const char *appaserver_folder_name,
		FILE *process_file,
		char *folder_name )
{
	char *select;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	char *sql_statement;
	char *table_name;

	if ( !process_file
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( process_file ) fclose( process_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( folder_name, "folder" ) == 0 )
	{
		char message[ 128 ];

		sprintf(message, "invalid folder_name=folder." );

		if ( process_file ) fclose( process_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		export_subschema_select(
			(char *)appaserver_folder_name );

	table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_table_name(
			(char *)appaserver_folder_name );

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
			folder_primary_where_string(
				FOLDER_PRIMARY_KEY,
				folder_name ) );

	input_pipe =
		export_subschema_input_pipe(
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

		fprintf(process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}

	pclose( input_pipe );

	free( select );
	free( system_string );
}

void export_subschema_output_subschema(
		FILE *process_file,
		char *folder_name )
{
	char *system_string;
	char *input;
	char *sql_statement;

	if ( !process_file
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		if ( process_file ) fclose( process_file );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			"subschema" /* select */,
			FOLDER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_primary_where_string(
				FOLDER_PRIMARY_KEY,
				folder_name ) );

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
			FOLDER_TABLE,
			"subschema" /* attribute_name_list_string */,
			input /* delimited_string */ );

	fprintf(process_file,
		"%s\n",
		sql_statement );

	free( system_string );
	free( input );
	free( sql_statement );
}

void export_subschema_output_process(
		FILE *export_subschema_process_file,
		const char *process_column_name,
		char *folder_name )
{
	char *column_fetch;

	if ( !export_subschema_process_file )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"export_subschema_process_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		fclose( export_subschema_process_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	column_fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		folder_column_fetch(
			process_column_name,
			folder_name );

	if ( column_fetch )
	{
		char *command_line_fetch;
		char *process_string;
		char *sql_statement;

		command_line_fetch =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_command_line_fetch(
				column_fetch /* process_name */ );

		process_string =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			export_subschema_process_string(
				SQL_DELIMITER,
				column_fetch
					/* process_name */,
				command_line_fetch
					/* command_line */ );

		if ( !process_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"export_subschema_process_string() returned empty." );

			fclose( export_subschema_process_file );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_TABLE,
				"process,command_line"
					/* attribute_name_list_string */,
				process_string
					/* delimited_string */ );

		fprintf(export_subschema_process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );
	}
}

char *export_subschema_process_string(
		const char sql_delimiter,
		char *process_name,
		char *command_line )
{
	static char process_string[ 1024 ];

	if ( !process_name
	||   !command_line )
	{
		return NULL;
	}

	snprintf(
		process_string,
		sizeof ( process_string ),
		"%s%c%s",
		process_name,
		sql_delimiter,
		command_line );

	return process_string;
}

void export_subschema_output_operation(
		FILE *export_subschema_process_file,
		char *folder_name )
{
	LIST *operation_list;
	FOLDER_OPERATION *folder_operation;
	char *operation_string;
	char *sql_statement;
	char *process_string;

	if ( !export_subschema_process_file )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"export_subschema_process_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		fclose( export_subschema_process_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	export_subschema_output_standard(
		"table_operation" /* appaserver_folder_name */,
		export_subschema_process_file,
		folder_name );

	operation_list =
		folder_operation_list(
			folder_name,
			(char *)0 /* role_name */,
			1 /* fetch_operation */,
			1 /* fetch_process */ );

	if ( list_rewind( operation_list ) )
	do {
		folder_operation = list_get( operation_list );

		if ( !folder_operation->operation
		||   !folder_operation->operation->operation_name
		||   !folder_operation->operation->process )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"folder_operation->operation is empty or incomplete." );

			fclose( export_subschema_process_file );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			/* Stub */
			/* ---- */
			exit( 1 );
		}

		if ( !folder_operation->operation->process->command_line )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"for operation=%s, command_line is empty.",
				folder_operation->operation->operation_name );

			fclose( export_subschema_process_file );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			/* Stub */
			/* ---- */
			exit( 1 );
		}

		operation_string =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			export_subschema_operation_string(
				SQL_DELIMITER,
				folder_operation->operation->operation_name,
				folder_operation->operation->output_boolean );

		if ( !operation_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
		"export_subschema_operation_string(%s,%d) returned empty.",
				folder_operation->
					operation->
					operation_name,
				folder_operation->
					operation->
					output_boolean );

			fclose( export_subschema_process_file );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				OPERATION_TABLE,
				"operation,output_yn"
					/* attribute_name_list_string */,
				operation_string
					/* delimited_string */ );

		fprintf(export_subschema_process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );

		process_string =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			export_subschema_process_string(
				SQL_DELIMITER,
				folder_operation->
					operation->
					operation_name
					/* process_name */,
				folder_operation->
					operation->
					process->
					command_line );

		if ( !process_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
		"export_subschema_operation_string(%s,%s) returned empty.",
				folder_operation->
					operation->
						operation_name,
				folder_operation->
					operation->
					process->
					command_line );

			fclose( export_subschema_process_file );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		sql_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				PROCESS_TABLE,
				"process,command_line"
					/* attribute_name_list_string */,
				process_string
					/* delimited_string */ );

		fprintf(export_subschema_process_file,
			"%s\n",
			sql_statement );

		free( sql_statement );

	} while ( list_next( operation_list ) );
}

char *export_subschema_operation_string(
		const char sql_delimiter,
		char *operation_name,
		boolean output_boolean )
{
	static char operation_string[ 128 ];

	if ( !operation_name ) return NULL;

	snprintf(
		operation_string,
		sizeof ( operation_string ),
		"%s%c%c",
		operation_name,
		sql_delimiter,
		(output_boolean) ? 'y' : 'n' );

	return operation_string;
}
