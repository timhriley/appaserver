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
					"command_line",			\
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

#define PROCESS_PARAMETER_SELECT	"process,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
					"populate_drop_down_process,"	\
					"populate_helper_process"

#define PROCESS_SET_PARAMETER_SELECT	"process_set,"			\
					"folder,"			\
					"attribute,"			\
					"drop_down_prompt,"		\
					"prompt,"			\
					"display_order,"		\
					"drop_down_multi_select_yn,"	\
					"drillthru_yn,"			\
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

typedef struct
{
	/* Input */
	/* ----- */
	char *process_name;
	boolean check_executable_inside_filesystem;
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

typedef struct
{
	/* Input */
	/* ----- */
	char *process_or_set_name;
	DICTIONARY *drillthru_dictionary;
	char *login_name;
	char *folder_name;
	char *attribute_name;
	char *drop_down_prompt_name;
	char *prompt_name;

	/* Process */
	/* ------- */
	int display_order;
	boolean drop_down_multi_select;
	boolean drillthru;
	char *populate_drop_down_process_name;
	char *populate_helper_process_name;
	LIST *primary_delimited_list;
	ATTRIBUTE *attribute;
	DROP_DOWN_PROMPT *drop_down_prompt;
	PROMPT *prompt;
} PROCESS_PARAMETER;

typedef struct
{
	/* Input */
	char *process_or_set_name;
	char *role_name;
	char *login_name;
	boolean is_preprompt;
	DICTIONARY *drillthru_dictionary;

	/* Process */
	/* ------- */
	char *process_name;
	char *process_set_name;
	PROCESS *process;
	PROCESS_SET *process_set;
	LIST *process_parameter_list;
} PROCESS_PROMPT_OUTPUT;

typedef struct
{
	/* Input */
	char *process_name;
	char *role_name;
	char *login_name;
	DICTIONARY *working_post_dictionary;

	/* Process */
	/* ------- */
	PROCESS *process;
	char *command_line;
} PROCESS_PROMPT_SUBMIT;

/* PROCESS_PARAMETER operations */
/* ---------------------------- */
PROCESS_PARAMETER *process_parameter_new(
			char *process_or_set_name,
			char *folder_name,
			char *attribute_name,
			char *drop_down_prompt_name,
			char *prompt_name );

/* Returns static memory */
/* --------------------- */
char *process_parameter_where(
			char *where,
			boolean is_preprompt );

/* PROCESS_PROMPT_OUTPUT operations */
/* -------------------------------- */
PROCESS_PROMPT_OUTPUT *process_prompt_output_calloc(
			void );

PROCESS_PROMPT_OUTPUT *process_prompt_output_fetch(
			char *process_or_set_name,
			char *role_name,
			char *login_name,
			boolean is_preprompt,
			char *document_root_directory,
			char *application_relative_source_directory,
			DICTIONARY *drillthru_dictionary );

/* PROCESS_PROMPT_SUBMIT operations */
/* -------------------------------- */
PROCESS_PROMPT_SUBMIT *process_prompt_submit_fetch(
			char *process_name,
			char *role_name,
			char *login_name,
			DICTIONARY *working_post_dictionary );

/* PROCESS_SET operations */
/* ---------------------- */
PROCESS_SET *process_set_fetch(
			char *process_set_name,
			char *role_name,
			char *document_root_directory,
			char *application_relative_source_directory,
			boolean fetch_process_set_member_name_list );

/* Returns static memory */
/* --------------------- */
char *process_set_primary_where(
			char *process_set_name );

char *process_set_system_string(
			char *where );

PROCESS_SET *process_set_parse(
			char *input,
			char *role_name,
			char *document_root_directory,
			char *application_relative_source_directory,
			boolean fetch_member_process_name_list );

PROCESS_SET *process_set_new(
			char *process_set_name );

LIST *process_set_member_name_list(
			char *process_set_primary_where,
			char *role_name );

/* PROCESS operations */
/* ------------------ */
PROCESS *process_fetch(
			char *process_name,
			char *document_root_directory,
			char *application_relative_source_directory,
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
			char *document_root_directory,
			char *application_relative_source_directory,
			boolean check_executable_inside_filesystem );

char *process_populate_drop_down_command_line(
			DICTIONARY *drillthru_dictionary,
			char *command_line,
			char *security_entity_where,
			char *state,
			char *login_name,
			char *role_name );

char *process_name_fetch(
			char *process_or_set_name );

char *process_set_name_fetch(
			char *process_or_set_name );

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
			DICTIONARY *drillthru_dictionary,
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
			DICTIONARY *drillthru_dictionary,
			char *login_name );

LIST *process_parameter_primary_delimited_list(
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *folder_name,
			char *populate_drop_down_process_name );

/* PROCESS generic operations */
/* -------------------------- */

void process_execution_count_increment(
			char *process_name );

char *process_dictionary_process_member(
			char *process_set,
			DICTIONARY *parsed_decoded_post_dictionary,
			char *from_starting_label );

boolean process_executable_ok(
			char *executable );

void process_set_one2m_folder_name_for_process(
			DICTIONARY *dictionary,
			char *one2m_folder_name );

boolean process_interpreted_executable_ok(
			char *which_string );

/* PROCESS command_line */
/* -------------------- */

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_choose_isa_command_line(
			char *command_line,
			char *application_name,
			char *security_entity_where,
			char *login_name,
			char *role_name,
			char *one2m_isa_folder_name );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_drop_down_command_line(
			char *command_line,
			char *application_name,
			char *one2m_folder_name,
			char *state,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *working_post_dictionary );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_update_row_command_line(
			char *command_line,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			int row,
			char *process_name,
			char *one2m_folder_name,
			LIST *primary_data_list );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_prompt_submit_command_line(
			char *command_line,
			char *application_name,
			char *process_name,
			char *role_name,
			char *login_name,
			DICTIONARY *working_post_dictionary );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_search_replace_where(
			char *command_line,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_replace_one2m_folder_name(
			char *command_line,
			char *one2m_folder_name );

/* Frees command_line and safely returns heap memory */
/* ------------------------------------------------- */
char *process_operation_command_line(
			char *command_line,
			char *application_name,
			char *operation_name,
			char *login_name,
			char *role_name,
			char *folder_name,
			pid_t parent_process_id,
			char *session_key,
			int operation_row_total,
			LIST *operation_row_primary_data_list,
			DICTIONARY *operation_single_row_dictionary );

#endif

