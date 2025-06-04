/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_insert.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_TABLE_INSERT_H
#define FORM_TABLE_INSERT_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "query.h"
#include "security.h"
#include "relation.h"
#include "ajax.h"
#include "form.h"

#define FORM_TABLE_INSERT_FORM_NAME		"form_table_insert"
#define FORM_TABLE_INSERT_DEFAULT_ROWS_NUMBER	5

typedef struct
{
	WIDGET_CONTAINER *widget_container;
	LIST *widget_container_list;
} FORM_TABLE_INSERT_ATTRIBUTE;

/* Usage */
/* ----- */
FORM_TABLE_INSERT_ATTRIBUTE *
	form_table_insert_attribute_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		LIST *form_table_insert_relation_list );

/* Process */
/* ------- */
FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute_calloc(
		void );

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	AJAX_CLIENT *ajax_client;
	LIST *relation_mto1_common_name_list;
	QUERY_DROP_DOWN *query_drop_down;
	WIDGET_CONTAINER *drop_down_widget_container;
	LIST *widget_container_list;
} FORM_TABLE_INSERT_RELATION;

/* Usage */
/* ----- */
FORM_TABLE_INSERT_RELATION *form_table_insert_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *folder_attribute_non_primary_name_list,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *process_command_line_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *form_table_insert_relation_list,
		char *attribute_name,
		int primary_key_index );

/* Process */
/* ------- */
FORM_TABLE_INSERT_RELATION *form_table_insert_relation_calloc(
		void );

/* Usage */
/* ----- */
boolean form_table_insert_relation_attribute_exists(
		char *attribute_name,
		LIST *form_table_insert_relation_list );

/* Usage */
/* ----- */
AJAX_CLIENT *form_table_insert_relation_ajax_client(
		char *role_name,
		RELATION_MTO1 *relation_mto1 );

/* Usage */
/* ----- */
LIST *form_table_insert_relation_ajax_client_list(
		LIST *form_table_insert_relation_list );

typedef struct
{
	LIST *widget_container_list;
	DICTIONARY *process_command_line_dictionary;
	LIST *form_table_insert_relation_list;
	LIST *form_table_insert_attribute_list;
} FORM_TABLE_INSERT_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_TABLE_INSERT_WIDGET_LIST *
	form_table_insert_widget_list_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *folder_attribute_non_primary_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		LIST *ignore_name_list,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity );

/* Process */
/* ------- */
FORM_TABLE_INSERT_WIDGET_LIST *form_table_insert_widget_list_calloc(
		void );

typedef struct
{
	WIDGET_CONTAINER *non_edit_text_widget_container;
	WIDGET_CONTAINER *hidden_widget_container;
	LIST *form_table_insert_relation_list;
	FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute;
	LIST *widget_container_list;
} FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
	form_table_insert_automatic_widget_list_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *process_command_line_dictionary,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity,
		char *relation_mto1_automatic_preselection_name );

/* Process */
/* ------- */
FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
	form_table_insert_automatic_widget_list_calloc(
		void );

typedef struct
{
	char *form_table_insert_action_string;
	char *form_tag;
	DICTIONARY *process_command_line_dictionary;
	QUERY_DROP_DOWN *query_drop_down;
	FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
		form_table_insert_automatic_widget_list;
	LIST *widget_container_heading_label_list;
	LIST *widget_container_heading_list;
	char *prompt_hidden_html;
	char *pair_one2m_hidden_html;
	LIST *form_table_insert_button_list;
	char *button_list_html;
	char *html;
} FORM_TABLE_INSERT_AUTOMATIC;

/* Usage */
/* ----- */
FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *pair_one2m_dictionary,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity,
		RELATION_MTO1 *relation_mto1_automatic_preselection,
		LIST *query_dictionary_delimited_list );

/* Process */
/* ------- */
FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_automatic_message(
		char *delimited_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_automatic_html(
		boolean no_initial_capital,
		LIST *query_dictionary_delimited_list,
		char *form_tag,
		char *widget_table_open_tag,
		LIST *widget_container_heading_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list
			/* in/out */,
		char *widget_table_close_tag,
		char *prompt_hidden_html,
		char *pair_one2m_hidden_html,
		char *button_list_html,
		char *form_close_tag );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_automatic_widget_html(
		boolean no_initial_capital,
		LIST *query_dictionary_delimited_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list
			/* in/out */ );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_automatic_hidden_html(
		LIST *query_dictionary_delimited_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list
			/* in/out */ );

/* Returns static memory or delimited_string */
/* ----------------------------------------- */
char *form_table_insert_automatic_key(
		char query_key_extra_delimiter,
		char *delimited_string );

typedef struct
{
	char *action_string;
	char *form_tag;
	FORM_TABLE_INSERT_WIDGET_LIST *form_table_insert_widget_list;
	LIST *widget_container_heading_label_list;
	LIST *widget_container_heading_list;
	char *prompt_hidden_html;
	char *pair_one2m_hidden_html;
	LIST *button_list;
	char *button_list_html;
	char *html;
} FORM_TABLE_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_TABLE_INSERT *form_table_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		int table_insert_rows_number,
		char *post_change_javascript,
		LIST *folder_attribute_non_primary_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *pair_one2m_dictionary,
		LIST *ignore_name_list,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity );

/* Process */
/* ------- */
FORM_TABLE_INSERT *form_table_insert_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_action_string(
		char *post_table_insert_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame );

LIST *form_table_insert_button_list(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_table_insert_html(
		int form_table_insert_default_rows_number,
		char *session_key,
		char *login_name,
		char *role_name,
		int table_insert_rows_number,
		char *form_tag,
		LIST *form_table_insert_relation_list,
		char *widget_table_open_tag,
		LIST *widget_container_heading_list,
		LIST *widget_container_list,
		char *widget_table_close_tag,
		char *prompt_hidden_html,
		char *pair_one2m_hidden_html,
		char *button_list_html,
		char *form_close_tag );

#endif
