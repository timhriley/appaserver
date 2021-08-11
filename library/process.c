/* $APPASERVER_HOME/library/process.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query.h"
#include "process_parameter_list.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "hash_table.h"
#include "list.h"
#include "list_usage.h"
#include "piece.h"
#include "basename.h"
#include "column.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver.h"
#include "appaserver_parameter_file.h"
#include "operation.h"
#include "security.h"
#include "folder.h"
#include "process.h"

LIST *process_fetch_list( char *command_line )
{
	return list_pipe_fetch( command_line );
}

void process_convert_parameters(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			LIST *attribute_list,
			LIST *prompt_list,
			LIST *primary_key_list,
			LIST *primary_data_list,
			int row,
			char *process_name,
			PROCESS_SET *process_set,
			char *one2m_folder_name_for_process,
			char *operation_row_count_string,
			char *prompt )
{
	char buffer[ 65536 ];
	char local_executable[ 65536 ];
	LIST *attribute_name_list = {0};
	DICTIONARY *local_parameter_dictionary = {0};
	char *process_set_name = {0};
	char process_id_string[ 16 ];

	timlib_strcpy( local_executable, *executable, 65536 );

	/* This memory is always strdup(). */
	/* ------------------------------- */
	free( *executable );

	if ( process_set ) process_set_name = process_set->process_set_name;

	sprintf( process_id_string, "%d", getpid() );

	if ( dictionary_length( parameter_dictionary ) )
	{
		char *data;

		local_parameter_dictionary =
			dictionary_copy_dictionary(
				parameter_dictionary );

		dictionary_parse_multi_attribute_relation_operator_keys(
			local_parameter_dictionary, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

		dictionary_set_pointer(
			local_parameter_dictionary,
			PROCESS_ID_LABEL,
			strdup( process_id_string ) );

		if ( list_length( attribute_list ) )
		{
			attribute_set_dictionary_date_international(
				local_parameter_dictionary,
				attribute_list );
		}

		if ( operation_row_count_string )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						OPERATION_ROW_COUNT_LABEL,
						operation_row_count_string );
		}

		if ( !dictionary_exists_key_index(
				local_parameter_dictionary,
				"login_name",
				0 ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"login_name_0",
						person );
		}

		if ( one2m_folder_name_for_process )
		{
			process_set_one2m_folder_name_for_process(
				local_parameter_dictionary,
				one2m_folder_name_for_process );
		}

		if ( process_name && *process_name )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"process",
						process_name );
		}

		if ( ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"from_execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"execute_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"really_yn_0",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"from_really_yn",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"really_yn",
						data );
		}
		else
		if ( ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"from_really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"really_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"execute_yn_0",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"from_execute_yn",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"execute_yn",
						data );
		}

		dictionary_search_replace_command_arguments(
			local_executable,
			local_parameter_dictionary, 
			row );

	} /* if parameter_dictionary */

	if ( primary_data_list )
	{
		search_replace_word(	local_executable,
					"$primary_data_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_data_list,
						SQL_DELIMITER ) );
	}

	if ( primary_key_list )
	{
		search_replace_word(	local_executable,
					"$primary_attribute_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_key_list,
						',' ) );

		search_replace_word(	local_executable,
					"$primary_key_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_key_list,
						',' ) );

	}

	if ( prompt_list )
	{
		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			prompt_list,
			local_parameter_dictionary,
			0, 	                  /* dictionary_key_offset */
			QUERY_FROM_STARTING_LABEL,/* dictionary_key_prepend */
			' '	                  /* replace_char_prepend */
			);

		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			prompt_list,
			local_parameter_dictionary,
			0, 	             /* dictionary_key_offset */
			"",		    /* dictionary_key_prepend */
			' '	              /* replace_char_prepend */
			);
	}

	if ( attribute_name_list )
	{
		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			attribute_name_list,
			local_parameter_dictionary,
			0, 	                  /* dictionary_key_offset */
			QUERY_FROM_STARTING_LABEL,/* dictionary_key_prepend */
			' '	                  /* replace_char_prepend */
			);

		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			attribute_name_list,
			local_parameter_dictionary,
			0, 	             /* dictionary_key_offset */
			"",		    /* dictionary_key_prepend */
			' '	              /* replace_char_prepend */
			);
	}

	process_replace_parameter_variables(
		local_executable,
		session,
		state,
		login_name,
		folder_name,
		role_name,
		target_frame,
		process_name,
		process_set_name,
		operation_row_count_string,
		one2m_folder_name,
		prompt,
		process_id_string );

	if ( state && *state )
	{
		dictionary_set_pointer(
			local_parameter_dictionary,
			"$state",
			state );
	}

	if ( timlib_exists_string( local_executable, "$dictionary" )
	&&   dictionary_length( local_parameter_dictionary ) )
	{
		search_replace_word(
			local_executable,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					local_parameter_dictionary, '&' ) 
				) );
	}

	if ( timlib_exists_string( local_executable, "$where" ) )
	{
		process_search_replace_executable_where(
			local_executable,
			application_name,
			folder_name,
			attribute_list,
			where_clause_dictionary );
	}

	sprintf(	local_executable + strlen( local_executable ),
			" 2>>%s",
			appaserver_error_get_filename(
				application_name ) );

	strcpy( buffer, local_executable );
	escape_character( local_executable, buffer, '$' );

	remove_character( local_executable, '`' );

	*executable = strdup( local_executable );
}

char *process_dictionary_process_member(
			char *process_set,
			DICTIONARY *parsed_decoded_post_dictionary,
			char *from_starting_label )
{
	char *process_member;
	char key[ 1024 ];
	int results;

	sprintf(key,
		"%s%s",
		from_starting_label,
		process_set );

	results =
		dictionary_get_index_data(
			&process_member,
			parsed_decoded_post_dictionary,
			key,
			0 /* index */ );

	if ( results > -1 )
		return process_member;
	else
		return (char *)0;
}

void process_replace_parameter_variables(	
			char *executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *process_name,
			char *process_set_name,
			char *operation_row_count_string,
			char *one2m_folder_name_for_process,
			char *prompt,
			char *process_id_string )
{
	char buffer[ 1024 ];

	search_replace_word(
		executable,
		"$process_id",
		double_quotes_around(	buffer, 
					process_id_string ) );

	if ( operation_row_count_string )
	{
		search_replace_word(
			executable,
			"$process_row_count",
			double_quotes_around(	buffer, 
						operation_row_count_string ) );

		search_replace_word(
			executable,
			"$operation_row_count",
			double_quotes_around(	buffer, 
						operation_row_count_string ) );
	}

	if ( person && *person )
	{
		search_replace_word(
			executable,
			"$person",
			double_quotes_around(
				buffer, 
				person ) );

		search_replace_word(
			executable,
			"$login_name",
			double_quotes_around(
				buffer, 
				person ) );

		search_replace_word(
			executable,
			"$login",
			double_quotes_around(
				buffer, 
				person ) );
	}

	if ( prompt && *prompt )
	{
		search_replace_word(
			executable,
			"$prompt",
			double_quotes_around(	buffer, 
						prompt ) );
	}

	if ( target_frame && *target_frame )
	{
		search_replace_word( 
			executable,
			"$target_frame",
			double_quotes_around(	buffer, 
				      		target_frame ) );
	}

	if ( session && *session )
	{
		search_replace_word(
			executable,
			"$session",
			double_quotes_around( 	buffer, 
				      		session ) );
	}

	if ( state && *state )
	{
		search_replace_word(
			executable,
			"$state",
			double_quotes_around( 	buffer, 
				      		state ) );
	}

	if ( application_name && *application_name )
	{
		search_replace_word(
			executable,
			"$entity",
			double_quotes_around( buffer, 
					      application_name ) );

		search_replace_word( 
			executable,
			"$application",
			double_quotes_around(
				buffer, 
				application_name ) );

		search_replace_word( 
			executable,
			"$customer",
			double_quotes_around( 	buffer, 
				      		application_name ) );
	}

	if ( folder_name && *folder_name )
	{
		search_replace_word(
			executable,
			"$folder",
			double_quotes_around( 	buffer, 
				      		folder_name ) );
	}

	if ( role_name && *role_name )
	{
		search_replace_word(
			executable,
			"$role",
			double_quotes_around( buffer, 
				      	role_name ) );
	}

	if ( process_set_name && *process_set_name )
	{
		search_replace_word(
			executable,
			"$process_set",
			double_quotes_around( 	buffer, 
				      		process_set_name ) );
	}
	else
	{
		search_replace_word(
			executable,
			"$process_set",
			double_quotes_around( 	buffer, 
				      		"" ) );
	}

	if ( process_name && *process_name )
	{
		search_replace_word(
			executable,
			"$process",
			double_quotes_around( 	buffer, 
				      		process_name ) );
	}

	if ( one2m_folder_name_for_process
	&&   *one2m_folder_name_for_process )
	{
		double_quotes_around(
			buffer, 
			one2m_folder_name_for_process );

		search_replace_word(
			executable,
			"$one2m_folder_name_for_processes",
			buffer );

		search_replace_word(
			executable,
			"$one2m_folder_name_for_process",
			buffer );

		search_replace_word(
			executable,
			"$one2m_folder_name",
			buffer );

		search_replace_word(
			executable,
			"$one2m_related_folder",
			buffer );

		search_replace_word(
			executable,
			"$one2m_folder",
			buffer );
	}

}

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

void process_set_one2m_folder_name_for_process(
			DICTIONARY *dictionary,
			char *one2m_folder_name )
{
	char key[ 128 ];

	strcpy( key, ONE2M_FOLDER_NAME_FOR_PROCESS );

	dictionary_add_pointer(
		dictionary,
		strdup( key ),
		one2m_folder_name );

	sprintf( key, "%s_0", ONE2M_FOLDER_NAME_FOR_PROCESS );

	dictionary_add_pointer(
		dictionary,
		strdup( key ),
		one2m_folder_name );
}

void process_search_replace_where(
			char *command_line,
			DICTIONARY *preprompt_dictionary,
			char *folder_name,
			char *role_name,
			char *login_name )
{
	QUERY *query;
	char *where_clause;
	char buffer[ QUERY_WHERE_BUFFER ];

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

	if ( ! ( query =
			query_search_replace_where(
				preprompt_dictionary,
				folder_name,
				role_name,
				login_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_process_parameter_new() returned empty.\n",
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
			query->where ) );
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

void process_operation_convert(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			LIST *append_isa_attribute_list,
			LIST *primary_key_list,
			LIST *primary_data_list,
			int row,
			char *process_name,
			char *operation_row_count_string )
{
	char buffer[ 65536 ];
	char local_executable[ 65536 ];
	DICTIONARY *local_parameter_dictionary = {0};
	char process_id_string[ 16 ];
	LIST *local_attribute_name_list;

	timlib_strcpy( local_executable, *executable, 65536 );

	/* This memory is always strdup(). */
	/* ------------------------------- */
	free( *executable );

	sprintf( process_id_string, "%d", getpid() );

	if ( dictionary_length( parameter_dictionary ) )
	{
		char *data;

		local_parameter_dictionary =
			dictionary_copy_dictionary(
				parameter_dictionary );

		dictionary_parse_multi_attribute_relation_operator_keys(
			local_parameter_dictionary, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

		dictionary_set_pointer(
			local_parameter_dictionary,
			PROCESS_ID_LABEL,
			strdup( process_id_string ) );

		attribute_set_dictionary_date_international(
			local_parameter_dictionary,
			append_isa_attribute_list );

		if ( operation_row_count_string )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						OPERATION_ROW_COUNT_LABEL,
						operation_row_count_string );
		}

		if ( !dictionary_exists_key_index(
				local_parameter_dictionary,
				"login_name",
				0 ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"login_name_0",
						person );
		}

		if ( process_name && *process_name )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"process",
						process_name );
		}

		if ( ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"from_execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"execute_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"really_yn_0",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"from_really_yn",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"really_yn",
						data );
		}
		else
		if ( ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"from_really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_parameter_dictionary,
						"really_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_parameter_dictionary,
						"execute_yn_0",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"from_execute_yn",
						data );

			dictionary_set_pointer(	local_parameter_dictionary,
						"execute_yn",
						data );
		}

		dictionary_search_replace_command_arguments(
			local_executable,
			local_parameter_dictionary, 
			row );

	} /* if parameter_dictionary */

	if ( primary_data_list )
	{
		search_replace_word(	local_executable,
					"$primary_data_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_data_list,
						FOLDER_DATA_DELIMITER ) );
	}

	if ( primary_key_list )
	{
		search_replace_word(	local_executable,
					"$primary_key",
					list_display_quoted_delimiter( 
						buffer,
						primary_key_list,
						',' ) );

		search_replace_word(	local_executable,
					"$primary_attribute_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_key_list,
						',' ) );

	}

	search_replace_list_index_prepend_double_quoted(
		local_executable, 
		( local_attribute_name_list =
			attribute_name_list_extract(
				append_isa_attribute_list ) ),
		local_parameter_dictionary,
		0, 	                  /* dictionary_key_offset */
		QUERY_FROM_STARTING_LABEL,/* dictionary_key_prepend */
		' '	                  /* replace_char_prepend */
		);

	search_replace_list_index_prepend_double_quoted(
		local_executable, 
		local_attribute_name_list,
		local_parameter_dictionary,
		0, 	             /* dictionary_key_offset */
		"",		    /* dictionary_key_prepend */
		' '	              /* replace_char_prepend */
		);

	process_replace_parameter_variables(
				local_executable,
				application_name,
				session,
				state,
				person,
				folder_name,
				role_name,
				target_frame,
				process_name,
				(char *)0 /* process_set_name */,
				operation_row_count_string,
				(char *)0
					/* one2m_folder_name_for_process */,
				(char *)0 /* prompt */,
				process_id_string );

	if ( state && *state )
	{
		dictionary_set_pointer(
			local_parameter_dictionary,
			"state",
			state );
	}

	if ( timlib_exists_string( local_executable, "$dictionary" )
	&&   dictionary_length( local_parameter_dictionary ) )
	{
		search_replace_word(
			local_executable,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					local_parameter_dictionary, '&' ) 
				) );
	}

	if ( timlib_exists_string( local_executable, "$where" ) )
	{
		process_search_replace_executable_where(
			local_executable,
			application_name,
			folder_name,
			append_isa_attribute_list,
			where_clause_dictionary );
	}

	sprintf(	local_executable + strlen( local_executable ),
			" 2>>%s",
			appaserver_error_get_filename(
				application_name ) );

	strcpy( buffer, local_executable );
	escape_character( local_executable, buffer, '$' );

	remove_character( local_executable, '`' );

	*executable = strdup( local_executable );

}

void process_prompt_convert_parameters(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			DICTIONARY *preprompt_dictionary,
			LIST *attribute_list,
			int row,
			char *process_name,
			char *one2m_folder_name_for_process )
{
	char buffer[ 65536 ];
	char local_executable[ 65536 ];
	LIST *attribute_name_list = {0};
	DICTIONARY *local_preprompt_dictionary = {0};
	char *process_set_name = {0};
	char process_id_string[ 16 ];

	timlib_strcpy( local_executable, *executable, 65536 );

	/* This memory is always strdup(). */
	/* ------------------------------- */
	free( *executable );

	sprintf( process_id_string, "%d", getpid() );

	if ( dictionary_length( preprompt_dictionary ) )
	{
		char *data;

		local_preprompt_dictionary =
			dictionary_copy_dictionary(
				preprompt_dictionary );

		dictionary_parse_multi_attribute_relation_operator_keys(
			local_preprompt_dictionary, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

		dictionary_set_pointer(
			local_preprompt_dictionary,
			PROCESS_ID_LABEL,
			strdup( process_id_string ) );

		if ( list_length( attribute_list ) )
		{
			attribute_set_dictionary_date_international(
				local_preprompt_dictionary,
				attribute_list );
		}

		if ( !dictionary_exists_key_index(
				local_preprompt_dictionary,
				"login_name",
				0 ) )
		{
			dictionary_set_pointer(	local_preprompt_dictionary,
						"login_name_0",
						person );
		}

		if ( one2m_folder_name_for_process )
		{
			process_set_one2m_folder_name_for_process(
				local_preprompt_dictionary,
				one2m_folder_name_for_process );
		}

		if ( process_name && *process_name )
		{
			dictionary_set_pointer(	local_preprompt_dictionary,
						"process",
						process_name );
		}

		if ( ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"from_execute_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"execute_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_preprompt_dictionary,
						"really_yn_0",
						data );

			dictionary_set_pointer(	local_preprompt_dictionary,
						"from_really_yn",
						data );

			dictionary_set_pointer(	local_preprompt_dictionary,
						"really_yn",
						data );
		}
		else
		if ( ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"from_really_yn" ) )
		||   ( data = dictionary_get_pointer(
						local_preprompt_dictionary,
						"really_yn_0" ) ) )
		{
			dictionary_set_pointer(	local_preprompt_dictionary,
						"execute_yn_0",
						data );

			dictionary_set_pointer(	local_preprompt_dictionary,
						"from_execute_yn",
						data );

			dictionary_set_pointer(	local_preprompt_dictionary,
						"execute_yn",
						data );
		}

		dictionary_search_replace_command_arguments(
			local_executable,
			local_preprompt_dictionary, 
			row );

	} /* if preprompt_dictionary */

	if ( attribute_name_list )
	{
		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			attribute_name_list,
			local_preprompt_dictionary,
			0, 	                  /* dictionary_key_offset */
			QUERY_FROM_STARTING_LABEL,/* dictionary_key_prepend */
			' '	                  /* replace_char_prepend */
			);

		search_replace_list_index_prepend_double_quoted(
			local_executable, 
			attribute_name_list,
			local_preprompt_dictionary,
			0, 	             /* dictionary_key_offset */
			"",		    /* dictionary_key_prepend */
			' '	              /* replace_char_prepend */
			);
	}

	process_replace_parameter_variables(
				local_executable,
				application_name,
				session,
				state,
				person,
				folder_name,
				role_name,
				(char *)0 /* target_frame */,
				process_name,
				process_set_name,
				(char *)0 /* operation_row_count_string */,
				one2m_folder_name_for_process,
				(char *)0 /* prompt */,
				process_id_string );

	if ( state && *state )
	{
		dictionary_set_pointer(
			local_preprompt_dictionary,
			"state",
			state );
	}

	if ( timlib_exists_string( local_executable, "$dictionary" )
	&&   dictionary_length( local_preprompt_dictionary ) )
	{
		search_replace_word(
			local_executable,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					local_preprompt_dictionary, '&' ) 
				) );
	}

	if ( timlib_exists_string( local_executable, "$where" ) )
	{
		process_search_replace_executable_where(
			local_executable,
			application_name,
			folder_name,
			attribute_list,
			local_preprompt_dictionary );
	}

	sprintf(	local_executable + strlen( local_executable ),
			" 2>>%s",
			appaserver_error_get_filename(
				application_name ) );

	strcpy( buffer, local_executable );
	escape_character( local_executable, buffer, '$' );

	remove_character( local_executable, '`' );

	*executable = strdup( local_executable );
}

PROMPT *prompt_fetch( char *prompt_name )
{
	return
	prompt_parse(
		string_fetch_pipe(
			prompt_system_string(
				prompt_primary_where(
					prompt_name ) ) ) );
}

char *prompt_primary_where( char *prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"prompt = '%s'",
		prompt_name );

	return where;
}

char *prompt_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		PROMPT_TABLE,
		where );

	return strdup( system_string );
}

PROMPT *prompt_new( char *prompt_name )
{
	PROMPT *p = calloc( 1, sizeof( PROMPT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( PROMPT ) );
		exit( 1 );
	}

	p->prompt_name = prompt_name;
	return p;
}

PROMPT *prompt_parse( char *input )
{
	char prompt_name[ 256 ];
	char buffer[ 4096 ];
	PROMPT *prompt;

	/* See attribute_list prompt */
	/* ------------------------- */
	piece( prompt_name, SQL_DELIMITER, input, 0 );
	prompt = prompt_new( strdup( prompt_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	prompt->input_width = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	prompt->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	prompt->upload_filename = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 4 );
	prompt->date = ( *buffer == 'y' );

	return prompt;
}

DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_new(
			char *drop_down_prompt_name,
			char *drop_down_prompt_data )
{
	DROP_DOWN_PROMPT_DATA *p = calloc( 1, sizeof( DROP_DOWN_PROMPT_DATA ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( DROP_DOWN_PROMPT_DATA ) );
		exit( 1 );
	}

	p->drop_down_prompt_name = drop_down_prompt_name;
	p->drop_down_prompt_data = drop_down_prompt_data;

	return p;
}

DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_parse(
			char *input )
{
	char drop_down_prompt_name[ 256 ];
	char drop_down_prompt_data[ 256 ];
	char display_order[ 128 ];
	DROP_DOWN_PROMPT_DATA *drop_down_prompt_data;

	/* See attribute_list drop_down_prompt_data */
	/* ---------------------------------------- */
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 0 );
	piece( drop_down_prompt_data, SQL_DELIMITER, input, 1 );
	piece( display_order, SQL_DELIMITER, input, 3 );

	drop_down_prompt_data =
		drop_down_prompt_data_new(
			strdup( drop_down_prompt_name ),
			strdup( drop_down_prompt_data ) );

	drop_down_prompt_data->display_order = atoi( display_order );

	return drop_down_prompt_data;
}

char *drop_down_prompt_data_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		DROP_DOWN_PROMPT_DATA_TABLE,
		where );

	return strdup( system_string );
}

LIST *drop_down_prompt_data_system_list(
			char *system_string )
{
	LIST *list = {0};
	char input[ 1024 ];
	DROP_DOWN_PROMPT_DATA *drop_down_prompt_data;
	FILE *pipe = popen( system_string, "r" );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( drop_down_prompt_data =
				drop_down_prompt_data_parse(
					input ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, drop_down_prompt_data );
		}
	}

	pclose( pipe );
	return list;
}

DROP_DOWN_PROMPT *drop_down_prompt_new(
			char *drop_down_prompt_name )
{
	DROP_DOWN_PROMPT *p = calloc( 1, sizeof( DROP_DOWN_PROMPT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc(1,%d) returned empty.",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( DROP_DOWN_PROMPT ) );
		exit( 1 );
	}

	p->drop_down_prompt_name = drop_down_prompt_name;
	return p;
}

DROP_DOWN_PROMPT *drop_down_prompt_fetch(
			char *drop_down_prompt_name )
{
	DROP_DOWN_PROMPT *drop_down_prompt;

	drop_down_prompt =
		drop_down_prompt_parse(
			string_fetch_pipe(
				drop_down_prompt_prompt_system_string(
					drop_down_prompt_primary_where(
						drop_down_prompt_name ) ) ) );

	drop_down_prompt->drop_down_prompt_data_list =
		drop_down_prompt_data_system_list(
			drop_down_prompt_data_system_string(
				drop_down_prompt_primary_where(
					drop_down_prompt_name ) ) );
	return drop_down_prompt;
}

DROP_DOWN_PROMPT *drop_down_prompt_parse(
			char *input )
{
	char drop_down_prompt_name[ 256 ];
	char buffer[ 2048 ];
	DROP_DOWN_PROMPT *drop_down_prompt;

	/* See attribute_list drop_down_prompt */
	/* ----------------------------------- */
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 0 );

	drop_down_prompt =
		drop_down_prompt_new(
			strdup( drop_down_prompt_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	drop_down_prompt->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	drop_down_prompt->optional_display = strdup( buffer );

	return drop_down_prompt;
}

char *drop_down_prompt_primary_where(
			char *drop_down_prompt_name )
{
	static char where[ 256 ];

	sprintf(where,
		"drop_down_prompt = '%s'",
		drop_down_prompt );

	return where;
}

char *drop_down_prompt_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\"",
		DROP_DOWN_PROMPT_TABLE,
		where );

	return strdup( system_string );
}

LIST *process_parameter_system_list(
			char *system_string,
			DICTIONARY *preprompt_dictionary,
			char *login_name )
{
	LIST *list = {0};
	char input[ 1024 ];
	PROCESS_PARAMETER *process_parameter;
	FILE *pipe = popen( system_string, "r" );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( process_parameter =
				process_parameter_parse(
					input,
					preprompt_dictionary,
					login_name ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, process_parameter );
		}
	}

	pclose( pipe );
	return list;
}

char *process_parameter_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		PROCESS_PARAMETER_SELECT,
		PROCESS_PARAMETER_TABLE,
		where );

	return strdup( system_string );
}

char *process_set_parameter_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" display_order",
		PROCESS_SET_PARAMETER_SELECT,
		PROCESS_SET_PARAMETER_TABLE,
		where );

	return strdup( system_string );
}

PROCESS_PARAMETER *process_parameter_parse(
			char *input,
			DICTIONARY *preprompt_dictionary,
			char *login_name )
{
	char process_or_process_set_name[ 128 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char drop_down_prompt_name[ 128 ];
	char prompt_name[ 128 ];
	char buffer[ 128 ];
	PROCESS_PARAMETER *process_parameter;
	boolean ok_return = 0;

	/* See PROCESS_PARAMETER_SELECT or PROCESS_SET_PARAMETER_SELECT */
	/* ------------------------------------------------------------ */
	piece( process_or_process_set_name, SQL_DELIMITER, input, 0 );
	piece( folder_name, SQL_DELIMITER, input, 1 );
	piece( attribute_name, SQL_DELIMITER, input, 2 );
	piece( drop_down_prompt_name, SQL_DELIMITER, input, 3 );
	piece( prompt_name, SQL_DELIMITER, input, 4 );

	process_parameter =
		process_parameter_new(
			strdup( process_or_process_set_name ),
			strdup( folder_name ),
			strdup( attribute_name ),
			strdup( drop_down_prompt_name ),
			strdup( prompt_name ) );

	process_parameter->preprompt_dictionary = preprompt_dictionary;
	process_parameter->login_name = login_name;

	piece( buffer, SQL_DELIMITER, input, 5 );
	process_parameter->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	process_parameter->drop_down_multi_select = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	process_parameter->preprompt = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	process_parameter->populate_drop_down_process_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	process_parameter->populate_helper_process_name = strdup( buffer );

	if ( strcmp( process_parameter->folder_name, "null" != 0 )
	{
		process_parameter->primary_delimited_list =
			process_parameter_primary_delimited_list(
				process_parameter->preprompt_dictionary,
				process_parameter->login_name,
				process_parameter->folder_name,
				process_parameter->
					populate_drop_down_process_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->attribute_name, "null" ) != 0 )
	{
		process_parameter->attribute =
			attribute_fetch(
				process_parameter->
					attribute_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->drop_down_prompt_name, "null" ) != 0 )
	{
		process_parameter->drop_down_prompt =
			drop_down_prompt_fetch(
				process_parameter->
					drop_down_prompt_name );
		ok_return = 1;
	}
	else
	if ( strcmp( process_parameter->prompt, "null" ) != 0 )
	{
		process_parameter->prompt =
			prompt_fetch(
				process_parameter->
					prompt_name );
		ok_return = 1;
	}

	if ( ok_return )
		return process_parameter;
	else
		return (PROCESS_PARAMETER *)0;
}

char *process_primary_where( char *process_name )
{
	static char where[ 256 ];

	sprintf(where,
		"process = '%s'",
		process_name );

	return where;
}

char *process_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\"",
		PROCESS_SELECT,
		PROCESS_TABLE,
		where );

	return strdup( system_string );
}

PROCESS *process_parse(	char *input,
			char *role_name,
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

	process->role_name = role_name;

	process->
		check_executable_inside_filesystem =
			check_executable_inside_filesystem;

	piece( buffer, SQL_DELIMITER, input, 1 );
	process->command_line = strdup( buffer );

	if ( process->check_executable_inside_filesystem
	&&   !process_executable_ok( process->command_line ) )
	{
		char msg[ 1024 ];

		sprintf( msg,
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

	return process;
}

PROCESS *process_fetch(
			char *process_name,
			char *role_name,
			boolean check_executable_inside_filesystem )
{
	return
	process_parse(
		string_pipe_fetch(
			process_system_string(
				process_primary_where(
					process_name ) ) ),
		role_name,
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
			boolean check_executable_inside_filesystem )
{
	return
	process_set_parse(
		string_pipe_fetch(
			process_set_system_string(
				process_set_primary_where(
					process_set_name ) ) ),
		role_name,
		check_executable_inside_filesystem );
}

char *process_set_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s"",
		PROCESS_SET_SELECT,
		PROCESS_SET_TABLE,
		where );

	return strdup( system_string );
}

PROCESS_SET *process_set_parse(
			char *input,
			char *role_name,
			boolean check_executable_inside_filesystem )
{
	PROCESS_SET *process_set;
	char process_set_name[ 128 ];
	char buffer[ 65536 ];

	/* See PROCESS_SET_SELECT */
	/* ---------------------- */
	piece( process_set_name, SQL_DELIMITER, input, 0 );
	process_set = process_set_new( strdup( process_set_name ) );

	process_set->role_name = role_name;

	process_set->
		check_executable_inside_filesystem =
			check_executable_inside_filesystem;

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

	process_set->process_set_role_process_name_list =
		process_set_role_process_name_list(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_set_primary_where(
				process_set_name ),
			role_name );

	if ( list_length( 
		process_set->
			process_set_role_process_name_list  ) )
	{
		process_set->process_set_process_where =
			process_set_process_where(
				process_set->
					process_set_role_process_name_list  );

		process_set->process_list =
			process_system_list(
				process_system_string(
					process_set->
						process_set_process_where ),
				process_set->
					check_executable_inside_filesystem );
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
		"select.sh process role_process_set_member \"%s\",
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

LIST *process_parameter_primary_delimited_list(
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *populate_drop_down_process_name )
{
	if ( *process_parameter->populate_drop_down_process_name )
	{
		PROCESS *process =
			process_fetch(
				populate_drop_down_process_name,
				1 /* check_executable... */ );

		if ( !process )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: process_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				populate_drop_down_process_name );
			exit( 1 );
		}

		return
			process_parameter_evaluate_list(
				process->command_line,
				preprompt_dictionary,
				login_name,
				role_name,
				folder_name );
	}
	else
	{
		LIST *list =
			folder_attribute_list(
				folder_name,
				(LIST *)0 /* exclude_attribute_name_list */ );

		return
		folder_query_primary_delimited_list(
			folder_table_name(
				environment_application_name(),
				folder_name ),
			list /* folder_attribute_list */,
			folder_attribute_primary_name_list( list ),
			login_name );
	}
}

LIST *process_parameter_fetch_list(
			char *command_line,
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	LIST *return_list;

	command_line =
		/* ------------------------------------------ */
		/* Frees command_line and returns heap memory */
		/* ------------------------------------------ */
		process_parameter_command_line_replace(
			command_line,
			preprompt_dictionary,
			login_name,
			role_name,
			folder_name,
			environment_application_name() );

fprintf( stderr, "%s/%s()/%d: command_line = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
command_line );

	return_list =
		process_fetch_list(
			command_line );

	if ( piece_multi_attribute_data_label_delimiter )
	{
		return_list =
			list_usage_piece_list(
				return_list,
				piece_multi_attribute_data_label_delimiter,
				0 /* offset */ );
	}

	return return_list;
}

char *process_parameter_command_line_replace(
			char *command_line,
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *application_name )
{
	char buffer[ STRING_WHERE_BUFFER ];
	char local_command_line[ STRING_WHERE_BUFFER ];
	char buffer[ 1024 ];
	char *data;

	string_strcpy( local_command_line, command_line, STRING_WHERE_BUFFER );

	search_replace_word(
		local_command_line,
		"$login_name",
		double_quotes_around(
			buffer, 
			login_name ) );

	search_replace_word(
		local_command_line,
		"$role",
		double_quotes_around(
			buffer, 
			role_name ) );

	search_replace_word(
		local_command_line,
		"$folder",
		double_quotes_around(
			buffer, 
			folder_name ) );

	if ( string_exists( local_command_line, "$dictionary" ) )
	{
		search_replace_word(
			local_command_line,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					preprompt_dictionary, '&' ) 
				) );
	}

	if ( string_exists( local_command_line, "$where" ) )
	{
		process_search_replace_where(
			local_command_line,
			preprompt_dictionary,
			login_name,
			role_name,
			folder_name );
	}

	if ( dictionary_length( preprompt_dictionary ) )
	{ 
		dictionary_search_replace_command_arguments(
			local_command_line,
			preprompt_dictionary, 
			0 /* row  */ );
	}

	sprintf(	local_command_line + strlen( local_command_line ),
			" 2>>%s",
			appaserver_error_get_filename(
				application_name ) );

	/* This memory is always heap */
	/* -------------------------- */
	free( command_line );

	return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			local_command_line ) );
}

char *process_update_command_line(
			char *command_line,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			int row,
			char *process_name,
			char *one2m_folder_name,
			LIST *primary_data_list )
{
	char buffer[ STRING_SYSTEM_BUFFER ];
	char local_command_line[ STRING_SYSTEM_BUFFER ];
	char process_id_string[ 16 ];

	string_strcpy(
		local_command_line,
		command_line,
		STRING_SYSTEM_BUFFER );

	sprintf( process_id_string, "%d", getpid() );

	if ( list_length( primary_data_list ) )
	{
		search_replace_word(
			local_command_line,
			"$primary_data_list",
			list_display_quoted_delimiter( 
				buffer,
				primary_data_list,
				SQL_DELIMITER ) );
	}

	process_replace_parameter_variables(
		local_command_line,
		session,
		state,
		login_name,
		folder_name,
		role_name,
		target_frame,
		process_name,
		process_set_name,
		operation_row_count_string,
		one2m_folder_name,
		prompt,
		process_id_string,
		query_dictionary );

	if ( string_exists( local_command_line, "$where" ) )
	{
		process_search_replace_where(
			local_command_line,
			preprompt_dictionary,
			folder_name,
			role_name,
			login_name );
	}

	sprintf(local_command_line + strlen( local_command_line ),
		" 2>>%s",
		appaserver_error_filename(
			environment_application_name() ) );

	string_strcpy( buffer, local_command_line, STRING_SYSTEM_BUFFER );
	escape_character( local_command_line, buffer, '$' );
	remove_character( local_command_line, '`' );

	/* This memory is always strdup(). */
	/* ------------------------------- */
	free( command_line );

	return strdup( local_command_line );
}

void process_replace_parameter_variables(	
			char *command_line,
			char *session,
			char *state,
			char *login_name,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *process_name,
			char *process_set_name,
			char *operation_row_count_string,
			char *one2m_folder_name,
			char *prompt,
			char *process_id_string,
			DICTIONARY *post_dictionary )
{
	char buffer[ 1024 ];

	if ( process_id_string && *process_id_string )
	{
		search_replace_word(
			command_line,
			"$process_id",
			double_quotes_around(
				buffer, 
				process_id_string ) );
	}

	if ( operation_row_count_string && *operation_row_count )
	{
		search_replace_word(
			command_line,
			"$operation_row_count",
			double_quotes_around(
				buffer, 
				operation_row_count_string ) );
	}

	if ( login_name && *login_name )
	{
		search_replace_word(
			command_line,
			"$login_name",
			double_quotes_around(
				buffer, 
				login_name ) );
	}

	if ( prompt && *prompt )
	{
		search_replace_word(
			command_line,
			"$prompt",
			double_quotes_around(
				buffer, 
				prompt ) );
	}

	if ( target_frame && *target_frame )
	{
		search_replace_word( 
			command_line,
			"$target_frame",
			double_quotes_around(
				buffer, 
				target_frame ) );
	}

	if ( session && *session )
	{
		search_replace_word(
			command_line,
			"$session",
			double_quotes_around(
				buffer, 
				session ) );
	}

	if ( state && *state )
	{
		search_replace_word(
			command_line,
			"$state",
			double_quotes_around(
				buffer, 
				state ) );
	}

	if ( folder_name && *folder_name )
	{
		search_replace_word(
			command_line,
			"$folder",
			double_quotes_around(
				buffer, 
				folder_name ) );
	}

	if ( role_name && *role_name )
	{
		search_replace_word(
			command_line,
			"$role",
			double_quotes_around(
				buffer, 
				role_name ) );
	}

	if ( process_set_name && *process_set_name )
	{
		search_replace_word(
			command_line,
			"$process_set",
			double_quotes_around(
				buffer, 
				process_set_name ) );
	}
	else
	{
		search_replace_word(
			command_line,
			"$process_set",
			double_quotes_around(
				buffer, 
				"" ) );
	}

	if ( process_name && *process_name )
	{
		search_replace_word(
			command_line,
			"$process",
			double_quotes_around(
				buffer, 
				process_name ) );
	}

	if ( one2m_folder_name && *one2m_folder_name )
	{
		double_quotes_around(
			buffer, 
			one2m_folder_name );

		search_replace_word(
			command_line,
			"$one2m_folder_name_for_processes",
			buffer );

		search_replace_word(
			command_line,
			"$one2m_folder_name_for_process",
			buffer );

		search_replace_word(
			command_line,
			"$one2m_folder_name",
			buffer );

		search_replace_word(
			command_line,
			"$one2m_related_folder",
			buffer );

		search_replace_word(
			command_line,
			"$one2m_folder",
			buffer );
	}

	if ( string_exists( command_line, "$dictionary" ) )
	{
		search_replace_word(
			command_line,
			"$dictionary",
			double_quotes_around(
				buffer, 
				dictionary_display_delimited(
					post_dictionary, '&' ) 
				) );
	}
}

void process_search_replace_where(
			char *command_line,
			DICTIONARY *preprompt_dictionary,
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
			query_process_parameter_new(
				preprompt_dictionary,
				folder_name,
				role_name,
				login_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_process_parameter_new() returned empty.\n",
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

