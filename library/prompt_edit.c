/* $APPASERVER_HOME/library/prompt_edit.c				*/
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
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			boolean menu_boolean,
			char *appaserver_mount_point,
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

	if ( ( prompt_edit->forbid = prompt_edit_forbid(
		role_folder_update( prompt_edit->role_folder_list ),
		role_folder_lookup( prompt_edit->role_folder_list ) ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: update or lookup permissions not set.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

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
				1 /* fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */,
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

	if ( menu_boolean )
	{
		prompt_edit->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				data_directory,
				role_name );
	
		prompt_edit->menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame,
				folder_menu->lookup_count_list );
	}

	prompt_edit_form->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_folder_new(
			prompt_edit->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				prompt_edit->
					folder->
					folder_attribute_list ) );

	prompt_edit->drillthru =
		drillthru_fetch(
			prompt_edit->
				dictionary_separate->
				drillthru_dictionary );

	prompt_edit->drillthru_skipped =
		drillthru_skipped(
			folder_name,
			(prompt_edit->drillthru)
				? prompt_edit->
					drillthru->
					base_folder_name
				: (char *)0,
			(prompt_edit->drillthru)
				? list_length(
					prompt_edit->
				     	    drillthru->
				     	    fulfilled_folder_name_list )
				: 0 );

	prompt_edit->omit_insert_button =
		prompt_edit_omit_insert_button(
			prompt_edit->drillthru_skipped );

	prompt_edit->omit_delete_button =
		prompt_edit_omit_delete_button(
			list_length(
				prompt_edit->
					folder->
					relation_mto1_isa_list ) );

	prompt_edit->omit_new_button =
		prompt_edit_omit_new_button(
			relation_exists_multi_select(
				prompt_edit->
					folder->
					relation_mto1_non_isa_list ) );

	prompt_edit->target_frame =
		/* ------------------------------------------- */
		/* Returns target_frame or frameset_edit_frame */
		/* ------------------------------------------- */
		prompt_edit_target_frame(
			target_frame,
			FRAMESET_EDIT_FRAME,
			prompt_edit->drillthru_skipped );

	prompt_edit->folder->relation_mto1_non_isa_list =
		/* -------------------------------------------- */
		/* Returns relation_mto1_non_isa_list or	*/
		/* those with only a single foreign key.	*/
		/* -------------------------------------------- */
		prompt_edit_drillthru_skipped(
			prompt_edit->folder->relation_mto1_non_isa_list,
			prompt_edit->drillthru_skipped );

	prompt_edit->title =
		prompt_edit_title(
			state,
			folder_name );

	prompt_edit->
	return prompt_edit;
}

boolean prompt_edit_forbid(
			boolean update,
			boolean lookup )
{
	if ( !update || !lookup )
		return 1;
	else
		return 0;
}

boolean prompt_edit_omit_insert_button(
			boolean drillthru_skipped )
{
	return drillthru_skipped;
}

boolean prompt_edit_omit_delete_button(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

boolean prompt_edit_omit_new_button(
			boolean relation_exists_multi_select )
{
	return relation_exists_multi_select;
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

