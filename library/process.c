/* $APPASERVER_HOME/library/process.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process and process_set ADTs.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "query.h"
#include "process_parameter_list.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "hashtbl.h"
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

PROCESS_SET *process_new_process_set(
				char *application_name,
				char *session, 
				char *process_set_name )
{
	PROCESS_SET *p = (PROCESS_SET *)calloc( 1, sizeof( PROCESS_SET ) );

	if ( !p )
	{
		char msg[ 1024 ];
		sprintf( msg, "ERROR in %s/%s(): cannot allocate %d bytes",
			 __FILE__,
			 __FUNCTION__,
			 (int)sizeof( PROCESS_SET ) );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	p->process_set_name = process_set_name;

	if ( !process_set_load(	&p->notepad,
				&p->html_help_file_anchor,
				&p->post_change_javascript,
				&p->prompt_display_text,
				&p->preprompt_help_text,
				&p->prompt_display_bottom,
				application_name,
				session,
				process_set_name ) )
	{
		return (PROCESS_SET *)0;
	}
	return p;
} /* process_new_process_set() */

PROCESS *process_new(		char *application_name,
				char *process_name )
{
	PROCESS *p = (PROCESS *)calloc( 1, sizeof( PROCESS ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate %d bytes",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 (int)sizeof( PROCESS ) );
		exit( 1 );
	}

	if ( !process_load(	&p->executable,
				&p->notepad,
				&p->html_help_file_anchor,
				&p->post_change_javascript,
				&p->process_set_display,
				&p->preprompt_help_text,
				&p->is_appaserver_process,
				application_name,
				process_name,
				1 /* with_check_executable_ok */ ) )
	{
		return (PROCESS *)0;
	}

	p->application_name = application_name;
	p->process_name = process_name;

	return p;

} /* process_new() */

PROCESS *process_new_process(	char *application_name,
				char *session,
				char *process_name,
				DICTIONARY *dictionary,
				char *role_name,
				boolean with_check_executable_ok )
{
	PROCESS *p = (PROCESS *)calloc( 1, sizeof( PROCESS ) );

	if ( !p )
	{
		char msg[ 1024 ];
		sprintf( msg, "ERROR in %s/%s(): cannot allocate %d bytes",
			 __FILE__,
			 __FUNCTION__,
			 (int)sizeof( PROCESS ) );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	if ( !process_load(	&p->executable,
				&p->notepad,
				&p->html_help_file_anchor,
				&p->post_change_javascript,
				&p->process_set_display,
				&p->preprompt_help_text,
				&p->is_appaserver_process,
				application_name,
				process_name,
				with_check_executable_ok ) )
	{
		if ( dictionary && dictionary_get_index_data(
							&process_name,
							dictionary,
							process_name,
							0 ) == -1 )
		{
			return (PROCESS *)0;
		}
		else
		{
			if ( !process_load(	&p->executable,
						&p->notepad,
						&p->html_help_file_anchor,
						&p->post_change_javascript,
						&p->process_set_display,
						&p->preprompt_help_text,
						&p->is_appaserver_process,
						application_name,
						process_name,
						with_check_executable_ok ) )
			{
				return (PROCESS *)0;
			}
		}

	}

	p->application_name = application_name;
	p->session = session;
	p->process_name = process_name;
	p->attribute_list = list_new_list();
	p->role_name = role_name;
	
	return p;

} /* process_new_process() */

void process_free( PROCESS *process )
{
	list_free( process->attribute_list );
	free( process->notepad );
	free( process->html_help_file_anchor );
	free( process->post_change_javascript );
	free( process->process_set_display );
	free( process );

} /* process_free() */


LIST *process2list( char *executable )
{
	return pipe2list( executable );
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
			LIST *primary_attribute_name_list,
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
						FOLDER_DATA_DELIMITER ) );
	}

	if ( primary_attribute_name_list )
	{
		search_replace_word(	local_executable,
					"$primary_attribute_list",
					list_display_quoted_delimiter( 
						buffer,
						primary_attribute_name_list,
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
				application_name,
				session,
				state,
				person,
				folder_name,
				role_name,
				target_frame,
				process_name,
				process_set_name,
				operation_row_count_string,
				one2m_folder_name_for_process,
				prompt,
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

} /* process_convert_parameters() */

char *process_get_process_member_from_dictionary(
				char *process_set,
				DICTIONARY *parsed_decoded_post_dictionary,
				char *from_starting_label )
{
	char *process_member;
	char key[ 1024 ];
	int results;

	sprintf( key, "%s%s",
		 from_starting_label,
		 process_set );

	results = dictionary_get_index_data(
				&process_member,
				parsed_decoded_post_dictionary,
				key,
				0 /* index */ );
	if ( results > -1 )
		return process_member;
	else
		return (char *)0;
} /* process_get_process_member_from_dictionary() */

void process_for_folder_or_attribute_parameters_populate_attribute_list(
				LIST *attribute_list,
				char *application_name,
				LIST *process_parameter_list )
{
	PROCESS_PARAMETER *process_parameter;
	char *type;

	if ( !list_reset( process_parameter_list ) ) return;

	do {
		process_parameter = list_get( process_parameter_list );

		if ( !process_parameter->type )
		{
			fprintf(stderr,
		"Warning: %s/%s() got an empty process_parameter\n",
				__FILE__,
				__FUNCTION__ );
			continue;
		}

		type = process_parameter->type;

		if ( strcmp( type, "folder" ) == 0 )
		{
			FOLDER *folder;

			if ( !process_parameter->
				parameter_folder->
					folder_name )
			{
				char msg[ 1024 ];
				sprintf(msg,
"ERROR: %s/%s() got a folder type but an empty folder_name\n",
					__FILE__,
					__FUNCTION__ );
				appaserver_output_error_message(
					application_name,
					msg,
					(char *)0 );
				exit( 1 );
			}

			folder = 
				folder_new_folder(
					application_name,
					BOGUS_SESSION,
					process_parameter->
					  	parameter_folder->
					  	folder_name );

			folder->attribute_list =
				attribute_get_attribute_list(
					folder->application_name,
					folder->folder_name,
					(char *)0 /* attribute_name */,
					(LIST *)0
					     /* mto1_isa_related_folder_list */,
					(char *)0 /* role_name */ );

			list_append_list(
				attribute_list,
				folder->attribute_list );
		}
		else
		if ( strcmp( type, "attribute" ) == 0 )
		{
			ATTRIBUTE *attribute;

			attribute =
				process_get_attribute(		
					application_name,
					process_parameter->
						parameter_attribute->
						attribute_name );

			list_append_pointer( attribute_list, attribute );
		}
	} while( list_next( process_parameter_list ) );

} /* process_for_folder_or_attribute_parameters_populate_attribute_list() */

LIST *process_get_prompt_list( 	LIST *process_parameter_list )
{
	LIST *prompt_list;
	PROCESS_PARAMETER *process_parameter;
	char *type;

	prompt_list = list_new();

	if ( list_reset( process_parameter_list ) )
		do {
			process_parameter = (PROCESS_PARAMETER *)
				list_get( process_parameter_list );

			if ( !process_parameter->type )
			{
				fprintf(stderr,
			"Warning: %s/%s() got an empty process_parameter\n",
					__FILE__,
					__FUNCTION__ );
				continue;
			}

			type = process_parameter->type;

			if ( strcmp( type, "prompt" ) == 0 )
			{
				list_append_string(	prompt_list,
							process_parameter->
							parameter_prompt->
							prompt );
			}
			else
			if ( strcmp( type, "folder" ) == 0 
			&&   process_parameter->parameter_folder->prompt )
			{
				list_append_string(	prompt_list,
							process_parameter->
							parameter_folder->
							prompt );
			}

		} while( list_next( process_parameter_list ) );

	list_separate( prompt_list, MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	return prompt_list;
} /* process_get_prompt_list() */

ATTRIBUTE *process_get_attribute( 	char *application_name, 
					char *attribute_name )
{
	return attribute_load_attribute(
			application_name,
			attribute_name );
}

void process_set_from_folder(	PROCESS *process,
				char *from_folder )
{
	process->from_folder = strdup( from_folder );
}


void process_append_related_folder(
				PROCESS *process,
				char *related_folder )
{
	list_append_string( process->related_folder_list, related_folder );
}

int process_set_load(	char **notepad,
			char **html_help_file_anchor,
			char **post_change_javascript,
			char **prompt_display_text,
			char **preprompt_help_text,
			boolean *prompt_display_bottom,
			char *application_name,
			char *session,
			char *process_set_name )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 2048 ];
	char *record;

	sprintf( sys_string,
		 "process_set_get_record.sh %s %s '%s' 2>>%s",
		 application_name,
		 session,
		 process_set_name,
		 appaserver_error_get_filename(
				application_name ) );

	if ( ! ( record = pipe2string( sys_string ) ) ) return 0;

	piece( piece_buffer, '^', record, 0 );
	*notepad = strdup( piece_buffer );

	piece( piece_buffer, '^', record, 1 );
	*html_help_file_anchor = strdup( piece_buffer );

	piece( piece_buffer, '^', record, 2 );
	*post_change_javascript = strdup( piece_buffer );

	piece( piece_buffer, '^', record, 3 );
	*prompt_display_text = strdup( piece_buffer );

	piece( piece_buffer, '^', record, 4 );
	*preprompt_help_text = strdup( piece_buffer );

	piece( piece_buffer, '^', record, 5 );
	*prompt_display_bottom = ( *piece_buffer == 'y' );

	free( record );
	return 1;
} /* process_set_load() */

void process_load_executable(	char **executable,
				char *process_name,
				char *application_name )
{
	process_get_executable(	executable,
				process_name,
				application_name );
}

void process_get_executable(	char **executable,
				char *process_name,
				char *application_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf( where, "process = '%s'", process_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=command_line	"
		 "			folder=process		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	*executable = pipe2string( sys_string );

} /* process_get_executable() */

boolean process_load(	char **executable,
			char **notepad,
			char **html_help_file_anchor,
			char **post_change_javascript,
			char **process_set_display,
			char **preprompt_help_text,
			boolean *is_appaserver_process,
			char *application_name,
			char *process_name,
			boolean with_check_executable_ok )
{
	char piece_buffer[ 2048 ];
	char process_name_search[ 128 ];
	char *record;
	static LIST *process_record_list = {0};

	if ( !process_record_list )
	{
		process_record_list =
			process_load_record_list( application_name );
	}

	if ( !list_rewind( process_record_list ) ) return 0;

	do {
		record = list_get_pointer( process_record_list );

		piece( process_name_search, '^', record, 0 );

		if ( timlib_strcmp(
				process_name,
				process_name_search ) == 0 )
		{
			piece( piece_buffer, '^', record, 1 );
			*executable = strdup( piece_buffer );

			if ( with_check_executable_ok
			&&   !process_executable_ok( *executable ) )
			{
				char msg[ 1024 ];

				sprintf( msg,
"ERROR in %s/%s()/%d: EXECUTABLE NOT OK for application = (%s) IN (%s)\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 application_name,
					 *executable );

				appaserver_output_error_message(
					application_name,
					msg,
					(char *)0 );

				return 0;
			}

			piece( piece_buffer, '^', record, 2 );
			*notepad = strdup( piece_buffer );

			piece( piece_buffer, '^', record, 3 );
			*html_help_file_anchor = strdup( piece_buffer );

			piece( piece_buffer, '^', record, 4 );
			*post_change_javascript =
				strdup( piece_buffer );

			piece( piece_buffer, '^', record, 5 );
			*process_set_display =
				strdup( piece_buffer );

			piece( piece_buffer, '^', record, 6 );
			*preprompt_help_text =
				strdup( piece_buffer );

			piece( piece_buffer, '^', record, 7 );
			*is_appaserver_process =
				(*piece_buffer == 'y');

			return 1;
		}
	} while( list_next( process_record_list ) );

	return 0;
} /* process_load() */

LIST *process_load_record_list( char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "process_get_record_list.sh %s 2>>%s",
		 application_name,
		 appaserver_error_get_filename(
				application_name ) );

	return pipe2list( sys_string );
} /* process_load_record_list() */

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

} /* process_replace_parameter_variables() */

void process_increment_execution_count(
				char *application_name,
				char *process_name,
				char *database_management_system )
{
	char sys_string[ 1024 ];
	char *table_name;

/* stub */
/* ---- */
database_management_system = (char *)0;

	if ( !process_name || !*process_name ) return;

	table_name = get_table_name( application_name, "process" );

	sprintf(sys_string,
		"echo \"update %s					 "
		"	set execution_count =				 "
		"		if(execution_count,execution_count+1,1)	 "
		"	where process = '%s';\"				|"
		"sql.e							 ",
		table_name,
		process_name );

	system( sys_string );

} /* process_increment_execution_count() */

boolean process_exists_appaserver_process(
					char *application_name,
					LIST *process_name_list )
{
	PROCESS *process;
	boolean is_appaserver_process = 0;

	if ( !process_name_list || !list_rewind( process_name_list ) )
		return 0;

	do {
		process =
			process_new_process(
				application_name,
				(char *)0 /* session */,
				list_get_pointer( process_name_list ),
				(DICTIONARY *)0 /* dictionary */,
				(char *)0 /* role_name */,
				0 /* not with_check_executable_ok */ );

		if ( !process )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: process_new_process returned null\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		is_appaserver_process =
			process->is_appaserver_process;

		process_free( process );
		if ( is_appaserver_process ) return 1;
	} while( list_next( process_name_list ) );
	return 0;
} /* process_exists_appaserver_process() */

boolean process_executable_ok(	char *executable )
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

	directory = basename_get_directory( which_string );

	/* Anything in $CGI_HOME is okay. */
	/* ------------------------------ */
	if ( *directory == '.' ) return 1;

	/* Must execute from $APPASERVER_HOME/src_* */
	/* ---------------------------------------- */
	appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_mount_point();

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
	if ( timlib_exists_string( command, "appaserver" ) ) return 0;

	return 1;

} /* process_executable_ok() */

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
} /* process_set_one2m_folder_name_for_process() */

void process_search_replace_executable_where(
			char *local_executable,
			char *application_name,
			char *folder_name,
			LIST *attribute_list,
			DICTIONARY *where_clause_dictionary )
{
	QUERY *query;
	char *where_clause;
	char buffer[ 65536 ];

	if ( folder_name )
	{
		query =
			query_primary_data_new(
				application_name,
				(char *)0 /* login_name */,
				folder_name,
				where_clause_dictionary,
				(ROLE *)0,
				0 /* max_rows; zero for unlimited */,
				1 /* include_root_folder */ );

		where_clause = query->query_output->where_clause;

		if ( where_clause )
		{
			search_replace_word( 	local_executable, 
						"$where", 
						double_quotes_around( 	
							buffer,
							where_clause ) );
		}
		return;
	}

	if ( list_length( attribute_list ) )
	{
		QUERY *query;

		query = query_process_parameter_new(
				application_name,
				attribute_list,
				where_clause_dictionary );

		where_clause = query->query_output->where_clause;

		if ( where_clause )
		{
			search_replace_word( 	local_executable, 
						"$where", 
						double_quotes_around( 	
							buffer,
							where_clause ) );
		}
	}

} /* process_search_replace_executable_where() */

boolean process_interpreted_executable_ok( char *which_string )
{
	char sys_string[ 1024 ];

	if ( !timlib_character_exists( which_string, '.' ) )
		return 1;

	sprintf( sys_string,
		 "grep APPASERVER_DATABASE %s | head -1 | wc -l",
		 which_string );

	return (boolean)atoi( pipe2string( sys_string ) );

} /* process_interpreted_executable_ok() */
