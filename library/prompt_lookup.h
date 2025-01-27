/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/prompt_lookup.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef PROMPT_LOOKUP_H
#define PROMPT_LOOKUP_H

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
#include "dictionary_separate.h"
#include "document.h"
#include "drillthru.h"
#include "form_prompt_lookup.h"

#define PROMPT_LOOKUP_EXECUTABLE	"output_prompt_lookup"

#define PROMPT_LOOKUP_FROM_PREFIX 	"ffrom_"
#define PROMPT_LOOKUP_TO_PREFIX 	"tto_"

typedef struct
{
	LIST *role_folder_list;
	boolean role_folder_lookup_boolean;
	boolean forbid;
	boolean role_folder_insert_boolean;
	ROLE *role;
	LIST *exclude_attribute_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_operation_list;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_DRILLTHRU *dictionary_separate_drillthru;
	DRILLTHRU_STATUS *drillthru_status;
	LIST *relation_one2m_join_list;
	int relation_one2m_pair_list_length;
} PROMPT_LOOKUP_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROMPT_LOOKUP_INPUT *prompt_lookup_input_new(
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
PROMPT_LOOKUP_INPUT *prompt_lookup_input_calloc(
		void );

boolean prompt_lookup_input_forbid(
		boolean role_folder_lookup_boolean );

typedef struct
{
	PROMPT_LOOKUP_INPUT *prompt_lookup_input;
	boolean relation_mto1_exists_multi_select;
	boolean omit_insert_button;
	boolean omit_delete_button;
	boolean include_chart_buttons;
	boolean include_sort_button;
	char *post_choose_folder_action_string;
	FORM_PROMPT_LOOKUP *form_prompt_lookup;
	int folder_attribute_calendar_date_name_list_length;
	char *application_title_string;
	char *title_string;
	DOCUMENT *document;
	char *document_form_html;
} PROMPT_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROMPT_LOOKUP *prompt_lookup_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean application_menu_horizontal_boolean,
		char *data_directory,
		POST_DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROMPT_LOOKUP *prompt_lookup_calloc(
		void );

boolean prompt_lookup_omit_insert_button(
		boolean role_folder_insert_boolean,
		boolean drillthru_status_skipped_boolean,
		boolean relation_mto1_exists_multi_select );

boolean prompt_lookup_omit_delete_button(
		LIST *folder_operation_list );

boolean prompt_lookup_include_chart_buttons(
		LIST *folder_attribute_append_isa_list );

boolean prompt_lookup_include_sort_button(
		char *attribute_name_sort_order,
		char *attribute_name_display_order,
		char *attribute_name_sequence_number,
		LIST *folder_attribute_name_list );

/* Returns static memory */
/* --------------------- */
char *prompt_lookup_title_string(
		char *state,
		char *folder_name );

#endif
