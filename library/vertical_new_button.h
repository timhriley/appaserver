/* APPASERVER_HOME/library/vertical_new_button.h			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new buttons on the prompt insert form.	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_H
#define VERTICAL_NEW_H

/* -------- */
/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "element.h"
#include "appaserver_link.h"

/* --------- */
/* Constants */
/* --------- */
#define VERTICAL_NEW_BUTTON_DISPLAY		"New"
#define VERTICAL_NEW_BUTTON_ONE_PREFIX		"nnew_button_one_"
#define VERTICAL_NEW_BUTTON_ONE_HIDDEN_LABEL	"nnew_button_hidden_one"
#define VERTICAL_NEW_BUTTON_MANY_HIDDEN_LABEL	"nnew_button_hidden_many"

/* ---------- */
/* Structures */
/* ---------- */

typedef struct
{
	/* Process */
	/* ------- */
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
} VERTICAL_NEW_BUTTON_FILENAME;

/* VERTICAL_NEW_BUTTON_FILENAME operations */
/* --------------------------------------- */
VERTICAL_NEW_BUTTON_FILENAME *vertical_new_button_filename_calloc(
			void );

/* Always returns true */
/* ------------------- */
VERTICAL_NEW_BUTTON_FILENAME *vertical_new_button_filename_new(
			char *application_name,
			char *session_key,
			char *document_root_directory );

typedef struct
{
	/* Process */
	/* ------- */
	char *one_folder_name;
	char *system_string;
	VERTICAL_NEW_BUTTON_FILENAME *filename;
} VERTICAL_NEW_BUTTON;

/* VERTICAL_NEW_BUTTON operations */
/* ------------------------------ */
VERTICAL_NEW_BUTTON *vertical_new_button_calloc(
			void );

/* Usage */
/* ----- */

/* Returns true if selected */
/* ------------------------ */
VERTICAL_NEW_BUTTON *vertical_new_button_post_prompt_insert_new(
			DICTIONARY *non_prefixed_dictionary /* in/out */,
			char *many_folder_name,
			char *vertical_new_button_one_prefix,
			DICTIONARY *drillthru_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Process */
/* ------- */
char *vertical_new_button_one_folder_name(
			char *vertical_new_button_one_prefix,
			DICTIONARY *non_prefixed_dictionary );

char *vertical_new_button_output_insert_table_system_string(
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *one_folder_name,
			char *role_name,
			char *target_frame );

/* Usage */
/* ----- */
VERTICAL_NEW_BUTTON *vertical_new_button_post_edit_table_new(
			char *vertical_new_button_many_hidden_label,
			DICTIONARY *non_prefixed_dictionary );

/* Process */
/* ------- */
char *vertical_new_button_many_folder_name(
			char *vertical_new_button_many_hidden_label,
			DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */
VERTICAL_NEW_BUTTON *vertical_new_button_output_insert_table_new(
			DICTIONARY *non_prefixed_dictionary /* in/out */,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *many_folder_name,
			char *document_root_directory,
			char *vertical_new_button_one_prefix );

/* Process */
/* ------- */
void vertical_new_button_blank_prompt_screen(
			char *output_filename,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *appaserver_parameter_data_directory );

/* Returns heap memory */
/* ------------------- */
char *vertical_new_button_onload_control_string(
			char *prompt_filename );

/* Usage */
/* ----- */
char *vertical_new_button_one_element_name(
			char *vertical_new_button_one_prefix,
			char *one_folder_name );

APPASERVER_ELEMENT *vertical_new_button_element(
			char *one_folder_name,
			LIST *role_folder_insert_list,
			char *form_name,
			char *vertical_new_button_one_prefix,
			char *vertical_new_button_display );

char *vertical_new_button_dictionary_many_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary );

/* Private */
/* ------- */
void vertical_new_button_dictionary_set(
			DICTIONARY *non_prefixed_dictionary,
			char *hidden_label,
			char *folder_name );

#endif

