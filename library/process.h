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
#include "folder.h"
#include "attribute.h"

/* Constants */
/* --------- */
#define PROCESS_TABLE			"process"
#define PROCESS_SET_TABLE		"process_set"
#define PROMPT_TABLE			"prompt"
#define DROP_DOWN_PROMPT_TABLE		"drop_down_prompt"
#define DROP_DOWN_PROMPT_DATA_TABLE	"drop_down_prompt_data"
#define PROCESS_PARAMETER_TABLE		"process_parameter"
#define PROCESS_SET_PARAMETER_TABLE	"process_set_parameter"

#define PROCESS_ID_LABEL		"pprocess_id"

#define PROCESS_SELECT			"process,"			\
					"command_line",			|
					"notepad,"			\
					"html_help_file_anchor,"	\
					"execution_count,"		\
					"post_change_javascript,"	\
					"process_set_display,"		\
					"process_group,"		\
					"preprompt_help_text"

#define PROCESS_SET_SELECT		"process_set,"			\
					"notepad,"			\
					"html_help_file_anchor,"	\
					"post_change_javascript,"	\
					"prompt_display_text,"		\
					"process_group,"		\
					"preprompt_help_text,"		\
					"prompt_display_bottom_yn"

#define PROCESS_PARAMETER_SELECT	"process,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"preprompt_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_SET_PARAMETER_SELECT	"process_set,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"preprompt_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

typedef struct
{
	/* Input */
	/* ----- */
	char *drop_down_prompt_name;
	char *drop_down_prompt_data;
	int display_order;
} DROP_DOWN_PROMPT_DATA;

typedef struct
{
	/* Input */
	/* ----- */
	char *drop_down_prompt_name;
	char *hint_message;
	char *optional_display;

	/* Process */
	/* ------- */
	LIST *drop_down_prompt_data_list;
} DROP_DOWN_PROMPT;

typedef struct
{
	/* Input */
	/* ----- */
	char *prompt_name;
	int input_width;
	char *hint_message;
	boolean upload_filename;
	boolean date;
} PROMPT;

typedef struct
{
	/* Input */
	/* ----- */
	char *process_set_name;
	boolean check_executable_inside_filesystem;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *prompt_display_text;
	char *process_group;
	char *preprompt_help_text;
	boolean prompt_display_bottom;

	/* Process */
	/* ------- */
	LIST *process_set_role_process_name_list;
	char *process_set_process_where;
	LIST *process_list;

} PROCESS_SET;

typedef struct
{
	/* Input */
	/* ----- */
	char *process_name;
	boolean check_executable_inside_filesystem;
	char *command_line;
	char *notepad;
	char *html_help_file_anchor;
	int execution_count;
	char *post_change_javascript;
	char *process_set_display;
	char *process_group;
	char *preprompt_help_text;
} PROCESS;

typedef struct
{
	/* Input */
	/* ----- */
	char *process_or_set_name;
	DICTIONARY *preprompt_dictionary;
	char *login_name;
	char *folder_name;
	char *attribute_name;
	char *drop_down_prompt_name;
	char *prompt_name;

	/* Process */
	/* ------- */
	LIST *primary_delimited_list;
	ATTRIBUTE *attribute;
	DROP_DOWN_PROMPT *drop_down_prompt;
	PROMPT *prompt;
} PROCESS_PARAMETER;

typedef struct
{
	/* Input */
	char *process_or_process_set_name;
	char *role_name;
	char *login_name;
	boolean check_executable_inside_filesystem;
	boolean is_preprompt;
	DICTIONARY *preprompt_dictionary;

	/* Process */
	/* ------- */
	boolean process_structure_is_set;
	PROCESS *process;
	PROCESS_SET *process_set;
	LIST *process_parameter_list;
} PROCESS_STRUCTURE;

/* PROMPT operations */
/* ----------------- */
PROMPT *prompt_fetch(	char *prompt_name );

/* Returns static memory */
/* --------------------- */
char *prompt_primary_where(
			char *prompt_name );

/* Returns heap memory */
/* ------------------- */
char *prompt_system_string(
			char *where );

PROMPT *prompt_parse(	char *input );

PROMPT *prompt_new(	char *prompt_name );

/* DROP_DOWN_PROMPT_DATA */
/* ===================== */
DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_new(
			char *drop_down_prompt_name,
			char *drop_down_prompt_data );

DROP_DOWN_PROMPT_DATA *drop_down_prompt_data_parse(
			char *input );

/* Returns heap memory */
/* ------------------- */
char *drop_down_prompt_data_system_string(
			char *where );

LIST *drop_down_prompt_data_system_list(
			char *system_string );

/* DROP_DOWN_PROMPT */
/* ================ */
DROP_DOWN_PROMPT *drop_down_prompt_new(
			char *drop_down_prompt_name );

DROP_DOWN_PROMPT *drop_down_prompt_fetch(
			char *drop_down_prompt_name );

DROP_DOWN_PROMPT *drop_down_prompt_parse(
			char *input );

/* Returns static memory */
/* --------------------- */
char *drop_down_prompt_primary_where(
			char *drop_down_prompt_name );

/* Returns heap memory */
/* ------------------- */
char *drop_down_prompt_system_string(
			char *where );

/* PROCESS_PARAMETER */
/* ================= */
LIST *process_parameter_system_list(
			char *system_string,
			DICTIONARY *preprompt_dictionary,
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *process_parameter_system_string(
			char *where );

/* Returns heap memory */
/* ------------------- */
char *process_set_parameter_system_string(
			char *where );

PROCESS_PARAMETER *process_parameter_parse(
			char *input,
			DICTIONARY *preprompt_dictionary,
			char *login_name );

LIST *process_parameter_primary_delimited_list(
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *folder_name,
			char *populate_drop_down_process_name );

/* Frees command_line and returns heap memory */
/* ------------------------------------------ */
char *process_parameter_command_line_replace(
			char *command_line,
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *application_name );

/* PROCESS */
/* ======= */
PROCESS *process_fetch(
			char *process_name,
			char *role_name,
			boolean check_executable_inside_filesystem );

PROCESS *process_new(
			char *process_name );

/* Returns static memory */
/* --------------------- */
char *process_primary_where(
			char *process_name );

/* Returns heap memory */
/* ------------------- */
char *process_system_string(
			char *where );

PROCESS *process_parse(	char *input,
			char *role_name,
			boolean check_executable_inside_filesystem );

LIST *process_fetch_list(
			char *command_line );

/* PROCESS_SET operations */
/* ---------------------- */
PROCESS_SET *process_set_new(
			char *process_set_name );

PROCESS_SET *process_set_fetch(
			char *process_set_name,
			char *role_name,
			boolean check_executable_inside_filesystem );

/* Returns static memory */
/* --------------------- */
char *process_set_primary_where(
			char *process_set_name );

char *process_set_system_string(
			char *where );

PROCESS_SET *process_set_parse(
			char *input,
			char *role_name,
			boolean check_executable_inside_filesystem );

/* Returns heap memory */
/* ------------------- */
char *process_set_process_where(
			LIST *process_name_list );

/* GENERIC operations */
/* ------------------ */

/* Frees command_line and returns heap memory */
/* ------------------------------------------ */
char *process_update_row_command_line(
			char *command_line,
			DICTIONARY *post_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			int row,
			char *process_name,
			LIST *primary_data_list );

void process_search_replace_where(
			char *command_line /* in/out */,
			DICTIONARY *preprompt_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name );

void process_replace_parameter_variables(
			char *command_line /* in/out */,
			char *session,
			char *state,
			char *login_name,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *process_name,
			char *process_set_name,
			char *operation_row_count_string,
			char *one2m_folder_name,
			char *prompt,
			char *process_id_string,
			DICTIONARY *post_dictionary );

void process_execution_count_increment(
			char *process_name );

void process_convert_parameters(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			LIST *attribute_list,
			LIST *prompt_list,
			LIST *primary_key_list,
			LIST *primary_data_list,
			int row,
			char *process_name,
			PROCESS_SET *process_set,
			char *one2m_folder_name_for_process,
			char *operation_row_count_string,
			char *prompt );

char *process_dictionary_process_member(
			char *process_set,
			DICTIONARY *parsed_decoded_post_dictionary,
			char *from_starting_label );

boolean process_executable_ok(
			char *executable );

void process_set_one2m_folder_name_for_process(
			DICTIONARY *dictionary,
			char *one2m_folder_name );

void process_search_replace_where(
			char *command_line /* in/out */,
			char *folder_name,
			char *role_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary );

boolean process_interpreted_executable_ok(
			char *which_string );

void process_operation_convert(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			LIST *append_isa_attribute_list,
			LIST *primary_key_list,
			LIST *primary_data_list,
			int row,
			char *process_name,
			char *operation_row_count_string );

void process_prompt_convert_parameters(
			char **executable,
			char *application_name,
			char *session,
			char *state,
			char *person,
			char *folder_name,
			char *role_name,
			DICTIONARY *preprompt_dictionary,
			LIST *attribute_list,
			int row,
			char *process_name,
			char *one2m_folder_name_for_process );

#endif

