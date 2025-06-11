/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "javascript.h"

#define PROCESS_APPLICATION_PLACEHOLDER		"$application"
#define PROCESS_MANY_PLACEHOLDER		"$many"
#define PROCESS_MANY_TABLE_PLACEHOLDER		"$many_table"
#define PROCESS_ONE_PLACEHOLDER1		"$one"
#define PROCESS_APPLICATION_PLACEHOLDER		"$application"
#define PROCESS_SESSION_PLACEHOLDER		"$session"
#define PROCESS_PID_PLACEHOLDER			"$process_id"
#define PROCESS_LOGIN_PLACEHOLDER		"$login"
#define PROCESS_LOGIN_NAME_PLACEHOLDER		"$login_name"
#define PROCESS_ROLE_PLACEHOLDER		"$role"
#define PROCESS_FOLDER_PLACEHOLDER		"$folder"
#define PROCESS_TABLE_PLACEHOLDER		"$table"
#define PROCESS_NAME_PLACEHOLDER		"$process"
#define PROCESS_STATE_PLACEHOLDER		"$state"
#define PROCESS_UPDATE_RESULTS_PLACEHOLDER	"$update_results"
#define PROCESS_UPDATE_ERROR_PLACEHOLDER	"$update_error"
#define PROCESS_TARGET_FRAME_PLACEHOLDER	"$target_frame"
#define PROCESS_PRIMARY_PLACEHOLDER		"$primary_data_list"
#define PROCESS_ROW_COUNT_PLACEHOLDER		"$operation_row_count"
#define PROCESS_DICTIONARY_PLACEHOLDER		"$dictionary"
#define PROCESS_WHERE_PLACEHOLDER		"$where"
#define PROCESS_REALLY_PLACEHOLDER		"really_yn"
#define PROCESS_FOLDER_NAME_PLACEHOLDER		"folder"
#define PROCESS_TABLE_NAME_PLACEHOLDER		"table_name"
#define PROCESS_ATTRIBUTE_PLACEHOLDER		"attribute"
#define PROCESS_COLUMN_PLACEHOLDER		"column_name"

#define PROCESS_TABLE			"process"

#define PROCESS_SELECT			"process,"			\
					"command_line,"			\
					"notepad,"			\
					"html_help_file_anchor,"	\
					"execution_count,"		\
					"post_change_javascript,"	\
					"process_set_display,"		\
					"process_group,"		\
					"preprompt_help_text,"		\
					"javascript_filename"

#define PROCESS_SET_SELECT		"notepad,"			\
					"html_help_file_anchor,"	\
					"post_change_javascript,"	\
					"prompt_display_text,"		\
					"process_group,"		\
					"preprompt_help_text,"		\
					"javascript_filename"

#define PROCESS_SET_TABLE		"process_set"

#define PROCESS_SET_DEFAULT_PROMPT	"Process"

typedef struct
{
	char *process_set_name;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *prompt_display_text;
	char *process_group;
	char *preprompt_help_text;
	char *javascript_filename;
	JAVASCRIPT *javascript;
	LIST *member_name_list;
} PROCESS_SET;

/* Usage */
/* ----- */
PROCESS_SET *process_set_fetch(
		char *process_set_name,
		char *role_name,
		char *document_root_directory,
		char *application_relative_source_directory,
		boolean fetch_process_set_member_name_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_set_primary_where(
		char *process_set_name );

/* Returns heap memory */
/* ------------------- */
char *process_set_system_string(
		char *process_set_select,
		char *process_set_table,
		char *where );

/* Usage */
/* ----- */
PROCESS_SET *process_set_parse(
		char *process_set_name,
		char *role_name,
		char *document_root_directory,
		char *application_relative_source_directory,
		boolean fetch_process_set_member_name_list,
		char *input );

/* Process */
/* ------- */
LIST *process_set_member_name_list(
		char *role_process_set_member_table,
		char *process_set_primary_where,
		char *role_name );

/* Usage */
/* ----- */
PROCESS_SET *process_set_new(
		char *process_set_name );

/* Process */
/* ------- */
PROCESS_SET *process_set_calloc(
		void );

/* Usage */
/* ----- */
char *process_set_process_where(
		LIST *process_set_member_name_list );

typedef struct
{
	char *process_name;
	char *command_line;
	char *notepad;
	char *html_help_file_anchor;
	int execution_count;
	char *post_change_javascript;
	char *process_set_display;
	char *process_group;
	char *preprompt_help_text;
	char *javascript_filename;
	JAVASCRIPT *javascript;
} PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS *process_fetch(
		char *process_name,
		/* ---------------------------------------- */
		/* If fetching process->javascript_filename */
		/* ---------------------------------------- */
		char *document_root,
		char *application_relative_source_directory,
		/* ----------------------- */
		/* If preparing to execute */
		/* ----------------------- */
		boolean check_executable_inside_filesystem,
		char *appaserver_parameter_mount_point );

/* Usage */
/* ----- */
LIST *process_fetch_list(
		void );

/* Usage */
/* ----- */
LIST *process_system_list(
		char *appaserver_system_string );

/* Usage */
/* ----- */
PROCESS *process_parse(
		char *input );

/* Usage */
/* ----- */
PROCESS *process_new(
		char *process_name );

/* Process */
/* ------- */
PROCESS *process_calloc(
		void );

/* Usage */
/* ----- */
PROCESS *process_seek(
		char *process_name,
		LIST *process_list );

/* Usage */
/* ----- */
boolean process_executable_okay(
		char *mount_point,
		char *command_line );

/* Usage */
/* ----- */
char *process_name_fetch(
		char *process_or_set_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_primary_where(
		char *process_name );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
DICTIONARY *process_command_line_dictionary(
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary );

/* Usage */
/* ----- */

/* Returns component of static process_list */
/* ---------------------------------------- */
char *process_command_line_fetch(
		char *process_name );

/* Public */
/* ------ */
void process_replace_pid_command_line(
		char *command_line,
		pid_t process_id,
		const char *process_pid_placeholder );

void process_replace_row_count_command_line(
		char *command_line,
		int operation_row_checked_count,
		const char *process_row_count_placeholder );

void process_replace_dictionary_command_line(
		char *command_line,
		DICTIONARY *dictionary_single_row,
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		const char *process_dictionary_placeholder );

char *process_set_name_fetch(
		char *process_or_set_name );

char *process_dictionary_process_member(
		char *process_set_name,
		DICTIONARY *parsed_decoded_post_dictionary,
		char *from_starting_label );

LIST *process_role_process_name_list(
		char *role_primary_where );

LIST *process_delimited_list(
		char *command_line );

void process_execution_count_increment(
		char *process_name );

void process_increment_count(
		char *process_name );

/* Returns static memory */
/* --------------------- */
char *process_title_heading_tag(
		char *process_name );

#endif

