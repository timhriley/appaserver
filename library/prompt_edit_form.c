/* $APPASERVER_HOME/library/prompt_edit_form.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "environ.h"
#include "appaserver_library.h"
#include "drillthru.h"
#include "prompt_recursive.h"
#include "folder_attribute.h"
#include "dictionary_separate.h"
#include "prompt_edit_form.h"

PROMPT_EDIT_FORM *prompt_edit_form_calloc( void )
{
	PROMPT_EDIT_FORM *prompt_edit_form;

	if ( ! ( prompt_edit_form =
			calloc( 1, sizeof( PROMPT_EDIT_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_edit_form;
}

PROMPT_EDIT_FORM *prompt_edit_form_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			char *appaserver_mount_point,
			POST_DICTIONARY *post_dictionary )
{
	PROMPT_EDIT_FORM *prompt_edit_form = prompt_edit_form_calloc();

	/* Input */
	/* ----- */
	prompt_edit_form->application_name = application_name;
	prompt_edit_form->login_name = login_name;
	prompt_edit_form->session = session;
	prompt_edit_form->folder_name = folder_name;
	prompt_edit_form->role_name = role_name;
	prompt_edit_form->state = state;
	prompt_edit_form->target_frame = target_frame;
	prompt_edit_form->post_dictionary = post_dictionary;

	/* Process */
	/* ------- */
	if ( ! ( prompt_edit_form->role_folder_list =
			role_folder_fetch_list(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: role_folder_fetch_list(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		return (PROMPT_EDIT_FORM *)0;
	}

	if ( prompt_edit_form_forbid(
		role_folder_update( prompt_edit_form->role_folder_list ),
		role_folder_lookup( prompt_edit_form->role_folder_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: update or lookup permissions not set.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (PROMPT_EDIT_FORM *)0;
	}

	if ( ! ( prompt_edit_form->role =
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

		return (PROMPT_EDIT_FORM *)0;
	}

	if ( ! ( prompt_edit_form->folder =
			folder_fetch(
				folder_name,
				role_name,
				role_exclude_lookup_attribute_name_list(
					prompt_edit_form->
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

		return (PROMPT_EDIT_FORM *)0;
	}

	prompt_edit_form->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_folder_new(
			prompt_edit_form->
				post_dictionary->
				original_post_dictionary,
			prompt_edit_form->application_name,
			prompt_edit_form->login_name,
			folder_attribute_date_name_list(
				prompt_edit_form->
					folder->
					folder_attribute_list ) );

	if ( !prompt_edit_form->dictionary_separate )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: dictionary_separate_folder_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (PROMPT_EDIT_FORM *)0;
	}

	prompt_edit_form->prompt_recursive =
		prompt_recursive_new(
			folder_name,
			prompt_edit_form->
				folder->
				relation_mto1_non_isa_list );

	prompt_edit_form->drillthru =
		drillthru_fetch(
			prompt_edit_form->
				dictionary_separate->
				drillthru_dictionary );

	prompt_edit_form->omit_insert_button =
		prompt_insert_form_omit_insert_button(
			drillthru_skipped(
				folder_name,
				(prompt_edit_form->drillthru)
					? prompt_edit_form->
						drillthru->
						base_folder_name
					: (char *)0,
				(prompt_edit_form->drillthru)
					? list_length(
						prompt_edit_form->
					     	    drillthru->
					     	    fulfilled_folder_name_list )
					: 0 ) );

	return prompt_edit_form;
}

boolean prompt_edit_form_forbid(
			boolean update,
			boolean lookup )
{
	if ( !update || !lookup )
		return 1;
	else
		return 0;
}

boolean prompt_edit_form_omit_insert_button(
			boolean drillthru_skipped )
{
	return drillthru_skipped;
}

boolean prompt_edit_form_omit_delete_button(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

boolean prompt_edit_form_omit_new_button(
			boolean relation_exists_multi_select )
{
	return relation_exists_multi_select;
}

/* Returns relation_mto1_non_isa_list or null */
/* ------------------------------------------ */
LIST *prompt_edit_form_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped )
{
	if ( drillthru_skipped )
		return (LIST *)0;
	else
		return relation_mto1_non_isa_list;
}

/* Returns target_frame or "edit_form" */
/* ----------------------------------- */
char *prompt_edit_form_target_frame(
			char *target_frame,
			boolean drillthru_skipped )
{
	if ( drillthru_skipped )
		return EDIT_FRAME;
	else
		return target_frame;
}

