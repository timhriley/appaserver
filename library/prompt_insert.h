/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/prompt_insert.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_INSERT_H
#define PROMPT_INSERT_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "post_dictionary.h"
#include "folder_menu.h"
#include "menu.h"
#include "dictionary_separate.h"
#include "drillthru.h"
#include "folder_row_level_restriction.h"
#include "document.h"
#include "form_prompt_insert.h"

#define PROMPT_INSERT_EXECUTABLE	"output_prompt_insert"
#define PROMPT_INSERT_LOOKUP_CHECKBOX	"llookup_checkbox"
#define PROMPT_INSERT_LOOKUP_LABEL	"Lookup"

typedef struct
{
	LIST *role_folder_list;
	boolean role_folder_insert_boolean;
	boolean forbid;
	boolean role_folder_lookup_boolean;
	ROLE *role;
	LIST *role_attribute_exclude_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *relation_one2m_pair_list;
	LIST *folder_attribute_append_isa_list;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_DRILLTHRU *dictionary_separate_drillthru;
	DRILLTHRU_STATUS *drillthru_status;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
} PROMPT_INSERT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROMPT_INSERT_INPUT *prompt_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
PROMPT_INSERT_INPUT *prompt_insert_input_calloc(
		void );

boolean prompt_insert_input_forbid(
		boolean role_folder_insert_boolean );

typedef struct
{
	PROMPT_INSERT_INPUT *prompt_insert_input;
	char *post_choose_folder_action_string;
	FORM_PROMPT_INSERT *form_prompt_insert;
	char *application_title_string;
	char *title_string;
	int folder_attribute_calendar_date_name_list_length;
	LIST *javascript_filename_list;
	DOCUMENT *document;
	char *document_form_html;
} PROMPT_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROMPT_INSERT *prompt_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
PROMPT_INSERT *prompt_insert_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *prompt_insert_title_string(
		const char *appaserver_insert_state,
		char *folder_name );

LIST *prompt_insert_javascript_filename_list(
		char *javascript_filename );

#endif
