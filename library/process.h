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
	char *process_set_name;
	boolean check_executable_inside_filesystem;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *prompt_display_text;
	char *process_group;
	char *preprompt_help_text;
	boolean prompt_display_bottom;
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
	char *folder_name;
	char *attribute_name;
	char *drop_down_prompt_name;
	char *prompt_name;

	/* Process */
	/* ------- */
	FOLDER *folder;
	ATTRIBUTE *attribute;
	DROP_DOWN_PROMPT *drop_down_prompt;
	PROMPT *prompt;
} PROCESS_PARAMETER;

typedef struct
{
	char *process_or_process_set_name;
	boolean check_executable_inside_filesystem;

} PROCESS_STRUCTURE;

/* Operations */
/* ---------- */
void process_execution_count_increment(
				char *process_name );

void process_increment_execution_count(
				char *application_name,
				char *process_name,
				char *database_management_system );

PROCESS_SET *process_new_process_set(
				char *application_name,
				char *session, 
				char *process_set_name );

/* Does a PROCESS fetch */
/* -------------------- */
PROCESS *process_new(		char *application_name,
				char *process_name,
				boolean check_executable_inside_filesystem );

PROCESS *process_new_process(	char *application_name,
				char *session, 
				char *process_name,
				DICTIONARY *dictionary,
				char *role_name,
				boolean check_executable_inside_filesystem );
LIST *process2list(		char *executable );

void process_convert_parameters(char **executable,
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
				LIST *primary_attribute_name_list,
				LIST *primary_data_list,
				int row,
				char *process_name,
				PROCESS_SET *process_set,
				char *one2m_folder_name_for_process,
				char *operation_row_count_string,
				char *prompt );


void process_for_folder_or_attribute_parameters_populate_attribute_list(
				LIST *attribute_list,
				char *application_name,
				LIST *process_parameter_list );

LIST *process_get_prompt_list( 	LIST *process_parameter_list );

char *process_display(		PROCESS *process );

char *process_get_process_member_from_dictionary(
				char *process_set,
				DICTIONARY *parsed_decoded_post_dictionary,
				char *from_starting_label );

void process_load_executable(	char **executable,
				char *process_name,
				char *application_name );

void process_get_executable( 	char **executable, 
				char *application_name,
				char *process_name );

void process_get_member_executable( 
				char *executable, 
				char *application_name,
				char *role,
				char *process,
				DICTIONARY *parsed_decoded_post_dictionary );

void process_get_executable_role( 	
				char *executable, 
				char *application_name,
				char *role,
				char *process_name );

ATTRIBUTE *process_get_attribute(
				char *application_name, 
				char *attribute_name );

void process_set_from_folder(	PROCESS *process,
				char *from_folder );

void process_append_related_folder(
				PROCESS *process,
				char *related_folder );

boolean process_load(	
				char **executable,
				char **notepad,
				char **html_help_file_anchor,
				char **post_change_javascript,
				char **process_set_display,
				char **preprompt_help_text,
				boolean *is_appaserver_process,
				char *application_name,
				char *process_name,
				boolean check_executable_inside_filesystem );

void process_set_dictionary(	PROCESS *process,
				DICTIONARY *parsed_decoded_post_dictionary );

void process_replace_parameter_variables(	
				char *executable,
				char *application_name,
				char *session,
				char *state,
				char *person,
				char *folder_name,
				char *role_name,
				char *target_frame,
				char *process_name,
				char *process_set_name,
				char *operation_row_count_string,
				char *one2m_folder_name_for_process,
				char *prompt,
				char *process_id_string );

void process_append_error_file(	char *executable );

int process_set_load(		char **notepad,
				char **html_help_file_anchor,
				char **post_change_javascript,
				char **prompt_display_text,
				char **preprompt_help_text,
				boolean *prompt_display_bottom,
				char *application_name,
				char *session,
				char *process_set_name );
LIST *process_load_record_list( char *application_name );

boolean process_exists_appaserver_process(
				char *application_name,
				LIST *process_name_list );

boolean process_executable_ok(	char *executable );

void process_set_one2m_folder_name_for_process(
				DICTIONARY *dictionary,
				char *one2m_folder_name );

void process_search_replace_executable_where(
				char *local_executable,
				char *application_name,
				char *folder_name,
				LIST *attribute_list,
				DICTIONARY *where_clause_dictionary );

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
			LIST *primary_attribute_name_list,
			LIST *primary_data_list,
			int row,
			char *process_name,
			char *operation_row_count_string );

void process_prompt_convert_parameters(char **executable,
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

/* PROMPT */
/* ====== */
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
			DICTIONARY *preprompt_dictionary );

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
			DICTIONARY *preprompt_dictionary );

/* PROCESS_STRUCTURE */
/* ================= */

/* PROCESS */
/* ======= */
PROCESS *process_fetch(
			char *process_name,
			boolean check_executable_inside_filesystem );

PROCESS *process_new(
			char *process_name );

#endif

