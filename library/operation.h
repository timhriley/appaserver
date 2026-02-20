/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/operation.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_H
#define OPERATION_H

#include <sys/types.h>
#include <unistd.h>
#include "process.h"
#include "boolean.h"
#include "list.h"
#include "widget.h"
#include "dictionary.h"

#define OPERATION_SELECT		"operation,output_yn"
#define OPERATION_TABLE			"operation"
#define OPERATION_ROW_COUNT_LABEL	"operation_row_count"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"

#define OPERATION_DELETE_WARNING_JAVASCRIPT \
					"table_edit_delete_button_warning()"

typedef struct
{
	boolean output_boolean;
	int count;
	LIST *primary_key_data_list;
	char *command_line;
} OPERATION_ROW_CHECKED;

/* Usage */
/* ----- */
OPERATION_ROW_CHECKED *operation_row_checked_new(
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *primary_key_list,
		char *post_table_edit_update_results_string,
		char *post_table_edit_update_error_string,
		int dictionary_key_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		DICTIONARY *dictionary_single_row,
		char *operation_name,
		char *process_command_line,
		boolean output_boolean,
		char *appaserver_error_filename );

/* Process */
/* ------- */
OPERATION_ROW_CHECKED *operation_row_checked_calloc(
		void );

/* Usage */
/* ----- */
boolean operation_row_checked_boolean(
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		char *operation_name );

/* Usage */
/* ----- */
int operation_row_checked_count(
		int dictionary_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		char *operation_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *operation_row_checked_key(
		int row_number,
		char *operation_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *operation_row_checked_command_line(
		const char attribute_multi_key_delimiter,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		char *update_results_string,
		char *update_error_string,
		DICTIONARY *operation_row_list_dictionary,
		DICTIONARY *dictionary_single_row,
		char *operation_name,
		char *process_command_line,
		char *appaserver_error_filename,
		int operation_row_checked_count,
		pid_t parent_process_id,
		LIST *primary_key_data_list );

/* Usage */
/* ----- */

/* Returns operation_error_message or null */
/* --------------------------------------- */
char *operation_row_checked_execute(
		char *command_line,
		boolean operation_row_list_output_boolean );

typedef struct
{
	LIST *operation_row_checked_list;
	char *appaserver_error_filename;
} OPERATION_ROW;

/* Usage */
/* ----- */
OPERATION_ROW *operation_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *folder_operation_list,
		LIST *primary_key_list,
		char *update_results_string,
		char *update_error_string,
		int dictionary_key_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		DICTIONARY *dictionary_single_row );

/* Process */
/* ------- */
OPERATION_ROW *operation_row_calloc(
		void );

typedef struct
{
	int dictionary_highest_index;
	DICTIONARY *dictionary;
	LIST *list;
	boolean output_boolean;
} OPERATION_ROW_LIST;

/* Usage */
/* ----- */
OPERATION_ROW_LIST *operation_row_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *folder_operation_list,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *operation_dictionary,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *drillthru_dictionary,
		char *update_results_string,
		char *update_error_string );

/* Process */
/* ------- */
OPERATION_ROW_LIST *operation_row_list_calloc(
		void );

/* Returns a combination of all dictionaries */
/* ----------------------------------------- */
DICTIONARY *operation_row_list_dictionary(
		char *dictionary_separate_no_display_prefix,
		char *dictionary_separate_drillthru_prefix,
		DICTIONARY *operation_dictionary,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *drillthru_dictionary );

boolean operation_row_list_output_boolean(
		LIST *operation_row_list );

/* Usage */
/* ----- */

/* Returns operation_error_message_list_string or null */
/* --------------------------------------------------- */
char *operation_row_list_execute(
		OPERATION_ROW_LIST *operation_row_list );

typedef struct
{
	char *operation_name;
	boolean output_boolean;
	PROCESS *process;
	boolean delete_boolean;
	boolean drilldown_boolean;
	char *image_source;
	char *delete_warning_javascript;
	WIDGET_CONTAINER *widget_container;
} OPERATION;

/* Usage */
/* ----- */
OPERATION *operation_fetch(
		char *operation_name,
		boolean fetch_process );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *operation_primary_where(
			char *operation_name );

/* Returns static memory */
/* --------------------- */
char *operation_system_string(
			char *operation_select,
			char *operation_table,
			char *operation_primary_where );

/* Usage */
/* ----- */
OPERATION *operation_parse(
		char *input,
		boolean fetch_process );

/* Process */
/* ------- */
OPERATION *operation_calloc(
		void );

boolean operation_output_boolean(
		char *output_yn );

boolean operation_delete_boolean(
		char *operation_name );

boolean operation_drilldownl_boolean(
		char *operation_name );

/* Returns heap memory or null */
/* --------------------------- */
char *operation_image_source(
		boolean operation_delete_boolean,
		boolean operation_drilldown_boolean );

/* Returns delete_warning_javascript or null */
/* ----------------------------------------- */
char *operation_delete_warning_javascript(
		const char *delete_warning_javascript,
		boolean operation_delete_boolean );

WIDGET_CONTAINER *operation_widget_container(
		char *operation_name,
		char *operation_image_source,
		char *operation_delete_warning_javascript );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *operation_html(
		WIDGET_CONTAINER *widget_container,
		char *state,
		int row_number,
		char *background_color,
		boolean delete_mask_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
OPERATION *operation_blank_new(
		void );

#endif

