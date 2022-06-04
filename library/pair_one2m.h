/* APPASERVER_HOME/library/pair_one2m.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef PAIR_ONE2M_H
#define PAIR_ONE2M_H

#include "boolean.h"
#include "list.h"
#include "String.h"
#include "piece.h"
#include "folder.h"
#include "sql.h"
#include "dictionary.h"
#include "element.h"
#include "button.h"

#define PAIR_ONE2M_ONE_FOLDER_LABEL	"pair_one2m_one_folder"
#define PAIR_ONE2M_MANY_FOLDER_LABEL	"pair_one2m_many_folder"
#define PAIR_ONE2M_FULFILLED_LIST_LABEL	"pair_one2m_fulfilled_list"

typedef struct
{
	char *many_folder_name;
	DICTIONARY *pair_one2m_dictionary;
} PAIR_ONE2M_POST_PROMPT_INSERT;

/* Usage */
/* ----- */
PAIR_ONE2M_POST_PROMPT_INSERT *
	pair_one2m_post_prompt_insert_new(
			char *one_folder_name,
			DICTIONARY *non_prefixed_dictionary );

/* Process */
/* ------- */
PAIR_ONE2M_POST_PROMPT_INSERT *
	pair_one2m_post_prompt_insert_calloc(
			void );

char *pair_one2m_post_prompt_insert_many_folder_name(
			char *pair_one2m_many_folder_label,
			DICTIONARY *non_prefixed_dictionary );

DICTIONARY *pair_one2m_post_prompt_insert_dictionary(
			char *pair_one2m_one_folder_label,
			char *one_folder_name );

typedef struct
{
	char *value_set_javascript;
	char *javascript;
	char *label;
	BUTTON *button;
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
char *pair_one2m_button_label(
			char *many_folder_name );

/* Returns heap memory */
/* ------------------- */
char *pair_one2m_button_javascript(
			char *recall_save_javascript,
			char *pair_one2m_button_value_set_javascript );

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
PAIR_ONE2M_PROMPT_INSERT *
	pair_one2m_prompt_insert_calloc(
			void );

/* Returns pair_one2m_many_folder_label */
/* ------------------------------------ */
char *pair_one2m_prompt_insert_hidden_name(
			char *pair_one2m_many_folder_label );

/* Public */
/* ------ */

/* Always succeeds */
/* --------------- */
LIST *pair_one2m_fulfilled_folder_name_list(
			char *pair_one2m_fulfilled_list_label,
			DICTIONARY *pair_one2m_dictionary );

/* Returns relation->many_folder_name or null */
/* ------------------------------------------ */
char *pair_one2m_next_folder_name(
			LIST *pair_one2m_fulfilled_folder_name_list,
			LIST *relation_one2m_pair_list );

/* Returns pair_one2m_fulfilled_list_label */
/* --------------------------------------- */
char *pair_one2m_fulfilled_list_key(
			char *pair_one2m_fulfilled_list_label );

/* Returns heap memory */
/* ------------------- */
char *pair_one2m_fulfilled_list_data(
			LIST *pair_one2m_fulfilled_folder_name_list,
			char *pair_one2m_next_folder_name,
			char sql_delimiter );

boolean pair_one2m_participating(
			DICTIONARY *pair_one2m_dictionary );

#endif

