/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/prompt_lookup.c				*/
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
#include "relation.h"
#include "role.h"
#include "attribute.h"
#include "folder_operation.h"
#include "post_drillthru.h"
#include "post_choose_folder.h"
#include "form_drillthru.h"
#include "prompt_lookup.h"

PROMPT_LOOKUP_INPUT *prompt_lookup_input_calloc( void )
{
	PROMPT_LOOKUP_INPUT *prompt_lookup_input;

	if ( ! ( prompt_lookup_input =
			calloc( 1,
				sizeof ( PROMPT_LOOKUP_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_lookup_input;
}

PROMPT_LOOKUP *prompt_lookup_calloc( void )
{
	PROMPT_LOOKUP *prompt_lookup;

	if ( ! ( prompt_lookup = calloc( 1, sizeof ( PROMPT_LOOKUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_lookup;
}

PROMPT_LOOKUP *prompt_lookup_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		POST_DICTIONARY *post_dictionary )
{
	PROMPT_LOOKUP *prompt_lookup;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !data_directory
	||   !post_dictionary )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_lookup = prompt_lookup_calloc();

	prompt_lookup->prompt_lookup_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prompt_lookup_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			application_menu_horizontal_boolean,
			data_directory,
			post_dictionary->original_post_dictionary );

	if ( prompt_lookup->prompt_lookup_input->forbid )
	{
		char message[ 128 ];

		sprintf(message, "prompt_lookup_input->forbid is set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_lookup->relation_mto1_exists_multi_select =
		relation_mto1_exists_multi_select(
			prompt_lookup->
				prompt_lookup_input->
				relation_mto1_list );

	prompt_lookup->omit_insert_button =
		prompt_lookup_omit_insert_button(
			prompt_lookup->
				prompt_lookup_input->
				role_folder_insert_boolean,
			prompt_lookup->
				prompt_lookup_input->
				drillthru_status->skipped_boolean,
			prompt_lookup->relation_mto1_exists_multi_select );

	prompt_lookup->omit_delete_button =
		prompt_lookup_omit_delete_button(
			prompt_lookup->
				prompt_lookup_input->
				folder_operation_list );

	prompt_lookup->include_chart_buttons =
		prompt_lookup_include_chart_buttons(
			prompt_lookup->
				prompt_lookup_input->
				folder_attribute_append_isa_list );

	prompt_lookup->include_sort_button =
		prompt_lookup_include_sort_button(
			ATTRIBUTE_NAME_SORT_ORDER,
			ATTRIBUTE_NAME_DISPLAY_ORDER,
			ATTRIBUTE_NAME_SEQUENCE_NUMBER,
			prompt_lookup->
				prompt_lookup_input->
				folder->
				folder_attribute_name_list );

	if ( prompt_lookup->
		prompt_lookup_input->
	  	drillthru_status->
	  	participating_boolean )
	{
		prompt_lookup->post_choose_folder_action_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_choose_folder_action_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_LOOKUP_STATE,
				prompt_lookup->
					prompt_lookup_input->
					drillthru_status->
					base_folder_name );
	}

	prompt_lookup->form_prompt_lookup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_lookup_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			prompt_lookup->omit_insert_button,
			prompt_lookup->omit_delete_button,
			prompt_lookup->include_chart_buttons,
			prompt_lookup->include_sort_button,
			prompt_lookup->
				prompt_lookup_input->
				folder->
				post_change_javascript,
			prompt_lookup->
				prompt_lookup_input->
				relation_mto1_list,
			prompt_lookup->
				prompt_lookup_input->
				relation_mto1_isa_list,
			prompt_lookup->
				prompt_lookup_input->
				relation_one2m_join_list,
			prompt_lookup->
				prompt_lookup_input->
				folder_attribute_append_isa_list,
			prompt_lookup->
				prompt_lookup_input->
				dictionary_separate_drillthru->
				drillthru_dictionary,
			prompt_lookup->
				prompt_lookup_input->
				drillthru_status->
				active_boolean,
			prompt_lookup->post_choose_folder_action_string );

	prompt_lookup->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
			prompt_lookup->
				prompt_lookup_input->
				folder_attribute_append_isa_list );

	prompt_lookup->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			application_name );

	prompt_lookup->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_lookup_title_string(
			APPASERVER_LOOKUP_STATE,
			folder_name );

	prompt_lookup->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			prompt_lookup->application_title_string,
			prompt_lookup->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			prompt_lookup->
				prompt_lookup_input->
				folder->
				notepad,
			(char *)0 /* onload_javascript_string */,
			(prompt_lookup->prompt_lookup_input->menu)
				? prompt_lookup->prompt_lookup_input->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(prompt_lookup->prompt_lookup_input->menu)
					? 1
					: 0 /* menu_boolean */ ),
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_calendar_setup_string(
			   prompt_lookup->
			     folder_attribute_calendar_date_name_list_length ),
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	prompt_lookup->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			prompt_lookup->document->html,
			prompt_lookup->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			prompt_lookup->document->document_body->html,
			prompt_lookup->form_prompt_lookup->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( prompt_lookup->form_prompt_lookup->html );

	return prompt_lookup;
}

PROMPT_LOOKUP_INPUT *prompt_lookup_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		DICTIONARY *original_post_dictionary )
{
	PROMPT_LOOKUP_INPUT *prompt_lookup_input;
	LIST *pair_list;

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

	prompt_lookup_input = prompt_lookup_input_calloc();

	prompt_lookup_input->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	prompt_lookup_input->role_folder_lookup_boolean =
		role_folder_lookup_boolean(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			folder_name,
			prompt_lookup_input->role_folder_list );

	if ( ( prompt_lookup_input->forbid =
		prompt_lookup_input_forbid(
			prompt_lookup_input->
				role_folder_lookup_boolean ) ) )
	{
		return prompt_lookup_input;
	}

	prompt_lookup_input->role_folder_insert_boolean =
		role_folder_insert_boolean(
			ROLE_PERMISSION_INSERT,
			folder_name,
			prompt_lookup_input->role_folder_list );

	prompt_lookup_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	prompt_lookup_input->exclude_attribute_name_list = list_new();

	list_set_list(
		prompt_lookup_input->exclude_attribute_name_list,
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_LOOKUP,
			prompt_lookup_input->
				role->
				role_attribute_exclude_list ) );

	list_set_list(
		prompt_lookup_input->exclude_attribute_name_list,
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_UPDATE,
			prompt_lookup_input->
				role->
				role_attribute_exclude_list ) );

	prompt_lookup_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			prompt_lookup_input->exclude_attribute_name_list,
			/* -------------------------------------------- */
			/* Sets folder_attribute_primary_list		*/
			/* Sets folder_attribute_primary_key_list	*/
			/* Sets folder_attribute_name_list		*/
			/* -------------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	prompt_lookup_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			prompt_lookup_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	prompt_lookup_input->relation_mto1_list =
		relation_mto1_without_omit_drillthru_list(
			prompt_lookup_input->relation_mto1_list );

	prompt_lookup_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			prompt_lookup_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	prompt_lookup_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			prompt_lookup_input->
				folder->
				folder_attribute_list,
			prompt_lookup_input->relation_mto1_isa_list );

	prompt_lookup_input->folder_operation_list =
		folder_operation_list(
			folder_name,
			role_name,
			0 /* not fetch_operation */,
			0 /* not fetch_process */ );

	if ( application_menu_horizontal_boolean )
	{
		prompt_lookup_input->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				data_directory,
				0 /* not folder_menu_remove_boolean */ );
	
		prompt_lookup_input->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				prompt_lookup_input->
					folder_menu->
					count_list );
	}

	prompt_lookup_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	prompt_lookup_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			prompt_lookup_input->
				folder_attribute_append_isa_list );

	prompt_lookup_input->dictionary_separate_drillthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_drillthru_new(
			original_post_dictionary,
			application_name,
			login_name,
			prompt_lookup_input->folder_attribute_date_name_list,
			prompt_lookup_input->folder_attribute_append_isa_list );

	prompt_lookup_input->drillthru_status =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_status_new(
			prompt_lookup_input->
				dictionary_separate_drillthru->
				drillthru_dictionary );

	if ( prompt_lookup_input->drillthru_status->skipped_boolean
	&&   list_length( prompt_lookup_input->relation_mto1_list ) )
	{
		prompt_lookup_input->relation_mto1_list =
			relation_mto1_foreign_key_less_equal_list(
				prompt_lookup_input->relation_mto1_list,
				1 /* max_foreign_key_list_length */ );
	}

	if ( !prompt_lookup_input->drillthru_status->participating_boolean
	||   !prompt_lookup_input->drillthru_status->skipped_boolean )
	{
		prompt_lookup_input->relation_one2m_join_list =
			relation_one2m_join_list(
				folder_name
					/* one_folder_name */,
				prompt_lookup_input->
					folder->
					folder_attribute_primary_key_list
					/* one_folder_primary_key_list */,
				(DICTIONARY *)0 /* no_display_dictionary */ );
	}

	pair_list =
		relation_one2m_pair_list(
			folder_name /* one_folder_name */,
			prompt_lookup_input->
				folder->
				folder_attribute_primary_key_list
				/* one_primary_key_list */ );

	prompt_lookup_input->relation_one2m_pair_list_length =
		list_length(
			pair_list );

	return prompt_lookup_input;
}

boolean prompt_lookup_input_forbid( boolean lookup_boolean )
{
	return 1 - lookup_boolean;
}

boolean prompt_lookup_omit_insert_button(
		boolean role_folder_insert_boolean,
		boolean drillthru_status_skipped_boolean,
		boolean relation_mto1_exists_multi_select )
{
	if ( !role_folder_insert_boolean )
		return 1;
	else
	if ( drillthru_status_skipped_boolean
	||   relation_mto1_exists_multi_select )
		return 1;
	else
		return 0;
}

boolean prompt_lookup_omit_delete_button(
		LIST *folder_operation_list )
{
	return !folder_operation_delete_boolean( folder_operation_list );
}

char *prompt_lookup_title_string(
		char *state,
		char *folder_name )
{
	static char title_string[ 128 ];
	char buffer1[ 16 ];
	char buffer2[ 128 ];

	sprintf(title_string,
		"%s %s",
		string_initial_capital(
			buffer1,
			state ),
		string_initial_capital(
			buffer2,
			folder_name ) );

	return title_string;
}

boolean prompt_lookup_include_sort_button(
		char *attribute_name_sort_order,
		char *attribute_name_display_order,
		char *attribute_name_sequence_number,
		LIST *folder_attribute_name_list )
{
	if ( !attribute_name_sort_order
	||   !attribute_name_display_order
	||   !attribute_name_sequence_number
	||   !list_length( folder_attribute_name_list ) )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_string_exists(
		attribute_name_sort_order /* string */,
		folder_attribute_name_list ) )
	{
		return 1;
	}

	if ( list_string_exists(
		attribute_name_display_order /* string */,
		folder_attribute_name_list ) )
	{
		return 1;
	}

	if ( list_string_exists(
		attribute_name_sequence_number /* string */,
		folder_attribute_name_list ) )
	{
		return 1;
	}

	return 0;
}

boolean prompt_lookup_restart_drillthru_button(
		boolean participating_boolean,
		boolean first_time_boolean )
{
	if ( !participating_boolean )
		return 0;
	else
		return 1 - first_time_boolean;
}

boolean prompt_lookup_include_chart_buttons(
		LIST *folder_attribute_append_isa_list )
{
	LIST *name_list;
	boolean return_value = 0;

	name_list =
		folder_attribute_float_name_list(
			folder_attribute_append_isa_list );

	if ( list_length( name_list ) )
	{
		return_value = 1;
		list_free_container( name_list );
	}
/* To be added later.
	else
	{
		name_list =
			folder_attribute_integer_name_list(
				folder_attribute_append_isa_list );

		if ( list_length( name_list ) )
		{
			return_value = 1;
			list_free_container( name_list );
		}
	}
*/

	return return_value;
}

