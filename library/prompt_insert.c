/* $APPASERVER_HOME/library/prompt_insert.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
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
#include "prompt_insert.h"

PROMPT_INSERT *prompt_insert_calloc( void )
{
	PROMPT_INSERT *prompt_insert;

	if ( ! ( prompt_insert = calloc( 1, sizeof( PROMPT_INSERT ) ) ) )
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

PROMPT_INSERT *prompt_insert_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean menu_horizontal_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary )
{
	PROMPT_INSERT *prompt_insert = prompt_insert_calloc();

	prompt_insert->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	prompt_insert->forbid =
		prompt_insert_forbid(
			role_folder_insert(
				prompt_insert->role_folder_list ) );

	if ( prompt_insert->forbid )
	{
		return prompt_insert;
	}

	if ( ! ( prompt_insert->role =
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

		prompt_insert->forbid = 1;
		return prompt_insert;
	}

	if ( ! ( prompt_insert->folder =
			folder_fetch(
				folder_name,
				role_name,
				role_exclude_insert_attribute_name_list(
					prompt_insert->
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

		prompt_insert->forbid = 1;
		return prompt_insert;
	}

	if ( menu_horizontal_boolean )
	{
		prompt_insert->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				data_directory );
	
		prompt_insert->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* frameset_menu_horizontal */,
				prompt_insert->
					folder_menu->
					count_list );
	}

	prompt_insert->folder_attribute_date_name_list_length =
		list_length(
			folder_attribute_date_name_list(
				prompt_insert->
					folder->
					folder_attribute_append_isa_list ) );

	prompt_insert->dictionary_separate_prompt_edit =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_prompt_insert_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				prompt_insert->
					folder->
					folder_attribute_append_isa_list ) );

	prompt_edit->drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		drillthru_continue(
			prompt_insert->
				dictionary_separate_prompt_insert->
				drillthru_dictionary /* in/out */,
			folder_name );

	prompt_insert->target_frame =
		/* ------------------------------------------- */
		/* Returns target_frame or FRAMESET_EDIT_FRAME */
		/* ------------------------------------------- */
		prompt_insert_target_frame(
			target_frame,
			FRAMESET_EDIT_FRAME,
			prompt_insert->drillthru->skipped );

	prompt_edit->folder->relation_mto1_non_isa_list =
		/* -------------------------------------------- */
		/* Returns relation_mto1_non_isa_list or	*/
		/* those with only a single foreign key.	*/
		/* -------------------------------------------- */
		prompt_insert_drillthru_skipped(
			prompt_insert->folder->relation_mto1_non_isa_list,
			prompt_insert->drillthru->skipped );

	prompt_insert->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_insert_title_html(
			APPASERVER_INSERT_STATE,
			folder_name );

	prompt_insert->security_entity =
		security_entity_new(
			login_name,
			prompt_insert->folder->non_owner_forbid,
			prompt_insert->role->override_row_restrictions );

	prompt_insert->form_prompt_insert =
		form_prompt_insert_new(
			folder_name,
			prompt_insert->action_string,
			prompt_insert->folder->folder_attribute_append_isa_list,
			prompt_insert->folder->relation_mto1_non_isa_list,
			prompt_insert->
				dictionary_separate_prompt_insert->
				drillthru_dictionary,
			login_name,
			security_entity_where(
				prompt_insert->security_entity,
				prompt_insert->
					folder->
					folder_attribute_list ),
			prompt_insert->drillthru->drillthru_participating,
			prompt_insert->drillthru->skipped,
			prompt_insert->drillthru->finished );

	if ( !prompt_insert->form_prompt_insert )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_prompt_insert_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_insert->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			application_title_string(
				application_name ),
			prompt_insert->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_horizontal_boolean,
			prompt_insert->menu,
			document_head_menu_setup_string(
				menu_horizontal_boolean ),
			document_head_calendar_setup_string(
				prompt_insert->
				       folder_attribute_date_name_list_length ),
			document_head_javascript_include_string(),
			(char *)0 /* input_onload_string */ );

	prompt_insert->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			prompt_insert->document->html,
			prompt_insert->document->document_head->html,
			document_head_close_html(),
			prompt_insert->document->document_body->html,
			prompt_insert->form_prompt_insert->html,
			document_body_close_html(),
			document_close_html() );

	free( prompt_insert->form_prompt_insert->html );

	return prompt_insert;
}

char *prompt_insert_output_system_string(
			char *executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !role_name
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
		"%s %s %s %s %s \"%s\" 2>>%s",
		executable,
		login_name,
		session_key,
		folder_name,
		role_name,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

boolean prompt_insert_forbid(
			boolean role_prompt_insert )
{
	return 1 - role_prompt_insert;
}

