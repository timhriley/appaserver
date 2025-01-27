/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_table.h				*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_TABLE_H
#define VERTICAL_NEW_TABLE_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "vertical_new_filename.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	VERTICAL_NEW_FILENAME *vertical_new_filename;
	char *onload_javascript_string;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	DOCUMENT *document;
} VERTICAL_NEW_TABLE;

/* Usage */
/* ----- */

/* Returns true if selected */
/* ------------------------ */
VERTICAL_NEW_TABLE *vertical_new_table_new(
		const char *vertical_new_prompt_one_hidden_label,
		const char *vertical_new_prompt_many_hidden_label,
		const char *frameset_prompt_frame,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *data_directory,
		DICTIONARY *non_prefixed_dictionary,
		boolean menu_horizontal_boolean );

/* Process */
/* ------- */
VERTICAL_NEW_TABLE *vertical_new_table_calloc(
		void );

/* Usage */
/* ----- */

/* Returns component of non_prefixed_dictionary or null */
/* ---------------------------------------------------- */
char *vertical_new_table_one_folder_name(
		const char *vertical_new_prompt_one_hidden_label,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns component of non_prefixed_dictionary or null */
/* ---------------------------------------------------- */
char *vertical_new_table_many_folder_name(
		const char *vertical_new_prompt_many_hidden_label,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *vertical_new_table_onload_javascript_string(
		const char *frameset_prompt_frame,
		char *prompt_filename );

/* Usage */
/* ----- */
boolean vertical_new_table_participating_boolean(
		const char *vertical_new_prompt_one_hidden_label,
		DICTIONARY *non_prefixed_dictionary );

/* Driver */
/* ------ */
void vertical_new_table_blank_prompt_frame(
		char *output_filename,
		DOCUMENT *document );

#endif

