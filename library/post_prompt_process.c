/* $APPASERVER_HOME/library/post_prompt_process.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "security.h"
#include "environ.h"
#include "dictionary_separate.h"
#include "process_parameter.h"
#include "folder_attribute.h"
#include "process.h"
#include "prompt_process.h"
#include "post_prompt_process.h"

POST_PROMPT_PROCESS *post_prompt_process_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean has_drillthru,
			boolean is_drillthru,
			POST_DICTIONARY *post_dictionary,
			char *document_root,
			char *application_relative_source_directory )
{
	POST_PROMPT_PROCESS *post_prompt_process =
		post_prompt_process_calloc();

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name
	||   !post_dictionary
	||   !document_root
	||   !application_relative_source_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_prompt_process->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name );

	post_prompt_process->process_parameter_folder_name_list =
		process_parameter_folder_name_list(
			post_prompt_process->
				session_process->
				process_name,
			post_prompt_process->
				session_process->
				process_set_name,
			is_drillthru );

	post_prompt_process->folder_attribute_name_list_attribute_list =
		folder_attribute_name_list_attribute_list(
			post_prompt_process->
				process_parameter_folder_name_list );

	post_prompt_process->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_prompt_process->
				folder_attribute_name_list_attribute_list );

	post_prompt_process->dictionary_separate_prompt_process =
		dictionary_separate_prompt_process_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			post_prompt_process->
				folder_attribute_date_name_list,
			post_prompt_process->
				folder_attribute_name_list_attribute_list );

	if ( !post_prompt_process->session_process->process_name )
	{
		post_prompt_process->session_process->process_name =
			post_prompt_process_name(
				PROCESS_SET_PROCESS_LABEL,
				process_set->prompt_display_text,
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary );
	}

	if ( is_drillthru )
	{
		char *send_string;

		send_string =
		    dictionary_separate_send_string(
			dictionary_separate_send_dictionary(
				(DICTIONARY *)0 /* sort_dictionary */,
				DICTIONARY_SEPARATE_SORT_PREFIX,
				post_prompt_process->
					dictionary_separate_prompt_process->
					query_dictionary,
				DICTIONARY_SEPARATE_QUERY_PREFIX,
				post_prompt_process->
					dictionary_separate_prompt_process->
					drillthru_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				(DICTIONARY *)0 /* ignore_dictionary */,
				DICTIONARY_SEPARATE_IGNORE_PREFIX,
				(DICTIONARY *)0 /* no_display_dictionary */,
				DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
				(DICTIONARY *)0 /* pair_one2m_dictionary */,
				DICTIONARY_SEPARATE_PAIR_PREFIX,
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary ) );

		post_prompt_process->prompt_process_output_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			prompt_process_output_system_string(
				PROMPT_PROCESS_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				process_or_set_name,
				send_string,
		 		1 /* has_drillthru */,
				0 /* not is_drillthru */,
				appaserver_error_filename(
					application_name ) );
	}
	else
	{
		post_prompt_process->
		post_prompt_process->process =
			process_fetch(
				post_prompt_process->
					session_process,
					process_name,
				document_root,
				application_relative_source_directory,
				1 /* check_executable_inside */ );

		if ( !post_prompt_process->process )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: process_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		post_prompt_process->command_line =
			post_prompt_process_command_line(
				post_prompt_process->process->command_line,
				session_key,
				login_name,
				role_name,
				post_prompt_process->
					session_process->
					process_name,
				post_prompt_process->
					dictionary_separate_prompt_process->
					query_dictionary,
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary,
				appaserver_error_filename(
					application_name ) );
	}

	return post_prompt_process;
}

POST_PROMPT_PROCESS *post_prompt_process_calloc( void )
{
	POST_PROMPT_PROCESS *post_prompt_process;

	if ( ! ( post_prompt_process =
			calloc( 1, sizeof( POST_PROMPT_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_prompt_process;
}

char *post_prompt_process_command_line(
			char *process_command_line,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			DICTIONARY *query_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			char *appaserver_error_filename )
{
	char local_command_line[ STRING_16K ];
	char buffer[ STRING_8K ];
	DICTIONARY *dictionary;

	string_strcpy(
		local_command_line,
		command_line,
		STRING_16K );

	search_replace_word(
		local_command_line,
		"$session",
		double_quotes_around(
			buffer, 
			session_key ) );

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
		"$process",
		double_quotes_around(
			buffer, 
			process_name ) );

	dictionary = dictionary_copy( query_dictionary );

	dictionary_append( dictionary, non_prefixed_dictionary );

	search_replace_word(
		local_command_line,
		"$dictionary",
		double_quotes_around(
			buffer, 
			dictionary_display_delimited(
				dictionary, '&' ) 
			) );

	sprintf(local_command_line + strlen( local_command_line ),
		" 2>%s",
		appaserver_error_filename );

	return strdup( local_command_line );
}

char *post_prompt_process_name(
			char *process_set_process_label,
			char *prompt_display_text,
			DICTIONARY *non_prefixed_dictionary )
{
	char key[ 128 ];
	char *data;

	if ( !process_set_process_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"%s_0",
		process_set_process_label );

	if ( ( data =
		dictionary_get(
			key,
			non_prefixed_dictionary ) ) )
	{
		return data;
	}

	if ( !prompt_display_text && !*prompt_display_text )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_display_text is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"%s_0",
		prompt_display_text );

	if ( ( data =
		dictionary_get(
			key,
			non_prefixed_dictionary ) ) )
	{
		return data;
	}

	return (char *)0;
}

char *post_prompt_process_action_string(
			char *executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean has_drillthru,
			boolean is_drillthru )
{
	char action_string[ 1024 ];

	if ( !executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name );

	return strdup( action_string );
}

