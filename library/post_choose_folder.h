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

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	boolean fetch_relation_mto1_isa_list;
	FOLDER *folder;
	LIST *relation_pair_one2m_list;
	DRILLTHRU *drillthru;
	char *form_name;
	boolean choose_isa_drop_down;
	boolean prompt_insert_form;
	boolean insert_table_form;
	boolean prompt_edit_form;
	boolean edit_table_form;
	char *system_string;
} POST_CHOOSE_FOLDER;

/* Operations */
/* ---------- */
POST_CHOOSE_FOLDER *post_choose_folder_calloc(
			void );

/* Always succeeds */
/* --------------- */
POST_CHOOSE_FOLDER *post_choose_folder_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			char *state );

boolean post_choose_folder_fetch_relation_mto1_isa_list(
			char *state /* only state=insert */ );

char *post_choose_folder_form_name(
			int relation_pair_one2m_list_length,
			char *folder_form );

boolean post_choose_folder_choose_isa_drop_down(
			int relation_mto1_isa_list_length );

boolean post_choose_folder_prompt_insert_form(
			char *folder_form_name,
			char *state );

boolean post_choose_folder_insert_table_form(
			char *folder_form_name,
			char *state );

boolean post_choose_folder_prompt_edit_form(
			char *folder_form_name,
			char *state,
			boolean drillthru_participating );

boolean post_choose_folder_edit_table_form(
			char *folder_form_name,
			char *state );

/* Returns heap memory */
/* ------------------- */
char *post_choose_folder_system_string(
			boolean choose_isa_drop_down,
			boolean prompt_insert_form,
			boolean insert_table_form,
			boolean prompt_edit_form,
			boolean edit_table_form,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			DICTIONARY *drillthru_dictionary,
			RELATION *first_one2m_isa_relation );

char *post_choose_folder_target_frame(
			boolean insert_table_form,
			boolean edit_table_form,
			boolean drillthru_participating );

#endif
