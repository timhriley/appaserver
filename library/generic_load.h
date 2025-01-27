/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/generic_load.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef GENERIC_LOAD_H
#define GENERIC_LOAD_H

#include <stdio.h>
#include "list.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "widget.h"
#include "folder.h"
#include "session.h"
#include "folder_attribute.h"
#include "post_dictionary.h"
#include "dictionary.h"
#include "relation_mto1.h"
#include "folder_row_level_restriction.h"
#include "appaserver_parameter.h"
#include "form_generic_load.h"

#define GENERIC_LOAD_SKIP_HEADER_ROWS_LABEL	"skip_header_rows"
#define GENERIC_LOAD_EXECUTE_YN_LABEL		"execute_yn"
#define GENERIC_LOAD_FILENAME_LABEL 		"generic_load_filename"
#define GENERIC_LOAD_POSITION_PREFIX		"position_"
#define GENERIC_LOAD_CONSTANT_PREFIX		"constant_"
#define GENERIC_LOAD_IGNORE_PREFIX		"ignore_"
#define GENERIC_LOAD_FORM_NAME			"generic_load"
#define GENERIC_LOAD_FOLDER_EXECUTABLE		"post_generic_load_choose"
#define GENERIC_LOAD_INSERT_EXECUTABLE		"post_generic_load_folder"

typedef struct
{
	/* Stub */
} GENERIC_LOAD;

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *generic_load_hypertext_reference(
		char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name );

typedef struct
{
	char *one_folder_name;
	LIST *relation_foreign_key_list;
	char *folder_table_name;
	LIST *foreign_delimited_list;
} GENERIC_LOAD_RELATION;

/* Usage */
/* ----- */
GENERIC_LOAD_RELATION *generic_load_relation_new(
		char *application_name,
		char *one_folder_name,
		LIST *relation_foreign_key_list );

/* Process */
/* ------- */
GENERIC_LOAD_RELATION *generic_load_relation_calloc(
		void );

/* Usage */
/* ----- */
void generic_load_relation_set(
		GENERIC_LOAD_RELATION *
			generic_load_relation /* in/out */,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *input );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *generic_load_relation_data_delimited(
		char sql_delimiter,
		LIST *foreign_data_list );

typedef struct
{
	LIST *list;
} GENERIC_LOAD_RELATION_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GENERIC_LOAD_RELATION_LIST *generic_load_relation_list_new(
		char *application_name,
		LIST *relation_mto1_list );

/* Process */
/* ------- */
GENERIC_LOAD_RELATION_LIST *generic_load_relation_list_calloc(
		void );

/* Usage */
/* ----- */
void generic_load_relation_list_set(
		GENERIC_LOAD_RELATION_LIST *
			generic_load_relation_list /* in/out */,
		char *load_filename,
		int skip_header_rows_integer,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list );

typedef struct
{
	GENERIC_LOAD_RELATION *generic_load_relation;
	LIST *foreign_delimited_list;
} GENERIC_LOAD_ORPHAN;

/* Usage */
/* ----- */
GENERIC_LOAD_ORPHAN *generic_load_orphan_new(
		GENERIC_LOAD_RELATION *generic_load_relation );

/* Process */
/* ------- */
GENERIC_LOAD_ORPHAN *generic_load_orphan_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *generic_load_orphan_input_system(
		char *generic_load_orphan_select_statement );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *generic_load_orphan_select_statement(
		char *folder_table_name,
		LIST *relation_foreign_key_list,
		LIST *foreign_data_list );

/* Usage */
/* ----- */
void generic_load_orphan_output(
		GENERIC_LOAD_ORPHAN *generic_load_orphan );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *generic_load_orphan_title_string(
		char *one_folder_name );

/* Returns static memory */
/* --------------------- */
char *generic_load_orphan_heading_string(
		LIST *relation_foreign_key_list );

/* Returns static memory */
/* --------------------- */
char *generic_load_orphan_output_system(
		char sql_delimiter,
		char *generic_load_orphan_title_string,
		char *generic_load_orphan_heading_string );

FILE *generic_load_orphan_output_pipe(
		char *generic_load_orphan_output_system );

typedef struct
{
	LIST *list;
} GENERIC_LOAD_ORPHAN_LIST;

/* Usage */
/* ----- */
GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list_new(
		GENERIC_LOAD_RELATION_LIST *
			generic_load_relation_list );

/* Process */
/* ------- */
GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list_calloc(
		void );

/* Usage */
/* ----- */
void generic_load_orphan_list_output(
		GENERIC_LOAD_ORPHAN_LIST *
			generic_load_orphan_list );

typedef struct
{
	char *load_filename;
	int skip_header_rows_integer;
	boolean execute;
	DICTIONARY *position_dictionary;
	DICTIONARY *constant_dictionary;
} GENERIC_LOAD_INPUT;

/* Usage */
/* ----- */
GENERIC_LOAD_INPUT *generic_load_input_new(
		char *generic_load_filename_label,
		char *generic_load_skip_header_rows_label,
		char *generic_load_execute_yn_label,
		char *generic_load_position_prefix,
		char *generic_load_constant_prefix,
		DICTIONARY *non_prefixed_dictionary );

/* Process */
/* ------- */
GENERIC_LOAD_INPUT *generic_load_input_calloc(
		void );

typedef struct
{
	SESSION_PROCESS *session_process;
	char *folder_name;
	LIST *role_folder_list;
	char *sub_title_string;
	LIST *upload_filename_list;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	GENERIC_LOAD_INPUT *generic_load_input;
	ROLE *role;
	LIST *role_attribute_exclude_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	GENERIC_LOAD_RELATION_LIST *generic_load_relation_list;
	GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list;
	char *folder_table_name;
	LIST *generic_load_row_list;
	char *system_string;
} GENERIC_LOAD_INSERT;

/* Usage */
/* ----- */
GENERIC_LOAD_INSERT *generic_load_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name );

/* Process */
/* ------- */
GENERIC_LOAD_INSERT *generic_load_insert_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *generic_load_insert_sub_title_string(
		char *folder_name );

LIST *generic_load_insert_upload_filename_list(
		void );

/* Returns program memory */
/* ---------------------- */
char *generic_load_insert_system_string(
		void );

typedef struct
{
	LIST *generic_load_attribute_list;
	char *insert_statement;
} GENERIC_LOAD_ROW;

/* Usage */
/* ----- */
LIST *generic_load_row_list(
		char *generic_load_input_load_filename,
		int skip_header_rows_integer,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *folder_table_name );

/* Process */
/* ------- */

/* Safely returns */
/* -------------- */
FILE *generic_load_row_input_file(
		char *generic_load_input_load_filename );

/* Usage */
/* ----- */
GENERIC_LOAD_ROW *generic_load_row_new(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *folder_table_name,
		char *input );

/* Process */
/* ------- */
GENERIC_LOAD_ROW *generic_load_row_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *generic_load_row_insert_statement(
		char *folder_table_name,
		char *generic_load_attribute_name_list_string,
		char *generic_load_attribute_value_list_string );

/* Usage */
/* ----- */
void generic_load_row_insert(
		LIST *generic_load_row_list,
		char *generic_load_insert_system_string );

/* Process */
/* ------- */
FILE *generic_load_row_output_pipe(
		char *system_string );

/* Usage */
/* ----- */
void generic_load_row_display(
		char *folder_name,
		LIST *generic_load_row_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *generic_load_row_title_string(
		char *folder_name );

/* Returns heap memory */
/* ------------------- */
char *generic_load_row_output_system(
		char delimiter,
		int html_table_queue_top_bottom,
		char *generic_load_row_title_string,
		char *generic_load_attribute_heading_string );

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *value;
} GENERIC_LOAD_ATTRIBUTE;

/* Usage */
/* ----- */
LIST *generic_load_attribute_list(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *input );

/* Usage */
/* ----- */
GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		char *input,
		FOLDER_ATTRIBUTE *folder_attribute );

/* Process */
/* ------- */
GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *generic_load_attribute_name_list_string(
		LIST *generic_load_attribute_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *generic_load_attribute_value_list_string(
		LIST *generic_load_attribute_list,
		boolean include_quotes );

/* Public */
/* ------ */
GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_seek(
		LIST *generic_load_attribute_list,
		char *attribute_name );

/* Returns static memory */
/* --------------------- */
char *generic_load_attribute_heading_string(
		LIST *generic_load_attribute_list );

typedef struct
{
	SESSION_PROCESS *session_process;
	boolean application_menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	LIST *role_folder_insert_list;
	char *title_string;
	DOCUMENT *document;
	LIST *widget_container_list;
	char *widget_container_list_html;
	char *document_form_html;
} GENERIC_LOAD_CHOOSE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GENERIC_LOAD_CHOOSE *generic_load_choose_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name );

/* Process */
/* ------- */
GENERIC_LOAD_CHOOSE *generic_load_choose_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *generic_load_choose_title_string(
		char *process_name );

/* Usage */
/* ----- */
LIST *generic_load_choose_widget_container_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		LIST *role_folder_insert_list );

/* Usage */
/* ----- */
WIDGET_CONTAINER *generic_load_choose_widget_container(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *generic_load_choose_prompt(
		char *folder_name );


typedef struct
{
	SESSION_PROCESS *session_process;
	char *folder_name;
	boolean application_menu_horizontal_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	LIST *role_folder_insert_list;
	char *generic_load_hypertext_reference;
	ROLE *role;
	LIST *role_attribute_exclude_name_list;
	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_key_list;
	LIST *relation_mto1_recursive_list;
	FOLDER_ROW_LEVEL_RESTRICTION *
		folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	FORM_GENERIC_LOAD *form_generic_load;
	char *application_title_string;
	char *title_string;
	LIST *javascript_module_list;
	char *javascript_include_string;
	int folder_attribute_calendar_date_name_list_length;
	char *document_head_calendar_setup_string;
	char *folder_notepad;
	DOCUMENT *document;
	char *document_form_html;
} GENERIC_LOAD_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GENERIC_LOAD_FOLDER *generic_load_folder_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name );

/* Process */
/* ------- */
GENERIC_LOAD_FOLDER *generic_load_folder_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *generic_load_folder_title_string(
		char *application_title_string,
		char *folder_name );

LIST *generic_load_folder_javascript_module_list(
		void );

#endif

