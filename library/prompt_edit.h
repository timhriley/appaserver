/* $APPASERVER_HOME/library/prompt_edit.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_EDIT_H
#define PROMPT_EDIT_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "drillthru.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */

typedef struct
{
	LIST *role_folder_list;
	boolean forbid;
	ROLE *role;
	FOLDER *folder;
	DICTIONARY_SEPARATE *dictionary_separate;
	DRILLTHRU *drillthru;
	boolean drillthru_skipped;
	boolean omit_insert_button;
	boolean omit_delete_button;
	boolean omit_new_button;
} PROMPT_EDIT;

/* PROMPT_EDIT operations */
/* ---------------------- */

/* ---------------------------- */
/* Always succeeds		*/
/* Note: check forbid flag.	*/
/* ---------------------------- */
PROMPT_EDIT *prompt_edit_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			char *appasever_mount_point,
			POST_DICTIONARY *post_dictionary );

boolean prompt_edit_forbid(
			boolean update,
			boolean lookup );

boolean prompt_edit_omit_insert_button(
			boolean drillthru_skipped );

boolean prompt_edit_omit_delete_button(
			int relation_mto1_isa_list_length );

boolean prompt_edit_omit_new_button(
			boolean relation_exists_multi_select );

/* Returns relation_mto1_non_isa_list or null */
/* ------------------------------------------ */
LIST *prompt_edit_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped );

/* Returns target_frame or frameset_edit_frame */
/* ------------------------------------------- */
char *prompt_edit_target_frame(
			char *target_frame,
			char *frameset_edit_frame,
			boolean drillthru_skipped );

/* Private */
/* ------- */
PROMPT_EDIT *prompt_edit_calloc(
			void );

#endif
