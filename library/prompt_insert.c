/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/prompt_insert.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "application.h"
#include "javascript.h"
#include "drillthru.h"
#include "post_choose_folder.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "role.h"
#include "prompt_insert.h"

PROMPT_INSERT *prompt_insert_calloc( void )
{
	PROMPT_INSERT *prompt_insert;

	if ( ! ( prompt_insert = calloc( 1, sizeof ( PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_insert;
}

PROMPT_INSERT_INPUT *prompt_insert_input_calloc( void )
{
	PROMPT_INSERT_INPUT *prompt_insert_input;

	if ( ! ( prompt_insert_input =
			calloc( 1,
			sizeof ( PROMPT_INSERT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_insert_input;
}

PROMPT_INSERT_INPUT *prompt_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		DICTIONARY *original_post_dictionary )
{
	PROMPT_INSERT_INPUT *prompt_insert_input;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_insert_input = prompt_insert_input_calloc();

	prompt_insert_input->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	prompt_insert_input->role_folder_insert_boolean =
		role_folder_insert_boolean(
			ROLE_PERMISSION_INSERT,
			folder_name,
			prompt_insert_input->role_folder_list );

	prompt_insert_input->forbid =
		prompt_insert_input_forbid(
			prompt_insert_input->role_folder_insert_boolean );

	if ( prompt_insert_input->forbid ) return prompt_insert_input;

	prompt_insert_input->role_folder_lookup_boolean =
		role_folder_lookup_boolean(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			folder_name,
			prompt_insert_input->role_folder_list );

	prompt_insert_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	prompt_insert_input->role_attribute_exclude_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_INSERT,
			prompt_insert_input->
			     role->
			     role_attribute_exclude_list );

	prompt_insert_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			prompt_insert_input->role_attribute_exclude_name_list,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	prompt_insert_input->folder_attribute_non_primary_name_list =
		folder_attribute_non_primary_name_list(
			prompt_insert_input->
				folder->
				folder_attribute_list );

	prompt_insert_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			prompt_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */ );

	prompt_insert_input->relation_mto1_list =
		relation_mto1_without_omit_drillthru_list(
			prompt_insert_input->relation_mto1_list );

	prompt_insert_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			prompt_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	prompt_insert_input->relation_one2m_pair_list =
		relation_one2m_pair_list(
			folder_name
				/* one_folder_name */,
			prompt_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* one_primary_key_list */ );

	prompt_insert_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			prompt_insert_input->
				folder->
				folder_attribute_list
				/* append_isa_list */,
			prompt_insert_input->relation_mto1_isa_list );

	if ( application_menu_horizontal_boolean )
	{
		prompt_insert_input->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				data_directory,
				0 /* not folder_menu_remove_boolean */ );

		prompt_insert_input->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				prompt_insert_input->
					folder_menu->
					count_list );
	}

	prompt_insert_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			prompt_insert_input->folder_attribute_append_isa_list );

	prompt_insert_input->dictionary_separate_drillthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_drillthru_new(
			original_post_dictionary,
			application_name,
			login_name,
			prompt_insert_input->folder_attribute_date_name_list,
			prompt_insert_input->
				folder_attribute_append_isa_list );

	prompt_insert_input->drillthru_status =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_status_new(
			prompt_insert_input->
				dictionary_separate_drillthru->
				drillthru_dictionary );

	prompt_insert_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	prompt_insert_input->security_entity =
		security_entity_new(
			login_name,
			prompt_insert_input->
				folder_row_level_restriction->
				non_owner_forbid,
			prompt_insert_input->role->override_row_restrictions );

	return prompt_insert_input;
}

PROMPT_INSERT *prompt_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		DICTIONARY *original_post_dictionary )
{
	PROMPT_INSERT *prompt_insert;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_insert = prompt_insert_calloc();

	prompt_insert->prompt_insert_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prompt_insert_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			application_menu_horizontal_boolean,
			data_directory,
			original_post_dictionary );

	if ( prompt_insert->prompt_insert_input->forbid )
	{
		char message[ 128 ];

		sprintf(message,
			"prompt_insert_forbid(%s,%s,%s) returned true.",
			login_name,
			role_name,
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( prompt_insert->
		prompt_insert_input->
		drillthru_status->
		participating_boolean )
	{
		prompt_insert->post_choose_folder_action_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_choose_folder_action_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_INSERT_STATE,
				prompt_insert->
					prompt_insert_input->
					drillthru_status->
					base_folder_name );
	}

	prompt_insert->form_prompt_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_insert_new(
			PROMPT_INSERT_LOOKUP_CHECKBOX,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			prompt_insert->
				prompt_insert_input->
				folder->
				post_change_javascript,
			prompt_insert->
				prompt_insert_input->
				folder_attribute_non_primary_name_list,
			prompt_insert->
				prompt_insert_input->
				relation_mto1_list,
			prompt_insert->
				prompt_insert_input->
				relation_mto1_isa_list,
			prompt_insert->
				prompt_insert_input->
				folder_attribute_append_isa_list,
			prompt_insert->
				prompt_insert_input->
				dictionary_separate_drillthru->
				drillthru_dictionary,
			prompt_insert->
				prompt_insert_input->
				security_entity,
			prompt_insert->
				prompt_insert_input->
				role_folder_lookup_boolean,
			prompt_insert->
				prompt_insert_input->
				relation_one2m_pair_list,
			prompt_insert->
				post_choose_folder_action_string );

	prompt_insert->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			application_name );

	prompt_insert->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_insert_title_string(
			APPASERVER_INSERT_STATE,
			folder_name );

	prompt_insert->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
			prompt_insert->
				prompt_insert_input->
				folder_attribute_append_isa_list );

	prompt_insert->javascript_filename_list =
		prompt_insert_javascript_filename_list(
			prompt_insert->
				prompt_insert_input->
				folder->
				javascript_filename );

	prompt_insert->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			prompt_insert->application_title_string,
			prompt_insert->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			prompt_insert->
				prompt_insert_input->
				folder->
				notepad,
			(char *)0 /* onload_javascript_string */,
			(prompt_insert->prompt_insert_input->menu)
				? prompt_insert->prompt_insert_input->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(prompt_insert->prompt_insert_input->menu)
					? 1 : 0 /* menu_boolean */ ),
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_calendar_setup_string(
			   prompt_insert->
			     folder_attribute_calendar_date_name_list_length ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				prompt_insert->javascript_filename_list ) );

	prompt_insert->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			prompt_insert->document->html,
			prompt_insert->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			prompt_insert->document->document_body->html,
			prompt_insert->form_prompt_insert->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( prompt_insert->form_prompt_insert->html );

	return prompt_insert;
}

boolean prompt_insert_input_forbid(
		boolean role_folder_insert_boolean )
{
	return 1 - role_folder_insert_boolean;
}

char *prompt_insert_title_string(
		const char *state,
		char *folder_name )
{
	static char title_string[ 128 ];
	char buffer1[ 16 ];
	char buffer2[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		title_string,
		sizeof ( title_string ),
		"%s %s",
		string_initial_capital(
			buffer1,
			(char *)state ),
		string_initial_capital(
			buffer2,
			folder_name ) );

	return title_string;
}

LIST *prompt_insert_javascript_filename_list(
		char *javascript_filename )
{
	return
	list_string_new( javascript_filename );
}

