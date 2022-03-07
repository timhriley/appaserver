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
#include "frameset.h"
#include "folder_menu.h"
#include "menu.h"
#include "frameset.h"
#include "dictionary_separate.h"
#include "document.h"
#include "form_prompt_edit.h"

/* Constants */
/* --------- */
#define PROMPT_EDIT_POST_EXECUTABLE	"post_prompt_edit"

typedef struct
{
	LIST *role_folder_list;
	boolean forbid;
	ROLE *role;
	FOLDER *folder;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	int folder_attribute_date_name_list_length;
	DICTIONARY_SEPARATE *dictionary_separate;
	DRILLTHRU *drillthru;
	boolean omit_insert_button;
	boolean omit_delete_button;
	char *target_frame;
	char *title_html;
	char *action_string;
	SECURITY_ENTITY *security_entity;
	FORM_PROMPT_EDIT *form_prompt_edit;
	DOCUMENT *document;
	char *document_form_html;
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
			boolean menu_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary );

boolean prompt_edit_forbid(
			boolean role_folder_update,
			boolean role_folder_lookup,
			char *state,
			char *appaserver_update_state );

boolean prompt_edit_omit_insert_button(
			boolean drillthru_skipped,
			boolean relation_exists_multi_select );


boolean prompt_edit_omit_delete_button(
			int relation_mto1_isa_list_length );

/* Returns relation_mto1_non_isa_list or	*/
/* those with only a single foreign key.	*/
/* -------------------------------------------- */
LIST *prompt_edit_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped );

/* Returns target_frame or frameset_edit_frame */
/* ------------------------------------------- */
char *prompt_edit_target_frame(
			char *target_frame,
			char *frameset_edit_frame,
			boolean drillthru_skipped );

/* Returns static memory */
/* --------------------- */
char *prompt_edit_title_html(
			char *state,
			char *folder_name );

char *prompt_edit_action_string(
			char *prompt_edit_post_executable,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *prompt_edit_target_frame,
			char *state );

/* Private */
/* ------- */
PROMPT_EDIT *prompt_edit_calloc(
			void );

#endif
