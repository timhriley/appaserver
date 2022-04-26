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
	LIST *element_list;
	char *ignore_name;
	APPASERVER_ELEMENT *ignore_appaserver_element;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	APPASERVER_ELEMENT *text_appaserver_element;
	APPASERVER_ELEMENT *non_edit_text_appaserver_element;
} FORM_PROMPT_INSERT_ATTRIBUTE;

/* FORM_PROMPT_INSERT_ATTRIBUTE operations */
/* --------------------------------------- */

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_insert_relation_list );

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

typedef struct
{
	LIST *element_list;
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;
	LIST *form_prompt_insert_relation_list;
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;
	char *appaserver_element_list_html;
} FORM_PROMPT_INSERT_ELEMENT_LIST;

/* FORM_PROMPT_INSERT_ELEMENT_LIST operations */
/* ------------------------------------------ */

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where );

/* Process */
/* ------- */
FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_calloc(
			void );

typedef struct
{
	char *target_frame;
	char *tag_html;
	FORM_PROMPT_INSERT_ELEMENT_LIST *form_prompt_insert_element_list;
	char *form_cookie_key;
	char *form_cookie_multi_key;
	char *form_keystrokes_save_javascript;
	char *form_keystrokes_recall_javascript;
	char *form_verify_notepad_widths_javascript;
	LIST *button_list;
	char *html;
} FORM_PROMPT_INSERT;

/* FORM_PROMPT_INSERT operations */
/* ----------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_INSERT *form_prompt_insert_new(
			char *folder_name,
			char *prompt_insert_action_string,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			boolean drillthru_participating,
			boolean drillthru_finished );

/* Process */
/* ------- */
FORM_PROMPT_INSERT *form_prompt_insert_calloc(
			void );

LIST *form_prompt_insert_radio_pair_list(
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

#endif
