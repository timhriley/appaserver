/* $APPASERVER_HOME/library/table_edit.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TABLE_EDIT_H
#define TABLE_EDIT_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "folder_menu.h"
#include "menu.h"
#include "row_security.h"
#include "query.h"
#include "session.h"
#include "form_table_edit.h"
#include "document.h"
#include "dictionary_separate.h"

#define TABLE_EDIT_OUTPUT_EXECUTABLE		"output_table_edit"

typedef struct
{
	ROLE *role;
	FOLDER *folder;
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate;
	int folder_attribute_date_name_list_length;
	boolean menu_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *state;
	boolean primary_keys_non_edit;
	SECURITY_ENTITY *security_entity;
	char *security_entity_where;
	ROW_SECURITY *row_security;
	QUERY_TABLE_EDIT *query_table_edit;
	char *spool_filename;
	int dictionary_list_length;
	int row_insert_count;
	int cell_update_count;
	char *cell_update_folder_list_string;
	char *results_string;
	char *post_table_edit_action_string;
	LIST *heading_name_list;
	char *title_html;
	char *message_html;
	DOCUMENT *document;
	FORM_TABLE_EDIT *form_table_edit;
	char *html;
	char *trailer_html;
} TABLE_EDIT;

/* TABLE_EDIT operations */
/* --------------------- */

/* Usage */
/* ----- */
TABLE_EDIT *table_edit_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			boolean menu_horizontal_boolean,
			DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
TABLE_EDIT *table_edit_calloc(
			void );

/* Returns program memory */
/* ---------------------- */
char *table_edit_state(
			LIST *role_folder_list );

boolean table_edit_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

/* Returns heap memory */
/* ------------------- */
char *table_edit_spool_filename(
			char *appaserver_parameter_data_directory,
			char *application_name,
			char *folder_name,
			char *session_key );

void table_edit_spool_file(
			char *table_edit_spool_filename,
			LIST *folder_attribute_name_list,
			LIST *row_dictionary_list,
			char sql_delimiter );

int table_edit_row_insert_count(
			char *rows_inserted_count_key,
			DICTIONARY *non_prefixed_dictionary );

int table_edit_cell_update_count(
			char *columns_updated_key,
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *table_edit_cell_update_folder_list_string(
			char *columns_updated_changed_folder_key,
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *table_edit_results_string(
			char *results_key_string,
			DICTIONARY *non_prefixed_dictionary );

/* Returns heap memory */
/* ------------------- */
char *table_edit_submit_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *detail_base_folder_name );

/* Returns list of heap memory */
/* --------------------------- */
LIST *table_edit_heading_name_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *table_edit_title_html(
			char *folder_name,
			char *table_edit_state );

/* Returns heap memory or null */
/* --------------------------- */
char *table_edit_message_html(
			int table_edit_row_insert_count,
			int table_edit_cell_update_count,
			char *table_edit_results_string );

/* Returns heap memory */
/* ------------------- */
char *table_edit_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html );

/* Returns heap memory */
/* ------------------- */
char *table_edit_trailer_html(
			char *document_body_close_html,
			char *document_close_html );

/* Usage */
/* ----- */
void table_edit_output(
			FILE *output_stream,
			char *application_name,
			char *table_edit_html,
			char *form_table_edit_html,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *table_edit_state,
			char *form_table_edit_trailer_html,
			char *table_edit_trailer_html );

void table_edit_apply_output(
			FILE *output_stream,
			char *application_name,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *table_edit_state );

LIST *table_edit_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

LIST *table_edit_operation_element_html_list(
			LIST *role_operation_list,
			int row_number,
			ROW_SECURITY_ROLE *row_security_role );

/* Returns heap memory or null */
/* --------------------------- */
char *table_edit_row_html(
			LIST *table_edit_apply_element_list /* in/out */,
			LIST *row_operation_list,
			char *application_name,
			char *form_background_color,
			ROW_SECURITY_ROLE *row_security_role,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Usage */
/* ----- */
void table_edit_hidden_output(
			FILE *output_stream,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list );

/* Process */
/* ------- */

/* Private */
/* ------- */
boolean table_edit_viewonly(
			DICTIONARY *row_dictionary,
			char *row_security_role_update_attribute_not_null );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *table_edit_output_system_string(
			char *table_edit_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *subsub_title,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

#endif
