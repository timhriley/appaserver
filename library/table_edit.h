/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/table_edit.h				*/
/* -------------------------------------------------------------------- */
/* No warrancy and freely available software. Visit appaserver.org	*/
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
#include "row_security_role_update.h"
#include "query.h"
#include "session.h"
#include "relation_join.h"
#include "form_table_edit.h"
#include "document.h"
#include "drillthru.h"
#include "dictionary_separate.h"
#include "date_convert.h"

#define TABLE_EDIT_EXECUTABLE			"output_table_edit"
#define TABLE_EDIT_FORCE_DROP_DOWN_ROW_COUNT	3
#define TABLE_EDIT_QUERY_MAX_ROWS		500
#define TABLE_EDIT_ONLOAD_JAVASCRIPT_KEY	"onload_javascript"

typedef struct
{
	LIST *role_folder_list;
	ROLE *role;
	LIST *exclude_lookup_attribute_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_operation_list;
	char *table_edit_state;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate;
	char *onload_javascript_string;
	LIST *relation_one2m_join_list;
	boolean menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list;
	DRILLTHRU_STATUS *drillthru_status;
	int max_foreign_key_list_length;
	LIST *viewonly_attribute_name_list;
	enum date_convert_format_enum date_convert_login_name_enum;
} TABLE_EDIT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TABLE_EDIT_INPUT *table_edit_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		boolean application_menu_horizontal_boolean,
		DICTIONARY *original_post_dictionary,
		char *data_directory,
		char *drilldown_base_folder_name,
		boolean viewonly_boolean,
		boolean omit_delete_boolean );

/* Process */
/* ------- */
TABLE_EDIT_INPUT *table_edit_input_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *table_edit_input_onload_javascript_string(
		const char *table_edit_onload_javascript_key,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */
LIST *table_edit_input_trim_delete_operation_list(
		LIST *folder_operation_list );

/* Usage */
/* ----- */
void table_edit_input_isa_delete_prompt_set(
		char *folder_name,
		LIST *folder_operation_list /* in/out */ );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *table_edit_input_isa_delete_prompt(
		char *folder_name );

/* Usage */
/* ----- */
LIST *table_edit_input_viewonly_attribute_name_list(
		LIST *role_attribute_exclude_list,
		LIST *relation_mto1_list,
		LIST *folder_attribute_append_isa_list,
		int table_edit_input_max_foreign_key_list_length,
		boolean drillthru_status_skipped_boolean );

/* Usage */
/* ----- */
boolean table_edit_input_relation_not_used(
		RELATION_MTO1 *relation_mto1,
		LIST *relation_mto1_foreign_key_less_equal_list );

/* Usage */
/* ----- */
int table_edit_input_max_foreign_key_list_length(
		boolean drillthru_status_skipped_boolean,
		char *drilldown_base_folder_name );

typedef struct
{
	char *title_string;
	int folder_attribute_calendar_date_name_list_length;
	char *document_head_calendar_setup_string;
	LIST *javascript_filename_list;
	char *javascript_include_string;
	char *javascript_replace;
	DOCUMENT *document;
	LIST *row_security_ajax_client_list;
	char *ajax_client_list_javascript;
	char *html;
	char *trailer_html;
} TABLE_EDIT_DOCUMENT;

/* Safely returns */
/* -------------- */
TABLE_EDIT_DOCUMENT *table_edit_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *table_edit_state,
		LIST *folder_attribute_append_isa_list,
		char *table_edit_input_onload_onload_javascript_string,
		char *results_string,
		char *error_string,
		char *javascript_filename,
		boolean sort_buttons_boolean,
		char *post_change_javascript,
		MENU *menu,
		ROW_SECURITY *row_security,
		char *table_edit_document_notepad );

/* Process */
/* ------- */
TABLE_EDIT_DOCUMENT *table_edit_document_calloc(
		void );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *table_edit_document_title_string(
		char *folder_name,
		char *table_edit_state );

LIST *table_edit_document_javascript_filename_list(
		char *folder_javascript_filename,
		boolean form_table_edit_sort_buttons_boolean );

/* Returns heap memory */
/* ------------------- */
char *table_edit_document_html(
		char *document_html,
		char *document_head_html,
		char *document_head_close_html,
		char *document_body_html,
		char *ajax_client_list_javascript );

/* Returns heap memory */
/* ------------------- */
char *table_edit_document_trailer_html(
		char *document_body_close_html,
		char *document_close_html );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *table_edit_document_javascript_replace(
		const char *appaserver_update_state,
		char *table_edit_input_onload_javascript_string,
		char *post_change_javascript );

typedef struct
{
	TABLE_EDIT_INPUT *table_edit_input;
	QUERY_TABLE_EDIT *query_table_edit;
	int query_row_list_length;
	ROW_SECURITY *row_security;
	char *post_action_string;
	LIST *heading_label_list;
	LIST *heading_name_list;
	FORM_TABLE_EDIT *form_table_edit;
	char *appaserver_update_filespecification;
	LIST *regular_widget_container_list;
	LIST *viewonly_widget_container_list;
	char *document_notepad;
	TABLE_EDIT_DOCUMENT *table_edit_document;
	RELATION_JOIN *relation_join;
} TABLE_EDIT;

/* Usage */
/* ----- */
TABLE_EDIT *table_edit_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		char *results_string,
		char *error_string,
		boolean application_menu_horizontal_boolean,
		DICTIONARY *original_post_dictionary,
		char *data_directory,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		LIST *drilldown_relation_foreign_key_list,
		boolean viewonly_boolean,
		boolean omit_delete_boolean );

/* Process */
/* ------- */
TABLE_EDIT *table_edit_calloc(
		void );

int table_edit_query_row_list_length(
		LIST *query_fetch_row_list );

/* Returns heap memory */
/* ------------------- */
char *table_edit_post_action_string(
		char *post_table_edit_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		LIST *drilldown_relation_foreign_key_list );

LIST *table_edit_heading_name_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list );

/* Returns folder->notepad or null */
/* ------------------------------- */
char *table_edit_document_notepad(
		const char *frameset_prompt_frame,
		char *target_frame,
		char *folder_notepad );

/* Usage */
/* ----- */
void table_edit_spool_file_output(
		char *appaserver_update_filespecification,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
FILE *table_edit_spool_file_open(
		char *appaserver_update_filespecification );

/* Usage */
/* ----- */
void table_edit_spool_file_cell_list_output(
		FILE *table_edit_spool_file_open,
		int line_number,
		LIST *cell_list );

/* Usage */
/* ----- */
void table_edit_output(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		char *document_html,
		char *form_table_edit_open_html,
		char *form_table_edit_heading_container_string,
		LIST *folder_operation_list,
		LIST *query_fetch_row_list,
		RELATION_JOIN *relation_join,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		char *table_edit_state,
		char *form_table_edit_close_html,
		char *document_trailer_html );

/* Usage */
/* ----- */
void table_edit_output_row_list(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		char *form_table_edit_heading_container_string,
		LIST *folder_operation_list,
		LIST *query_fetch_row_list,
		RELATION_JOIN *relation_join,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		char *table_edit_state );

/* Usage */
/* ----- */
LIST *table_edit_apply_widget_container_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		boolean table_edit_row_viewonly );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *table_edit_row_html(
		LIST *row_operation_list,
		char *table_edit_state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *table_edit_apply_widget_container_list,
		boolean table_edit_row_viewonly_boolean,
		char *form_background_color );

/* Process */
/* ------- */
boolean table_edit_delete_mask_boolean(
		boolean operation_delete_boolean,
		boolean table_edit_row_viewonly_boolean );

/* Usage */
/* ----- */
void table_edit_output_row_list_hidden(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		LIST *query_fetch_row_list,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list );

/* Usage */
/* ----- */
boolean table_edit_row_viewonly_boolean(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		LIST *query_row_cell_list,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list );

/* Usage */
/* ----- */

/* Returns program memory or null */
/* ------------------------------ */
char *table_edit_state(
		char *folder_name,
		boolean viewonly_boolean,
		LIST *role_folder_list );

/* Usage */
/* ----- */
LIST *table_edit_heading_label_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			row_security_regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			row_security_viewonly_widget_list );

/* Usage */
/* ----- */
LIST *table_edit_heading_name_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			row_security_regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			row_security_viewonly_widget_list );

#endif
