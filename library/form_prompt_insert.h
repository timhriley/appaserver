/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_insert.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_INSERT_H
#define FORM_PROMPT_INSERT_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "security.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "recall.h"
#include "pair_one2m.h"
#include "relation_mto1.h"
#include "vertical_new_checkbox.h"
#include "post_prompt_insert.h"
#include "ajax.h"
#include "form.h"

#define FORM_PROMPT_INSERT_NAME			"prompt_insert"
#define FORM_PROMPT_INSERT_IGNORE_PROMPT	"Ignore"

typedef struct
{
	WIDGET_CONTAINER *ignore_widget_container;
	WIDGET_CONTAINER *prompt_widget_container;
	WIDGET_CONTAINER *widget_container;
	WIDGET_CONTAINER *hint_message_widget_container;
	LIST *widget_container_list;
	boolean attribute_is_notepad;
	boolean attribute_is_password;
} FORM_PROMPT_INSERT_ATTRIBUTE;

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *hint_message,
		boolean form_prompt_insert_ignore_boolean,
		LIST *form_prompt_insert_relation_list,
		int tab_order );

/* Process */
/* ------- */
FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_calloc(
		void );

LIST *form_prompt_insert_button_container_list(
		char *post_choose_folder_action_string,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *recall_load_javascript,
		char *form_verify_notepad_widths_javascript,
		LIST *pair_one2m_button_list );

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	WIDGET_CONTAINER *ignore_widget_container;
	boolean role_folder_insert_boolean;
	VERTICAL_NEW_CHECKBOX *vertical_new_checkbox;
	LIST *relation_mto1_to_one_list;
	AJAX_CLIENT *ajax_client;
	QUERY_DROP_DOWN *query_drop_down;
	LIST *widget_drop_down_option_list;
	WIDGET_CONTAINER *prompt_widget_container;
	WIDGET_CONTAINER *drop_down_widget_container;
	WIDGET_CONTAINER *hint_message_widget_container;
	LIST *widget_container_list;
} FORM_PROMPT_INSERT_RELATION;

/* Usage */
/* ----- */
FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *role_folder_insert_list,
		LIST *form_prompt_insert_relation_list,
		char *attribute_name,
		boolean form_prompt_insert_ignore_boolean,
		int tab_order );

/* Process */
/* ------- */
boolean form_prompt_insert_relation_attribute_exists(
		char *attribute_name,
		LIST *form_prompt_insert_relation_list );

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_calloc(
		void );

/* Usage */
/* ----- */
void form_prompt_insert_relation_javascript_save_set(
		LIST *form_prompt_insert_relation_list,
		char *widget_checkbox_submit_javascript );

/* Usage */
/* ----- */
LIST *form_prompt_insert_relation_ajax_client_list(
		LIST *form_prompt_insert_relation_list );

typedef struct
{
	LIST *role_folder_insert_list;
	LIST *widget_container_list;
	LIST *form_prompt_insert_relation_list;
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;
	LIST *widget_hidden_container_list;
} FORM_PROMPT_INSERT_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_INSERT_WIDGET_LIST *
	form_prompt_insert_widget_list_new(
		const char *prompt_insert_lookup_checkbox,
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *folder_attribute_append_isa_list,
		boolean role_folder_lookup_boolean,
		boolean form_prompt_insert_ignore_boolean );

/* Process */
/* ------- */
FORM_PROMPT_INSERT_WIDGET_LIST *
	form_prompt_insert_widget_list_calloc(
		void );

LIST *form_prompt_insert_widget_list_lookup_list(
		const char *prompt_insert_lookup_checkbox );

typedef struct
{
	char *action_string;
	char *form_tag;
	int relation_one2m_pair_list_length;
	boolean ignore_boolean;
	FORM_PROMPT_INSERT_WIDGET_LIST *form_prompt_insert_widget_list;
	char *form_multi_select_all_javascript;
	char *form_verify_notepad_widths_javascript;
	RECALL_SAVE *recall_save;
	RECALL_LOAD *recall_load;
	char *widget_checkbox_submit_javascript;
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;
	LIST *button_container_list;
	char *button_list_html;
	char *html;
} FORM_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_INSERT *form_prompt_insert_new(
		const char *prompt_insert_lookup_checkbox,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		boolean role_folder_lookup_boolean,
		LIST *relation_one2m_pair_list,
		char *post_choose_folder_action_string );

/* Process */
/* ------- */
FORM_PROMPT_INSERT *form_prompt_insert_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_insert_action_string(
		const char *post_prompt_insert_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

boolean form_prompt_insert_ignore_boolean(
		int relation_one2m_pair_list_length );

LIST *form_prompt_insert_button_list(
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *recall_load_javascript,
		char *form_verify_notepad_widths_javascript,
		LIST *pair_one2m_button_list );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_insert_button_list_html(
		LIST *form_prompt_insert_button_container_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_insert_html(
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *form_prompt_insert_relation_list,
		char *form_tag,
		LIST *widget_container_list,
		char *button_container_list_html,
		DICTIONARY *pair_one2m_dictionary,
		char *form_close_tag );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *form_prompt_insert_capacity_message(
		const char *function_name,
		const int max_strlen,
		const double document_capacity_threshold,
		char *folder_name,
		int strlen_html );

#endif
