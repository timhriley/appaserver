/* $APPASERVER_HOME/library/process.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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

#define PROCESS_ONE_FOLDER_PLACEHOLDER	"$one_folder"
#define PROCESS_SESSION_PLACEHOLDER	"$session"
#define PROCESS_PID_PLACEHOLDER		"$process_id"
#define PROCESS_LOGIN_PLACEHOLDER	"$login"
#define PROCESS_ROLE_PLACEHOLDER	"$role"
#define PROCESS_FOLDER_PLACEHOLDER	"$folder"
#define PROCESS_NAME_PLACEHOLDER	"$process"
#define PROCESS_STATE_PLACEHOLDER	"$state"
#define PROCESS_PRIMARY_PLACEHOLDER	"$primary_data"
#define PROCESS_ROW_COUNT_PLACEHOLDER	"$operation_row_count"
#define PROCESS_DICTIONARY_PLACEHOLDER	"$dictionary"
#define PROCESS_WHERE_PLACEHOLDER	"$where"

#define PROCESS_SET_TABLE		"process_set"

#define PROCESS_SET_SELECT		"process_set,"			\
					"notepad,"			\
					"html_help_file_anchor,"	\
					"post_change_javascript,"	\
					"prompt_display_text,"		\
					"process_group,"		\
					"preprompt_help_text,"		\
					"javascript_filename"

#define PROCESS_SET_DEFAULT_PROMPT	"Process"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *process_set_name;
	char *role_name;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *prompt_display_text;
	char *process_group;
	char *preprompt_help_text;
	char *javascript_filename;

	/* Process */
	/* ------- */
	LIST *member_name_list;
	JAVASCRIPT *javascript;
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

char *process_set_system_string(
			char *process_set_select,
			char *process_set_table,
			char *where );

PROCESS_SET *process_set_parse(
			char *input,
			char *role_name,
			char *document_root_directory,
			char *application_relative_source_directory,
			boolean fetch_member_process_name_list );

PROCESS_SET *process_set_new(
			char *process_set_name );

PROCESS_SET *process_set_calloc(
			void );

LIST *process_set_member_name_list(
			char *process_set_primary_where,
			char *role_name );

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

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *process_name;
	char *role_name;
	char *command_line;
	char *notepad;
	char *html_help_file_anchor;
	int execution_count;
	char *post_change_javascript;
	char *process_set_display;
	char *process_group;
	char *preprompt_help_text;
	char *javascript_filename;

	/* Process */
	/* ------- */
	JAVASCRIPT *javascript;
} PROCESS;

/* Usage */
/* ----- */
PROCESS *process_fetch(
			char *process_name,
			char *document_root,
			char *application_relative_source_directory,
			boolean check_executable_inside_filesystem );

/* Process */
/* ------- */
PROCESS *process_new(	char *process_name );

/* Returns static memory */
/* --------------------- */
char *process_primary_where(
			char *process_name );

/* Returns heap memory */
/* ------------------- */
char *process_system_string(
			char *process_select,
			char *process_table,
			char *where );

PROCESS *process_parse(	char *input,
			char *document_root,
			char *application_relative_source_directory,
			boolean check_executable_inside_filesystem );

/* Private */
/* ------- */
boolean process_interpreted_executable_ok(
			char *which_string );

/* Public */
/* ------ */
void process_replace_one_folder_command_line(
			char *command_line,
			char *one_folder_name,
			char *process_one_folder_placeholder );

void process_replace_session_command_line(
			char *command_line,
			char *session_key,
			char *process_session_placeholder );

void process_replace_pid_command_line(
			char *command_line,
			pid_t process_id,
			char *process_pid_placeholder );

void process_replace_login_command_line(
			char *command_line,
			char *login_name,
			char *process_login_placeholder );

void process_replace_role_command_line(
			char *command_line,
			char *role_name,
			char *process_role_placeholder );

void process_replace_folder_command_line(
			char *command_line,
			char *folder_name,
			char *process_folder_placeholder );

void process_replace_name_command_line(
			char *command_line,
			char *process_name,
			char *process_name_placeholder );

void process_replace_state_command_line(
			char *command_line,
			char *state,
			char *process_state_placeholder );

void process_replace_primary_command_line(
			char *command_line,
			char *primary_data_string,
			char *process_primary_placeholder );

void process_replace_row_count_command_line(
			char *command_line,
			int operation_row_checked_count,
			char *process_row_count_placeholder );

void process_replace_dictionary_command_line(
			char *command_line,
			DICTIONARY *dictionary_single_row,
			char dictionary_delimiter,
			char *process_dictionary_placeholder );

void process_replace_where_command_line(
			char *command_line,
			char *where_string,
			char *process_where_placeholder );

boolean process_executable_ok(
			char *executable );

char *process_name_fetch(
			char *process_or_set_name );

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

#endif

