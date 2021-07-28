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
			DICTIONARY *post_dictionary )
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

	if ( ! ( prompt_edit_form->folder =
			folder_fetch(
				folder_name,
				1 /* fetch_attribute_list */,
				0 /* not fetch_row_level_restriction */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		return (PROMPT_EDIT_FORM *)0;
	}

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				prompt_edit_form->post_dictionary,
				prompt_edit_form->application_name,
				prompt_edit_form->folder->attribute_list,
				(LIST *)0 /* operation_name_list */) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: dictionary_appaserver_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (PROMPT_EDIT_FORM *)0;
	}

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

