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
#include "document.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	/* Process */
	/* ------- */
	ROLE *role;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	char *state;
	boolean menu_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	SECURITY_ENTITY *security_entity;
	char *state;
	boolean primary_keys_non_edit;
	ROW_SECURITY *row_security;
	QUERY_EDIT_TABLE *query_edit_table;
	int row_insert_count;
	int cell_update_count;
	char *cell_update_folder_list_string;
	char *results_string;
	LIST *heading_name_list;
	LIST *key_list;
	char *title;
	char *message_html;
	DOCUMENT_EDIT_TABLE *document_edit_table;
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

int edit_table_row_insert_count(
			DICTIONARY *non_prefixed_dictionary );

int edit_table_cell_update_count(
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *edit_table_cell_update_folder_list_string(
			DICTIONARY *non_prefixed_dictionary );

/* Returns non_prefixed_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *edit_table_results_string(
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

char *edit_table_title(
			char *folder_name,
			char *edit_table_state );

/* Returns heap memory */
/* ------------------- */
char *edit_table_message_html(
			char *edit_table_title,
			int edit_table_row_insert_count,
			int edit_table_cell_update_count,
			char *edit_table_results_string );

/* Returns document_edit_table_html */
/* -------------------------------- */
char *edit_table_html(
			char *document_edit_table_html );

/* Returns document_edit_table_trailer_html */
/* ---------------------------------------- */
char *edit_table_trailer_html(
			char *document_edit_table_trailer_html );

/* Public */
/* ------ */
void edit_table_output(
			FILE *output_stream,
			char *edit_table_html,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *state,
			char *edit_table_trailer_html );

LIST *edit_table_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

/* Returns heap memory or null */
/* --------------------------- */
char *edit_table_row_html(
			DICTIONARY *row_dictionary,
			LIST *edit_table_apply_element_list,
			char *edit_table_background_color,
			char *state,
			int row_number );

/* Private */
/* ------- */
static char **edit_table_background_color_array(
			int *background_color_array_length,
			char *application_name );

char *edit_table_background_color(
			void );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *target_frame;
	char *detail_base_folder_name;

	/* Process */
	/* ------- */
	SESSION *session;
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
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame,
			char *detail_base_folder_name );

#endif
