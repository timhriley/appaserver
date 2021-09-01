/* $APPASERVER_HOME/library/process.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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
#include "basename.h"
#include "column.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "security.h"
#include "process_parameter.h"
#include "process.h"

void process_execution_count_increment(
			char *process_name )
{
	char sys_string[ 1024 ];

	if ( !process_name || !*process_name ) return;

	sprintf(sys_string,
		"echo \"update %s					 "
		"	set execution_count =				 "
		"		if(execution_count,execution_count+1,1)	 "
		"	where process = '%s';\"				|"
		"sql							 ",
		"process",
		process_name );

	if ( system( sys_string ) ){}
}

boolean process_executable_ok( char *executable )
{
	char command[ 128 ];
	char sys_string[ 1024 ];
	char *which_string;
	char *appaserver_mount_point;
	char *directory;
	char check_directory[ 128 ];

	/* Can't redirect anything. */
	/* ------------------------ */
	if ( character_exists( executable, '>' ) ) return 0;
	if ( character_exists( executable, '<' ) ) return 0;

	/* Can't fork a subshell. */
	/* ---------------------- */
	if ( character_exists( executable, '(' ) ) return 0;

	/* Can't execute multiple commands. */
	/* -------------------------------- */
	if ( character_exists( executable, ';' ) ) return 0;
	if ( character_exists( executable, '&' ) ) return 0;

	column( command, 0, executable );

	/* Executable must live in $APPASERVER_HOME/src_* */
	/* ---------------------------------------------- */
	sprintf( sys_string, "which %s", command );

	if ( ! ( which_string = pipe2string( sys_string ) ) )
	{
		/* If not written yet, then okay. */
		/* ------------------------------ */
		return 1;
	}

	directory = basename_directory( which_string );

	/* Anything in $CGI_HOME is okay. */
	/* ------------------------------ */
	if ( *directory == '.' ) return 1;

	/* Must execute from $APPASERVER_HOME/src_* */
	/* ---------------------------------------- */
	appaserver_mount_point =
		appaserver_parameter_file_appaserver_mount_point();

	sprintf( check_directory,
		 "%s/src_",
		 appaserver_mount_point );

	if ( timlib_strncmp( directory, check_directory ) != 0 ) return 0;

	/* ------------------------------------------------------------ */
	/* It's okay to execute shell scripts containing 'appaserver'	*/
	/* if APPASERVER_DATABASE is referenced.			*/
	/* ------------------------------------------------------------ */
	if ( process_interpreted_executable_ok( which_string ) )
		return 1;

	/* Forbidden executables */
	/* --------------------- */
	if ( timlib_strncmp( command, "get_folder_data" ) == 0 ) return 0;
	if ( timlib_strncmp( command, "sql" ) == 0 ) return 0;
	if ( timlib_strncmp( command, "select" ) == 0 ) return 0;
	if ( timlib_exists_string( command, "appaserver" ) ) return 0;

	return 1;
}


boolean process_interpreted_executable_ok( char *which_string )
{
	char sys_string[ 1024 ];

	if ( !timlib_character_exists( which_string, '.' ) )
		return 1;

	sprintf( sys_string,
		 "grep APPASERVER_DATABASE %s | head -1 | wc -l",
		 which_string );

	return (boolean)atoi( pipe2string( sys_string ) );

}

char *process_primary_where( char *process_name )
{
	static char where[ 256 ];

	sprintf(where,
		"process = '%s'",
		process_name );

	return where;
}

PROCESS *process_parse(	char *input,
			char *document_root_directory,
			char *relative_source_directory,
			boolean check_executable_inside_filesystem )
{
	PROCESS *process;
	char process_name[ 128 ];
	char buffer[ 65536 ];

	if ( !input || !*input ) return (PROCESS *)0;

	/* See PROCESS_SELECT */
	/* ------------------ */
	piece( process_name, SQL_DELIMITER, input, 0 );
	process = process_new( strdup( process_name ) );

	process->
		check_executable_inside_filesystem =
			check_executable_inside_filesystem;

	piece( buffer, SQL_DELIMITER, input, 1 );
	process->command_line = strdup( buffer );

	if ( process->check_executable_inside_filesystem
	&&   !process_executable_ok( process->command_line ) )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"ERROR in %s/%s()/%d: EXECUTABLE NOT OK for [%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			process->command_line );

		appaserver_output_error_message(
			environment_application_name(),
			msg,
			(char *)0 );

		return (PROCESS *)0;
	}

	piece( buffer, SQL_DELIMITER, input, 2 );
	process->notepad = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	process->html_help_file_anchor = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	process->execution_count = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	process->post_change_javascript = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process->process_set_display = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process->process_group = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	process->preprompt_help_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	process->javascript_filename = strdup( buffer );

	if ( *process->javascript_filename
	&&   document_root_directory
	&&   relative_source_directory )
	{
		process->javascript =
			javascript_new(
				process->javascript_filename,
				document_root_directory,
				relative_source_directory );

		if ( !process->javascript )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: javascript_new(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				process->javascript_filename );
			exit( 1 );
		}
	}

	return process;
}

PROCESS *process_fetch(	char *process_name,
			char *document_root_directory,
			char *relative_source_directory,
			boolean check_executable_inside_filesystem )
{
	return
	process_parse(
		string_pipe_fetch(
			process_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_primary_where(
					process_name ) ) ),
		document_root_directory,
		relative_source_directory,
		check_executable_inside_filesystem );
}

PROCESS *process_new( char *process_name )
{
	PROCESS *p = calloc( 1, sizeof( PROCESS ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( PROCESS ) );
		exit( 1 );
	}

	p->process_name = process_name;
	return p;
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
		string_pipe_fetch(
			process_set_system_string(
				process_set_primary_where(
					process_set_name ) ) ),
		role_name,
		document_root_directory,
		relative_source_directory,
		fetch_process_set_member_name_list );
}

PROCESS_SET *process_set_parse(
			char *input,
			char *role_name,
			char *document_root_directory,
			char *relative_source_directory,
			boolean fetch_process_set_member_name_list )
{
	PROCESS_SET *process_set;
	char process_set_name[ 128 ];
	char buffer[ 65536 ];

	/* See PROCESS_SET_SELECT */
	/* ---------------------- */
	piece( process_set_name, SQL_DELIMITER, input, 0 );
	process_set = process_set_new( strdup( process_set_name ) );

	process_set->role_name = role_name;

	piece( buffer, SQL_DELIMITER, input, 1 );
	process_set->notepad = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	process_set->html_help_file_anchor = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	process_set->post_change_javascript = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	process_set->prompt_display_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	process_set->process_group = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process_set->preprompt_help_text = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process_set->prompt_display_bottom = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	process_set->javascript_filename = strdup( buffer );

	if ( *process_set->javascript_filename )
	{
		process_set->javascript =
			javascript_new(
				process_set->javascript_filename,
				document_root_directory,
				relative_source_directory );

		if ( !process_set->javascript )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: javascript_new(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				process_set->javascript_filename );
			exit( 1 );
		}
	}

	if ( fetch_process_set_member_name_list )
	{
		process_set->process_set_member_name_list =
			process_set_member_name_list(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_set_primary_where(
					process_set_name ),
				role_name );
	}

	return process_set;
}

PROCESS_SET *process_set_new(
			char *process_set_name )
{
	PROCESS_SET *p = calloc( 1, sizeof( PROCESS_SET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( PROCESS_SET ) );
		exit( 1 );
	}

	p->process_set_name = process_set_name;
	return p;
}

char *process_set_primary_where(
			char *process_set_name )
{
	static char where[ 128 ];

	sprintf(where,
		"process_set = '%s'",
		process_set_name );

	return where;
}

char *process_set_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		PROCESS_SET_SELECT,
		PROCESS_SET_TABLE,
		where );

	return strdup( system_string );
}

char *process_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		PROCESS_SELECT,
		PROCESS_TABLE,
		where );

	return strdup( system_string );
}

LIST *process_set_role_process_name_list(
			char *primary_where,
			char *role_name )
{
	char system_string[ 1024 ];
	char where[ 256 ];

	sprintf(where,
		"%s and role = '%s'",
		primary_where,
		role_name );

	sprintf(system_string,
		"select.sh process role_process_set_member \"%s\"",
		where );

	return list_pipe_fetch( system_string );
}

char *process_set_process_where(
			LIST *process_name_list )
{
	char where[ 65536 ];

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

char *process_prompt_submit_command_line(
			char *command_line,
			char *application_name,
			char *process_name,
			char *role_name,
			char *login_name,
			DICTIONARY *working_post_dictionary )
{
	char buffer[ 65536 ];
	char local_command_line[ STRING_INPUT_LINE ];

	string_strcpy( local_command_line, command_line, STRING_INPUT_LINE );

	if ( process_name && *process_name )
	{
		search_replace_word(
			local_command_line,
			"$process",
			double_quotes_around(
				buffer, 
				process_name ) );
	}

	if ( role_name && *role_name )
	{
		search_replace_word(
			local_command_line,
			"$role",
			double_quotes_around(
				buffer, 
				role_name ) );
	}

	if ( login_name && *login_name )
	{
		search_replace_word(
			local_command_line,
			"$login_name",
			double_quotes_around(
				buffer, 
				login_name ) );
	}

	if ( string_exists( local_command_line, "$dictionary" ) )
	{
		search_replace_word(
			local_command_line,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					working_post_dictionary, '&' ) 
				) );
	}

	dictionary_search_replace_command_arguments(
		local_command_line,
		working_post_dictionary, 
		0 /* row  */ );

	sprintf(local_command_line + strlen( local_command_line ),
		" 2>>%s",
		appaserver_error_filename(
			application_name ) );

	/* This memory is always heap */
	/* -------------------------- */
	free( command_line );

	return strdup( command_line );
}



#ifdef NOT_DEFINED
char *process_search_replace_where(
			char *command_line,
			DICTIONARY *drillthru_dictionary,
			char *folder_name,
			char *role_name,
			char *login_name )
{
	QUERY *query;
	char buffer[ STRING_WHERE_BUFFER ];

	if ( !command_line )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !string_exists( local_command_line, "$where" ) )
	{
		return;
	}

	if ( ! ( query =
			query_process_drop_down_new(
				drillthru_dictionary,
				folder_name,
				role_name,
				login_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_process_drop_down_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	search_replace_word(
		command_line, 
		"$where", 
		double_quotes_around( 	
			buffer,
			query->where_clause ) );
}
#endif

PROCESS_PROMPT_OUTPUT *process_prompt_output_calloc( void )
{
	PROCESS_PROMPT_OUTPUT *process_prompt_output;

	if ( ! ( process_prompt_output =
			calloc( 1, sizeof( PROCESS_PROMPT_OUTPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return process_prompt_output;
}

PROCESS_PROMPT_OUTPUT *process_prompt_output_fetch(
			char *process_or_set_name,
			char *role_name,
			char *login_name,
			boolean is_preprompt,
			char *document_root_directory,
			char *relative_source_directory,
			DICTIONARY *drillthru_dictionary )
{
	PROCESS_PROMPT_OUTPUT *process_prompt_output;
	char *process_name;
	char *process_set_name = {0};

	if ( ! ( process_name =
			process_name_fetch(
				process_or_set_name ) ) )
	{
		if ( ! ( process_set_name =
				process_set_name_fetch(
					process_or_set_name ) ) )
		{
			return (PROCESS_PROMPT_OUTPUT *)0;
		}
	}

	process_prompt_output = process_prompt_output_calloc();

	process_prompt_output->process_or_set_name = process_or_set_name;
	process_prompt_output->role_name = role_name;
	process_prompt_output->login_name = login_name;
	process_prompt_output->is_preprompt = is_preprompt;
	process_prompt_output->drillthru_dictionary = drillthru_dictionary;

	if ( process_name )
	{
		process_prompt_output->process =
			process_fetch(
				process_name,
				document_root_directory,
				relative_source_directory,
				0 /* not check_executable_inside */ );

		if ( !process_prompt_output->process )
		{
			return (PROCESS_PROMPT_OUTPUT *)0;
		}

		process_prompt_output->process_parameter_list =
			process_parameter_system_list(
				process_parameter_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					process_parameter_where(
						/* --------------------- */
						/* Returns static memory */
						/* --------------------- */
						process_primary_where(
							process_name ),
						is_preprompt ) ),
				role_name,
				login_name,
				drillthru_dictionary );
	}
	else
	{
		process_prompt_output->process_set =
			process_set_fetch(
				process_prompt_output->
					process_or_set_name,
				process_prompt_output->role_name,
				document_root_directory,
				relative_source_directory,
				1 /* fetch_member_process_name_list */ );

		process_prompt_output->process_parameter_list =
			process_parameter_system_list(
				process_set_parameter_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					process_set_primary_where(
						process_prompt_output->
							process_set->
							process_set_name ) ),
				role_name,
				login_name,
				drillthru_dictionary );
	}

	if ( !process_prompt_output->process_set
	&&   !process_prompt_output->process )
	{
		return (PROCESS_PROMPT_OUTPUT *)0;
	}

	return process_prompt_output;
}

char *process_choose_isa_command_line(
			char *command_line,
			char *application_name,
			char *security_entity_where,
			char *login_name,
			char *role_name,
			char *one2m_isa_folder_name )
{
	char local_command_line[ STRING_WHERE_BUFFER ];
	char buffer[ 1024 ];

	string_strcpy( local_command_line, command_line, STRING_WHERE_BUFFER );

	if ( login_name && *login_name )
	{
		search_replace_word(
			local_command_line,
			"$login_name",
			double_quotes_around(
				buffer, 
				login_name ) );
	}

	if ( role_name && *role_name )
	{
		search_replace_word(
			local_command_line,
			"$role",
			double_quotes_around(
				buffer, 
				role_name ) );
	}

	if ( security_entity_where
	&&   *security_entity_where )
	{
		search_replace_word(
			local_command_line, 
			"$where", 
			double_quotes_around( 	
				buffer,
				security_entity_where ) );
	}

	if ( one2m_isa_folder_name
	&&   *one2m_isa_folder_name )
	{
		search_replace_word(
			local_command_line,
			"$one2m_folder_name",
			buffer );

		search_replace_word(
			local_command_line,
			"$one2m_folder",
			buffer );
	}

	sprintf(local_command_line + strlen( local_command_line ),
		" 2>>%s",
		appaserver_error_filename(
			application_name ) );

	/* This memory is always heap */
	/* -------------------------- */
	free( command_line );

	return strdup( local_command_line );
}

LIST *process_set_member_name_list(
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

	sprintf(where,
		"%s and role = '%s'",
		process_set_primary_where,
		role_name );

	sprintf(system_string,
		"select.sh process %s \"%s\" process",
		"role_process_set_member",
		where );

	return list_fetch_pipe( system_string );
}

char *process_operation_command_line(
			char *command_line,
			char *application_name,
			char *operation_name,
			char *login_name,
			char *role_name,
			char *folder_name,
			pid_t parent_process_id,
			char *session_key,
			int operation_row_total,
			LIST *primary_data_list,
			DICTIONARY *single_row_dictionary )
{
	char local_command_line[ STRING_INPUT_LINE ];
	char buffer[ 65536 ];

	string_strcpy( local_command_line, command_line, STRING_INPUT_LINE );
	free( command_line );

	if ( application_name )
	{
		search_replace_word(
			local_command_line,
			"$application",
			double_quotes_around(
				buffer, 
				application_name ) );
	}

	if ( operation_name )
	{
		search_replace_word(
			local_command_line,
			"$process",
			double_quotes_around(
				buffer, 
				operation_name ) );
	}

	if ( login_name )
	{
		search_replace_word(
			local_command_line,
			"$login_name",
			double_quotes_around(
				buffer, 
				login_name ) );
	}

	if ( role_name )
	{
		search_replace_word(
			local_command_line,
			"$role",
			double_quotes_around(
				buffer, 
				role_name ) );
	}

	if ( folder_name )
	{
		search_replace_word(
			local_command_line,
			"$folder",
			double_quotes_around(
				buffer, 
				folder_name ) );
	}

	if ( parent_process_id )
	{
		char process_id_string[ 16 ];

		sprintf(process_id_string,
			"%d",
			(int)parent_process_id );

		search_replace_word(
			local_command_line,
			"$process_id",
			double_quotes_around(
				buffer, 
				process_id_string ) );
	}

	if ( session_key )
	{
		search_replace_word(
			local_command_line,
			"$session",
			double_quotes_around(
				buffer, 
				session_key ) );
	}

	if ( operation_row_total )
	{
		char operation_row_total_string[ 16 ];

		search_replace_word(
			local_command_line,
			"$process_row_count",
			double_quotes_around(
				buffer, 
				operation_row_total_string ) );

		search_replace_word(
			local_command_line,
			"$operation_row_count",
			double_quotes_around(
				buffer, 
				operation_row_total_string ) );

		search_replace_word(
			local_command_line,
			"$operation_row_total",
			double_quotes_around(
				buffer, 
				operation_row_total_string ) );
	}

	if ( primary_data_list )
	{
		search_replace_word(
			local_command_line,
			"$primary_data_list",
			list_display_quoted_delimiter( 
				buffer,
				primary_data_list,
				SQL_DELIMITER ) );
	}

	if ( single_row_dictionary )
	{
		search_replace_word(
			local_command_line,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					single_row_dictionary, '&' ) 
				) );
	}

	dictionary_search_replace_command_arguments(
		local_command_line,
		single_row_dictionary, 
		0 /* row_number */ );

	sprintf(local_command_line + strlen( local_command_line ),
		" 2>>%s",
		appaserver_error_filename(
			application_name ) );

	return strdup( local_command_line );
}

char *process_name_fetch( char *process_or_set_name )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh process %s \"%s\"",
		PROCESS_TABLE,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_primary_where(
			process_or_set_name
				/* process_name */ ) );

	return string_pipe_fetch( system_string );
}

char *process_set_name_fetch( char *process_or_set_name )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh process_set %s \"%s\"",
		PROCESS_SET_TABLE,
		process_set_primary_where(
			process_or_set_name
				/* process_set_name */ ) );

	return string_pipe_fetch( system_string );
}

LIST *process_delimited_list_fetch(
			char *process_name )
{
	PROCESS *process;

	if ( !process_name
	||   !*process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process =
		process_fetch(
			process_name,
			(char *)0 /* document_root_directory */,
			(char *)0 /* relative_source_directory */,
			0 /* not check_executable_inside_filesystem */ );

	return (LIST *)0;
}
