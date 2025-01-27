/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "list.h"
#include "environ.h"
#include "piece.h"
#include "sql.h"
#include "basename.h"
#include "column.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "security.h"
#include "process_parameter.h"
#include "dictionary.h"
#include "process.h"

void process_increment_count( char *process_name )
{
	char system_string[ 1024 ];

	if ( !process_name || !*process_name ) return;

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"update %s					 "
		"	set execution_count =				 "
		"		if(execution_count,execution_count+1,1)	 "
		"	where process = '%s';\"				|"
		"sql							 ",
		PROCESS_TABLE,
		process_name );

	if ( system( system_string ) ){}
}

void process_execution_count_increment( char *process_name )
{
	process_increment_count( process_name );
}

boolean process_executable_okay(
		char *mount_point,
		char *command_line )
{
	char command[ 256 ];
	char system_string[ 1024 ];
	char *which_string;
	char *directory;
	char src_directory[ 128 ];

	if ( !mount_point
	||   !command_line )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Can't redirect anything. */
	/* ------------------------ */
	if ( character_exists( command_line, '>' ) ) return 0;
	if ( character_exists( command_line, '<' ) ) return 0;

	/* Can't execute multiple commands. */
	/* -------------------------------- */
	if ( character_exists( command_line, ';' ) ) return 0;
	if ( character_exists( command_line, '&' ) ) return 0;

	/* Can't fork a subshell. */
	/* ---------------------- */
	if ( character_exists( command_line, '(' ) ) return 0;
	if ( character_exists( command_line, '`' ) ) return 0;

	column( command, 0, command_line );

	sprintf( system_string, "which %s", command );

	/* cwd=$CGI_HOME */
	/* ------------- */
	environment_prepend_path( "." );

	if ( ! ( which_string = string_pipe_fetch( system_string ) ) )
	{
		/* If not written yet, then okay. */
		/* ------------------------------ */
		return 1;
	}

	directory = basename_directory( which_string );

	/* Anything in $CGI_HOME is okay. */
	/* ------------------------------ */
	if ( *directory == '.' ) return 1;

	/* ------------------------------------ */
	/* Can’t reside in		   	*/
	/* $APPASERVER_HOME/src_appaserver 	*/
	/* ------------------------------------ */
	sprintf(src_directory,
		"%s/src_appaserver",
		mount_point );

	if ( string_strncmp(
			directory,
			src_directory ) == 0 )
	{
		return 0;
	}

	/* Must reside in $APPASERVER_HOME/src_* */
	/* ------------------------------------- */
	sprintf(src_directory,
		"%s/src_",
		mount_point );

	if ( string_strncmp( directory, src_directory ) != 0 ) return 0;

	return 1;
}

PROCESS *process_parse(	char *input )
{
	PROCESS *process;
	char process_name[ 128 ];
	char buffer[ 65536 ];

	if ( !input || !*input ) return (PROCESS *)0;

	/* See PROCESS_SELECT */
	/* ------------------ */
	piece( process_name, SQL_DELIMITER, input, 0 );
	process = process_new( strdup( process_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) process->command_line = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) process->notepad = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) process->html_help_file_anchor = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) process->execution_count = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) process->post_change_javascript = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) process->process_set_display = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) process->process_group = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) process->preprompt_help_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) process->javascript_filename = strdup( buffer );

	return process;
}

PROCESS *process_fetch(
		char *process_name,
		char *document_root,
		char *application_relative_source_directory,
		boolean check_executable_inside_filesystem,
		char *mount_point )
{
	static LIST *process_list = {0};
	PROCESS *process;

	if ( !process_name )
	{
		char message[ 128 ];

		sprintf(message, "process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !process_list ) process_list = process_fetch_list();

	if ( ! ( process =
			process_seek(
				process_name,
				process_list ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"process_seek(%s) returned empty.",
			process_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	process->javascript_filename
	&&	document_root
	&&	application_relative_source_directory )
	{
		process->javascript =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			javascript_new(
				process->javascript_filename,
				document_root,
				application_relative_source_directory );
	}

	if ( check_executable_inside_filesystem )
	{
		if ( !process_executable_okay(
			mount_point,
			process->command_line ) )
		{
			char message[ 65536 ];

			sprintf(message,
				"%s(%s) returned not okay.",
				"process_executable_okay",
				process_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	return process;
}

PROCESS *process_calloc( void )
{
	PROCESS *process;

	if ( ! ( process = calloc( 1, sizeof ( PROCESS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return process;
}

PROCESS *process_new( char *process_name )
{
	PROCESS *process;

	if ( !process_name || !*process_name )
	{
		char message[ 128 ];

		sprintf(message, "process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process = process_calloc();
	process->process_name = process_name;

	return process;
}

PROCESS_SET *process_set_fetch(
		char *process_set_name,
		char *role_name,
		char *document_root_directory,
		char *relative_source_directory,
		boolean fetch_process_set_member_name_list )
{
	return
	process_set_parse(
		process_set_name,
		role_name,
		document_root_directory,
		relative_source_directory,
		fetch_process_set_member_name_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			process_set_system_string(
				PROCESS_SET_SELECT,
				PROCESS_SET_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_set_primary_where(
					process_set_name )
						/* where */ ) )
							/* input */ );
}

PROCESS_SET *process_set_parse(
		char *process_set_name,
		char *role_name,
		char *document_root_directory,
		char *relative_source_directory,
		boolean fetch_process_set_member_name_list,
		char *input )
{
	PROCESS_SET *process_set;
	char buffer[ STRING_64K ];

	process_set = process_set_new( process_set_name );

	/* See PROCESS_SET_SELECT */
	/* ---------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer ) process_set->notepad = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) process_set->html_help_file_anchor = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) process_set->post_change_javascript = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) process_set->prompt_display_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) process_set->process_group = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) process_set->preprompt_help_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) process_set->javascript_filename = strdup( buffer );

	if ( document_root_directory
	&&   relative_source_directory
	&&   process_set->javascript_filename )
	{
		process_set->javascript =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			javascript_new(
				process_set->javascript_filename,
				document_root_directory,
				relative_source_directory );
	}

	if ( fetch_process_set_member_name_list
	&&   role_name )
	{
		process_set->member_name_list =
			process_set_member_name_list(
				ROLE_PROCESS_SET_MEMBER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_set_primary_where(
					process_set_name ),
				role_name );
	}

	return process_set;
}

PROCESS_SET *process_set_calloc( void )
{
	PROCESS_SET *process_set = calloc( 1, sizeof ( PROCESS_SET ) );

	if ( !process_set )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return process_set;
}

PROCESS_SET *process_set_new( char *process_set_name )
{
	PROCESS_SET *process_set = process_set_calloc();
	process_set->process_set_name = process_set_name;

	return process_set;
}

char *process_set_primary_where( char *process_set_name )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"process_set = '%s'",
		process_set_name );

	return where;
}

char *process_set_system_string(
		char *process_set_select,
		char *process_set_table,
		char *where )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		process_set_select,
		process_set_table,
		where );

	return strdup( system_string );
}

LIST *process_set_role_process_name_list(
		char *primary_where,
		char *role_name )
{
	char system_string[ 1024 ];
	char where[ 256 ];

	snprintf(
		where,
		sizeof ( where ),
		"%s and role = '%s'",
		primary_where,
		role_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh process role_process_set_member \"%s\"",
		where );

	return list_pipe_fetch( system_string );
}

char *process_set_process_where(
		LIST *process_name_list )
{
	char where[ STRING_COMMAND_BUFFER ];

	if ( !list_length( process_name_list ) )
	{
		strcpy( where, "1 = 1" );
	}
	else
	{
		sprintf(where,
			"process in (%s)",
			string_in_clause( process_name_list ) );
	}

	return strdup( where );
}

LIST *process_set_member_name_list(
		char *role_process_set_member_table,
		char *process_set_primary_where,
		char *role_name )
{
	char where[ 256 ];
	char system_string[ 1024 ];

	if ( !process_set_primary_where || !*process_set_primary_where )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: process_set_primary_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		where,
		sizeof ( where ),
		"%s and role = '%s'",
		process_set_primary_where,
		role_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh process %s \"%s\" process",
		role_process_set_member_table,
		where );

	return list_fetch_pipe( system_string );
}

char *process_name_fetch( char *process_or_set_name )
{
	if ( !process_or_set_name || !*process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "process_or_set_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_pipe_fetch(
		appaserver_system_string(
			"process" /* select */,
			PROCESS_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_primary_where(
				process_or_set_name
					/* process_name */ ) ) );
}

char *process_set_name_fetch( char *process_or_set_name )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh process_set %s \"%s\"",
		PROCESS_SET_TABLE,
		process_set_primary_where(
			process_or_set_name
				/* process_set_name */ ) );

	return string_pipe_fetch( system_string );
}

LIST *process_delimited_list( char *command_line )
{
	return list_fetch_pipe( command_line );
}

void process_replace_row_count_command_line(
		char *command_line,
		int operation_row_checked_count,
		const char *process_row_count_placeholder )
{
	char row_count_string[ 128 ];

	sprintf(row_count_string,
		"%d",
		operation_row_checked_count );

	string_replace_command_line(
		command_line /* in/out */,
		row_count_string,
		(char *)process_row_count_placeholder );
}

void process_replace_dictionary_command_line(
		char *command_line,
		DICTIONARY *dictionary_single_row,
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		const char *process_dictionary_placeholder )
{
	char *tmp;

	if ( !dictionary_length( dictionary_single_row ) ) return;

	string_replace_command_line(
		command_line /* in/out */,
		( tmp =
			/* ------------------- */ 
			/* Returns heap memory */
			/* ------------------- */ 
			dictionary_display_delimited(
				dictionary_single_row,
				dictionary_attribute_datum_delimiter,
				dictionary_element_delimiter ) ),
		(char *)process_dictionary_placeholder );

	free( tmp );
}

void process_replace_pid_command_line(
		char *command_line,
		pid_t process_id,
		const char *process_pid_placeholder )
{
	char process_id_string[ 128 ];

	sprintf(process_id_string,
		"%d",
		(int)process_id );

	string_replace_command_line(
		command_line,
		process_id_string,
		(char *)process_pid_placeholder );
}

char *process_title_heading_tag( char *process_name )
{
	static char tag[ 128 ];
	char destination[ 128 ];

	snprintf(
		tag,
		sizeof ( tag ),
		"<h1>%s</h1>",
		string_initial_capital(
			destination,
			process_name ) );

	return tag;
}

LIST *process_fetch_list( void )
{
	return
	process_system_list(
		appaserver_system_string(
			PROCESS_SELECT,
			PROCESS_TABLE,
			(char *)0 /* where */ ) );
}

PROCESS *process_seek(	char *process_name,
			LIST *process_list )
{
	PROCESS *process;

	if ( !process_name )
	{
		char message[ 128 ];

		sprintf(message, "process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( process_list ) )
	do {
		process = list_get( process_list );

		if ( strcmp(
			process->process_name,
			process_name ) == 0 )
		{
			return process;
		}

	} while ( list_next( process_list ) );

	return (PROCESS *)0;
}

LIST *process_system_list( char *appaserver_system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *list = list_new();

	if ( !appaserver_system_string || !*appaserver_system_string )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_pipe =
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			appaserver_system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			process_parse( input ) );
	}

	pclose( input_pipe );

	return list;
}

char *process_primary_where( char *process_name )
{
	static char where[ 128 ];

	if ( !process_name || !*process_name )
	{
		char message[ 128 ];

		sprintf(message, "process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"process = '%s'",
		process_name );

	return where;
}

DICTIONARY *process_command_line_dictionary(
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary )
{
	if ( dictionary_length( drillthru_dictionary ) )
		return drillthru_dictionary;
	else
		return prompt_dictionary;
}

char *process_command_line_fetch( char *process_name )
{
	PROCESS *process;

	if ( !process_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_fetch(
			process_name,
			/* Not fetching javascript */
			(char *)0 /* document_root */,
			(char *)0 /* relative_source_directory */,
			/* Not preparing to execute */
			0 /* not check_executable_inside_filesystem */,
			(char *)0 /* mount_point */ );

	return process->command_line;
}

