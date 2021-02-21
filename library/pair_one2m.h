/* APPASERVER_HOME/library/pair_one2m.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef PAIR_ONE2M_H
#define PAIR_ONE2M_H

/* -------- */
/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "dictionary.h"

/* --------- */
/* Constants */
/* --------- */
#define PAIR_ONE2M_ONE_FOLDER_LABEL	"pair_one2m_one_folder"
#define PAIR_ONE2M_MANY_FOLDER_LABEL	"pair_one2m_many_folder"
#define PAIR_ONE2M_FULFILLED_LIST_LABEL	"pair_one2m_fulfilled_list"
#define PAIR_ONE2M_DUPLICATE_KEY	"pair_one2m_duplicate"

/* ---------- */
/* Structures */
/* ---------- */
typedef struct
{
	char *many_folder_name;
	char *copy_function;
	char *folder_button_string;
} PAIR_ONE2M_FOLDER;

typedef struct
{
	char *one_folder_name;
	char *many_folder_name;
	char *keystrokes_save_function;
	char *prompt_one_element_name;
	char *prompt_many_element_name;
	LIST *one2m_pair_relation_list;
	LIST *prompt_form_folder_list;
	LIST *fulfilled_folder_name_list;
	char *next_folder_name;
	DICTIONARY *pair_one2m_dictionary;
	boolean participating;
	boolean duplicate;
} PAIR_ONE2M;

/* ---------- */
/* Prototypes */
/* ---------- */

/* Usage */
/*------ */
PAIR_ONE2M *pair_one2m_calloc(
			void );

PAIR_ONE2M *pair_one2m_prompt_form_new(
			char *one_folder_name );

PAIR_ONE2M *pair_one2m_post_new(
			DICTIONARY *pair_one2m_dictionary );

PAIR_ONE2M_FOLDER *pair_one2m_folder_new(
			char *many_folder_name );

LIST *pair_one2m_prompt_form_folder_list(
			LIST *one2m_relation_list );

char *pair_folder_element_copy_function(
			char *prompt_many_element_name,
			char *many_folder_name );

char *pair_folder_button_string(
			char *many_folder_name,
			char *keystrokes_save_function,
			char *copy_function );

LIST *pair_one2m_fulfilled_folder_name_list(
			char *pair_one2m_fulfilled_list_label,
			DICTIONARY *pair_one2m_dictionary );

LIST *pair_one2m_fulfilled_folder_name_list(
			char *pair_one2m_fulfilled_list_label,
			DICTIONARY *pair_one2m_dictionary );

DICTIONARY *pair_one2m_fulfilled_dictionary(
			/* -------------------------------------- */
			/* Sets and returns pair_one2m_dictionary */
			/* -------------------------------------- */
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_fulfilled_list_label,
			LIST *pair_one2m_fulfilled_folder_name_list );

char *pair_one2m_next_folder_name(
			LIST *pair_one2m_fulfilled_folder_name_list,
			LIST *relation_one2m_pair_relation_list,
			char *one_folder_name,
			char *many_folder_name );

char *pair_one2m_folder_name(
			char *folder_label,
			DICTIONARY *pair_one2m_dictionary );

char *pair_one2m_prompt_element_name(
			char *pair_one2m_prefix,
			char *folder_label );

boolean pair_one2m_participating(
			DICTIONARY *pair_one2m_dictionary );

boolean pair_one2m_duplicate(
			char *message_key,
			char *message );

void pair_one2m_duplicate_set(
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_duplicate_key );

void pair_one2m_duplicate_unset(
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_duplicate_key );

boolean pair_one2m_inserted_duplicate(
			char *pair_one2m_duplicate_key,
			DICTIONARY *pair_one2m_dictionary );

void pair_one2m_folder_set(
			DICTIONARY *pair_one2m_dictionary,
			char *folder_label,
			char *folder_name );

void pair_one2m_set_folder_button_string(
			LIST *prompt_form_folder_list,
			char *keystrokes_save_function );

#endif

