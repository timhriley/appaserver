/* $APPASERVER_HOME/library/post_choose_folder.h	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef POST_CHOOSE_FOLDER_H
#define POST_CHOOSE_FOLDER_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "relation.h"
#include "drillthru.h"

/* Constants */
/* --------- */
#define POST_CHOOSE_FOLDER_EXECUTABLE	"post_choose_folder"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	DRILLTHRU *drillthru;
	char *name;
	boolean relation_isa_boolean;
	FOLDER *folder;
	char *form_name;
	boolean isa_drop_down;
	boolean prompt_insert;
	boolean insert_table;
	boolean prompt_edit;
	boolean edit_table;
	char *system_string;
} POST_CHOOSE_FOLDER;

/* POST_CHOOSE_FOLDER operations */
/* ----------------------------- */

/* Usage */
/* ----- */

POST_CHOOSE_FOLDER *post_choose_folder_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state );

/* Process */
/* ------- */
char *post_choose_folder_name(
			char *folder_name,
			char *drillthru_start_current_folder_name );

boolean post_choose_folder_relation_isa_boolean(
			char *state,
			char *appaserver_state_insert );

char *post_choose_folder_form_name(
			int relation_pair_one2m_list_length,
			char *folder_form );

boolean post_choose_folder_isa_drop_down(
			int relation_mto1_isa_list_length );

boolean post_choose_folder_prompt_insert(
			char *folder_form_name,
			char *state );

boolean post_choose_folder_insert_table(
			char *folder_form_name,
			char *state );

boolean post_choose_folder_prompt_edit(
			char *folder_form_name,
			char *state,
			boolean drillthru_participating );

boolean post_choose_folder_edit_table(
			char *folder_form_name,
			char *state );

/* Returns frameset_prompt_frame or frameset_edit_frame */
/* ---------------------------------------------------- */
char *post_choose_folder_target_frame(
			boolean insert_table,
			boolean edit_table,
			boolean drillthru_participating,
			char *frameset_prompt_frame,
			char *framset_edit_frame );

/* Returns heap memory */
/* ------------------- */
char *post_choose_folder_system_string(
			boolean isa_drop_down,
			boolean prompt_insert_form,
			boolean insert_table_form,
			boolean prompt_edit_form,
			boolean edit_table_form,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame,
			DICTIONARY *drillthru_dictionary,
			RELATION *first_one2m_isa_relation );

/* Private */
/* ------- */
POST_CHOOSE_FOLDER *post_choose_folder_calloc(
			void );

/* Public */
/* ------ */
char *post_choose_folder_action_string(
			char *post_choose_folder_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *frameset_prompt_frame );

#endif
