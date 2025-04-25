/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/drilldown.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DRILLDOWN_H
#define DRILLDOWN_H

#include "list.h"
#include "boolean.h"
#include "session.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "table_edit.h"

typedef struct
{
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *relation_one2m_list;
	LIST *folder_attribute_append_isa_list;
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate;
	char *post_table_edit_folder_name;
	boolean application_menu_horizontal_boolean;
	boolean menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	LIST *primary_attribute_data_list;
	pid_t process_id;
} DRILLDOWN_INPUT;

/* Usage */

/* Safely returns */
/* -------------- */
DRILLDOWN_INPUT *drilldown_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
DRILLDOWN_INPUT *drilldown_input_calloc(
		void );

/* Returns component of non_prefixed_dictionary or null */
/* ---------------------------------------------------- */
char *drilldown_input_post_table_edit_folder_name(
		const char *post_table_edit_folder_label,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */
LIST *drilldown_input_primary_attribute_data_list(
		const char attribute_multi_key_delimiter,
		char *drilldown_primary_data_list_string );

typedef struct
{
	char *one_folder_name;
	LIST *attribute_data_list;
	DICTIONARY *query_dictionary;
	DICTIONARY *drilldown_original_post_dictionary;
	TABLE_EDIT *table_edit;
} DRILLDOWN_MANY_TO_ONE;

/* Usage */
/* ----- */
DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		pid_t process_id,
		DICTIONARY *sort_dictionary,
		char *one_folder_name,
		LIST *relation_translate_list,
		LIST *one_folder_primary_key_list,
		DICTIONARY *query_fetch_dictionary );

/* Process */
/* ------- */
DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one_calloc(
		void );

/* Usage */
/* ----- */
LIST *drilldown_many_to_one_attribute_data_list(
		LIST *relation_translate_list,
		LIST *one_folder_primary_key_list,
		DICTIONARY *query_fetch_dictionary );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY *drilldown_many_to_one_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *one_folder_primary_key_list,
		LIST *drilldown_many_to_one_attribute_data_list );

typedef struct
{
	char *relation_many_folder_name;
	DICTIONARY *query_dictionary;
	DICTIONARY *drilldown_original_post_dictionary;
	TABLE_EDIT *table_edit;
} DRILLDOWN_ONE_TO_MANY;

/* Usage */
/* ----- */
DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		pid_t process_id,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *sort_dictionary,
		LIST *primary_attribute_data_list,
		char *relation_many_folder_name,
		LIST *relation_foreign_key_list );

/* Process */
/* ------- */
DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY *drilldown_one_to_many_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *primary_attribute_data_list,
		LIST *relation_foreign_key_list );

typedef struct
{
	TABLE_EDIT *table_edit;
} DRILLDOWN_PRIMARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLDOWN_PRIMARY *drilldown_primary_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		pid_t process_id,
		DICTIONARY *drilldown_original_post_dictionary );

/* Process */
/* ------- */
DRILLDOWN_PRIMARY *drilldown_primary_calloc(
		void );

typedef struct
{
	char *title_string;
	char *document_head_menu_setup_string;
	char *document_head_calendar_setup_string;
	LIST *javascript_filename_list;
	char *javascript_include_string;
	char *onload_string;
	DOCUMENT *document;
	LIST *ajax_client_list;
	char *ajax_client_list_javascript;
} DRILLDOWN_DOCUMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLDOWN_DOCUMENT *drilldown_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *update_results_string,
		char *update_error_string,
		MENU *menu,
		char *folder_notepad,
		char *folder_javascript_filename,
		char *folder_post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_one2m_list,
		DRILLDOWN_PRIMARY *drilldown_primary,
		LIST *drilldown_one_to_many_list );

/* Process */
/* ------- */
DRILLDOWN_DOCUMENT *drilldown_document_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *drilldown_document_title_string(
		char *drilldown_base_folder_name );

/* Returns update_results_string or null */
/* ------------------------------------- */
char *drilldown_document_update_results_string(
		char *update_results_string );

/* Usage */
/* ----- */
LIST *drilldown_document_javascript_filename_list(
		char *folder_javascript_filename,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *drilldown_document_onload_string(
		char *folder_post_change_javascript,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *drilldown_document_ajax_client_list(
		DRILLDOWN_PRIMARY *drilldown_primary,
		LIST *drilldown_one_to_many_list );

typedef struct
{
	SESSION_FOLDER *session_folder;
	DRILLDOWN_INPUT *drilldown_input;
	DICTIONARY *query_dictionary;
	DICTIONARY *original_post_dictionary;
	DRILLDOWN_PRIMARY *drilldown_primary;
	LIST *drilldown_one_to_many_list;
	LIST *drilldown_many_to_one_list;
	DRILLDOWN_DOCUMENT *drilldown_document;
} DRILLDOWN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLDOWN *drilldown_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *update_results_string,
		char *update_error_string,
		DICTIONARY *original_post_dictionary,
		char *data_directory );

/* Process */
/* ------- */
DRILLDOWN *drilldown_calloc(
		void );

/* Usage */
/* ------ */

/* Returns query_dictionary or dictionary_small() */
/* ---------------------------------------------- */
DICTIONARY *drilldown_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *query_dictionary,
		LIST *key_list,
		LIST *attribute_data_list );

/* Usage */
/* ----- */
void drilldown_table_edit_output(
		const char *appaserver_user_login_name,
		char *login_name,
		TABLE_EDIT *table_edit );

/* Safely returns */
/* -------------- */
DICTIONARY *drilldown_original_post_dictionary(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_sort_prefix,
		const char *dictionary_separate_query_prefix,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *sort_dictionary,
		DICTIONARY *drilldown_query_dictionary );

#endif
