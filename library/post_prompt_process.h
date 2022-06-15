/* $APPASERVER_HOME/library/post_prompt_process.h			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST_PROMPT_PROCESS_H
#define POST_PROMPT_PROCESS_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

#define POST_PROMPT_PROCESS_EXECUTABLE	"post_prompt_process"

typedef struct
{
	SESSION_PROCESS *session_process;
	POST_DICTIONARY *post_dictionary;
	LIST *process_parameter_folder_name_list;
	LIST *folder_attribute_name_list_attribute_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_PROMPT_PROCESS *dictionary_separate_prompt_process;
	char *prompt_process_output_system_string;
	PROCESS *process;
	PROCESS_SET *process_set;
	char *command_line;
} POST_PROMPT_PROCESS;

/* Usage */
/* ----- */
POST_PROMPT_PROCESS *post_prompt_process_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean is_drillthru,
			char *document_root,
			char *application_relative_source_directory );

/* Process */
/* ------- */
POST_PROMPT_PROCESS *post_prompt_process_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *post_prompt_process_command_line(
			char *process_command_line,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			DICTIONARY *query_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			char *application_error_directory );

/* Private */
/* ------- */
char *post_prompt_process_name(
			char *process_set_default_prompt,
			char *prompt_display_text,
			DICTIONARY *non_prefixed_dictionary );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *post_prompt_process_action_string(
			char *post_prompt_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean is_drillthru );

/* Returns heap memory */
/* ------------------- */
char *post_prompt_process_system_string(
			char *post_prompt_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *appaserver_error_filename );

#endif
