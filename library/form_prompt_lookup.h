/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_lookup.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_LOOKUP_H
#define FORM_PROMPT_LOOKUP_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "security.h"
#include "radio.h"
#include "query.h"
#include "recall.h"
#include "relation_mto1.h"
#include "dictionary_separate.h"
#include "form_prompt_attribute.h"
#include "form.h"

#define FORM_PROMPT_LOOKUP_NAME			"prompt_lookup"
#define FORM_PROMPT_LOOKUP_FROM_MAX_LENGTH 	1024
#define FORM_PROMPT_LOOKUP_FROM_SIZE		25
#define FORM_PROMPT_LOOKUP_TO_SIZE		10

typedef struct
{
	LIST *widget_container_list;
} FORM_PROMPT_LOOKUP_YES_NO;

/* Usage */
/* ----- */
FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no_new(
		char *attribute_name,
		char *folder_attribute_prompt,
		char *hint_message,
		char *post_change_javascript,
		boolean no_display_boolean,
		LIST *form_prompt_lookup_relation_list );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no_calloc(
		void );

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	LIST *widget_container_list;
	WIDGET_CONTAINER *no_display_widget_container;
	WIDGET_CONTAINER *prompt_widget_container;
	QUERY_DROP_DOWN *query_drop_down;
	LIST *widget_drop_down_option_list;
	WIDGET_CONTAINER *drop_down_widget_container;
	WIDGET_CONTAINER *hint_message_widget_container;
} FORM_PROMPT_LOOKUP_RELATION;

/* Usage */
/* ----- */
FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		char *attribute_name,
		int primary_key_index,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		SECURITY_ENTITY *security_entity,
		boolean no_display_boolean,
		boolean drop_down_extra_options_boolean,
		LIST *form_prompt_lookup_relation_list );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_calloc(
		void );

boolean form_prompt_lookup_relation_attribute_exists(
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

typedef struct
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *no_display_widget_container;
	FORM_PROMPT_ATTRIBUTE *form_prompt_attribute;
} FORM_PROMPT_LOOKUP_ATTRIBUTE;

/* Usage */
/* ----- */
FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_new(
		char *application_name,
		char *login_name,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *hint_message,
		boolean no_display_boolean,
		LIST *form_prompt_lookup_relation_list );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_calloc(
		void );

typedef struct
{
	LIST *widget_container_list;
	LIST *form_prompt_lookup_relation_list;
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;
	LIST *join_widget_container_list;
} FORM_PROMPT_LOOKUP_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_LOOKUP_WIDGET_LIST *
	form_prompt_lookup_widget_list_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		SECURITY_ENTITY *security_entity,
		boolean no_display_boolean,
		boolean drop_down_extra_options_boolean );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_WIDGET_LIST *
	form_prompt_lookup_widget_list_calloc(
		void );

typedef struct
{
	char *form_tag;
	LIST *no_display_all_widget_list;
	LIST *radio_pair_list;
	RADIO_LIST *radio_list;
	char *action_string;
	FORM_PROMPT_LOOKUP_WIDGET_LIST *form_prompt_lookup_widget_list;
	LIST *widget_hidden_container_list;
	char *form_multi_select_all_javascript;
	RECALL_SAVE *recall_save;
	RECALL_LOAD *recall_load;
	LIST *button_list;
	char *html;
} FORM_PROMPT_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean prompt_lookup_omit_insert_button,
		boolean prompt_lookup_omit_delete_button,
		boolean prompt_lookup_include_chart_buttons,
		boolean prompt_lookup_include_sort_button,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		char *post_choose_folder_action_string );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_calloc(
		void );

LIST *form_prompt_lookup_radio_pair_list(
		boolean prompt_lookup_omit_insert_button,
		boolean prompt_lookup_omit_delete_button,
		boolean prompt_lookup_include_chart_buttons,
		boolean prompt_lookup_include_sort_button,
		int relation_mto1_list_length );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_action_string(
		char *post_prompt_lookup_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Usage */
/* ----- */
LIST *form_prompt_lookup_button_list(
		char *form_name,
		boolean button_drillthru_skip_boolean,
		char *post_choose_folder_action_string,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *recall_load_javascript );

/* Usage */
/* ----- */
LIST *form_prompt_lookup_join_container_list(
		char *dictionary_separate_no_display_prefix,
		LIST *relation_one2m_join_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_html(
		char *form_tag,
		LIST *form_prompt_lookup_no_display_all_widget_list,
		char *radio_list_html,
		LIST *widget_container_list,
		char *button_list_html,
		char *form_close_tag );

/* Usage */
/* ----- */

/* Note: ends with dangling table open tag */
/* --------------------------------------- */
LIST *form_prompt_lookup_no_display_all_widget_list(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_prompt_lookup_no_display_all_onclick(
		char *form_no_display_prefix );

#endif
