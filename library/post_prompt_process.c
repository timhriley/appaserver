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
#include "post_prompt_process.h"

POST_PROMPT_PROCESS *post_prompt_process_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean has_preprompt,
			boolean is_preprompt,
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
				process_set_name );

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
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary );
	}

	if ( has_preprompt && is_preprompt )
	{
		post_prompt_process->prompt_process_output_system_string =
			prompt_process_output_system_string(
				PROMPT_PROCESS_OUTPUT_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				process_or_set_name,
				dictionary_separate_send_string(),
		 		1 /* has_preprompt */,
				0 /* not is_preprompt */ );
	}
	else
	{
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
					non_prefixed_dictionary,
				application_error_directory(
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
			DICTIONARY *non_prefixed_dictionary,
			char *application_error_directory )
{
	return (char *)0;
}

char *post_prompt_process_name( DICTIONARY *non_prefixed_dictionary )
{
	return (char *)0;
}

char *post_prompt_process_action_string(
			char *post_prompt_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean has_preprompt,
			boolean is_preprompt )
{
	return (char *)0;
}

