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
#include "post_dictionary.h"
#include "dictionary_separate.h"

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
	POST_DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	LIST *role_folder_list;
	ROLE *role;
	FOLDER *folder;
	DICTIONARY_SEPARATE *dictionary_separate;
	DRILLTHRU *drillthru;
	boolean drillthru_skipped;
	PROMPT_RECURSIVE *prompt_recursive;
	boolean omit_insert_button;
	boolean omit_delete_button;
	boolean omit_new_button;

} PROMPT_EDIT_FORM;

/* Prototypes */
/* ---------- */
PROMPT_EDIT_FORM *prompt_edit_form_calloc(
			void );

PROMPT_EDIT_FORM *prompt_edit_form_fetch(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state /* maybe drillthru for state=insert */,
			char *appasever_mount_point,
			POST_DICTIONARY *post_dictionary );

boolean prompt_edit_form_forbid(
			boolean update,
			boolean lookup );

boolean prompt_edit_form_omit_insert_button(
			boolean drillthru_skipped );

boolean prompt_edit_form_omit_delete_button(
			int relation_mto1_isa_list_length );

boolean prompt_edit_form_omit_new_button(
			boolean relation_exists_multi_select );

/* Returns relation_mto1_non_isa_list or null */
/* ------------------------------------------ */
LIST *prompt_edit_form_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped );

/* Returns target_frame or "edit_form" */
/* ----------------------------------- */
char *prompt_edit_form_target_frame(
			char *target_frame,
			boolean drillthru_skipped );

#endif
