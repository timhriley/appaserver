/* APPASERVER_HOME/library/pair_one2m.h			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PAIR_ONE2M_H
#define PAIR_ONE2M_H

#include "boolean.h"
#include "list.h"
#include "String.h"
#include "piece.h"
#include "folder.h"
#include "sql.h"
#include "dictionary.h"
#include "widget.h"
#include "button.h"

#define PAIR_ONE2M_COMPLETE_HTML	"<h2>Insert Complete</h2>"
#define PAIR_ONE2M_ONE_FOLDER_KEY	"pair_one2m_one_folder"
#define PAIR_ONE2M_MANY_FOLDER_KEY	"pair_one2m_many_folder"
#define PAIR_ONE2M_UNFULFILLED_LIST_KEY	"pair_one2m_unfulfilled_list"
#define PAIR_ONE2M_FULFILLED_LIST_KEY	"pair_one2m_fulfilled_list"
#define PAIR_ONE2M_FOLDER_DELIMITER	','

typedef struct
{
	WIDGET_CONTAINER *widget_container;
	char *action_string;
} PAIR_ONE2M_BUTTON;

/* Usage */
/* ----- */
LIST *pair_one2m_button_list(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		LIST *relation_one2m_pair_list );

/* Usage */
/* ----- */
PAIR_ONE2M_BUTTON *pair_one2m_button_new(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		char *many_folder_name );

/* Process */
/* ------- */
PAIR_ONE2M_BUTTON *pair_one2m_button_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *pair_one2m_button_action_string(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		char *many_folder_name );

typedef struct
{
	char *one_folder_name;
	char *many_folder_name;
	char *unfulfilled_list_string;
	LIST *unfulfilled_list;
	char *next_folder;
} PAIR_ONE2M_POST_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert_new(
		const char *pair_one2m_one_folder_key,
		const char *pair_one2m_many_folder_key,
		const char *pair_one2m_unfulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		DICTIONARY *pair_one2m_dictionary );

/* Process */
/* ------- */
PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert_calloc(
		void );

/* Returns many_folder_name, a component of unfulfilled_list, or null */
/* ------------------------------------------------------------------ */
char *pair_one2m_post_prompt_insert_next_folder(
		char *many_folder_name,
		LIST *unfulfilled_list );

typedef struct
{
	DICTIONARY *pair_one2m_dictionary;
	char *unfulfilled_list_string;
	LIST *pair_one2m_button_list;
	WIDGET_CONTAINER *hidden_widget;
} PAIR_ONE2M_PROMPT_INSERT;

/* Usage */
/* ----- */
PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_new(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_one_folder_key,
		const char *pair_one2m_many_folder_key,
		const char *pair_one2m_unfulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		char *one_folder_name,
		char *recall_save_javascript,
		LIST *relation_one2m_pair_list );

/* Process */
/* ------- */
PAIR_ONE2M_PROMPT_INSERT *
	pair_one2m_prompt_insert_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *pair_one2m_prompt_insert_unfulfilled_list_string(
		const char pair_one2m_folder_delimiter,
		LIST *relation_one2m_pair_list );

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *pair_one2m_prompt_insert_hidden_widget(
		char *dictionary_separate_pair_prefix,
		char *pair_one2m_many_folder_key );

typedef struct
{
	LIST *fulfilled_list;
	LIST *unfulfilled_list;
	char *fulfilled_list_string;
	char *unfulfilled_list_string;
	char *next_folder_name;
} PAIR_ONE2M_POST_TABLE_INSERT;

/* Usage */
/* ----- */
PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert_new(
		DICTIONARY *pair_one2m_dictionary /* in/out */,
		const char *pair_one2m_unfulfilled_list_key,
		const char *pair_one2m_fulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		char *many_folder_name );

/* Process */
/* ------- */
PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert_calloc(
		void );


/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *pair_one2m_post_table_insert_fulfilled_list(
		DICTIONARY *pair_one2m_dictionary,
		char *pair_one2m_fulfilled_list_key,
		char pair_one2m_folder_delimiter );

/* Usage */
/* ----- */
LIST *pair_one2m_post_table_insert_unfulfilled_list(
		DICTIONARY *pair_one2m_dictionary,
		char *pair_one2m_unfulfilled_list_key,
		char pair_one2m_folder_delimiter );

/* Usage */
/* ----- */
char *pair_one2m_post_table_insert_next_folder_name(
		LIST *pair_one2m_post_table_insert_unfulfilled_list );

typedef struct
{
	char *one_folder_name;
	LIST *folder_attribute_primary_key_list;
} PAIR_ONE2M_TABLE_INSERT;

/* Usage */
/* ----- */
PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert_new(
		const char *pair_one2m_one_folder_key,
		DICTIONARY *pair_one2m_dictionary );

/* Process */
/* ------- */
PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert_calloc(
		void );

#endif

