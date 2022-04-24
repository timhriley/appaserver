/* $APPASERVER_HOME/library/prompt_insert.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_INSERT_H
#define PROMPT_INSERT_H

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
#include "form_prompt_insert.h"

/* Constants */
/* --------- */
#define PROMPT_INSERT_POST_EXECUTABLE	"post_prompt_insert"
#define PROMPT_INSERT_OUTPUT_EXECUTABLE	"output_prompt_insert"

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
	char *target_frame;
	char *title_html;
	char *action_string;
	SECURITY_ENTITY *security_entity;
	FORM_PROMPT_INSERT *form_prompt_insert;
	DOCUMENT *document;
	char *document_form_html;
} PROMPT_INSERT;

/* PROMPT_INSERT operations */
/* ------------------------ */

/* Usage */
/* ----- */

/* ---------------------------- */
/* Always succeeds		*/
/* Note: check forbid flag.	*/
/* ---------------------------- */
PROMPT_INSERT *prompt_insert_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean menu_horizontal_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROMPT_INSERT *prompt_insert_calloc(
			void );

boolean prompt_insert_forbid(
			boolean role_prompt_insert );

/* Public */
/* ------ */
char *prompt_insert_output_system_string(
			char *prompt_insert_output_executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */

#endif
