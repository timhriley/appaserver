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
#include "timlib.h"
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
			boolean is_drillthru,
			char *document_root,
			char *application_relative_source_directory )
{
	POST_PROMPT_PROCESS *post_prompt_process;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name
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

	post_prompt_process = post_prompt_process_calloc();

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

	post_prompt_process->post_dictionary =
		post_dictionary_stdin_new(
			(char *)0 /* upload_directory */,
			(char *)0 /* session_key */ );

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
			post_prompt_process->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_prompt_process->
				folder_attribute_date_name_list,
			post_prompt_process->
				folder_attribute_name_list_attribute_list );

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

		return post_prompt_process;
	}

	if ( post_prompt_process->session_process->process_set_name )
	{
		post_prompt_process->process_set =
			process_set_fetch(
				post_prompt_process->
					session_process->
					process_set_name,
				(char *)0 /* role_name */,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source_directory */,
				0 /* not fetch_process_set_member_..._list */ );

		post_prompt_process->session_process->process_name =
			post_prompt_process_name(
				PROCESS_SET_DEFAULT_PROMPT,
				post_prompt_process->
					process_set->
					prompt_display_text,
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary );
	}

	post_prompt_process->process =
		process_fetch(
			post_prompt_process->
				session_process->
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

	if ( !post_prompt_process->process->command_line )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_prompt_process->command_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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
			appaserver_error_filename( application_name ) );

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
	char command_line[ STRING_16K ];
	char buffer[ STRING_8K ];
	DICTIONARY *dictionary;

	if ( !process_command_line
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy(
		command_line,
		process_command_line,
		STRING_16K );

	search_replace_word(
		command_line,
		"$session",
		double_quotes_around(
			buffer, 
			session_key ) );

	search_replace_word(
		command_line,
		"$login_name",
		double_quotes_around(
			buffer, 
			login_name ) );

	search_replace_word(
		command_line,
		"$role",
		double_quotes_around(
			buffer, 
			role_name ) );

	search_replace_word(
		command_line,
		"$process",
		double_quotes_around(
			buffer, 
			process_name ) );

	dictionary = dictionary_copy( query_dictionary );

	dictionary_append( dictionary, non_prefixed_dictionary );

	search_replace_word(
		command_line,
		"$dictionary",
		double_quotes_around(
			buffer, 
			dictionary_display_delimited( dictionary, '&' ) 
			) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return strdup( command_line );
}

char *post_prompt_process_name(
			char *process_set_default_prompt,
			char *prompt_display_text,
			DICTIONARY *non_prefixed_dictionary )
{
	char key[ 128 ];
	char *data;

	if ( !process_set_default_prompt )
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
		process_set_default_prompt );

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
			char *process_or_set_name,
			boolean is_drillthru )
{
	char action_string[ 1024 ];

	if ( !executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%c",
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
		process_or_set_name,
		(is_drillthru) ? 'y' : 'n' );

	return strdup( action_string );
}

char *post_prompt_process_system_string(
			char *executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *appaserver_error_filename )
{
	char system_string[ STRING_8K ];

	if ( !executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s %c 2>>%s",
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name,
		'n' /* not is_drillthru_yn */,
		appaserver_error_filename );

	return strdup( system_string );
}

