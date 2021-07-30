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

/* Constants */
/* --------- */
#define PROCESS_TABLE		"process"
#define PROCESS_SET_TABLE	"process_set"
#define PROCESS_ID_LABEL	"pprocess_id"

typedef struct
{
	char *process_set_name;
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

	/* Process */
	/* ------- */
} PROCESS;

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

void process_free(		PROCESS *process );

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

PROCESS *process_calloc(
			void );

#endif

