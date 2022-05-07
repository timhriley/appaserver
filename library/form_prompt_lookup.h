/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_lookup.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_LOOKUP_H
#define FORM_PROMPT_LOOKUP_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "form.h"

/* Constants */
/* --------- */
#define FORM_PROMPT_LOOKUP_RELATIONAL_PREFIX \
					"form_prompt_lookup_relational_"

#define FORM_PROMPT_LOOKUP_ORIGINAL_PREFIX \
					"original_"

#define FORM_PROMPT_LOOKUP_RELATION_PREFIX \
					"relation_"

#define FORM_PROMPT_LOOKUP_FROM_ATTRIBUTE_WIDTH 100

#define FORM_PROMPT_LOOKUP_NAME		"prompt_lookup"

/* This is a query row having a mto1 relation drop-down. */
/* ----------------------------------------------------- */
typedef struct
{
	RELATION *relation;
	LIST *element_list;
	QUERY_WIDGET *query_widget;
	char *name;
	char *no_display_name;
	APPASERVER_ELEMENT *no_display_appaserver_element;
	char *prompt;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	char *original_name;
	char *element_name;
	APPASERVER_ELEMENT *drop_down_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_LOOKUP_RELATION;

/* FORM_PROMPT_LOOKUP_RELATION operations */
/* ------------------------------------ */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_lookup_relation_list );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_calloc(
			void );

boolean form_prompt_lookup_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_lookup_relation_list );

char *form_prompt_lookup_relation_no_display_name(
			char *form_prompt_lookup_no_display_prefix,
			char *relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_relation_prompt(
			char *relation_name,
			int primary_key_index );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_relation_element_name(
			char *form_prompt_lookup_relation_prefix,
			char *relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_relation_original_name(
			char *form_prompt_lookup_original_prefix,
			char *relation_name );

/* This is a query row having a relational operation drop-down. */
/* ------------------------------------------------------------ */
typedef struct
{
	LIST *element_list;
	LIST *operation_list;
	APPASERVER_ELEMENT *relation_operator_appaserver_element;
	APPASERVER_ELEMENT *text_from_appaserver_element;
	APPASERVER_ELEMENT *and_appaserver_element;
	APPASERVER_ELEMENT *text_to_appaserver_element;
} FORM_PROMPT_LOOKUP_RELATIONAL;

/* FORM_PROMPT_LOOKUP_RELATIONAL operations */
/* -------------------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP_RELATIONAL *
	form_prompt_lookup_relational_new(
			char *form_prompt_lookup_attribute_relational_name,
			char *form_prompt_lookup_attribute_from_name,
			char *form_prompt_lookup_attribute_to_name,
			char *datatype_name,
			int attribute_width );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_RELATIONAL *
	form_prompt_lookup_relational_calloc(
			void );
LIST *form_prompt_lookup_relational_operation_list(
			char *datatype_name );

typedef struct
{
	LIST *element_list;
	char *no_display_name;
	APPASERVER_ELEMENT *no_display_appaserver_element;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	char *from_name;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	char *relational_name;
	char *to_name;
	FORM_PROMPT_LOOKUP_RELATIONAL *form_prompt_lookup_relational;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_LOOKUP_ATTRIBUTE;

/* FORM_PROMPT_LOOKUP_ATTRIBUTE operations */
/* ------------------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_lookup_relation_list );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_attribute_no_display_name(
			char *form_prompt_lookup_no_display_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_attribute_from_name(
			char *form_prompt_lookup_from_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_attribute_relational_name(
			char *form_prompt_lookup_attribute_relational_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_attribute_to_name(
			char *form_prompt_lookup_to_prefix,
			char *attribute_name );

typedef struct
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;
	LIST *form_prompt_lookup_relation_list;
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;
	LIST *element_list;
	LIST *join_element_list;
	char *appaserver_element_list_html;
} FORM_PROMPT_LOOKUP_ELEMENT_LIST;

/* FORM_PROMPT_LOOKUP_ELEMENT_LIST operations */
/* ---------------------------------------- */

/* Usage */
/* ----- */
FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_calloc(
			void );

LIST *form_prompt_lookup_element_list_join_element_list(
			char *form_prompt_lookup_no_display_prefix,
			LIST *relation_join_one2m_list );

typedef struct
{
	LIST *radio_pair_list;
	RADIO_LIST *radio_list;
	char *target_frame;
	char *action_string;
	char *form_tag_html;
	FORM_PROMPT_LOOKUP_ELEMENT_LIST *form_prompt_lookup_element_list;
	char *form_multi_select_all_javascript;
	char *form_cookie_key;
	char *form_cookie_multi_key;
	char *form_keystrokes_save_javascript;
	char *form_keystrokes_multi_save_javascript;
	char *form_keystrokes_recall_javascript;
	char *form_keystrokes_multi_recall_javascript;
	char *form_verify_notepad_widths_javascript;
	LIST *button_list;
	char *html;
} FORM_PROMPT_LOOKUP;

/* FORM_PROMPT_LOOKUP operations */
/* --------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			boolean prompt_lookup_omit_insert_button,
			boolean prompt_lookup_omit_delete_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *security_entity_where );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_calloc(
			void );

LIST *form_prompt_lookup_radio_pair_list(
			boolean prompt_lookup_omit_insert_button,
			boolean prompt_lookup_omit_delete_button,
			int relation_mto1_non_isa_list_length );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_action_string(
			char *post_prompt_lookup_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name );

LIST *form_prompt_lookup_button_list(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript,
			char *form_verify_notepad_widths_javascript );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_html(
			char *form_tag_html,
			char *radio_list_html,
			char *form_prompt_lookup_element_list_html,
			char *button_list_html,
			char *form_close_html );

#endif
