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
#include <malloc.h>
#include "environ.h"
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
	if ( ! ( prompt_edit_form->role_folder =
			role_folder_fetch(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: role_folder_fetch(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		return (PROMPT_EDIT_FORM *)0;
	}

	if ( prompt_edit_form_forbid(
		prompt_edit_form->role_folder->update_yn,
		prompt_edit_form->role_folder->lookup_yn ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update or lookup permissions not set.\n",
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
			"ERROR in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		exit( 1 );
	}

	if ( ! ( prompt_edit_form->folder =
			folder_fetch(
				folder_name,
				role_exclude_lookup_attribute_name_list(
					prompt_edit_form->
						role->
						attribute_exclude_list ),
				/* --------------------------------------- */
				/* Also sets folder_attribute_primary_list */
				/* and primary_key_list			   */
				/* --------------------------------------- */
				1 /* fetch_folder_attribute_list */,
				0 /* not fetch_relation_mto1_non_isa_list */,
				/* ------------------------------------------*/
				/* Also sets folder_attribute_append_isa_list*/,
				/* ------------------------------------------*/
				1 /* fetch_relation_mto1_isa_list */,
				1 /* fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		return (EDIT_TABLE_FORM *)0;
	}

	dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_folder_new(
			prompt_edit_form->post_dictionary,
			prompt_edit_form->application_name,
			prompt_edit_form->login_name,
			folder_attribute_date_name_list(
				prompt_edit_form->
					folder->
					folder_attribute_list ) );

	return prompt_edit_form;
}

boolean prompt_edit_form_forbid(
			char update_yn,
			char lookup_yn )
{
	if ( update_yn != 'y' || lookup_yn != 'y' )
		return 1;
	else
		return 0;
}

