/* $APPASERVER_HOME/library/prompt_edit_form.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_EDIT_FORM_H
#define PROMPT_EDIT_FORM_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *login_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *state;
	char *target_frame;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	ROLE_FOLDER *role_folder;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	boolean prompt_edit_form_omit_new_button;

} PROMPT_EDIT_FORM;

/* Prototypes */
/* ---------- */
PROMPT_EDIT_FORM *prompt_edit_form_calloc(
			void );

PROMPT_EDIT_FORM *prompt_edit_form_new(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			DICTIONARY *post_dictionary );

boolean prompt_edit_form_forbid(
			char update_yn,
			char lookup_yn );

#endif
