/* $APPASERVER_HOME/library/prompt_lookup.c				*/
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

PROMPT_LOOKUP *prompt_lookup_calloc( void )
{
	PROMPT_LOOKUP *prompt_lookup;

	if ( ! ( prompt_lookup = calloc( 1, sizeof( PROMPT_LOOKUP ) ) ) )
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
			char *state,
			boolean menu_horizontal_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary )
{
	PROMPT_LOOKUP *prompt_lookup = prompt_lookup_calloc();

	if ( ! ( prompt_lookup->role_folder_list =
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

		prompt_lookup->forbid = 1;
		return prompt_lookup;
	}

	if ( ( prompt_lookup->forbid =
		prompt_lookup_forbid(
			role_folder_update(
				prompt_lookup->role_folder_list ),
			role_folder_lookup(
				prompt_lookup->role_folder_list ),
			state,
			APPASERVER_UPDATE_STATE ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: prompt_lookup_forbid(%s/%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name,
			state );

		return prompt_lookup;
	}

	if ( ! ( prompt_lookup->role =
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

		prompt_lookup->forbid = 1;
		return prompt_lookup;
	}

	if ( ! ( prompt_lookup->folder =
			folder_fetch(
				folder_name,
				role_name,
				role_exclude_lookup_attribute_name_list(
					prompt_lookup->
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

		prompt_lookup->forbid = 1;
		return prompt_lookup;
	}

	if ( menu_horizontal_boolean )
	{
		prompt_lookup->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				data_directory );
	
		prompt_lookup->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* frameset_menu_horizontal */,
				prompt_lookup->
					folder_menu->
					count_list );
	}

	prompt_lookup->folder_attribute_date_name_list_length =
		folder_attribute_date_name_list_length(
			prompt_lookup->
				folder->
				folder_attribute_append_isa_list );

	prompt_lookup->dictionary_separate_drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_drillthru_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				prompt_lookup->
					folder->
					folder_attribute_append_isa_list ),
			prompt_lookup->
				folder->
				folder_attribute_append_isa_list );

	prompt_lookup->drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		drillthru_continue(
			prompt_lookup->
				dictionary_separate_drillthru->
				drillthru_dictionary /* in/out */,
			folder_name );

	if ( !prompt_lookup->drillthru->drillthru_participating
	||   prompt_lookup->drillthru->finished )
	{
		prompt_lookup->folder->relation_join_one2m_list =
			relation_join_one2m_list(
				prompt_lookup->folder->
					relation_one2m_recursive_list,
				(DICTIONARY *)0 /* ignore_dictionary */ );
	}

	prompt_lookup->omit_insert_button =
		prompt_lookup_omit_insert_button(
			prompt_lookup->drillthru->skipped,
			relation_exists_multi_select(
				prompt_lookup->
					folder->
					relation_mto1_non_isa_list ) );

	prompt_lookup->omit_delete_button =
		prompt_lookup_omit_delete_button(
			list_length(
				prompt_lookup->
					folder->
					relation_mto1_isa_list ) );

	prompt_lookup->folder->relation_mto1_non_isa_list =
		/* -------------------------------------------- */
		/* Returns relation_mto1_non_isa_list or	*/
		/* those with only a single foreign key.	*/
		/* -------------------------------------------- */
		prompt_lookup_drillthru_skipped(
			prompt_lookup->folder->relation_mto1_non_isa_list,
			prompt_lookup->drillthru->skipped );

	prompt_lookup->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_lookup_title_html(
			state,
			folder_name );

	prompt_lookup->security_entity =
		security_entity_new(
			login_name,
			prompt_lookup->folder->non_owner_forbid,
			prompt_lookup->role->override_row_restrictions );

	prompt_lookup->form_prompt_lookup =
		form_prompt_lookup_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			prompt_lookup->omit_insert_button,
			prompt_lookup->omit_delete_button,
			prompt_lookup->folder->folder_attribute_append_isa_list,
			prompt_lookup->folder->relation_mto1_non_isa_list,
			prompt_lookup->folder->relation_join_one2m_list,
			prompt_lookup->
				dictionary_separate_drillthru->
				drillthru_dictionary,
			security_entity_where(
				prompt_lookup->security_entity,
				prompt_lookup->
					folder->
					folder_attribute_list ) );

	if ( !prompt_lookup->form_prompt_lookup )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_prompt_lookup_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prompt_lookup->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			application_title_string(
				application_name ),
			prompt_lookup->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_horizontal_boolean,
			prompt_lookup->menu,
			document_head_menu_setup_string(
				menu_horizontal_boolean ),
			document_head_calendar_setup_string(
				prompt_lookup->
				       folder_attribute_date_name_list_length ),
			document_head_javascript_include_string(),
			(char *)0 /* input_onload_string */ );

	prompt_lookup->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			prompt_lookup->document->html,
			prompt_lookup->document->document_head->html,
			document_head_close_html(),
			prompt_lookup->document->document_body->html,
			prompt_lookup->form_prompt_lookup->html,
			document_body_close_html(),
			document_close_html() );

	free( prompt_lookup->form_prompt_lookup->html );

	return prompt_lookup;
}

boolean prompt_lookup_forbid(
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

boolean prompt_lookup_omit_insert_button(
			boolean drillthru_skipped,
			boolean relation_exists_multi_select )
{
	return drillthru_skipped || relation_exists_multi_select;
}

boolean prompt_lookup_omit_delete_button(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

LIST *prompt_lookup_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped )
{
	if ( drillthru_skipped )
		return (LIST *)0;
	else
		return relation_mto1_non_isa_list;
}

char *prompt_lookup_title_html(
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

char *prompt_lookup_output_system_string(
			char *executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
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
		session_key,
		login_name,
		role_name,
		folder_name,
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		appaserver_error_filename );

	return strdup( system_string );
}
