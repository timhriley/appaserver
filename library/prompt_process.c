/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/prompt_process.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
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
		char *process_name,
		char *process_set_name,
		PROCESS *process,
		PROCESS_SET *process_set,
		boolean has_drillthru,
		POST_DICTIONARY *post_dictionary,
		MENU *menu,
		LIST *javascript_filename_list )
{
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;
	DICTIONARY *drillthru_dictionary = {0};

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_process_not_drillthru = prompt_process_not_drillthru_calloc();

	if ( has_drillthru )
	{
		LIST *list;

		list =
			process_parameter_list(
				process_name,
				process_set_name,
				0 /* not drillthru_boolean */ );

		prompt_process_not_drillthru->
			process_parameter_folder_name_list =
				process_parameter_folder_name_list(
					list );

		prompt_process_not_drillthru->
			folder_attribute_name_list_attribute_list =
				folder_attribute_name_list_attribute_list(
					prompt_process_not_drillthru->
					   process_parameter_folder_name_list );

		if ( list_length(
			prompt_process_not_drillthru->
				process_parameter_folder_name_list ) )
		{
			prompt_process_not_drillthru->
				folder_attribute_name_list_attribute_list =
				    folder_attribute_name_list_attribute_list(
				    	prompt_process_not_drillthru->
					   process_parameter_folder_name_list );
		}

		prompt_process_not_drillthru->
			process_parameter_date_name_list =
		  		process_parameter_date_name_list(
			    		list );

		prompt_process_not_drillthru->
			dictionary_separate_prompt_process =
		   	dictionary_separate_prompt_process_new(
				post_dictionary->original_post_dictionary,
				application_name,
				login_name,
				prompt_process_not_drillthru->
				    process_parameter_date_name_list,
				prompt_process_not_drillthru->
				    folder_attribute_name_list_attribute_list );

		drillthru_dictionary =
			prompt_process_not_drillthru->
				dictionary_separate_prompt_process->
				drillthru_dictionary;
	}

	if ( process )
	{
		prompt_process_not_drillthru->
			process_parameter_where =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_where(
					process->process_name,
					0 /* not is_drillthru */ );

		prompt_process_not_drillthru->
			process_parameter_system_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				process_parameter_system_string(
					PROCESS_PARAMETER_SELECT,
					PROCESS_PARAMETER_TABLE,
					prompt_process_not_drillthru->
						process_parameter_where );

		prompt_process_not_drillthru->process_parameter_list =
			process_parameter_system_list(
				application_name,
				login_name,
				role_name,
				drillthru_dictionary,
				prompt_process_not_drillthru->
					process_parameter_system_string );

		prompt_process_not_drillthru->post_change_javascript =
			process->post_change_javascript;
	}
	else
	if ( process_set
	&&   list_length( process_set->member_name_list ) )
	{
		prompt_process_not_drillthru->
			process_parameter_set_where =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				process_parameter_set_where(
					process_set->process_set_name,
					0 /* not is_drillthru */ );

		prompt_process_not_drillthru->
			process_parameter_set_system_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				process_parameter_set_system_string(
					PROCESS_SET_PARAMETER_SELECT,
					PROCESS_SET_PARAMETER_TABLE,
					prompt_process_not_drillthru->
						process_parameter_set_where );

		prompt_process_not_drillthru->process_parameter_list =
			process_parameter_system_list(
				application_name,
				login_name,
				role_name,
				drillthru_dictionary,
				prompt_process_not_drillthru->
					process_parameter_set_system_string );

		prompt_process_not_drillthru->process_parameter_list =
			/* ------------------------------ */
			/* Returns process_parameter_list */
			/* ------------------------------ */
			process_parameter_set_member_append(
				prompt_process_not_drillthru->
					process_parameter_list,
				PROCESS_SET_DEFAULT_PROMPT,
				process_set->prompt_display_text,
				process_set->member_name_list );

		prompt_process_not_drillthru->post_change_javascript =
			process_set->post_change_javascript;
	}

	if ( menu )
	{
		prompt_process_not_drillthru->
			document_head_menu_setup_string =
				document_head_menu_setup_string(
					1 /* menu_boolean */ );

		prompt_process_not_drillthru->
			document_head_calendar_setup_string =
			    document_head_calendar_setup_string(
				process_parameter_date_boolean(
					prompt_process_not_drillthru->
						process_parameter_list ) );
	}

	prompt_process_not_drillthru->prompt_process_notepad =
		/* ------------------------ */
		/* Returns either's notepad */
		/* ------------------------ */
		prompt_process_notepad(
			process,
			process_set );

	prompt_process_not_drillthru->form_prompt_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			prompt_process_not_drillthru->
				process_parameter_list,
			prompt_process_not_drillthru->
				post_change_javascript,
			0 /* not is_drillthru */ );

	prompt_process_not_drillthru->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_not_drillthru_title_string(
			process_or_set_name );

	prompt_process_not_drillthru->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			javascript_filename_list );

	prompt_process_not_drillthru->onload_javascript_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		document_body_set_onload_javascript_string(
			prompt_process_not_drillthru->
				form_prompt_process->
		       		recall_load->
				javascript
				/* javascript_string */,
			prompt_process_not_drillthru->
				post_change_javascript
				/* append_javascript_string */ );

	prompt_process_not_drillthru->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ),
			prompt_process_not_drillthru->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			prompt_process_not_drillthru->
				prompt_process_notepad,
			prompt_process_not_drillthru->
				onload_javascript_string,
			(menu) ? menu->html : (char *)0,
			prompt_process_not_drillthru->
				document_head_menu_setup_string,
			prompt_process_not_drillthru->
				document_head_calendar_setup_string,
			prompt_process_not_drillthru->
				javascript_include_string );

	prompt_process_not_drillthru->
		document_form_html =
			document_form_html(
				prompt_process_not_drillthru->
					document->
					html,
				prompt_process_not_drillthru->
					document->
					document_head->html,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_head_close_tag(),
				prompt_process_not_drillthru->
					document->
					document_body->
					html,
				prompt_process_not_drillthru->
					form_prompt_process->
					form_prompt_lookup_html,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_body_close_tag(),
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_close_tag() );

	free( prompt_process_not_drillthru->
		form_prompt_process->
		form_prompt_lookup_html );

	return prompt_process_not_drillthru;
}

PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_calloc( void )
{
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;

	if ( ! ( prompt_process_not_drillthru =
			calloc(	1,
				sizeof ( PROMPT_PROCESS_NOT_DRILLTHRU ) ) ) )
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
		MENU *menu,
		LIST *javascript_filename_list )
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
				application_name,
				login_name,
				role_name,
				(DICTIONARY *)0 /* drillthru_dictionary */,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				process_parameter_system_string(
					PROCESS_PARAMETER_SELECT,
					PROCESS_PARAMETER_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					process_parameter_where(
						process->process_name,
						1 /* is_drillthru */ ) ) );

		prompt_process_is_drillthru->post_change_javascript =
			process->post_change_javascript;
	}
	else
	if ( process_set )
	{
		prompt_process_is_drillthru->process_parameter_list =
			process_parameter_system_list(
				application_name,
				login_name,
				role_name,
				(DICTIONARY *)0 /* drillthru_dictionary */,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				process_parameter_set_system_string(
					PROCESS_SET_PARAMETER_SELECT,
					PROCESS_SET_PARAMETER_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					process_parameter_set_where(
						process_set->process_set_name,
						1 /* is_drillthru */ ) ) );

		prompt_process_is_drillthru->post_change_javascript =
			process_set->post_change_javascript;
	}

	if ( menu )
	{
		prompt_process_is_drillthru->
			document_head_menu_setup_string =
				document_head_menu_setup_string(
					1 /* menu_boolean */ );

		prompt_process_is_drillthru->
			document_head_calendar_setup_string =
				document_head_calendar_setup_string(
				    process_parameter_date_boolean(
					prompt_process_is_drillthru->
						process_parameter_list ) );
	}

	prompt_process_is_drillthru->prompt_process_notepad =
		/* ------------------------ */
		/* Returns either's notepad */
		/* ------------------------ */
		prompt_process_notepad(
			process,
			process_set );

	prompt_process_is_drillthru->form_prompt_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			prompt_process_is_drillthru->process_parameter_list,
			prompt_process_is_drillthru->post_change_javascript,
			1 /* is_drillthru */ );

	prompt_process_is_drillthru->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_process_is_drillthru_title_string(
			process_or_set_name );

	prompt_process_is_drillthru->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			javascript_filename_list );

	prompt_process_is_drillthru->onload_javascript_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		document_body_set_onload_javascript_string(
			prompt_process_is_drillthru->
				form_prompt_process->
		       		recall_load->
				javascript
				/* javascript_string */,
			prompt_process_is_drillthru->
				post_change_javascript
				/* append_javascript_string */ );

	prompt_process_is_drillthru->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ),
			prompt_process_is_drillthru->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			prompt_process_is_drillthru->prompt_process_notepad,
			prompt_process_is_drillthru->onload_javascript_string,
			(menu) ? menu->html : (char *)0,
			prompt_process_is_drillthru->
				document_head_menu_setup_string,
			prompt_process_is_drillthru->
				document_head_calendar_setup_string,
			prompt_process_is_drillthru->
				javascript_include_string );

	prompt_process_is_drillthru->document_form_html =
		document_form_html(
			prompt_process_is_drillthru->document->html,
			prompt_process_is_drillthru->
				document->
				document_head->
				html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			prompt_process_is_drillthru->
				document->
				document_body->
				html,
			prompt_process_is_drillthru->
				form_prompt_process->
				form_prompt_lookup_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( prompt_process_is_drillthru->
		form_prompt_process->
		form_prompt_lookup_html );

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
		char *mount_point,
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
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_process = prompt_process_calloc();

	if ( menu_horizontal_boolean )
	{
		prompt_process->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				data_directory,
				0 /* not folder_menu_remove_boolean */ );
	
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
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_fetch(
				prompt_process->process_name,
				document_root,
				application_relative_source_directory,
				1 /* check_executable_inside */,
				mount_point );
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

	prompt_process->javascript_filename_list =
		prompt_process_javascript_filename_list(
			prompt_process->process,
			prompt_process->process_set );

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
					prompt_process->menu,
					prompt_process->
						javascript_filename_list );
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
					prompt_process->process_name,
					prompt_process->process_set_name,
					prompt_process->process,
					prompt_process->process_set,
					has_drillthru,
					post_dictionary,
					prompt_process->menu,
					prompt_process->
						javascript_filename_list );
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

	if ( ! ( prompt_process = calloc( 1, sizeof ( PROMPT_PROCESS ) ) ) )
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

	return
	string_initial_capital(
		title_string,
		process_or_set_name );
}

PROMPT_PROCESS_IS_DRILLTHRU *
	prompt_process_is_drillthru_calloc(
		void )
{
	PROMPT_PROCESS_IS_DRILLTHRU *prompt_process_is_drillthru;

	if ( ! ( prompt_process_is_drillthru =
			calloc(	1,
				sizeof ( PROMPT_PROCESS_IS_DRILLTHRU ) ) ) )
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
	char buffer[ 64 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_string,
		"%s: Drillthru",
		string_initial_capital( buffer, process_or_set_name ) );

	return title_string;
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

char *prompt_process_notepad(
		PROCESS *process,
		PROCESS_SET *process_set )
{
	if ( process )
		return process->notepad;
	else
	if ( process_set )
		return process_set->notepad;
	else
	{
		char message[ 128 ];

		sprintf(message, "both process and process_set are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Stub */
	/* ---- */
	return NULL;
}

LIST *prompt_process_javascript_filename_list(
		PROCESS *process,
		PROCESS_SET *process_set )
{
	LIST *filename_list = {0};

	if ( process && process->javascript_filename )
	{
		filename_list = list_new();
		list_set( filename_list, process->javascript_filename );
	}
	else
	if ( process_set && process_set->javascript_filename )
	{
		filename_list = list_new();
		list_set( filename_list, process_set->javascript_filename );
	}

	return filename_list;
}
