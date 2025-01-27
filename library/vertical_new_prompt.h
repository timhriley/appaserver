/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_prompt.h			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_PROMPT_H
#define VERTICAL_NEW_PROMPT_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"

#define VERTICAL_NEW_PROMPT_ONE_HIDDEN_LABEL	"nnew_checkbox_hidden_one"
#define VERTICAL_NEW_PROMPT_MANY_HIDDEN_LABEL	"nnew_checkbox_hidden_many"

typedef struct
{
	char *one_folder_name;
	char *many_folder_name;
	char *execute_system_string_table_insert;
} VERTICAL_NEW_PROMPT;

/* Usage */
/* ----- */
VERTICAL_NEW_PROMPT *vertical_new_prompt_new(
		DICTIONARY *non_prefixed_dictionary,
		const char *vertical_new_checkbox_prefix,
		const char *vertical_new_one_hidden_label,
		const char *vertical_new_many_hidden_label,
		const char *table_insert_executable,
		const char *frameset_table_frame,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		DICTIONARY *drillthru_dictionary );

/* Process */
/* ------- */
VERTICAL_NEW_PROMPT *vertical_new_prompt_calloc(
		void );

/* Safely returns */
/* -------------- */
DICTIONARY *vertical_new_prompt_dictionary(
		const char *vertical_new_prompt_one_hidden_label,
		const char *vertical_new_prompt_many_hidden_label,
		char *many_folder_name,
		char *vertical_new_prompt_one_folder_name );

/* Usage */
/* ----- */

/* Returns component of non_prefixed_dictionary or null */
/* ---------------------------------------------------- */
char *vertical_new_prompt_one_folder_name(
		const char *vertical_new_checkbox_prefix,
		DICTIONARY *non_prefixed_dictionary );

#endif

