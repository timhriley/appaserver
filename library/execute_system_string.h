/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/execute_system_string.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef EXECUTE_SYSTEM_STRING_H
#define EXECUTE_SYSTEM_STRING_H

#include "boolean.h"

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_table_edit(
		const char *table_edit_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *results_string,
		char *error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_table_insert(
		const char *table_insert_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *results_string,
		char *error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_folder_isa(
		const char *choose_isa_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_prompt_insert(
		const char *prompt_insert_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_prompt_lookup(
		const char *prompt_lookup_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_statistic(
		const char *lookup_statistic_executable,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_group_count(
		const char *group_count_executable,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_spreadsheet(
		const char *lookup_spreadsheet_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_delete(
		const char *lookup_delete_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename,
		char *one_two_state );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_grace(
		const char *lookup_grace_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_google(
		const char *lookup_google_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_histogram(
		const char *lookup_histogram_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_lookup_sort(
		const char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_choose_role(
		const char *choose_role_executable,
		char *session_key,
		char *login_name,
		boolean application_menu_horizontal_boolean,
		char *frameset_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_menu(
		const char *menu_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizonal_boolean );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_frameset(
		const char *frameset_executable,
		char *session_key,
		char *login_name,
		char *post_login_output_pipe_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_drillthru(
		const char *drillthru_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_drilldown(
		const char *drilldown_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *target_frame,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		char *update_results_string,
		char *update_row_list_error_string,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_post_prompt_insert(
		const char *post_prompt_insert_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_separate_send_string,
		char *appaserver_error_filename );

/* Returns heap memory */
/* ------------------- */
char *execute_system_string_create_application(
		const char *application_create_executable,
		char *session_key,
		char *login_name,
		char *destination_application,
		char *application_title,
		char *appaserver_error_filename );

#endif
