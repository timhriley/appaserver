/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_post.h				*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_POST_H
#define VERTICAL_NEW_POST_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "vertical_new_filename.h"
#include "prompt_insert.h"

typedef struct
{
	char *data_directory;
	char *many_folder_name;
	boolean application_menu_horizontal_boolean;
	DICTIONARY *original_post_dictionary;
	VERTICAL_NEW_FILENAME *vertical_new_filename;
	PROMPT_INSERT *prompt_insert;
	char *onload_javascript_string;
} VERTICAL_NEW_POST;

/* Usage */
/* ----- */
VERTICAL_NEW_POST *vertical_new_post_new(
		const char *vertical_new_prompt_many_hidden_label,
		const char *frameset_prompt_frame,
		const char *dictionary_separate_drillthru_prefix,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *non_prefixed_dictionary,
		char *data_directory );

/* Process */
/* ------- */
VERTICAL_NEW_POST *vertical_new_post_calloc(
	void );

/* Returns component of non_prefixed_dictionary or null */
/* ---------------------------------------------------- */
char *vertical_new_post_many_folder_name(
		const char *vertical_new_prompt_many_hidden_label,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *vertical_new_post_onload_javascript_string(
		const char *frameset_prompt_frame,
		char *prompt_filename );

/* Usage */
/* ----- */

/* Returns dictionary_medium() */
/* --------------------------- */
DICTIONARY *vertical_new_post_original_dictionary(
		const char *dictionary_separate_drillthru_prefix,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *non_prefixed_dictionary );

/* Driver */
/* ------ */
void vertical_new_post_prompt_insert_create(
		VERTICAL_NEW_FILENAME *vertical_new_filename,
		PROMPT_INSERT *prompt_insert );

#endif

