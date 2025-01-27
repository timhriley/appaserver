/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/ajax.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef AJAX_H
#define AJAX_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "widget.h"
#include "relation_mto1.h"
#include "process.h"

#define AJAX_CLIENT_WINDOW_EXECUTABLE		"ajax_window.sh"
#define AJAX_CLIENT_FILL_LABEL			"Fill"

#define AJAX_SERVER_POST_EXECUTABLE		"post_ajax_window"

/* Synchronize with $APPASERVER_HOME/src_appaserver/ajax_window.sh */
/* --------------------------------------------------------------- */
#define AJAX_SERVER_DATA_DELIMITER		'&'
#define AJAX_SERVER_FORMAT_DELIMITER		'|'

typedef struct
{
	char *many_folder_name;
	char *form_folder_name;
	char *ajax_folder_name;
	char *form_widget_name;
	char *form_widget_id;
	char *fill_button_action_string;
	WIDGET_CONTAINER *drop_down_widget_container;
	WIDGET_CONTAINER *fill_button_widget_container;
	char *select_primary_key_string;
	char *popup_widget_name;
	LIST *widget_container_list;
} AJAX_CLIENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
AJAX_CLIENT *ajax_client_new(
		char *many_folder_name,
		char *form_folder_name,
		LIST *form_foreign_key_list,
		LIST *form_primary_key_list,
		char *ajax_folder_name,
		LIST *ajax_primary_key_list,
		LIST *ajax_foreign_key_list,
		boolean top_select_boolean );

/* Process */
/* ------- */
AJAX_CLIENT *ajax_client_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *ajax_client_form_widget_name(
		const char sql_delimiter,
		LIST *form_foreign_key_list );

/* Returns heap memory */
/* ------------------- */
char *ajax_client_form_widget_id(
		const char sql_delimiter,
		char *many_folder_name,
		LIST *form_foreign_key_list );

/* Returns heap memory */
/* ------------------- */
char *ajax_client_popup_widget_name(
		const char sql_delimiter,
		LIST *ajax_foreign_key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_client_fill_button_action_string(
		char *ajax_folder_name,
		char *ajax_client_form_widget_id );

/* Usage */

/* Returns heap memory */
/* ------------------- */
char *ajax_client_select_primary_key_string(
		LIST *ajax_primary_key_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *ajax_client_list_javascript(
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *ajax_client_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_client_execute_window_system_string(
		const char *ajax_client_window_executable,
		const char *ajax_server_post_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *form_folder_name,
		char *ajax_folder_name,
		char *popup_widget_name,
		char *select_primary_key_string );

/* Usage */
/* ----- */

/* Returns null if not participating */
/* --------------------------------- */
AJAX_CLIENT *ajax_client_relation_mto1_new(
		RELATION_MTO1 *relation_mto1,
		LIST *relation_mto1_to_one_list,
		boolean top_select_boolean );

typedef struct
{
	FOLDER *form_folder;
	LIST *relation_mto1_list;
	RELATION_MTO1 *relation_mto1_to_one;
	PROCESS *populate_drop_down_process;
	char *foreign_key_list_string;
	LIST *attribute_data_list;
	char *query_data_where;
	char *select_string;
	char *system_string;
	LIST *result_list;
	char *return_string;
} AJAX_SERVER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
AJAX_SERVER *ajax_server_new(
		char *application_name,
		char *form_folder_name,
		char *role_name,
		char *ajax_folder_name,
		char *value );

/* Process */
/* ------- */
AJAX_SERVER *ajax_server_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *ajax_server_foreign_key_list_string(
		const char sql_delimiter,
		LIST *foreign_key_list );

LIST *ajax_server_attribute_data_list(
		const char sql_delimiter,
		char *value );

/* Returns heap memory */
/* ------------------- */
char *ajax_server_select_string(
		LIST *folder_attribute_primary_key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_process_system_string(
		char *value,
		PROCESS *populate_drop_down_process,
		char *foreign_key_list_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_folder_system_string(
		char *application_name,
		char *form_folder_name,
		char *query_data_where,
		char *ajax_server_select_string );

/* Usage */
/* ----- */
LIST *ajax_server_result_list(
		char *system_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *ajax_server_return_string(
		const char sql_delimiter,
		const char *widget_drop_down_dash_delimiter,
		const char ajax_server_data_delimiter,
		const char ajax_server_format_delimiter,
		LIST *ajax_server_result_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_delimited_data_string(
		const char ajax_server_data_delimiter,
		LIST *ajax_server_result_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_delimited_format_string(
		const char ajax_server_data_delimiter,
		LIST *ajax_server_result_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_data_string(
		const char ajax_server_data_delimiter,
		LIST *ajax_server_result_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_list_format_string(
		const char sql_delimiter,
		const char ajax_server_data_delimiter,
		const char *widget_drop_down_dash_delimiter,
		LIST *ajax_server_result_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *ajax_server_format_string(
		const char *widget_drop_down_dash_delimiter,
		char *sql_delimiter_string,
		char *result );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *ajax_server_format_return_string(
		const char ajax_server_format_delimiter,
		char *data_string,
		char *format_string );

#endif
