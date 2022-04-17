/* $APPASERVER_HOME/library/edit_table.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EDIT_TABLE_H
#define EDIT_TABLE_H

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
#include "form_edit_table.h"
#include "document.h"
#include "dictionary_separate.h"

#define EDIT_TABLE_MAX_BACKGROUND_COLOR_ARRAY	10
#define EDIT_TABLE_OUTPUT_EXECUTABLE		"output_edit_table"

typedef struct
{
	ROLE *role;
	FOLDER *folder;
	DICTIONARY_SEPARATE_EDIT_TABLE *dictionary_separate;
	int folder_attribute_date_name_list_length;
	boolean menu_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *state;
	boolean primary_keys_non_edit;
	SECURITY_ENTITY *security_entity;
	char *security_entity_where;
	ROW_SECURITY *row_security;
	QUERY_EDIT_TABLE *query_edit_table;
	char *spool_filename;
	int dictionary_list_length;
	int row_insert_count;
	int cell_update_count;
	char *cell_update_folder_list_string;
	char *results_string;
	char *post_edit_table_action_string;
	LIST *heading_name_list;
	char *title_html;
	char *message_html;
	DOCUMENT *document;
	FORM_EDIT_TABLE *form_edit_table;
	char *html;
	char *trailer_html;
} EDIT_TABLE;

/* EDIT_TABLE operations */
/* --------------------- */

/* Usage */
/* ----- */
EDIT_TABLE *edit_table_new(
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
EDIT_TABLE *edit_table_calloc(
			void );

/* Returns program memory */
/* ---------------------- */
char *edit_table_state(
			LIST *role_folder_list );

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

/* Returns heap memory */
/* ------------------- */
char *edit_table_spool_filename(
			char *appaserver_parameter_data_directory,
			char *application_name,
			char *folder_name,
			char *session_key );

void edit_table_spool_file(
			char *edit_table_spool_filename,
			LIST *folder_attribute_name_list,
			LIST *row_dictionary_list,
			char sql_delimiter );

int edit_table_row_insert_count(
			char *rows_inserted_count_key,
			DICTIONARY *non_prefixed_dictionary );

int edit_table_cell_update_count(
			char *columns_updated_key,
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *edit_table_cell_update_folder_list_string(
			char *columns_updated_changed_folder_key,
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *edit_table_results_string(
			char *results_key_string,
			DICTIONARY *non_prefixed_dictionary );

/* Returns heap memory */
/* ------------------- */
char *edit_table_submit_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *detail_base_folder_name );

/* Returns list of heap memory */
/* --------------------------- */
LIST *edit_table_heading_name_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *edit_table_title_html(
			char *folder_name,
			char *edit_table_state );

/* Returns heap memory or null */
/* --------------------------- */
char *edit_table_message_html(
			int edit_table_row_insert_count,
			int edit_table_cell_update_count,
			char *edit_table_results_string );

/* Returns heap memory */
/* ------------------- */
char *edit_table_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html );

/* Returns heap memory */
/* ------------------- */
char *edit_table_trailer_html(
			char *document_body_close_html,
			char *document_close_html );

/* Usage */
/* ----- */
void edit_table_output(
			FILE *output_stream,
			char *application_name,
			char *edit_table_html,
			char *form_edit_table_html,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *edit_table_state,
			char *form_edit_table_trailer_html,
			char *edit_table_trailer_html );

void edit_table_apply_output(
			FILE *output_stream,
			char *application_name,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *edit_table_state );

LIST *edit_table_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

LIST *edit_table_operation_element_html_list(
			LIST *role_operation_list,
			int row_number,
			ROW_SECURITY_ROLE *row_security_role );

/* Returns heap memory or null */
/* --------------------------- */
char *edit_table_row_html(
			LIST *edit_table_apply_element_list /* in/out */,
			LIST *row_operation_list,
			char *application_name,
			char *edit_table_background_color,
			ROW_SECURITY_ROLE *row_security_role,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Usage */
/* ----- */
void edit_table_hidden_output(
			FILE *output_stream,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list );

/* Process */
/* ------- */

/* Private */
/* ------- */
char **edit_table_background_color_array(
			int *background_color_array_length );

char *edit_table_background_color(
			void );


boolean edit_table_viewonly(
			DICTIONARY *row_dictionary,
			char *row_security_role_update_attribute_not_null );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *edit_table_output_system_string(
			char *edit_table_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

#endif
