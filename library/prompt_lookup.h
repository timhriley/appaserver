/* $APPASERVER_HOME/library/prompt_lookup.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_LOOKUP_H
#define PROMPT_LOOKUP_H

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
#include "form_prompt_lookup.h"

/* Constants */
/* --------- */
#define PROMPT_LOOKUP_OUTPUT_EXECUTABLE	"output_prompt_lookup"

typedef struct
{
	LIST *role_folder_list;
	boolean forbid;
	ROLE *role;
	FOLDER *folder;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	int folder_attribute_date_name_list_length;
	DICTIONARY_SEPARATE_DRILLTHRU *dictionary_separate_drillthru;
	DRILLTHRU *drillthru;
	boolean omit_insert_button;
	boolean omit_delete_button;
	char *title_html;
	SECURITY_ENTITY *security_entity;
	FORM_PROMPT_LOOKUP *form_prompt_lookup;
	DOCUMENT *document;
	char *document_form_html;
} PROMPT_LOOKUP;

/* PROMPT_LOOKUP operations */
/* ---------------------- */

/* Usage */
/* ----- */

/* ---------------------------- */
/* Always succeeds		*/
/* Note: check forbid flag.	*/
/* ---------------------------- */
PROMPT_LOOKUP *prompt_lookup_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *state,
			boolean menu_horizontal_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROMPT_LOOKUP *prompt_lookup_calloc(
			void );

boolean prompt_lookup_forbid(
			boolean role_folder_update,
			boolean role_folder_lookup,
			char *state,
			char *appaserver_update_state );

boolean prompt_lookup_omit_insert_button(
			boolean drillthru_skipped,
			boolean relation_exists_multi_select );


boolean prompt_lookup_omit_delete_button(
			int relation_mto1_isa_list_length );

/* Returns relation_mto1_non_isa_list or	*/
/* those with only a single foreign key.	*/
/* -------------------------------------------- */
LIST *prompt_lookup_drillthru_skipped(
			LIST *relation_mto1_non_isa_list,
			boolean drillthru_skipped );

/* Returns static memory */
/* --------------------- */
char *prompt_lookup_title_html(
			char *state,
			char *folder_name );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *prompt_lookup_output_system_string(
			char *prompt_lookup_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

#endif
