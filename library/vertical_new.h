/* APPASERVER_HOME/library/vertical_new.h				*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new buttons on the prompt insert form.	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_H
#define VERTICAL_H

#include "boolean.h"
#include "list.h"
#include "element.h"
#include "appaserver_link.h"

#define VERTICAL_NEW_DISPLAY		"New"
#define VERTICAL_NEW_ONE_PREFIX		"nnew_button_one_"
#define VERTICAL_NEW_ONE_HIDDEN_LABEL	"nnew_button_hidden_one"
#define VERTICAL_NEW_MANY_HIDDEN_LABEL	"nnew_button_hidden_many"

/* VERTICAL_NEW operations */
/* ----------------------- */
void vertical_new_dictionary_set(
			DICTIONARY *non_prefixed_dictionary,
			char *hidden_label,
			char *folder_name );

char *vertical_new_dictionary_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary );

/* VERTICAL_NEW_OUTPUT_PROMPT_INSERT operations */
/* -------------------------------------------- */
APPASERVER_ELEMENT *vertical_new_output_prompt_insert_element(
			char *one_folder_name,
			LIST *role_folder_insert_list,
			char *form_name,
			char *vertical_new_one_prefix,
			char *vertical_new_display );

typedef struct
{
	/* Process */
	/* ------- */
	char *one_folder_name;
	char *system_string;
} VERTICAL_NEW_POST_PROMPT_INSERT;

/* VERTICAL_NEW_POST_PROMPT_INSERT operations */
/* ------------------------------------------ */
VERTICAL_NEW_POST_PROMPT_INSERT *vertical_new_post_prompt_insert_calloc( void );

VERTICAL_NEW_POST_PROMPT_INSERT *vertical_new_post_prompt_insert_new(
			DICTIONARY *non_prefixed_dictionary /* in/out */,
			char *many_folder_name,
			DICTIONARY *drillthru_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *vertical_new_one_prefix,
			char *vertical_new_one_hidden_label,
			char *vertical_new_many_hidden_label,
			char *frameset_prompt_frame );

/* Process */
/* ------- */
char *vertical_new_post_prompt_insert_one_folder_name(
			char *vertical_new_one_prefix,
			DICTIONARY *non_prefixed_dictionary );

char *vertical_new_post_prompt_insert_system_string(
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			char *application_name,
			char *login_name,
			char *session_key,
			char *one_folder_name,
			char *role_name,
			char *frameset_prompt_frame );


typedef struct
{
	/* Process */
	/* ------- */
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
} VERTICAL_NEW_BLANK_FILENAME;

/* VERTICAL_NEW_BLANK_FILENAME operations */
/* -------------------------------------- */
VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename_calloc(
			void );

/* Always returns true */
/* ------------------- */
VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename_new(
			char *application_name,
			char *session_key,
			char *document_root_directory );

typedef struct
{
	/* Process */
	/* ------- */
	char *one_folder_name;
	VERTICAL_NEW_BLANK_FILENAME *vertical_new_blank_filename;
	char *onload_control_string;
} VERTICAL_NEW_OUTPUT_INSERT_TABLE

/* VERTICAL_NEW_OUTPUT_INSERT_TABLE operations */
/* ------------------------------------------- */
VERTICAL_NEW_OUTPUT_INSERT_TABLE *vertical_new_output_insert_table_calloc(
			void );

/* Returns true if selected */
/* ------------------------ */
VERTICAL_NEW_OUTPUT_INSERT_TABLE *
	vertical_new_output_insert_table_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			char *document_root_directory,
			DICTIONARY *non_prefixed_dictionary,
			char *vertical_new_one_hidden_label,
			char *frameset_prompt_frame,
			char *data_directory );

/* Process */
/* ------- */
void vertical_new_output_insert_table_blank_prompt_frame(
			char *output_filename,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *frameset_prompt_frame,
			char *data_directory );

char *vertical_new_output_insert_table_onload_control_string(
			char *prompt_filename,
			char *frameset_prompt_frame );


typedef struct
{
} VERTICAL_NEW_POST_INSERT_TABLE;

/* Usage */
/* ----- */
VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table_calloc(
			void );

VERTICAL_NEW_POST_INSERT_TABLE *vertical_new_post_insert_table_new(
			char *vertical_new_button_many_hidden_label,
			DICTIONARY *non_prefixed_dictionary );

/* Process */
/* ------- */

#endif

