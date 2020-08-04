/* process_parameter_list.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_parameter_list ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_PARAMETER_LIST_H
#define PROCESS_PARAMETER_LIST_H

#include "process.h"
#include "element.h"
#include "boolean.h"
#include "folder.h"
#include "list.h"

typedef struct
{
	char *drop_down_prompt;
	LIST *drop_down_prompt_data_list;
	char *hint_message;
	char *optional_display;
	boolean multi_select;
} PARAMETER_DROP_DOWN_PROMPT;

typedef struct
{
	char *prompt;
	PROCESS *populate_drop_down_process;
	boolean multi_select;
} PARAMETER_PROCESS;

typedef struct
{
	char *folder_name;
	LIST *primary_key_list;
	LIST *primary_data_list;
	char *prompt;
	boolean multi_select;
} PARAMETER_FOLDER;

typedef struct
{
	char *folder_name;
	char *attribute_name;
	int width;
	char *hint_message;
} PARAMETER_ATTRIBUTE;

typedef struct
{
	char *prompt;
	int width;
	char *hint_message;
	int input_width;
	char upload_filename_yn;
	char date_yn;
} PARAMETER_PROMPT;

typedef struct
{
	char *type;
	PARAMETER_FOLDER *parameter_folder;
	PARAMETER_ATTRIBUTE *parameter_attribute;
	PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt;
	PARAMETER_PROMPT *parameter_prompt;
	PARAMETER_PROCESS *parameter_process;
	char *populate_helper_process;
} PROCESS_PARAMETER;

typedef struct
{
	LIST *distinct_folder_list;
	LIST *process_parameter_list;
	char *application_name;
	char *process_name;
	boolean is_process_set;
} PROCESS_PARAMETER_LIST;

/* Operations */
/* ---------- */
LIST *process_parameter_get_records4process_set_list(
				char *application_name,
				char *process_name,
				boolean is_preprompt,
				char *error_file );

LIST *process_parameter_get_records4process_list(
				char *application_name,
				char *process_name,
				boolean is_preprompt,
				char *error_file );

PROCESS_PARAMETER_LIST *process_parameter_list_new( 
				char *login_name,
				char *application_name,
				char *process_name,
				boolean override_row_restrictions,
				boolean is_preprompt,
				DICTIONARY *parameter_dictionary,
				char *role_name,
				char *session,
				boolean with_populate_drop_downs );

LIST *process_parameter_get_folder_element_list(
			char *post_change_javascript,
			char *folder_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			boolean multi_select,
			char *populate_helper_process_string,
			char *document_root_directory,
			char *application_name,
			char *session,
			char *login_name );

ELEMENT_APPASERVER *process_parameter_get_attribute_element(
				PROCESS_PARAMETER *process_parameter );

void process_parameter_set_folder( 	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_FOLDER *parameter_folder );

void process_parameter_set_attribute( 	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_ATTRIBUTE *parameter_attribute );

void process_parameter_set_prompt(	
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_PROMPT *parameter_prompt );

PARAMETER_PROMPT *parameter_prompt_new(	
				char *prompt,
				int prompt_width,
				char *hint_message,
				char upload_filename_yn,
				char date_yn );

PARAMETER_ATTRIBUTE *parameter_attribute_new(	
				char *application_name,
				char *attribute_name );

PARAMETER_FOLDER *parameter_folder_new(
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *prompt,
			char *multi_select_folder_yn,
			LIST *distinct_folder_list,
			boolean override_row_restrictions,
			DICTIONARY *parameter_dictionary,
			char *role_name,
			boolean with_populate_drop_downs,
/*
			LIST *preprompt_folder_name_list,
			LIST *preprompt_attribute_name_list,
*/
			char *populate_drop_down_process_string,
			char *process_name );

LIST *process_parameter_get_primary_data_list(
				char *login_name,
				char *application_name,
				char *folder_name,
				LIST *distinct_folder_list,
				boolean override_row_restrictions,
				char *role_name,
				char *state );
int process_parameter_get_attribute_width(
				char *application_name,
				char *folder_name,
				char *attribute_name,
				LIST *distinct_folder_list );
FOLDER *process_parameter_get_and_set_distinct_folder(
				LIST *distinct_folder_list,
				char *application_name,
				char *folder_name,
				boolean override_row_restrictions,
				char *role_name );
PROCESS_PARAMETER *process_parameter_new(
				void );
LIST *process_parameter_get_prompt_element_list(
				PROCESS_PARAMETER *process_parameter,
				char *hint_message,
				char upload_filename_yn,
				char date_yn,
				char *post_change_javascript );

LIST *process_parameter_get_attribute_element_list(
				boolean *exists_date_element,
				boolean *exists_time_element,
				char *application_name,
				PROCESS_PARAMETER *process_parameter,
				char *post_change_javascript );

ELEMENT_APPASERVER *process_parameter_get_folder_prompt_element(
				char *login_name,
				char *application_name,
				boolean override_row_restrictions,
				boolean multi_select,
				char *folder_name,
				char *prompt_string,
				LIST *primary_data_list,
				char *role_name );

LIST *process_parameter_get_folder_prompt_element_list(
				char *login_name,
				char *application_name,
				boolean override_row_restrictions,
				char *post_change_javascript,
				char *prompt_string,
				boolean multi_select,
				char *folder_name,
				LIST *primary_data_list,
				char *role_name );
void process_parameter_list_append_process_members4set(
				LIST *process_record_list,
				char *process,
				char *role );

LIST *process_parameter_get_process_set_output_element_list(
				char *process_set_name,
				char *application_name,
				char *role,
				char *post_change_javascript,
				char *prompt_display_text );

LIST *process_parameter_get_process_members4set_list(
				char *application_name,
				char *process,
				char *role );

LIST *process_parameter_get_primary_key_and_data_list(
			LIST **primary_key_list,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			LIST *distinct_folder_list,
			boolean override_row_restrictions,
			DICTIONARY *parameter_dictionary,
			char *role_name,
/*
			LIST *preprompt_folder_name_list,
			LIST *preprompt_attribute_name_list,
*/
			char *populate_drop_down_process_string,
			char *process_name,
			char *prompt );

char *process_parameter_list_display(
				PROCESS_PARAMETER_LIST *p );
LIST *process_parameter_list_get_process_parameter_record_list(
				boolean *is_process_set,
				char *application_name,
				char *process,
				char *error_file,
				boolean is_preprompt );
PARAMETER_DROP_DOWN_PROMPT *parameter_drop_down_prompt_new(
				char *application_name,
				char *drop_down_prompt,
				char *hint_message,
				char *optional_display,
				char *drop_down_multi_select_yn,
				boolean with_populate_drop_downs,
				char *populate_drop_down_process,
				DICTIONARY *parameter_dictionary );

LIST *process_parameter_get_drop_down_prompt_data_list(
				char *application_name,
				char *drop_down_prompt,
				char *populate_drop_down_process,
				DICTIONARY *parameter_dictionary );

void process_parameter_set_drop_down_prompt(
				PROCESS_PARAMETER *process_parameter,
				PARAMETER_DROP_DOWN_PROMPT *p );
LIST *process_parameter_get_drop_down_prompt_element_list(
				PARAMETER_DROP_DOWN_PROMPT
					*parameter_drop_down_prompt,
				char *post_change_javascript );
int process_parameter_list_element_name_boolean(
				char *element_name );
char *process_parameter_get_prompt_hint_message(
				char *application_name,
				char *prompt,
				PROCESS_PARAMETER *process_parameter );
boolean process_parameter_get_prompt_date(
				char *application_name,
				char *prompt,
				PROCESS_PARAMETER *process_parameter );
ELEMENT_APPASERVER *process_parameter_get_process_set_output_drop_down_element(
				char *process_set_name,
				char *application_name,
				char *role_name );
PARAMETER_PROCESS *parameter_process_new(
				char *prompt,
				PROCESS *populate_drop_down_process,
				char drop_down_multi_select );

LIST *process_parameter_get_prompt_process_element_list(
				char *login_name,
				char *session,
				char *application_name,
				char *role_name,
				char *prompt_string,
				PROCESS *populate_drop_down_process,
				boolean multi_select,
				DICTIONARY *parameter_dictionary );

LIST *process_parameter_get_folder_process_element_list(
				char *login_name,
				char *session,
				char *application_name,
				char *role_name,
				char *prompt_string,
				PROCESS *populate_drop_down_process,
				boolean multi_select,
				char *state );

ELEMENT_APPASERVER *process_parameter_get_prompt_process_element(
				char *login_name,
				char *application_name,
				boolean multi_select,
				char *prompt_string,
				char *role_name,
				PROCESS *populate_drop_down_process,
				char *session,
				DICTIONARY *parameter_dictionary );

LIST *process_parameter_list_get_process_folder_name_list(
				char *application_name,
				char *process_name );

void process_parameter_list_dictionary_set_drop_down_prompt_optional_display(
				DICTIONARY *dictionary,
				LIST *process_parameter_list );

LIST *process_parameter_list_get_preprompt_folder_name_list(
				char *application_name,
				char *process_name );

void process_parameter_parse_parameter_record(
				char *folder_name,
				char *attribute_name,
				char *prompt,
				char *drop_down_prompt,
				char *input_width,
				char *prompt_hint_message,
				char *drop_down_prompt_hint_message,
				char *drop_down_prompt_optional_display,
				char *drop_down_multi_select_yn,
				char *populate_drop_down_process,
				char *upload_filename_yn,
				char *prompt_date_yn,
				char *populate_helper_process,
				char *parameter_record );

ELEMENT_APPASERVER *process_parameter_get_folder_process_element(
				char *login_name,
				char *application_name,
				boolean multi_select,
				char *prompt_string,
				char *role_name,
				PROCESS *populate_drop_down_process,
				char *session,
				char *state );

LIST *process_parameter_get_prompt_process_list(
				char *application_name,
				char *session,
				char *folder_name,
				char *login_name,
				PROCESS *populate_drop_down_process,
				char *role_name,
				DICTIONARY *parameter_dictionary );

LIST *process_parameter_get_preprompt_attribute_name_list(
					char *application_name,
					char *process_name,
					char *appaserver_error_file );

boolean process_parameter_get_no_initial_capital(
				char *application_name,
				char *folder_name );


/* Returns heap memory. */
/* -------------------- */
char *process_parameter_command_line(
				char *application_name,
				char *populate_drop_down_process_string,
				DICTIONARY *parameter_dictionary );

#endif

