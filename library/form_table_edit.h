/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_edit.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_TABLE_EDIT_H
#define FORM_TABLE_EDIT_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "radio.h"
#include "query.h"
#include "form.h"
#include "dictionary_separate.h"

#define FORM_TABLE_EDIT_LABEL		"form_table_edit"
#define FORM_TABLE_EDIT_MINIMUM_SORT	2

typedef struct
{
	int form_increment_number;
	char *form_name;
	LIST *top_button_widget_container_list;
	LIST *bottom_button_widget_container_list;
	boolean sort_buttons_boolean;
	LIST *sort_container_list;
	LIST *heading_container_list;
	char *heading_container_string;
	char *query_dictionary_hidden_html;
	char *drillthru_dictionary_hidden_html;
	char *no_display_dictionary_hidden_html;
	char *form_tag;
	char *open_html;
	char *close_html;
	LIST *blank_folder_operation_list;
} FORM_TABLE_EDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_TABLE_EDIT *form_table_edit_new(
		char *folder_name,
		char *target_frame,
		char *post_change_javascript,
		DICTIONARY *query_dictionary,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *no_display_dictionary,
		LIST *folder_operation_list,
		int table_edit_query_row_list_length,
		char *table_edit_post_action_string,
		LIST *table_edit_heading_label_list,
		LIST *table_edit_heading_name_list );

/* Process */
/* ------- */
FORM_TABLE_EDIT *form_table_edit_calloc(
		void );

boolean form_table_edit_sort_buttons_boolean(
		int form_table_edit_minimum_sort,
		int table_edit_query_row_list_length );

LIST *form_table_edit_sort_container_list(
		char *form_name,
		int folder_operation_list_length,
		LIST *table_edit_heading_name_list );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_heading_container_string(
		LIST *form_table_edit_heading_container_list );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_open_html(
		char *form_tag,
		LIST *top_button_widget_container_list,
		char *table_tag,
		LIST *form_table_edit_sort_container_list,
		char *form_table_edit_heading_container_string );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_close_html(
		LIST *bottom_button_widget_container_list,
		char *query_dictionary_hidden_html,
		char *drillthru_dictionary_hidden_html,
		char *no_display_dictionary_hidden_html,
		char *form_close_tag );

/* Usage */
/* ----- */
LIST *form_table_edit_button_widget_container_list(
		char *form_name,
		char *post_change_javascript,
		boolean button_to_top_boolean );

/* Usage */
/* ----- */
LIST *form_table_edit_blank_folder_operation_list(
		int folder_operation_list_length );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_table_edit_heading_name(
		char sql_delimiter,
		char *form_sort_label,
		char *heading_name );

/* Usage */
/* ----- */
LIST *form_table_edit_heading_container_list(
		DICTIONARY *no_display_dictionary,
		LIST *folder_operation_list,
		LIST *table_edit_heading_label_list,
		char *form_name );

/* Usage */
/* ----- */
WIDGET_CONTAINER *form_table_edit_heading_checkbox_container(
		char *form_name,
		char *operation_name,
		WIDGET_CONTAINER *input_widget_container,
		char *delete_warning_javascript );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
char *form_table_edit_heading_checkbox_string(
		char *operation_name,
		char *checkbox_prompt );

#endif
