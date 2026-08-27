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
#include "update.h"
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
			is_drillthru );

	post_prompt_process->process_parameter_upload_filename_list =
		process_parameter_upload_filename_list(
			post_prompt_process->
				process_parameter_list );

	post_prompt_process->post_dictionary =
		post_dictionary_stdin_new(
			WIDGET_UPLOAD_RECALL_PREFIX,
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
				non_prefixed_dictionary /* in/out */,
			appaserver_error_filespecification(
				application_name ) );

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
		char *appaserver_error_filespecification )
{
	char *execute_yn;

	if ( !process_command_line
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !appaserver_error_filespecification )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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

	execute_yn =
		dictionary_get(
			APPASERVER_EXECUTE_YN,
			non_prefixed_dictionary );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	process_replace_command_line(
		UPDATE_PREUPDATE_PREFIX,
		application_name,
		session_key,
		login_name,
		role_name,
		(char *)0 /* folder_name */,
		(char *)0 /* target_frame */,
		(char *)0 /* state */,
		process_name,
		(char *)0 /* many_folder_name */,
		(char *)0 /* one_folder_name */,
		(char *)0 /* related_column */,
		(char *)0 /* update_results_string */,
		(char *)0 /* update_error_string */,
		non_prefixed_dictionary
			/* operation_row_list_dictionary */,
		non_prefixed_dictionary
			/* dictionary_single_row */,
		(char *)0 /* where_string */,
		0 /* row_number */,
		0 /* row_count */,
		0 /* parent_process_id */,
		(LIST *)0 /* primary_key_data_list */,
		execute_yn,
		(LIST *)0 /* insert_datum_list */,
		(LIST *)0 /* update_attribute_list */,
		(LIST *)0 /* query_cell_list */,
		appaserver_error_filespecification,
		process_command_line );
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

