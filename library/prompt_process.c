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
#include "process_parameter.h"
#include "form_prompt_process.h"
#include "prompt_process.h"

PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			PROCESS *process,
			PROCESS_SET *process_set,
			boolean has_drillthru,
			POST_DICTIONARY *post_dictionary,
			MENU *menu )
{
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;
	DICTIONARY *drillthru_dictionary = {0};

	if ( !application_name
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

	prompt_process_not_drillthru = prompt_process_not_drillthru_calloc();

	if ( !has_drillthru ) goto no_drillthru;

	prompt_process_not_drillthru->
		process_parameter_folder_name_list =
			process_parameter_folder_name_list(
				(process)
					? process->process_name
					: (char *)0,
				(process_set)
					? process_set->process_set_name
					: (char *)0,
				0 /* not is_drillthru */ );

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

	drillthru_dictionary =
		prompt_process_not_drillthru->
			dictionary_separate_prompt_process->
			drillthru_dictionary;

no_drillthru:

	if ( process )
	{
		prompt_process_not_drillthru->process_parameter_list =
			process_parameter_system_list(
				process_parameter_system_string(
					PROCESS_PARAMETER_SELECT,
					PROCESS_PARAMETER_TABLE,
					process_parameter_where(
						process->process_name,
						0 /* not is_drillthru */ ) ),
				login_name,
				role_name,
				drillthru_dictionary );

		prompt_process_not_drillthru->post_change_javascript =
			process->post_change_javascript;
	}
	else
	if ( process_set )
	{
		prompt_process_not_drillthru->process_parameter_list =
			process_parameter_system_list(
				process_parameter_set_system_string(
					PROCESS_SET_PARAMETER_SELECT,
					PROCESS_SET_PARAMETER_TABLE,
					process_parameter_set_where(
						process_set->process_set_name,
						0 /* not is_drillthru */ ) ),
				login_name,
				role_name,
				drillthru_dictionary );

		prompt_process_not_drillthru->post_change_javascript =
			process_set->post_change_javascript;
	}

	if ( menu )
	{
		prompt_process_not_drillthru->
			document_head_menu_setup_string =
				document_head_menu_setup_string(
					1 /* frameset_menu_horizontal */ );

		prompt_process_not_drillthru->
			document_head_calendar_setup_string =
			    document_head_calendar_setup_string(
				process_parameter_date_boolean(
					prompt_process_not_drillthru->
						process_parameter_list ) );
	}

	prompt_process_not_drillthru->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_not_drillthru_title_string(
			process_or_set_name );

	prompt_process_not_drillthru->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_not_drillthru_title_html(
			process_or_set_name );

	prompt_process_not_drillthru->document =
		document_new(
			application_name,
			prompt_process_not_drillthru->title_string,
			prompt_process_not_drillthru->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			(menu) ? 1 : 0 /* frameset_menu_horizontal */,
			menu,
			prompt_process_not_drillthru->
				document_head_menu_setup_string,
			prompt_process_not_drillthru->
				document_head_calendar_setup_string,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	prompt_process_not_drillthru->form_prompt_process =
		form_prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			prompt_process_not_drillthru->
				process_parameter_list,
			prompt_process_not_drillthru->post_change_javascript );

	if ( !prompt_process_not_drillthru->form_prompt_process )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_prompt_process_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_process_not_drillthru->
		document_form_html =
			document_form_html(
				prompt_process_not_drillthru->
					document->
					html,
				prompt_process_not_drillthru->
					document->
					document_head->html,
				document_head_close_html(),
				prompt_process_not_drillthru->
					document->
					document_body->
					html,
				prompt_process_not_drillthru->
					form_prompt_process->
					html,
				document_body_close_html(),
				document_close_html() );

	free( prompt_process_not_drillthru->form_prompt_process->html );

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

	if ( !application_name
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

	prompt_process_is_drillthru = prompt_process_is_drillthru_calloc();

	if ( process )
	{
		prompt_process_is_drillthru->process_parameter_list =
			process_parameter_system_list(
				process_parameter_system_string(
					PROCESS_PARAMETER_SELECT,
					PROCESS_PARAMETER_TABLE,
					process_parameter_where(
						process->process_name,
						1 /* is_drillthru */ ) ),
				login_name,
				role_name,
				(DICTIONARY *)0 /* drillthru_dictionary */ );

		prompt_process_is_drillthru->post_change_javascript =
			process->post_change_javascript;
	}
	else
	if ( process_set )
	{
		prompt_process_is_drillthru->process_parameter_list =
			process_parameter_system_list(
				process_parameter_set_system_string(
					PROCESS_SET_PARAMETER_SELECT,
					PROCESS_SET_PARAMETER_TABLE,
					process_parameter_set_where(
						process_set->process_set_name,
						1 /* is_drillthru */ ) ),
				login_name,
				role_name,
				(DICTIONARY *)0 /* drillthru_dictionary */ );

		prompt_process_is_drillthru->post_change_javascript =
			process_set->post_change_javascript;
	}

	if ( menu )
	{
		prompt_process_is_drillthru->
			document_head_menu_setup_string =
				document_head_menu_setup_string(
					1 /* frameset_menu_horizontal */ );

		prompt_process_is_drillthru->
			document_head_calendar_setup_string =
				document_head_calendar_setup_string(
				    process_parameter_date_boolean(
					prompt_process_is_drillthru->
						process_parameter_list ) );
	}

	prompt_process_is_drillthru->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_is_drillthru_title_string(
			process_or_set_name );

	prompt_process_is_drillthru->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_is_drillthru_title_html(
			process_or_set_name );

	prompt_process_is_drillthru->document =
		document_new(
			application_name,
			prompt_process_is_drillthru->title_string,
			prompt_process_is_drillthru->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			(menu) ? 1 : 0 /* frameset_menu_horizontal */,
			menu,
			prompt_process_is_drillthru->
				document_head_menu_setup_string,
			prompt_process_is_drillthru->
				document_head_calendar_setup_string,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	prompt_process_is_drillthru->form_prompt_process =
		form_prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			prompt_process_is_drillthru->process_parameter_list,
			prompt_process_is_drillthru->post_change_javascript );

	if ( !prompt_process_is_drillthru->form_prompt_process )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_prompt_process_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_process_is_drillthru->document_form_html =
		document_form_html(
			prompt_process_is_drillthru->document->html,
			prompt_process_is_drillthru->
				document->
				document_head->
				html,
			document_head_close_html(),
			prompt_process_is_drillthru->
				document->
				document_body->
				html,
			prompt_process_is_drillthru->
				form_prompt_process->
				html,
			document_body_close_html(),
			document_close_html() );

	free( prompt_process_is_drillthru->form_prompt_process->html );

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
			char *data_directory,
			char *document_root,
			char *application_relative_source_directory )
{
	PROMPT_PROCESS *prompt_process;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name
	||   !data_directory
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

	if ( menu_horizontal_boolean )
	{
		prompt_process->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				data_directory );
	
		prompt_process->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* frameset_menu_horizontal */,
				prompt_process->folder_menu->count_list );
	}

	prompt_process->process_name =
		process_name_fetch(
			process_or_set_name );

	if ( !prompt_process->process_name )
	{
		prompt_process->process_set_name = process_or_set_name;
	}

	if ( prompt_process->process_name )
	{
		prompt_process->process =
			process_fetch(
				prompt_process->process_name,
				document_root,
				application_relative_source_directory,
				0 /* not check_executable_inside */ );
	}
	else
	if ( prompt_process->process_set_name )
	{
		prompt_process->process_set =
			process_set_fetch(
				prompt_process->process_set_name,
				role_name,
				document_root,
				application_relative_source_directory,
				1 /* fetch_member_process_name_list */ );
	}

	if ( is_drillthru )
	{
		prompt_process->
			prompt_process_is_drillthru =
				prompt_process_is_drillthru_new(
					application_name,
					session_key,
					login_name,
					role_name,
					process_or_set_name,
					prompt_process->process,
					prompt_process->process_set,
					prompt_process->menu );
	}
	else
	{
		prompt_process->
			prompt_process_not_drillthru =
				prompt_process_not_drillthru_new(
					application_name,
					session_key,
					login_name,
					role_name,
					process_or_set_name,
					prompt_process->process,
					prompt_process->process_set,
					has_drillthru,
					post_dictionary,
					prompt_process->menu );
	}

	prompt_process->html =
		/* --------------------------------- */
		/* Returns either document_form_html */
		/* --------------------------------- */
		prompt_process_html(
			prompt_process->prompt_process_is_drillthru,
			prompt_process->prompt_process_not_drillthru );

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


char *prompt_process_output_system_string(
			char *executable,
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

	if ( !executable
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name
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
		"%s %s %s %s %s \"%s\" %c %c 2>>%s",
		executable,
		session_key,
		login_name,
		role_name,
		process_or_set_name,
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		(has_drillthru) ? 'y' : 'n',
		(is_drillthru) ? 'y' : 'n',
		appaserver_error_filename );

	return strdup( system_string );
}

char *prompt_process_not_drillthru_title_string( char *process_or_set_name )
{
	static char title_string[ 128 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_initial_capital( title_string, process_or_set_name );

	return title_string;
}

char *prompt_process_not_drillthru_title_html(
			char *process_or_set_name )
{
	static char title_html[ 256 ];
	char buffer[ 128 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_initial_capital( buffer, process_or_set_name );

	sprintf(title_html,
		"<h1>%s: First, filter a subsequent drop-down</h1>",
		buffer );

	return title_html;
}

PROMPT_PROCESS_IS_DRILLTHRU *
	prompt_process_is_drillthru_calloc(
			void )
{
	PROMPT_PROCESS_IS_DRILLTHRU *prompt_process_is_drillthru;

	if ( ! ( prompt_process_is_drillthru =
			calloc(	1,
				sizeof( PROMPT_PROCESS_IS_DRILLTHRU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_process_is_drillthru;
}

char *prompt_process_is_drillthru_title_string( char *process_or_set_name )
{
	static char title_string[ 128 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_initial_capital( title_string, process_or_set_name );

	return title_string;
}

char *prompt_process_is_drillthru_title_html(
			char *process_or_set_name )
{
	static char title_html[ 256 ];
	char buffer[ 128 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_initial_capital( buffer, process_or_set_name );

	sprintf(title_html,
		"<h1>%s</h1>",
		buffer );

	return title_html;
}

char *prompt_process_html(
			PROMPT_PROCESS_IS_DRILLTHRU *
				prompt_process_is_drillthru,
			PROMPT_PROCESS_NOT_DRILLTHRU *
				prompt_process_not_drillthru )
{
	if ( !prompt_process_is_drillthru
	&&   !prompt_process_not_drillthru )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( prompt_process_is_drillthru )
		return prompt_process_is_drillthru->document_form_html;
	else
		return prompt_process_not_drillthru->document_form_html;
}

