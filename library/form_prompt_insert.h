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
#include "form.h"

/* Constants */
/* --------- */
#define FORM_PROMPT_INSERT_NAME		"prompt_insert"

#define FORM_PROMPT_INSERT_IGNORE_PREFIX \
					"ignore_"

typedef struct
{
	RELATION *relation;
	LIST *element_list;
	QUERY_WIDGET *query_widget;
	char *name;
	char *no_display_name;
	APPASERVER_ELEMENT *ignore_appaserver_element;
	char *prompt;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	char *original_name;
	char *element_name;
	APPASERVER_ELEMENT *drop_down_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_INSERT_RELATION;

/* FORM_PROMPT_INSERT_RELATION operations */
/* ------------------------------------ */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_insert_relation_list );

/* Process */
/* ------- */
boolean form_prompt_insert_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_insert_relation_list );

FORM_PROMPT_INSERT_RELATION *form_prompt_edit_relation_calloc(
			void );

char *form_prompt_insert_relation_ignore_name(
			char *form_prompt_edit_ignore_prefix,
			char *relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_insert_relation_element_name(
			char *form_prompt_edit_relation_prefix,
			char *relation_name );

typedef struct
{
	LIST *element_list;
	char *ignore_name;
	APPASERVER_ELEMENT *ignore_appaserver_element;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	APPASERVER_ELEMENT *text_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_EDIT_ATTRIBUTE;

/* FORM_PROMPT_EDIT_ATTRIBUTE operations */
/* ------------------------------------- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_new(
			char *attribute_name,
			int primary_key_index,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_edit_relation_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_attribute_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_attribute_prompt(
			char *attribute_name,
			int primary_key_index );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_attribute_from_name(
			char *form_prompt_edit_from_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_attribute_relational_name(
			char *form_prompt_edit_attribute_relational_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_attribute_to_name(
			char *form_prompt_edit_to_prefix,
			char *attribute_name );

/* Private */
/* ------- */
FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_calloc(
			void );

typedef struct
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;
	LIST *form_prompt_edit_relation_list;
	FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute;
	LIST *element_list;
	LIST *join_element_list;
	char *element_list_html;
} FORM_PROMPT_EDIT_ELEMENT_LIST;

/* FORM_PROMPT_EDIT_ELEMENT_LIST operations */
/* ---------------------------------------- */
FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where );

LIST *form_prompt_edit_element_list_join_element_list(
			char *form_prompt_edit_no_display_prefix,
			LIST *relation_join_one2m_list );

/* Private */
/* ------- */
FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_calloc(
			void );

typedef struct
{
	LIST *radio_pair_list;
	RADIO_LIST *radio_list;
	char *target_frame;
	char *tag_html;
	FORM_PROMPT_EDIT_ELEMENT_LIST *form_prompt_edit_element_list;
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
} FORM_PROMPT_EDIT;

/* FORM_PROMPT_EDIT operations */
/* --------------------------- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_EDIT *form_prompt_edit_new(
			char *folder_name,
			char *prompt_edit_action_string,
			boolean prompt_edit_omit_insert_button,
			boolean prompt_edit_omit_delete_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished );

/* Process */
/* ------- */
LIST *form_prompt_edit_radio_pair_list(
			boolean prompt_edit_omit_insert_button,
			boolean prompt_edit_omit_delete_button,
			int relation_mto1_non_isa_list_length );

/* Returns frameset_prompt_frame or frameset_edit_frame */
/* ---------------------------------------------------- */
char *form_prompt_edit_target_frame(
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished,
			char *frameset_prompt_frame,
			char *frameset_edit_frame );

LIST *form_prompt_edit_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary );

LIST *form_prompt_edit_button_list(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript,
			char *form_verify_notepad_widths_javascript );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_html(
			char *tag_html,
			char *radio_list_html,
			char *form_prompt_edit_element_list_html,
			char *button_list_html,
			char *form_close_html );

/* Private */
/* ------- */
FORM_PROMPT_EDIT *form_prompt_edit_calloc(
			void );

#endif
