/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/table_insert.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef TABLE_INSERT_H
#define TABLE_INSERT_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "relation_mto1.h"
#include "folder_row_level_restriction.h"
#include "folder_menu.h"
#include "menu.h"
#include "dictionary.h"
#include "document.h"
#include "dictionary_separate.h"
#include "query.h"
#include "pair_one2m.h"
#include "vertical_new_table.h"
#include "form_table_insert.h"

#define TABLE_INSERT_EXECUTABLE	"output_table_insert"

#define TABLE_INSERT_PRIMARY_IGNORED_MESSAGE \
				"Error: the entire primary key was ignored."

typedef struct
{
	LIST *role_folder_list;
	boolean role_folder_insert_boolean;
	boolean table_insert_forbid;
	char *title_string;
	ROLE *role;
	LIST *role_attribute_exclude_insert_name_list;
	FOLDER *folder;
	LIST *folder_attribute_non_primary_name_list;
	LIST *relation_mto1_list;
	RELATION_MTO1 *relation_mto1_automatic_preselection;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_date_name_list;
	int folder_attribute_calendar_date_name_list_length;
	DICTIONARY_SEPARATE_TABLE_INSERT *dictionary_separate;
	PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert;
	char *appaserver_parameter_data_directory;
	boolean application_menu_horizontal_boolean;
	boolean vertical_new_table_participating_boolean;
	boolean menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
} TABLE_INSERT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TABLE_INSERT_INPUT *table_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
TABLE_INSERT_INPUT *table_insert_input_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *table_insert_input_title_string(
		char *appaserver_insert_state,
		char *folder_name );

/* Usage */
/* ----- */
boolean table_insert_input_menu_horizontal_boolean(
		const char *frameset_prompt_frame,
		char *target_frame,
		boolean application_menu_horizontal_boolean,
		boolean vertical_new_table_participating_boolean );

typedef struct
{
	TABLE_INSERT_INPUT *table_insert_input;
	boolean entire_primary_key_ignored;
	VERTICAL_NEW_TABLE *vertical_new_table;
	QUERY_DICTIONARY *query_dictionary;
	FORM_TABLE_INSERT_AUTOMATIC *
		form_table_insert_automatic /* html is freed */;
	int rows_number;
	DICTIONARY *prompt_dictionary;
	FORM_TABLE_INSERT *
		form_table_insert /* html is freed */;
	char *document_head_calendar_setup_string;
	char *onload_javascript_string;
	LIST *javascript_filename_list;
	char *javascript_include_string;
	DOCUMENT *document;
	char *document_form_html;
} TABLE_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TABLE_INSERT *table_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *results_string,
		char *error_string,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
TABLE_INSERT *table_insert_calloc(
		void );

int table_insert_rows_number(
		int folder_insert_rows_number,
		VERTICAL_NEW_TABLE *vertical_new_table );

/* Create a hidden widget for the many_folder_name			*/
/* -------------------------------------------------------------------- */
/* Returns dictionary_separate_prompt_dictionary or dictionary_small()  */
/* -------------------------------------------------------------------- */
DICTIONARY *table_insert_prompt_dictionary(
		const char *vertical_new_many_hidden_label,
		DICTIONARY *dictionary_separate_prompt_dictionary,
		VERTICAL_NEW_TABLE *vertical_new_table );

/* Returns component of vertical_new_table or null */
/* ----------------------------------------------- */
char *table_insert_onload_javascript_string(
		VERTICAL_NEW_TABLE *vertical_new_table );

LIST *table_insert_javascript_filename_list(
		char *folder_javascript_filename );

/* Returns either's html */
/* --------------------- */
char *table_insert_form_html(
		FORM_TABLE_INSERT_AUTOMATIC *
			form_table_insert_automatic,
		FORM_TABLE_INSERT *
			form_table_insert );

/* Usage */
/* ----- */
boolean table_insert_forbid(
		boolean role_folder_insert_boolean );

/* Usage */
/* ----- */
boolean table_insert_entire_primary_key_ignored(
		LIST *folder_attribute_primary_key_list,
		LIST *ignore_name_list );

#endif
