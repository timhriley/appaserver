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

/* Constants */
/* --------- */
#define GENERIC_LOAD_SKIP_HEADER_ROWS	"skip_header_rows"
#define GENERIC_LOAD_DROP_DOWN_NAME 	"generic_load_drop_down"
#define GENERIC_LOAD_UPLOAD_LABEL 	"generic_load_upload"
#define GENERIC_LOAD_POSITION_PREFIX	"position_"
#define GENERIC_LOAD_IGNORE_PREFIX	"ignore_"
#define GENERIC_LOAD_FOLDER_FORM_NAME	"generic_load_folder"

#define GENERIC_LOAD_REPLACE_EXISTING_YN \
					"replace_existing_records_yn"

#define GENERIC_LOAD_EXECUTE_YN		"execute_yn"

#define GENERIC_LOAD_FOLDER_EXECUTABLE	"generic_load_folder"

#define GENERIC_LOAD_CHOOSE_POST_EXECUTABLE \
					"post_generic_load_choose"

#define GENERIC_LOAD_FOLDER_POST_EXECUTABLE \
					"post_generic_load_folder"

typedef struct
{
	FOLDER *folder;
	boolean generic_load_folder_forbid;
	LIST *generic_load_attribute_list;
	char *filename;
	boolean replace_existing;
	boolean execute;
	int skip_header_rows;
	LIST *generic_load_sql_statement_list;
} GENERIC_LOAD_FOLDER_POST;

/* GENERIC_LOAD_FOLDER_POST operations */
/* ----------------------------------- */
GENERIC_LOAD_FOLDER_POST *generic_load_folder_post_new(
			char *application_name,
			char *folder_name,
			char *role_name,
			DICTIONARY *working_post_dictionary );

char *generic_load_folder_post_filename(
			char *generic_load_upload_label,
			DICTIONARY *working_post_dictionary );

boolean generic_load_folder_post_replace_existing(
			char *generic_load_replace_existing_yn,
			DICTIONARY *working_post_dictionary );

boolean generic_load_folder_post_execute(
			char *generic_load_execute_yn,
			DICTIONARY *working_post_dictionary );

int generic_load_folder_post_skip_header_rows(
			char *generic_load_skip_header_rows,
			DICTIONARY *working_post_dictionary );

/* Private */
/* ------- */
GENERIC_LOAD_FOLDER_POST *generic_load_folder_post_calloc(
			void );

typedef struct
{
	char *attribute_name;
	char *string;
	char *security_sql_injection_escape_quote_delimit;
} GENERIC_LOAD_ATTRIBUTE_DATA;

/* GENERIC_LOAD_ATTRIBUTE_DATA operations */
/* -------------------------------------- */
LIST *generic_load_attribute_data_list(
			LIST *generic_load_attribute_list,
			char *input_line );

GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_new(
			char *attribute_name,
			char *constant_data,
			int position,
			boolean ignore,
			char *input_line );

/* Process */
/* ------- */
char *generic_load_attribute_data_string(
			char *constant_data,
			int position,
			boolean ignore,
			char *input_line );

/* Public */
/* ------ */
GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_seek(
			char *attribute_name,
			LIST *generic_load_attribute_data_list );

LIST *generic_load_attribute_data_name_list(
			LIST *generic_load_attribute_data_list );

LIST *generic_load_attribute_data_escape_list(
			LIST *generic_load_attribute_data_list );

/* Private */
/* ------- */
GENERIC_LOAD_ATTRIBUTE_DATA *generic_load_attribute_data_calloc(
			void );

typedef struct
{
	char *foreign_data_delimited;
	boolean orphan;
} GENERIC_LOAD_RELATION;

/* GENERIC_LOAD_RELATION operations */
/* -------------------------------- */
GENERIC_LOAD_RELATION *generic_load_relation_new(
			LIST *generic_load_attribute_data_list,
			LIST *relation_mto1_non_isa_list );

/* Returns heap memory or null */
/* --------------------------- */
char *generic_load_relation_foreign_data_delimited(
			LIST *generic_load_attribute_data_list,
			LIST *foreign_key_list );

boolean generic_load_relation_orphan(
			char *generic_load_relation_foreign_data_delimited,
			LIST *delimited_list );

/* Private */
/* ------- */
GENERIC_LOAD_RELATION *generic_load_relation_calloc(
			void );

typedef struct
{
	LIST *generic_load_attribute_data_list;
	GENERIC_LOAD_RELATION *generic_load_relation;
	char *string;
} GENERIC_LOAD_SQL_STATEMENT;

/* GENERIC_LOAD_SQL_STATEMENT operations */
/* ------------------------------------- */
LIST *generic_load_sql_statement_list(
			char *folder_table_name,
			LIST *generic_load_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *generic_load_folder_post_filename,
			boolean generic_load_folder_post_replace_existing,
			int generic_load_folder_post_skip_header_rows );

GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement_new(
			char *folder_table_name,
			LIST *generic_load_attribute_list,
			LIST *relation_mto1_non_isa_list,
			boolean generic_load_folder_post_replace_existing,
			char *input_line );

/* Returns heap memory */
/* ------------------- */
char *generic_load_sql_statement_string(
			char *folder_table_name,
			LIST *generic_load_attribute_data_list,
			boolean generic_load_folder_post_replace_existing );

/* Private */
/* ------- */
GENERIC_LOAD_SQL_STATEMENT *generic_load_sql_statement_calloc(
			void );

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *constant_data;
	int position;
	boolean ignore;
	RELATION *consumes_relation;
} GENERIC_LOAD_ATTRIBUTE;

/* GENERIC_LOAD_ATTRIBUTE operations */
/* --------------------------------- */
LIST *generic_load_attribute_list(
			DICTIONARY *working_post_dictionary,
			LIST *folder_attribute_list );

/* Always succeeds */
/* --------------- */
GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
			DICTIONARY *working_post_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute );

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
			char *generic_load_ignore_prefix );

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

typedef struct
{
	LIST *element_list;
	int position;

	GENERIC_LOAD_ATTRIBUTE_ELEMENT_LIST *
		generic_load_attribute_element_list;

	char *html;
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

char *generic_load_folder_element_list_html(
			LIST *element_list );

/* Private */
/* ------- */
GENERIC_LOAD_FOLDER_ELEMENT_LIST *generic_load_folder_element_list_calloc(
			void );

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
			char *generic_load_choose_prompt_html,
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
	GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form;
	char *html;
} GENERIC_LOAD_CHOOSE;

/* GENERIC_LOAD_CHOOSE operations */
/* ------------------------------ */
GENERIC_LOAD_CHOOSE *generic_load_choose_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean menu_boolean,
			boolean frameset_menu_horizontal );

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_post_action_string(
			char *generic_load_choose_post_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name );

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
	FOLDER_MENU *folder_menu;
	MENU *menu;
	DOCUMENT *document;
	char *post_action_string;
	char *prompt_html;
	char *title_string;
	FOLDER *folder;
	boolean forbid;
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form;
	char *html;
} GENERIC_LOAD_FOLDER;

/* GENERIC_LOAD_FOLDER operations */
/* ------------------------------ */
GENERIC_LOAD_FOLDER *generic_load_folder_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			boolean menu_boolean,
			boolean frameset_menu_horizontal );

GENERIC_LOAD_FOLDER *generic_load_folder_calloc(
			void );

boolean generic_load_folder_forbid(
			boolean role_folder_insert );

char *generic_load_folder_post_action_string(
			char *generic_load_folder_post_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name );

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
			char *document_body_html,
			char *generic_load_folder_form_html,
			char *document_body_close_html,
			char *document_close_html );

#endif

