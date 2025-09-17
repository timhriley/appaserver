/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_prompt_process.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
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
#include "post_choose_process.h"
#include "post_prompt_process.h"

POST_PROMPT_PROCESS *post_prompt_process_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		boolean is_drillthru )
{
	POST_PROMPT_PROCESS *post_prompt_process;

	if ( !argc
	||   !argv
	||   !application_name
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

	post_prompt_process->
		application_menu_horizontal_boolean =
			application_menu_horizontal_boolean(
				application_name );

	post_prompt_process->
		menu_horizontal_boolean =
			menu_horizontal_boolean(
				FRAMESET_PROMPT_FRAME /* target_frame */,
				FRAMESET_PROMPT_FRAME,
				post_prompt_process->
					application_menu_horizontal_boolean );

	post_prompt_process->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_prompt_process->process_parameter_list =
		process_parameter_list(
			post_prompt_process->
				session_process->
				process_name,
			post_prompt_process->
				session_process->
				process_set_name,
			is_drillthru );

	post_prompt_process->process_parameter_upload_filename_list =
		process_parameter_upload_filename_list(
			post_prompt_process->
				process_parameter_list );

	post_prompt_process->post_dictionary =
		post_dictionary_stdin_new(
			application_name,
			post_prompt_process->
				appaserver_parameter->
				upload_directory,
			post_prompt_process->
				process_parameter_upload_filename_list );

	post_prompt_process->process_parameter_date_name_list =
		process_parameter_date_name_list(
			post_prompt_process->
				process_parameter_list );

	post_prompt_process->process_parameter_folder_name_list =
		process_parameter_folder_name_list(
			post_prompt_process->
				process_parameter_list );

	post_prompt_process->folder_attribute_name_list_attribute_list =
		folder_attribute_name_list_attribute_list(
			post_prompt_process->
				process_parameter_folder_name_list );

	post_prompt_process->dictionary_separate_prompt_process =
		dictionary_separate_prompt_process_new(
			post_prompt_process->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_prompt_process->
				process_parameter_date_name_list,
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
				(DICTIONARY *)0 /* query_dictionary */,
				DICTIONARY_SEPARATE_QUERY_PREFIX,
				post_prompt_process->
					dictionary_separate_prompt_process->
					drillthru_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				(DICTIONARY *)0 /* ignore_dictionary */,
				DICTIONARY_SEPARATE_IGNORE_PREFIX,
				(DICTIONARY *)0 /* no_display_dictionary */,
				DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
				(DICTIONARY *)0 /* pair_dictionary */,
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
			/* -------------------------------------------- */
			/* Returns component of non_prefixed_dictionary */
			/* -------------------------------------------- */
			post_prompt_process_name(
				PROCESS_SET_DEFAULT_PROMPT,
				post_prompt_process->
					process_set->
					prompt_display_text,
				post_prompt_process->
					dictionary_separate_prompt_process->
					non_prefixed_dictionary );
	}

	post_prompt_process->application_relative_source_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_relative_source_directory(
			application_name );

	post_prompt_process->process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_fetch(
			post_prompt_process->
				session_process->
				process_name,
			post_prompt_process->
				appaserver_parameter->
				document_root,
			post_prompt_process->
				application_relative_source_directory,
			1 /* check_executable_inside */,
			post_prompt_process->
				appaserver_parameter->
				mount_point );

	if ( !post_prompt_process->process->command_line )
	{
		char message[ 128 ];

		sprintf(message, "process->command_line is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_prompt_process->command_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_prompt_process_command_line(
			post_prompt_process->process->command_line,
			application_name,
			session_key,
			login_name,
			role_name,
			post_prompt_process->
				session_process->
				process_name,
			post_prompt_process->
				dictionary_separate_prompt_process->
				non_prefixed_dictionary,
			appaserver_error_filename( application_name ) );

	post_prompt_process->post_choose_process_no_parameters =
		post_choose_process_no_parameters(
			post_prompt_process->
				session_process->
				process_name );

	if ( post_prompt_process->
		post_choose_process_no_parameters )
	{
		if ( post_prompt_process->menu_horizontal_boolean )
		{
			post_prompt_process->folder_menu =
				folder_menu_fetch(
					application_name,
					session_key,
					role_name,
					post_prompt_process->
						appaserver_parameter->
						data_directory,
					0 /* not folder_menu_remove */ );

			post_prompt_process->menu =
				menu_new(
					application_name,
					session_key,
					login_name,
					role_name,
					1 /* menu_horizontal_boolean */,
					post_prompt_process->
						folder_menu->
						count_list );
		}

		post_prompt_process->document =
			document_new(
				application_name,
				application_title_string( application_name ),
				(char *)0 /* title_string */,
				(char *)0 /* sub_title_string */,
				(char *)0 /* sub_sub_title_string */,
				(char *)0 /* notepad */,
				(char *)0 /* onload_javascript_string */,
				(post_prompt_process->menu)
					? post_prompt_process->
						menu->
						html
					: (char *)0,
				document_head_menu_setup_string(
					(post_prompt_process->menu)
						? 1
						: 0 /* menu_boolean */ ),
				(char *)0 /* calendar_setup_string */,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				javascript_include_string(
					application_name,
					(LIST *)0
					/* javascript_filename_list */ ) );

		post_prompt_process->document_form_html =
			document_form_html(
				post_prompt_process->
					document->
					html,
				post_prompt_process->
					document->
					document_head->
					html,
				document_head_close_tag(),
				post_prompt_process->
					document->
					document_body->
					html,
				(char *)0 /* form_html */,
				(char *)0 /* document_body_close_tag */,
				(char *)0 /* document_close_tag */ );
	}

	return post_prompt_process;
}

POST_PROMPT_PROCESS *post_prompt_process_calloc( void )
{
	POST_PROMPT_PROCESS *post_prompt_process;

	if ( ! ( post_prompt_process =
			calloc( 1, sizeof ( POST_PROMPT_PROCESS ) ) ) )
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
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		DICTIONARY *non_prefixed_dictionary,
		char *appaserver_error_filename )
{
	char command_line[ STRING_16K ];
	char *tmp;
	char *execute_yn;

	if ( !process_command_line
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		process_command_line,
		STRING_16K );

	string_replace_command_line(
		command_line,
		application_name,
		PROCESS_APPLICATION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		session_key,
		PROCESS_SESSION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		role_name,
		PROCESS_ROLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		process_name,
		PROCESS_NAME_PLACEHOLDER );

	non_prefixed_dictionary =
		/* ------------------------------- */
		/* Returns non_prefixed_dictionary */
		/* ------------------------------- */
		post_prompt_process_non_prefixed_dictionary(
			PROCESS_FOLDER_NAME_PLACEHOLDER,
			PROCESS_TABLE_NAME_PLACEHOLDER,
			PROCESS_ATTRIBUTE_PLACEHOLDER,
			PROCESS_COLUMN_PLACEHOLDER,
			non_prefixed_dictionary /* in/out */ );

	tmp =
		/* Returns heap memory */
		/* ------------------- */
		dictionary_display_delimited(
			non_prefixed_dictionary,
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER );

	string_replace_command_line(
		command_line,
		tmp,
		PROCESS_DICTIONARY_PLACEHOLDER );

	free( tmp );

	dictionary_replace_command_line( 	
		command_line /* source_destination */,
		non_prefixed_dictionary );

	if ( ( execute_yn =
		dictionary_get(
			APPASERVER_EXECUTE_YN,
			non_prefixed_dictionary ) ) )
	{
		string_replace_command_line(
			command_line /* in/out */,
			execute_yn,
			APPASERVER_REALLY_YN );
	}

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	string_escape_dollar( command_line );
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
		"%s %s %s %s %s %s %c %c 2>>%s",
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name,
		'n' /* not is_drillthru_yn */,
		'y' /* omit_content_type */,
		appaserver_error_filename );

	return strdup( system_string );
}

DICTIONARY *post_prompt_process_non_prefixed_dictionary(
		const char *process_folder_name_placeholder,
		const char *process_table_name_placeholder,
		const char *process_attribute_placeholder,
		const char *process_column_placeholder,
		DICTIONARY *non_prefixed_dictionary /* in/out */ )
{
	char *get;

	if ( !dictionary_length( non_prefixed_dictionary ) )
		return non_prefixed_dictionary;

	if ( ( get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			(char *)process_table_name_placeholder /* key */,
			non_prefixed_dictionary ) ) )
	{
		dictionary_set(
			non_prefixed_dictionary,
			(char *)process_folder_name_placeholder /* key */,
			get );
	}

	if ( ( get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			(char *)process_column_placeholder /* key */,
			non_prefixed_dictionary ) ) )
	{
		dictionary_set(
			non_prefixed_dictionary,
			(char *)process_attribute_placeholder /* key */,
			get );
	}

	return non_prefixed_dictionary;
}

