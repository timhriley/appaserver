/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_edit.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_EDIT_H
#define FORM_PROMPT_EDIT_H

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
#define FORM_PROMPT_EDIT_RELATIONAL_PREFIX \
					"form_prompt_edit_relational_"

#define FORM_PROMPT_EDIT_ORIGINAL_PREFIX \
					"original_"

#define FORM_PROMPT_EDIT_RELATION_PREFIX \
					"relation_"

#define FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX \
					"no_display_"

#define FORM_PROMPT_EDIT_FROM_ATTRIBUTE_WIDTH 100

#define FORM_PROMPT_EDIT_NAME		"prompt_edit"

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
} FORM_PROMPT_EDIT_RELATION;

/* FORM_PROMPT_EDIT_RELATION operations */
/* ------------------------------------ */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_new(
			char *attribute_name,
			int primary_key_index,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_edit_relation_list );

boolean form_prompt_edit_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_edit_relation_list );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_relation_name(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list );

char *form_prompt_edit_relation_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *form_prompt_edit_relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_relation_prompt(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list,
			int primary_key_index );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_relation_element_name(
			char *form_prompt_edit_relation_prefix,
			char *form_prompt_edit_relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_relation_original_name(
			char *form_prompt_edit_original_prefix,
			char *form_prompt_edit_relation_name );

/* Private */
/* ------- */
FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_calloc(
			void );

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
} FORM_PROMPT_EDIT_RELATIONAL;

/* FORM_PROMPT_EDIT_RELATIONAL operations */
/* -------------------------------------- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_new(
			char *form_prompt_edit_attribute_relational_name,
			char *form_prompt_edit_attribute_from_name,
			char *form_prompt_edit_attribute_to_name,
			char *datatype_name,
			int attribute_width );

LIST *form_prompt_edit_relational_operation_list(
			char *datatype_name );

/* Private */
/* ------- */
FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_calloc(
			void );

typedef struct
{
	LIST *element_list;
	char *no_display_name;
	APPASERVER_ELEMENT *no_display_appaserver_element;
	char *prompt;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	char *from_name;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	char *relational_name;
	char *to_name;
	FORM_PROMPT_EDIT_RELATIONAL *form_prompt_edit_relational;
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
