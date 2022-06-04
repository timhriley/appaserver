/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_insert.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_INSERT_H
#define FORM_PROMPT_INSERT_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "recall.h"
#include "pair_one2m.h"
#include "post_prompt_insert.h"
#include "form.h"

#define FORM_PROMPT_INSERT_NAME		"prompt_insert"

typedef struct
{
	LIST *element_list;
	char *ignore_name;
	APPASERVER_ELEMENT *ignore_appaserver_element;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	APPASERVER_ELEMENT *text_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_INSERT_ATTRIBUTE;

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_insert_relation_list,
			int tab_order );

/* Process */
/* ------- */
FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_calloc(
			void );

char *form_prompt_insert_attribute_ignore_name(
			char *form_prompt_insert_ignore_prefix,
			char *attribute_name );

typedef struct
{
	RELATION *relation;
	LIST *element_list;
	QUERY_WIDGET *query_widget;
	char *name;
	char *form_ignore_name;
	APPASERVER_ELEMENT *ignore_appaserver_element;
	char *prompt;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	APPASERVER_ELEMENT *drop_down_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_INSERT_RELATION;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_new(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_insert_relation_list,
			char *attribute_name,
			int tab_order );

/* Process */
/* ------- */
boolean form_prompt_insert_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_insert_relation_list );

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_calloc(
			void );

char *form_prompt_insert_relation_ignore_name(
			char *form_prompt_edit_ignore_prefix,
			char *relation_name );

typedef struct
{
	LIST *element_list;
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;
	LIST *form_prompt_insert_relation_list;
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;
	char *appaserver_element_list_html;
} FORM_PROMPT_INSERT_ELEMENT_LIST;

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_new(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *folder_attribute_append_isa_list,
			boolean role_folder_lookup );

/* Process */
/* ------- */
FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_calloc(
			void );

LIST *form_prompt_insert_element_list_lookup_list(
			char *prompt_insert_lookup_name,
			char *prompt_insert_lookup_label );

typedef struct
{
	char *action_string;
	char *form_tag_html;
	FORM_PROMPT_INSERT_ELEMENT_LIST *form_prompt_insert_element_list;
	char *form_multi_select_all_javascript;
	char *form_verify_notepad_widths_javascript;
	RECALL *recall;
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;
	LIST *button_list;
	char *hidden_html;
	char *html;
} FORM_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_INSERT *form_prompt_insert_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *security_entity_where,
			boolean role_folder_lookup,
			LIST *relation_pair_one2m_list );

/* Process */
/* ------- */
FORM_PROMPT_INSERT *form_prompt_insert_calloc(
			void );

char *form_prompt_insert_action_string(
			char *post_prompt_insert_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name );

LIST *form_prompt_insert_button_list(
			char *form_multi_select_all_javascript,
			char *recall_save_javascript,
			char *recall_load_javascript,
			char *form_verify_notepad_widths_javascript,
			LIST *pair_one2m_button_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_prompt_insert_hidden_html(
			APPASERVER_ELEMENT *transmit_hidden_element );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_insert_html(
			char *form_tag_html,
			char *form_prompt_insert_element_list_html,
			char *button_list_html,
			char *form_prompt_insert_hidden_html,
			char *form_close_html );

#endif
