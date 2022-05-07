/* $APPASERVER_HOME/library/process.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_H
#define PROCESS_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "javascript.h"

/* Constants */
/* --------- */
#define PROCESS_TABLE			"process"
#define PROCESS_SET_TABLE		"process_set"

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

#define PROCESS_SET_SELECT		"process_set,"			\
					"notepad,"			\
					"html_help_file_anchor,"	\
					"post_change_javascript,"	\
					"prompt_display_text,"		\
					"process_group,"		\
					"preprompt_help_text,"		\
					"prompt_display_bottom_yn,"	\
					"javascript_filename"

typedef struct
{
	/* Input */
	/* ----- */
	char *process_set_name;
	char *role_name;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *prompt_display_text;
	char *process_group;
	char *preprompt_help_text;
	boolean prompt_display_bottom;
	char *javascript_filename;

	/* Process */
	/* ------- */
	LIST *process_set_member_name_list;
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

typedef struct
{
	/* Input */
	/* ----- */
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
			char *document_root_directory,
			char *application_relative_source_directory,
			boolean check_executable_inside_filesystem );

/* Public */
/* ------ */
char *process_populate_drop_down_command_line(
			DICTIONARY *drillthru_dictionary,
			char *command_line,
			char *security_entity_where,
			char *state,
			char *login_name,
			char *role_name );

void process_set_one2m_folder_name_for_process(
			DICTIONARY *dictionary,
			char *one2m_folder_name );

boolean process_interpreted_executable_ok(
			char *which_string );

/* PROCESS command_line */
/* -------------------- */

/* Safely returns heap memory */
/* -------------------------- */
char *process_choose_isa_command_line(
			char *command_line /* in */,
			char *application_name,
			char *security_entity_where,
			char *login_name,
			char *role_name,
			char *one2m_isa_folder_name );

/* Safely returns heap memory */
/* -------------------------- */
char *process_prompt_submit_command_line(
			char *command_line,
			char *application_name,
			char *process_name,
			char *role_name,
			char *login_name,
			DICTIONARY *working_post_dictionary );

void process_replace_one_folder_command_line(
			/* -------------------- */
			/* Assumes stack memory */
			/* -------------------- */
			char *command_line,
			char *one_folder_name );

/* Returns heap memory */
/* ------------------- */
char *process_operation_command_line(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *dictionary_single_row,
			char *operation_name,
			char *command_line,
			pid_t parent_process_id,
			int operation_row_checked_count,
			LIST *primary_data_list );

/* Safely returns heap memory */
/* -------------------------- */
char *process_parameter_command_line(
			char *command_line,
			char *process_name,
			char *login_name,
			char *role_name,
			DICTIONARY *drillthru_dictionary );

/* Returns heap memory */
/* ------------------- */
char *process_widget_command_line(
			char *command_line,
			char *application_name,
			char *security_entity_where,
			char *one_folder_name,
			char *state,
			DICTIONARY *drillthru_dictionary );

void process_replace_where_command_line(
			char *command_line /* in/out */,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

void process_replace_state_command_line(
			/* -------------------- */
			/* Assumes stack memory */
			/* -------------------- */
			char *command_line,
			char *state );

/* PROCESS public */
/* -------------- */
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

