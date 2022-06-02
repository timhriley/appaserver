/* APPASERVER_HOME/library/pair_one2m.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef PAIR_ONE2M_H
#define PAIR_ONE2M_H

#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "dictionary.h"
#include "element.h"

#define PAIR_ONE2M_ONE_FOLDER_LABEL	"pair_one2m_one_folder"
#define PAIR_ONE2M_MANY_FOLDER_LABEL	"pair_one2m_many_folder"
#define PAIR_ONE2M_FULFILLED_LIST_LABEL	"pair_one2m_fulfilled_list"
#define PAIR_ONE2M_DUPLICATE_KEY	"pair_one2m_duplicate"

typedef struct
{
	char *value_set_javascript;
	char *javascript;
	char *label;
	APPASERVER_ELEMENT *element_button;
} PAIR_ONE2M_BUTTON;

/* Usage */
/* ----- */
PAIR_ONE2M_BUTTON *pair_one2m_button_new(
			char *recall_save_javascript,
			char *pair_one2m_prompt_insert_hidden_name,
			char *many_folder_name );

/* Process */
/* ------- */
PAIR_ONE2M_BUTTON *pair_one2m_button_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *pair_one2m_button_value_set_javascript(
			char *pair_one2m_prompt_insert_hidden_name,
			char *many_folder_name );

/* Returns heap memory */
/* ------------------- */
char *pair_one2m_button_javascript(
			char *recall_save_javascript,
			char *pair_one2m_button_value_set_javascript );

/* Returns heap memory */
/* ------------------- */
char *pair_one2m_button_label(
			char *many_folder_name );

typedef struct
{
	LIST *pair_one2m_button_list;
	char *hidden_name;
	APPASERVER_ELEMENT *transmit_hidden_element;
	RELATION *relation;
	LIST *element_list;
} PAIR_ONE2M_PROMPT_INSERT;

/* Usage */
/* ----- */
PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_new(
			char *recall_save_javascript,
			LIST *relation_pair_one2m_list );

/* Process */
/* ------- */
PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_calloc(
			void );

/* Returns pair_one2m_many_folder_label */
/* ------------------------------------ */
char *pair_one2m_prompt_insert_hidden_name(
			char *pair_one2m_many_folder_label );

LIST *pair_one2m_prompt_insert_element_list(
			APPASERVER_ELEMENT *transmit_hidden_element,
			LIST *pair_one2m_button_list );

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

/* Usage */
/* ----- */
PAIR_ONE2M *pair_one2m_prompt_form_new(
			char *one_folder_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
PAIR_ONE2M *pair_one2m_post_new(
			DICTIONARY *pair_one2m_dictionary );

/* Process */
/* ------- */

/* Private */
/*-------- */
PAIR_ONE2M *pair_one2m_calloc(
			void );

/* Process */
/* ------- */
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
			LIST *relation_one2m_pair_relation_list );

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

void pair_one2m_set_fulfilled_name_list(
			LIST *fulfilled_name_list,
			char *many_folder_name,
			LIST *one2m_pair_relation_list );

#endif

