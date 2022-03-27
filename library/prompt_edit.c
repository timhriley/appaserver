/* $APPASERVER_HOME/library/prompt_edit.c				*/
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
#include "prompt_edit.h"

PROMPT_EDIT *prompt_edit_calloc( void )
{
	PROMPT_EDIT *prompt_edit;

	if ( ! ( prompt_edit = calloc( 1, sizeof( PROMPT_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_edit;
}

PROMPT_EDIT *prompt_edit_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *state,
			boolean menu_horizontal_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary )
{
	PROMPT_EDIT *prompt_edit = prompt_edit_calloc();

	if ( ! ( prompt_edit->role_folder_list =
			role_folder_list(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: role_folder_list(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		prompt_edit->forbid = 1;
		return prompt_edit;
	}

	if ( ( prompt_edit->forbid =
		prompt_edit_forbid(
			role_folder_update(
				prompt_edit->role_folder_list ),
			role_folder_lookup(
				prompt_edit->role_folder_list ),
			state,
			APPASERVER_UPDATE_STATE ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: prompt_edit_forbid(%s/%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name,
			state );

		return prompt_edit;
	}

	if ( ! ( prompt_edit->role =
			role_fetch(
				role_name,
				1 /* fetch_attribute_exclude_list */ ) ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );

		prompt_edit->forbid = 1;
		return prompt_edit;
	}

	if ( ! ( prompt_edit->folder =
			folder_fetch(
				folder_name,
				role_name,
				role_exclude_lookup_attribute_name_list(
					prompt_edit->
						role->
						attribute_exclude_list ),
				/* --------------------------------------- */
				/* Also sets folder_attribute_primary_list */
				/* and primary_key_list			   */
				/* --------------------------------------- */
				1 /* fetch_folder_attribute_list */,
				1 /* fetch_relation_mto1_non_isa_list */,
				/* ------------------------------------------ */
				/* Also sets folder_attribute_append_isa_list */
				/* ------------------------------------------ */
				1 /* fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				0 /* not fetch_role_folder_list */,
				1 /* fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		prompt_edit->forbid = 1;
		return prompt_edit;
	}

	if ( menu_horizontal_boolean )
	{
		prompt_edit->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				data_directory );
	
		prompt_edit->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* frameset_menu_horizontal */,
				prompt_edit->
					folder_menu->
					count_list );
	}

	prompt_edit->folder_attribute_date_name_list_length =
		list_length(
			folder_attribute_date_name_list(
				prompt_edit->
					folder->
					folder_attribute_append_isa_list ) );

	prompt_edit->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_folder_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				prompt_edit->
					folder->
					folder_attribute_append_isa_list ) );

	prompt_edit->drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		drillthru_continue(
			prompt_edit->
				dictionary_separate->
				drillthru_dictionary /* in/out */,
			folder_name );

	if ( !prompt_edit->drillthru->drillthru_participating
	||   prompt_edit->drillthru->finished )
	{
		prompt_edit->folder->relation_join_one2m_list =
			relation_join_one2m_list(
				prompt_edit->folder->
					relation_one2m_recursive_list,
				(DICTIONARY *)0 /* ignore_dictionary */ );
	}

	prompt_edit->omit_insert_button =
		prompt_edit_omit_insert_button(
			prompt_edit->drillthru->skipped,
			relation_exists_multi_select(
				prompt_edit->
					folder->
					relation_mto1_non_isa_list ) );

	prompt_edit->omit_delete_button =
		prompt_edit_omit_delete_button(
			list_length(
				prompt_edit->
					folder->
					relation_mto1_isa_list ) );

	prompt_edit->target_frame =
		/* ------------------------------------------- */
		/* Returns target_frame or FRAMESET_EDIT_FRAME */
		/* ------------------------------------------- */
		prompt_edit_target_frame(
			target_frame,
			FRAMESET_EDIT_FRAME,
			prompt_edit->drillthru->skipped );

	prompt_edit->folder->relation_mto1_non_isa_list =
		/* -------------------------------------------- */
		/* Returns relation_mto1_non_isa_list or	*/
		/* those with only a single foreign key.	*/
		/* -------------------------------------------- */
		prompt_edit_drillthru_skipped(
			prompt_edit->folder->relation_mto1_non_isa_list,
			prompt_edit->drillthru->skipped );

	prompt_edit->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_edit_title_html(
			state,
			folder_name );

	prompt_edit->action_string =
		prompt_edit_action_string(
			PROMPT_EDIT_POST_EXECUTABLE,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			prompt_edit->target_frame,
			state );

	prompt_edit->security_entity =
		security_entity_new(
			login_name,
			prompt_edit->folder->non_owner_forbid,
			prompt_edit->role->override_row_restrictions );

	prompt_edit->form_prompt_edit =
		form_prompt_edit_new(
			folder_name,
			prompt_edit->action_string,
			prompt_edit->omit_insert_button,
			prompt_edit->omit_delete_button,
			prompt_edit->folder->folder_attribute_append_isa_list,
			prompt_edit->folder->relation_mto1_non_isa_list,
			prompt_edit->folder->relation_join_one2m_list,
			prompt_edit->dictionary_separate->drillthru_dictionary,
			login_name,
			security_entity_where(
				prompt_edit->security_entity,
				prompt_edit->
					folder->
					folder_attribute_list ),
			prompt_edit->drillthru->drillthru_participating,
			prompt_edit->drillthru->skipped,
			prompt_edit->drillthru->finished );

	if ( !prompt_edit->form_prompt_edit )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_prompt_edit_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_edit->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			application_title_string(
				application_name ),
			prompt_edit->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			frameset_menu_horizontal,
			prompt_edit->menu,
			document_head_menu_setup_string(
				menu_horizontal_boolean ),
			document_head_calendar_setup_string(
				prompt_edit->
				       folder_attribute_date_name_list_length ),
			document_head_javascript_include_string(),
			(char *)0 /* input_onload_string */ );

	prompt_edit->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			prompt_edit->document->html,
			prompt_edit->document->document_head->html,
			document_head_close_html(),
			prompt_edit->document->document_body->html,
			prompt_edit->form_prompt_edit->html,
			document_body_close_html(),
			document_close_html() );

	free( prompt_edit->form_prompt_edit->html );

	return prompt_edit;
}

boolean prompt_edit_forbid(
			boolean update,
			boolean lookup,
			char *state,
			char *appaserver_update_state )
{
	if ( !state || !*state ) return 1;

	if ( !update && !lookup ) return 1;

	if ( string_strcmp( state, appaserver_update_state ) == 0
	&&   !update )
	{
		return 1;
	}

	return 0;
}

boolean prompt_edit_omit_insert_button(
			boolean drillthru_skipped,
			boolean relation_exists_multi_select )
{
	return drillthru_skipped || relation_exists_multi_select;
}

boolean prompt_edit_omit_delete_button(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

LIST *prompt_edit_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped )
{
	if ( drillthru_skipped )
		return (LIST *)0;
	else
		return relation_mto1_non_isa_list;
}

char *prompt_edit_target_frame(
			char *target_frame,
			char *frameset_edit_frame,
			boolean drillthru_skipped )
{
	if ( drillthru_skipped )
		return frameset_edit_frame;
	else
		return target_frame;
}

char *prompt_edit_title_html(
			char *state,
			char *folder_name )
{
	static char title_html[ 128 ];
	char buffer1[ 16 ];
	char buffer2[ 128 ];

	sprintf(title_html,
		"<h1>%s %s</h1>",
		string_initial_capital(
			buffer1,
			state ),
		string_initial_capital(
			buffer2,
			folder_name ) );

	return title_html;
}

char *prompt_edit_action_string(
			char *prompt_edit_post_executable,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state )
{
	char action_string[ 1024 ];

	if ( !prompt_edit_post_executable
	||   !application_name
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !role_name
	||   !target_frame
	||   !state )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		prompt_edit_post_executable,
		application_name,
		login_name,
		session_key,
		folder_name,
		role_name,
		target_frame,
		state );

	return strdup( action_string );
}

char *prompt_edit_output_system_string(
			char *executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !role_name
	||   !target_frame
	||   !state
	||   !dictionary_separate_send_string
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
		"%s %s %s %s %s %s %s \"%s\" 2>>%s",
		executable,
		login_name,
		session_key,
		folder_name,
		role_name,
		target_frame,
		state,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

