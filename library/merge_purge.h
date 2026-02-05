/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/merge_purge.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef MERGE_PURGE_H
#define MERGE_PURGE_H

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
#include "process.h"
#include "update.h"
#include "delete.h"
#include "appaserver_parameter.h"
#include "form_merge_purge.h"

#define MERGE_PURGE_KEEP_LABEL			"keep"

#define MERGE_PURGE_DELETE_LABEL		"delete"

#define MERGE_PURGE_CHOOSE_POST_EXECUTABLE	"post_merge_purge_choose"

#define MERGE_PURGE_FOLDER_POST_EXECUTABLE	"post_merge_purge_folder"

#define MERGE_PURGE_NO_UPDATE_TEMPLATE				\
	"<h3>Warning: nothing to merge in %s. Just delete the row.</h3>\n"

#define MERGE_PURGE_PROCESS_SAME_MESSAGE			\
	"<h3>Warning: please select different values.</h3>\n"

#define MERGE_PURGE_NO_DELETE_PERMISSION_TEMPLATE		\
	"<h3>ERROR: delete permission not set for %s.</h3>\n"

#define MERGE_PURGE_ALREADY_OCCURRED_TEMPLATE			\
	"<h3>Warning: purge already occurred for this %s.</h3>\n"

typedef struct
{
	/* Stub */
} MERGE_PURGE;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *merge_purge_hypertext_reference(
		char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name );

typedef struct
{
	SESSION_PROCESS *session_process;
	char *folder_name;
	LIST *role_folder_list;
	FOLDER *folder;
	boolean application_menu_horizontal_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *merge_purge_hypertext_reference;
	ROLE *role;
	char *keep_prompt;
	char *delete_prompt;
	FORM_MERGE_PURGE *form_merge_purge;
	char *application_title_string;
	char *title_string;
	DOCUMENT *document;
	char *document_form_html;
} MERGE_PURGE_FOLDER;

/* Usage */
/* ----- */
MERGE_PURGE_FOLDER *merge_purge_folder_new(
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
MERGE_PURGE_FOLDER *merge_purge_folder_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *merge_purge_folder_keep_prompt(
		char *folder_name );

/* Returns static memory */
/* --------------------- */
char *merge_purge_folder_delete_prompt(
		char *folder_name );

/* Returns static memory */
/* --------------------- */
char *merge_purge_folder_title_string(
		char *process_name,
		char *folder_name );

typedef struct
{
	DICTIONARY *multi_row_dictionary;
	DICTIONARY *file_dictionary;
	UPDATE *update;
} MERGE_PURGE_UPDATE;

/* Usage */
/* ----- */
MERGE_PURGE_UPDATE *merge_purge_update_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *keep_string_list,
		LIST *delete_string_list,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_primary_list );

/* Process */
/* ------- */
MERGE_PURGE_UPDATE *merge_purge_update_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY *merge_purge_update_dictionary(
		LIST *string_list,
		LIST *folder_attribute_primary_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *merge_purge_update_key(
		char *attribute_name );

typedef struct
{
	LIST *delete_primary_query_cell_list;
	DELETE *delete;
} MERGE_PURGE_DELETE;

/* Usage */
/* ----- */
MERGE_PURGE_DELETE *merge_purge_delete_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *delete_delimited_string,
		LIST *folder_attribute_primary_key_list );

/* Process */
/* ------- */
MERGE_PURGE_DELETE *merge_purge_delete_calloc(
		void );

typedef struct
{
	SESSION_PROCESS *session_process;
	char *title_string;
	char *sub_sub_title_string;
	PROCESS *process;
	char *document_body_html;
	char *role_folder_where;
	LIST *role_folder_name_list;
	LIST *widget_container_list;
	char *widget_container_list_html;
	char *document_form_html;
} MERGE_PURGE_CHOOSE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MERGE_PURGE_CHOOSE *merge_purge_choose_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name );

/* Process */
/* ------- */
MERGE_PURGE_CHOOSE *merge_purge_choose_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *merge_purge_choose_sub_sub_title_string(
		void );

/* Returns static memory */
/* --------------------- */
char *merge_purge_choose_role_folder_where(
		const char *role_folder_table,
		const char *relation_table,
		const char *appaserver_update_state,
		char *role_name );

LIST *merge_purge_choose_role_folder_name_list(
		char *role_folder_table,
		char *merge_purge_choose_role_folder_where );

/* Returns static memory */
/* --------------------- */
char *merge_purge_choose_title_string(
		char *process_name );

/* Usage */
/* ----- */
LIST *merge_purge_choose_widget_container_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		LIST *merge_purge_choose_role_folder_name_list );

/* Usage */
/* ----- */
WIDGET_CONTAINER *merge_purge_choose_widget_container(
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
char *merge_purge_choose_prompt(
		char *folder_name );

typedef struct
{
	SESSION_PROCESS *session_process;
	char *folder_name;
	LIST *role_folder_list;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	char *keep_delimited_string;
	char *delete_delimited_string;
	boolean same_boolean;
	LIST *keep_string_list;
	LIST *delete_string_list;
	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_list;
	LIST *folder_attribute_primary_key_list;
	LIST *relation_one2m_list;
	LIST *relation_mto1_isa_list;
	boolean execute_boolean;
	MERGE_PURGE_UPDATE *merge_purge_update;
	MERGE_PURGE_DELETE *merge_purge_delete;
	boolean row_exists_boolean;
} MERGE_PURGE_PROCESS;

/* Usage */
/* ----- */
MERGE_PURGE_PROCESS *merge_purge_process_new(
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
MERGE_PURGE_PROCESS *merge_purge_process_calloc(
		void );

boolean merge_purge_process_same_boolean(
		char *keep_delimited_string,
		char *delete_delimited_string );

LIST *merge_purge_process_keep_string_list(
		char sql_delimiter,
		char *keep_delimited_string );

LIST *merge_purge_process_delete_string_list(
		char sql_delimiter,
		char *delete_delimited_string );

/* Usage */
/* ----- */
boolean merge_purge_process_execute_boolean(
		const char *appaserver_execute_yn,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *merge_purge_process_exists_system_string(
		char *folder_name,
		char *delete_where );

/* Usage */
/* ----- */
boolean merge_purge_process_row_exists_boolean(
		char *merge_purge_process_exists_system_string );

/* Usage */
/* ----- */
void merge_purge_process_execute(
		const char *sql_executable,
		char *appliction_name,
		MERGE_PURGE_UPDATE *merge_purge_update,
		MERGE_PURGE_DELETE *merge_purge_delete );

/* Usage */
/* ----- */
void merge_purge_process_html_display(
		char *delete_delimited_string,
		UPDATE *update,
		DELETE *delete );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *merge_purge_process_heading_html(
		const char *widget_drop_down_dash_delimiter,
		const char sql_delimiter,
		char *delete_delimited_string );

#endif

