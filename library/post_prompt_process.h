/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_prompt_process.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_PROMPT_PROCESS_H
#define POST_PROMPT_PROCESS_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

#define POST_PROMPT_PROCESS_EXECUTABLE	"post_prompt_process"

typedef struct
{
	boolean application_menu_horizontal_boolean;
	boolean menu_horizontal_boolean;
	SESSION_PROCESS *session_process;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *process_parameter_list;
	LIST *process_parameter_upload_filename_list;
	POST_DICTIONARY *post_dictionary;
	LIST *process_parameter_date_name_list;
	LIST *process_parameter_folder_name_list;
	LIST *folder_attribute_name_list_attribute_list;
	DICTIONARY_SEPARATE_PROMPT_PROCESS *dictionary_separate_prompt_process;
	char *prompt_process_output_system_string;
	PROCESS_SET *process_set;
	char *application_relative_source_directory;
	PROCESS *process;
	char *command_line;
	boolean post_choose_process_no_parameters;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	DOCUMENT *document;
	char *document_form_html;
} POST_PROMPT_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_PROMPT_PROCESS *post_prompt_process_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		boolean is_drillthru );

/* Process */
/* ------- */
POST_PROMPT_PROCESS *post_prompt_process_calloc(
		void );

/* Returns component of non_prefixed_dictionary */
/* -------------------------------------------- */
char *post_prompt_process_name(
		char *process_set_default_prompt,
		char *prompt_display_text,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_prompt_process_command_line(
		char *process_command_line,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		DICTIONARY *non_prefixed_dictionary,
		char *application_error_directory );

/* Process */
/* ------- */

/* Returns non_prefixed_dictionary */
/* ------------------------------- */
DICTIONARY *post_prompt_process_non_prefixed_dictionary(
		const char *process_folder_name_placeholder,
		const char *process_table_name_placeholder,
		const char *process_attribute_placeholder,
		const char *process_column_placeholder,
		DICTIONARY *non_prefixed_dictionary /* in/out */ );

/* Public */
/* ------ */

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
