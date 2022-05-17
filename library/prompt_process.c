/* $APPASERVER_HOME/library/prompt_process.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver.h"
#include "application.h"
#include "relation.h"
#include "prompt_lookup.h"

PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			PROCESS *process,
			PROCESS_SET *process_set,
			booleanhas_drillthru,
			POST_DICTIONARY *post_dictionary,
			MENU *menu )
{
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;

	if ( !application_name,
	||   !session_key,
	||   !login_name,
	||   !role_name,
	||   !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_process_not_drillthru = prompt_process_not_drillthru_calloc();

	prompt_process_not_drillthru->
		process_parameter_folder_name_list =
			process_parameter_folder_name_list(
				(process)
					? process->process_name
					: (char *)0,
				(process_set)
					? process_set->process_set_name
					: (char *)0 );

	if ( list_length(
		prompt_process_not_drillthru->
			process_parameter_folder_name_list ) )
	{
		prompt_process_not_drillthru->
			folder_attribute_name_list_attribute_list =
				folder_attribute_name_list_attribute_list(
				prompt_process_not_drillthru->
					process_parameter_folder_name_list );

		prompt_process_not_drillthru->
			folder_attribute_date_name_list =
			  folder_attribute_date_name_list(
				prompt_process_not_drillthru->
				    folder_attribute_name_list_attribute_list );
	}

	prompt_process_not_drillthru->
		dictionary_separate_prompt_process =
		   dictionary_separate_prompt_process_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			prompt_process_not_drillthru->
				folder_attribute_date_name_list,
			prompt_process_not_drillthru->
				folder_attribute_name_list_attribute_list );

	if ( process )
	{
		LIST *process_parameter_list =
			process_parameter_system_list(
				process_parameter_system_string(
					PROCESS_PARAMETER_SELECT,
					PROCESS_PARAMETER_TABLE,
					process_parameter_where(
						process->process_name,
						is_drillthru ) ),
				login_name,
				role_name,
				dictionary_separte_prompt_process->
					drillthru_dictionary );

		char *post_change_javascript =
			process->post_change_javascript;
	}

	if ( process_set )
	{
		LIST *process_parameter_list =
			process_parameter_system_list(
				process_parameter_set_system_string(
					PROCESS_SET_PARAMETER_SELECT,
					PROCESS_SET_PARAMETER_TABLE,
					process_parameter_set_where(
						process_set->process_set_name,
						is_drillthru ) ),
				login_name,
				role_name,
				dictionary_separate_prompt_process->
					drillthru_dictionary );

		char *post_change_javascript =
			process_set->post_change_javascript;
	}

	if ( menu )
	{
		char *document_head_menu_setup_string(
			1 /* frameset_menu_horizontal */ );

		char *document_head_calendar_setup_string(
			process_parameter_date_boolean(
				process_parameter_list ) );
	}

	process_parameter_not_drillthru->document =
		document_new(
			application_name,
			prompt_process_is_drillthru_title_string(),
			prompt_process_is_drillthru_title_html(),
			prompt_process_is_drillthru_subtitle_html(),
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			(menu) ? 1 : 0 /* frameset_menu_horizontal */,
			menu,
			document_head_menu_setup_string,
			document_head_calendar_setup_string,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	process_parameter_not_drillthru->form_prompt_process =
		form_prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			process_parameter_not_drillthru->
				process_parameter_list,
			post_change_javascript );

	process_parameter_not_drillthru->
		document_form_html =
			document_form_html(
				process_parameter_not_drillthru->
					document->
					html,
				process_parameter_not_drillthru->
					document->
					document_head->html,
				document_head_close_html(),
				process_parameter_not_drillthru->
					document->
					document_body->
					html,
				process_parameter_not_drillthru->
					form_prompt_process->
					html,
				document_body_close_html(),
				document_close_html() );

	free( process_parameter_not_drillthru->form_prompt_process->html );

	return prompt_process_not_drillthru;
}

PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_calloc( void )
{
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;

	if ( ! ( prompt_process_not_drillthru =
			calloc(	1,
				sizeof( PROMPT_PROCESS_NOT_DRILLTHRU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_process_not_drillthru;
}

PROMPT_PROCESS_IS_DRILLTHRU *
	prompt_process_is_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			PROCESS *process,
			PROCESS_SET *process_set,
			MENU *menu )
{
	PROMPT_PROCESS_IS_DRILLTHRU *prompt_process_is_drillthru;

	if ( !application_name,
	||   !session_key,
	||   !login_name,
	||   !role_name,
	||   !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_process_is_drillthru = prompt_process_is_drillthru_calloc();

	return prompt_process_is_drillthru;
}

PROMPT_PROCESS *prompt_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean has_drillthru,
			boolean is_drillthru,
			POST_DICTIONARY *post_dictionary,
			boolean menu_horizontal_boolean,
			char *document_root,
			char *application_relative_source_directory )
{
	PROMPT_PROCESS *prompt_process;

	if ( !application_name,
	||   !session_key,
	||   !login_name,
	||   !role_name,
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

	prompt_process = prompt_process_calloc();

	return prompt_process;
}

PROMPT_PROCESS *prompt_process_calloc( void )
{
	PROMPT_PROCESS *prompt_process;

	if ( ! ( prompt_process = calloc( 1, sizeof( PROMPT_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_process;
}


boolean prompt_process_has_drillthru( char *process_or_set_name )
{
}

char *prompt_process_output_system_string(
			char *prompt_process_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *dictionary_separate_send_string,
			boolean has_drillthru,
			boolean is_drillthru,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	*system_string = '\0';

	return strdup( system_string );
}
