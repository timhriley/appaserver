/* $APPASERVER_HOME/library/generic_load.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef GENERIC_LOAD_H
#define GENERIC_LOAD_H

#include <stdio.h>
#include "list.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "element.h"
#include "folder.h"
#include "folder_attribute.h"
#include "dictionary.h"
#include "relation.h"
#include "form.h"

/* Constants */
/* --------- */
#define GENERIC_LOAD_SKIP_HEADER_ROWS	"skip_header_rows"
#define GENERIC_LOAD_DROP_DOWN_NAME 	"generic_load_drop_down"
#define GENERIC_LOAD_UPLOAD_LABEL 	"generic_load_upload"
#define GENERIC_LOAD_POSITION_PREFIX	"position_"
#define GENERIC_LOAD_IGNORE_PREFIX	"ignore_"

#define GENERIC_LOAD_REPLACE_EXISTING_YN \
					"replace_existing_records_yn"

#define GENERIC_LOAD_FOLDER_EXECUTABLE	"generic_load_folder"

#define GENERIC_LOAD_CHOOSE_POST_EXECUTABLE \
					"post_generic_load_choose"

#define GENERIC_LOAD_FOLDER_POST_EXECUTABLE \
					"post_generic_load_folder"

typedef struct
{
	char *constant_data;
	int position;
	boolean ignore;
	boolean is_primary_key_date;
	boolean is_primary_key_time;
	RELATION *consumes_relation;
} GENERIC_LOAD_ATTRIBUTE;

/* GENERIC_LOAD_ATTRIBUTE operations */
/* --------------------------------- */
LIST *generic_load_attribute_list(
			DICTIONARY *working_post_dictionary,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list );

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_fetch(
			DICTIONARY *working_post_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			LIST *relation_mto1_non_isa_list );

/* Process */
/* ------- */
char *generic_load_attribute_constant_data(
			DICTIONARY *working_post_dictionary,
			char *attribute_name );

int generic_load_attribute_position(
			DICTIONARY *working_post_dictionary,
			char *attribute_name,
			char *generic_load_position_prefix );

boolean generic_load_attribute_ignore(
			DICTIONARY *working_post_dictionary,
			char *attribute_name,
			char *GENERIC_LOAD_IGNORE_PREFIX );

boolean generic_load_attribute_is_primary_key_date(
			char *datatype_name,
			int primary_key_index );

boolean generic_load_attribute_is_primary_key_time(
			char *datatype_name,
			int primary_key_index );

RELATION *generic_load_attribute_relation(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list );

/* Private */
/* ------- */
GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_calloc(
			void );

typedef struct
{
	char *folder_name;
	char *system_string;
} GENERIC_LOAD_CHOOSE_POST;

/* GENERIC_LOAD_CHOOSE_POST operations */
/* ----------------------------------- */

GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			DICTIONARY *working_post_dictionary );

/* Returns working_post_dictionary->hash_table->other_data */
/* ------------------------------------------------------- */
char *generic_load_choose_post_folder_name(
			char *generic_load_drop_down_name,
			DICTIONARY *working_post_dictionary );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_post_system_string(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *generic_load_folder_executable,
			char *generic_load_choose_post_folder_name );

/* Private */
/* ------- */
GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_calloc(
			void );

typedef struct
{
	LIST *element_list;
	ROW_SECURITY_RELATION *row_security_relation;
} GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST;

/* GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST operations */
/* ---------------------------------------------- */
GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
	generic_load_attribute_element_list_new(
			LIST *relation_mto1_non_isa_list,
			FOLDER_ATTRIBUTE *folder_attribute,
			char *login_name,
			int position );

/* Private */
/* ------- */
GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
	generic_load_attribute_element_list_calloc(
			void );

typedef struct
{
	LIST *element_list;
	int position;

	GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
		generic_load_attribute_element_list;

} GENERIC_LOAD_FOLDER_ELEMENT_LIST;

/* GENERIC_LOAD_FOLDER_ELEMENT_LIST operations */
/* ------------------------------------------- */
GENERIC_LOAD_FOLDER_ELEMENT_LIST *
	generic_load_folder_element_list_new(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *generic_load_upload_label,
			char *generic_load_skip_header_rows,
			char *login_name  );

/* Private */
/* ------- */
GENERIC_LOAD_FOLDER_ELEMENT_LIST *generic_load_folder_element_list_calloc(
			void );

typedef struct
{
	char *tag_html;
	GENERIC_LOAD_FOLDER_ELEMENT_LIST *generic_load_folder_element_list;
	char *html;
} GENERIC_LOAD_FOLDER_FORM;

/* GENERIC_LOAD_FOLDER_FORM operations */
/* ----------------------------------- */
GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_new(
			char *generic_load_folder_prompt_html,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *generic_load_folder_post_action_string,
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *generic_load_folder_form_html(
			char *tag_html,
			char *prompt_html,
			char *generic_load_folder_element_list_html,
			char *form_close_html );

/* Private */
/* ------- */
GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_calloc(
			void );

typedef struct
{
	char *tag_html;
	LIST *element_list;
	char *element_list_html;
	char *html;
} GENERIC_LOAD_CHOOSE_FORM;

/* GENERIC_LOAD_CHOOSE_FORM operations */
/* ----------------------------------- */
GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_calloc(
			void );

GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_new(
			char *generic_load_choose_prompt_html,
			char *generic_load_choose_drop_down_name,
			LIST *role_folder_insert_name_list,
			char *generic_load_choose_post_action_string );

LIST *generic_load_choose_form_element_list(
			LIST *role_folder_insert_name_list,
			char *generic_load_choose_drop_down_name );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_form_element_list_html(
			LIST *generic_load_choose_form_element_list );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_form_html(
			char *tag_html,
			char *generic_load_choose_form_element_list_html,
			char *form_close_html );

typedef struct
{
	ROLE *role;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *post_action_string;
	char *prompt_html;
	char *title_string;
	DOCUMENT *document;
	FORM_GENERIC_LOAD_CHOOSE *form_generic_load_choose;
	char *html;
} GENERIC_LOAD_CHOOSE;

/* GENERIC_LOAD_CHOOSE operations */
/* ------------------------------ */
GENERIC_LOAD_CHOOSE *generic_load_choose_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *process_name,
			char *role_name,
			boolean menu_boolean );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_post_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *process_name,
			char *role_name,
			char *generic_load_choose_post_executable );

/* Return static memory */
/* -------------------- */
char *generic_load_choose_prompt_html(
			char *process_name );

/* Return static memory */
/* -------------------- */
char *generic_load_choose_title_string(
			char *process_name );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *form_generic_load_html,
			char *document_body_close_html,
			char *document_close_html );

/* Private */
/* ------- */
GENERIC_LOAD_CHOOSE *generic_load_choose_calloc(
			void );

typedef struct
{
	char *attribute_name;
	boolean is_primary_key;
	boolean is_primary_key_date_datatype;
	boolean is_primary_key_time_datatype;
	int position;
	char *constant;
	char *datatype;
	boolean is_participating;
} GENERIC_LOAD_ATTRIBUTE;

typedef struct
{
	FOLDER_MENU *folder_menu;
	MENU *menu;
	DOCUMENT *document;
	char *post_action_string;
	char *prompt_html;
	char *title_string;
	FOLDER *folder;
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form;
	char *html;
} GENERIC_LOAD_FOLDER;

/* GENERIC_LOAD_FOLDER operations */
/* ------------------------------ */
GENERIC_LOAD_FOLDER *generic_load_folder_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			boolean menu_boolean );

char *generic_load_folder_post_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			char *generic_load_folder_post_executable );

/* Returns static memory */
/* --------------------- */
char *generic_load_folder_prompt_html(
			char *folder_name );

/* Returns static memory */
/* --------------------- */
char *generic_load_folder_title_string(
			char *folder_name );

/* Returns heap memory */
/* ------------------- */
char *generic_load_folder_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *generic_load_folder_form_html,
			char *document_body_close_html,
			char *document_close_html );

/* Private */
/* ------- */
GENERIC_LOAD_FOLDER *generic_load_folder_calloc(
			void );

/* Prototypes */
/* ---------- */
char *generic_load_get_primary_key_list_string(
				LIST *generic_load_attribute_list );

void generic_load_reset_row_count(
				void );

boolean generic_load_skip_header_rows(
				int skip_header_rows );

boolean generic_load_delimiters_only(
				char *input_buffer,
				char delimiter );

boolean generic_load_has_participating(
				LIST *generic_load_attribute_list );

char *generic_load_get_heading_string(
				LIST *generic_load_attribute_list );

void generic_load_attribute_set_is_participating(
				LIST *generic_load_folder_list,
				char *delimiter,
				char *application_name,
				char *load_filename,
				int skip_header_rows );

boolean position_all_valid(	DICTIONARY *position_dictionary );

void generic_load_get_primary_key_date_time_offset(
				int *primary_key_date_offset,
				int *primary_key_time_offset,
				LIST *generic_load_attribute_list );

boolean generic_load_fix_time(
				char *input_buffer,
				char delimiter,
				int primary_key_time_offset );

void generic_load_replace_time_2400_with_0000(
				char *input_buffer,
				char delimiter,
				int primary_key_date_offset,
				int primary_key_time_offset );

char *generic_load_get_piece_buffer(
				char *input_buffer,
				char *delimiter,
				char *application_name,
				char *datatype,
				char *constant,
				int piece_offset,
				boolean is_primary_key );

GENERIC_LOAD_FOLDER *generic_load_get_folder(
				char *folder_name,
				LIST *attribute_list,
				DICTIONARY *position_dictionary,
				DICTIONARY *constant_dictionary,
				boolean primary_attributes_only,
				char *login_name,
				char *related_attribute_name );

GENERIC_LOAD *generic_load_new(
				void );

GENERIC_LOAD_FOLDER *generic_load_folder_new(
				char *folder_name );

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
				char *attribute_name,
				boolean is_primary_key );

/*
FOLDER *generic_load_get_database_folder(
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean with_mto1_related_folders );
*/

LIST *generic_load_get_folder_list(
				FOLDER *folder,
				DICTIONARY *position_dictionary,
				DICTIONARY *constant_dictionary,
				char *login_name );

/*
boolean generic_load_delete_from_database(
				char *application_name,
				char *load_filename,
				char delimiter,
				GENERIC_LOAD_FOLDER *generic_load_folder,
				int skip_header_rows_integer );
*/

int generic_load_output_to_database(
				char *application_name,
				char *load_filename,
				char delimiter,
				LIST *generic_load_folder_list,
				int skip_header_rows_integer );

int generic_load_output_test_only_report(
				char *load_filename,
				char delimiter,
				LIST *generic_load_attribute_list,
				int primary_key_date_offset,
				int primary_key_time_offset,
				char *application_name,
				boolean display_errors,
				char *folder_name,
				int skip_header_rows_integer,
				boolean with_sort_unique );

boolean generic_load_build_sys_string(
				char *sys_string,
				char *application_name,
				char *folder_name,
				LIST *generic_load_attribute_list,
				boolean sort_unique );

void generic_load_set_international_date(
				char *input_buffer,
				char *application_name,
				char delimiter,
				int primary_key_date_offset );

int generic_load_get_position_integer(	DICTIONARY *position_dictionary,
				char *attribute_name,
				char *related_attribute_name );

/* Private */
/* ------- */
LIST *generic_load_buttons_element_list(
			void );

LIST *generic_load_upload_element_list(
			char *generic_load_upload_label );

LIST *generic_load_skip_header_rows_element_list(
			char *generic_load_skip_header_rows );

LIST *generic_load_execute_checkbox_element_list(
			void );

LIST *generic_load_dialog_box_element_list(
			void );

LIST *generic_load_position_element_list(
			char *attribute_name,
			int primary_key_index,
			char *hint_message,
			int position );

APPASERVER_ELEMENT *generic_load_prompt_element(
			char *attribute_name,
			int primary_key_index,
			char *hint_message );

APPASERVER_ELEMENT *generic_load_position_element(
			char *attribute_name,
			int position,
			char *generic_load_position_prefix );

APPASERVER_ELEMENT *generic_load_constant_element(
			char *attribute_name,
			char *datatype_name,
			int width );

APPASERVER_ELEMENT *generic_load_ignore_element(
			char *attribute_name,
			char *generic_load_position_prefix,
			char *generic_load_ignore_prefix );

#endif

