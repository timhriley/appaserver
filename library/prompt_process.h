/* $APPASERVER_HOME/library/prompt_process.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_PROCESS_H
#define PROMPT_PROCESS_H

#include "boolean.h"
#include "list.h"
#include "process.h"
#include "drillthru.h"
#include "post_dictionary.h"
#include "frameset.h"
#include "folder_menu.h"
#include "menu.h"
#include "frameset.h"
#include "dictionary_separate.h"
#include "document.h"
#include "form_prompt_process.h"

#define PROMPT_PROCESS_OUTPUT_EXECUTABLE	"output_prompt_process"

typedef struct
{
	LIST *process_parameter_folder_name_list;
	LIST *folder_attribute_name_list_attribute_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_PROMPT_PROCESS *dictionary_separate_prompt_process;
	LIST *process_parameter_list;
	char *post_change_javascript;
	char *document_head_menu_setup_string;
	char *document_head_calendar_setup_string;
	char *title_string;
	char *title_html;
	DOCUMENT *document;
	FORM_PROMPT_PROCESS *form_prompt_process;
	char *document_form_html;
} PROMPT_PROCESS_NOT_DRILLTHRU;

/* Usage */
/* ----- */
PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			PROCESS *process,
			PROCESS_SET *process_set,
			boolean has_drillthru,
			POST_DICTIONARY *post_dictionary,
			MENU *menu );

/* Process */
/* ------- */
PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *prompt_process_not_drillthru_title_string(
			char *process_or_set_name );

/* Returns static memory */
/* --------------------- */
char *prompt_process_not_drillthru_title_html(
			char *process_or_set_name );

typedef struct
{
	LIST *process_parameter_list;
	char *post_change_javascript;
	char *document_head_menu_setup_string;
	char *document_head_calendar_setup_string;
	char *title_string;
	char *title_html;
	DOCUMENT *document;
	FORM_PROMPT_PROCESS *form_prompt_process;
	char *document_form_html;
} PROMPT_PROCESS_IS_DRILLTHRU;

/* Usage */

PROMPT_PROCESS_IS_DRILLTHRU *
	prompt_process_is_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			PROCESS *process,
			PROCESS_SET *process_set,
			MENU *menu );

/* Process */
/* ------- */
PROMPT_PROCESS_IS_DRILLTHRU *
	prompt_process_is_drillthru_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *prompt_process_is_drillthru_title_string(
			char *process_or_set_name );

/* Returns static memory */
/* --------------------- */
char *prompt_process_is_drillthru_title_html(
			char *process_or_set_name );

typedef struct
{
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *process_name;
	char *process_set_name;
	PROCESS *process;
	PROCESS_SET *process_set;
	PROMPT_PROCESS_IS_DRILLTHRU *prompt_process_is_drillthru;
	PROMPT_PROCESS_NOT_DRILLTHRU *prompt_process_not_drillthru;
	char *html;
} PROMPT_PROCESS;

/* Usage */
/* ----- */

/* --------------- */
/* Always succeeds */
/* --------------- */
PROMPT_PROCESS *prompt_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			boolean has_drillthru,
			boolean is_drillthru,
			POST_DICTIONARY *post_dictionary,
			boolean menu_horizontal_boolean,
			char *data_directory,
			char *document_root,
			char *application_relative_source_directory );

/* Process */
/* ------- */
PROMPT_PROCESS *prompt_process_calloc(
			void );


/* Returns either document_form_html */
/* --------------------------------- */
char *prompt_process_html(
			PROMPT_PROCESS_IS_DRILLTHRU *
				prompt_process_is_drillthru,
			PROMPT_PROCESS_NOT_DRILLTHRU *
				prompt_process_not_drillthru );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *prompt_process_output_system_string(
			char *prompt_process_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *dictionary_separate_send_string,
			boolean has_drillthru,
			boolean is_drillthru,
			char *appaserver_error_filename );

#endif
