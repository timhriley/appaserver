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
	char *many_folder_name;
	char *one_folder_name;
} VERTICAL_NEW_BUTTON;

/* ---------- */
/* Prototypes */
/* ---------- */
VERTICAL_NEW_BUTTON *vertical_new_button_calloc(
			void );

char *vertical_new_button_one_element_name(
			char *vertical_new_button_one_prefix,
			char *one_folder_name );

char *vertical_new_button_many_element_name(
			char *vertical_new_button_many_label );

APPASERVER_ELEMENT *vertical_new_button_element(
			char *one_folder_name,
			LIST *role_folder_insert_list,
			char *form_name,
			char *vertical_new_button_one_prefix,
			char *vertical_new_button_display );

char *vertical_new_button_dictionary_one_folder_name(
			char *vertical_new_button_one_prefix,
			DICTIONARY *non_prefixed_dictionary );

char *vertical_new_button_dictionary_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary );

void vertical_new_button_dictionary_set(
			DICTIONARY *non_prefixed_dictionary,
			char *hidden_label,
			char *folder_name );

#endif

