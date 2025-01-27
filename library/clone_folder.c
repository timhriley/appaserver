/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/clone_folder.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "sql.h"
#include "insert.h"
#include "html.h"
#include "piece.h"
#include "date.h"
#include "shell_script.h"
#include "clone_folder.h"

CLONE_FOLDER *clone_folder_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *old_data,
		char *new_data,
		boolean delete_boolean,
		char *data_directory )
{
	CLONE_FOLDER *clone_folder;
	char input[ 65536 ];

	if ( !application_name
	||   !folder_name
	||   !attribute_name
	||   !old_data
	||   !new_data
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

	clone_folder = clone_folder_calloc();

	clone_folder->sql_statement_list = list_new();

	clone_folder->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	if ( delete_boolean )
	{
		clone_folder->delete_statement =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			clone_folder_delete_statement(
				clone_folder->folder_table_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				clone_folder_where(
					attribute_name,
					new_data ) );

		list_set(
			clone_folder->sql_statement_list,
			clone_folder->delete_statement );

		return clone_folder;
	}

	clone_folder->folder_attribute_list =
		folder_attribute_list(
			(char *)0 /* role_name */,
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_attribute */ );

	clone_folder->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			clone_folder->folder_attribute_list );

	clone_folder->piece_offset =
		clone_folder_piece_offset(
			attribute_name,
			clone_folder->folder_attribute_name_list );

	if ( clone_folder->piece_offset == -1 )
	{
		char message[ 128 ];

		sprintf(message,
			"clone_folder_piece_offset(%s) returned -1.",
			attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	clone_folder->select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		clone_folder_select(
			clone_folder->folder_attribute_name_list );

	clone_folder->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		clone_folder_where(
			attribute_name,
			old_data );

	clone_folder->appaserver_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			clone_folder->select,
			clone_folder->folder_table_name,
			clone_folder->where );

	clone_folder->input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		clone_folder_input_pipe(
			clone_folder->appaserver_system_string );

	while ( string_input( input, clone_folder->input_pipe, 65536 ) )
	{
		piece_replace(
			input /* source_destination */,
			SQL_DELIMITER,
			new_data,
			clone_folder->piece_offset );

		list_set(
			clone_folder->sql_statement_list,
			insert_folder_string_sql_statement(
				SQL_DELIMITER,
				clone_folder->folder_table_name,
				clone_folder->select
					/* attribute_name_list_string */,
				input /* delimited_string */ ) );
	}

	pclose( clone_folder->input_pipe );

	clone_folder->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		clone_folder_filename(
			folder_name,
			data_directory,
			date_now_yyyy_mm_dd( date_utc_offset() ) );

	shell_script_spool_string_list(
		application_name,
		clone_folder->filename,
		clone_folder->sql_statement_list /* string_list */ );

	return clone_folder;
}

CLONE_FOLDER *clone_folder_calloc( void )
{
	CLONE_FOLDER *clone_folder;

	if ( ! ( clone_folder = calloc( 1, sizeof ( CLONE_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return clone_folder;
}

FILE *clone_folder_input_pipe( char *appaserver_system_string )
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


int clone_folder_piece_offset(
		char *attribute_name,
		LIST *folder_attribute_name_list )
{
	if ( !attribute_name
	||   !list_length( folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	list_seek_offset(
		attribute_name /* item */,
		folder_attribute_name_list );
}

char *clone_folder_select( LIST *folder_attribute_name_list )
{
	if ( !list_length( folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return list_delimited( folder_attribute_name_list, ',' );
}

char *clone_folder_where(
		char *attribute_name,
		char *old_data )
{
	static char where[ 256 ];

	if ( !attribute_name
	||   !old_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"%s = '%s'",
		attribute_name,
		old_data );

	return where;
}

char *clone_folder_filename(
		char *folder_name,
		char *data_directory,
		char *date_yyyy_mm_dd )
{
	static char filename[ 128 ];

	if ( !folder_name
	||   !data_directory
	||   !date_yyyy_mm_dd )
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
		"%s/clone_folder_%s_%s.sh",
		data_directory,
		folder_name,
		date_yyyy_mm_dd );

	return filename;
}

char *clone_folder_delete_statement(
		char *folder_table_name,
		char *clone_folder_where )
{
	static char statement[ 512 ];

	if ( !folder_table_name
	||   !clone_folder_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(statement,
		"delete from %s where %s;",
		folder_table_name,
		clone_folder_where );

	return statement;
}

void clone_folder_execute( LIST *sql_statement_list )
{
	FILE *execute_pipe;

	if ( !list_rewind( sql_statement_list ) ) return;

	execute_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		clone_folder_execute_pipe(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			clone_folder_execute_system_string() );

	do {
		fprintf(execute_pipe,
			"%s\n",
			(char *)list_get( sql_statement_list ) );

	} while ( list_next( sql_statement_list ) );

	pclose( execute_pipe );
}

char *clone_folder_execute_system_string( void )
{
	return
	"sql.e 2>&1 | html_paragraph_wrapper.e";
}

FILE *clone_folder_execute_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Always succeeds */
	/* --------------- */
	return popen( system_string, "w" );
}

void clone_folder_display( LIST *sql_statement_list )
{
	html_string_list_stdout(
		HTML_TABLE_QUEUE_TOP_BOTTOM,
		"" /* title */,
		"SQL Statement" /* heading_string */,
		sql_statement_list
			/* string_list */ );
}

