/* $APPASERVER_HOME/library/edit_table.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EDIT_TABLE_H
#define EDIT_TABLE_H

/* Includes */
/* -------- */
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

/* Constants */
/* --------- */
#define EDIT_TABLE_OUTPUT_EXECUTABLE	"output_edit_table"

/* Data structures */
/* --------------- */
typedef struct
{
	ROLE *role;
	FOLDER *folder;
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
	int dictionary_list_length;
	int row_insert_count;
	int cell_update_count;
	char *cell_update_folder_list_string;
	char *results_string;
	char *submit_action_string;
	LIST *heading_name_list;
	char *title_html;
	char *message_html;
	DOCUMENT *document;
	FORM_EDIT_TABLE *form_edit_table;
	char *html;
	char *trailer_html;
} EDIT_TABLE;

/* Prototypes */
/* ---------- */
EDIT_TABLE *edit_table_calloc(
			void );

EDIT_TABLE *edit_table_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean menu_boolean,
			DICTIONARY *query_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *ignore_select_attribute_name_list );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *edit_table_state(
			LIST *role_folder_list );

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

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
			char *form_edit_table_html );

/* Returns heap memory */
/* ------------------- */
char *edit_table_trailer_html(
			char *form_edit_table_trailer_html,
			char *document_body_close_html,
			char *document_close_html );

/* Public */
/* ------ */
void edit_table_output(
			FILE *output_stream,
			char *application_name,
			char *edit_table_html,
			char *form_edit_table_html,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *state,
			char *form_edit_table_trailer_html,
			char *edit_table_trailer_html );

void edit_table_regular_output(
			FILE *output_stream,
			char *application_name,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *state );

LIST *edit_table_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

/* Returns heap memory or null */
/* --------------------------- */
char *edit_table_row_html(
			LIST *edit_table_apply_element_list /* in/out */,
			char *application_name,
			char *edit_table_background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

void edit_table_hidden_output(
			FILE *output_stream,
			LIST *row_dictionary_list,
			LIST *regular_element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *edit_table_hidden_row_html(
			LIST *regular_element_list /* in/out */,
			int row_number,
			DICTIONARY *row_dictionary );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *edit_table_output_system_string(
			char *edit_table_output_executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */
char **edit_table_background_color_array(
			int *background_color_array_length,
			char *application_name );

char *edit_table_background_color(
			char *application_name );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *target_frame;
	char *detail_base_folder_name;

	/* Process */
	/* ------- */
	SESSION_FOLDER *session_folder;
} EDIT_TABLE_POST;

/* EDIT_TABLE_POST operations */
/* -------------------------- */
EDIT_TABLE_POST *edit_table_post_calloc(
			void );

/* Always succeeds */
/* --------------- */
EDIT_TABLE_POST *edit_table_post_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame,
			char *detail_base_folder_name );

#endif
