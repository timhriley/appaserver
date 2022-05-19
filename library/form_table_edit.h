/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_edit.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_TABLE_EDITH
#define FORM_TABLE_EDITH

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "form.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define FORM_TABLE_EDIT_NAME	"form_table_edit"

typedef struct
{
	char *tag;
	LIST *top_button_element_list;
	char *top_button_element_list_html;
	LIST *bottom_button_element_list;
	char *bottom_button_element_list_html;
	LIST *sort_checkbox_element_list;
	char *sort_checkbox_element_list_html;
	LIST *heading_element_list;
	char *heading_element_list_html;
	char *query_dictionary_hidden_html;
	char *sort_dictionary_hidden_html;
	char *drillthru_dictionary_hidden_html;
	char *no_display_dictionary_hidden_html;
	char *html;
	char *trailer_html;
} FORM_TABLE_EDIT;

/* FORM_TABLE_EDIT operations */
/* -------------------------- */
FORM_TABLE_EDIT *form_table_edit_calloc(
			void );

/* Always succeeds */
/* --------------- */
FORM_TABLE_EDIT *form_table_edit_new(
			char *folder_name,
			char *post_change_javascript,
			int dictionary_list_length,
			char *post_table_edit_action_string,
			LIST *role_operation_list,
			LIST *table_edit_heading_name_list,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *no_display_dictionary );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_tag(
			char *form_table_edit_name,
			char *post_table_edit_action_string,
			char *target_frame );

LIST *form_table_edit_button_element_list(
			char *post_change_javascript,
			int dictionary_list_length );

LIST *form_table_edit_sort_checkbox_element_list(
			char *folder_name,
			int operation_list_length,
			LIST *table_edit_heading_name_list );

LIST *form_table_edit_heading_element_list(
			LIST *role_operation_list,
			LIST *table_edit_heading_name_list );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_html(
			char *form_table_edit_tag,
			char *top_button_element_list_html,
			char *element_table_open_html,
			char *sort_checkbox_element_list_html,
			char *heading_element_list_html );

/* Returns heap memory */
/* ------------------- */
char *form_table_edit_trailer_html(
			char *bottom_button_element_list_html,
			char *query_dictionary_hidden_html,
			char *sort_dictionary_hidden_html,
			char *drillthru_dictionary_hidden_html,
			char *no_display_dictionary_hidden_html,
			char *form_close_html );

/* Private */
/* ------- */
APPASERVER_ELEMENT *form_table_edit_operation_checkbox_element(
			char *operation_name,
			char *delete_warning_javascript );

LIST *form_table_edit_operation_element_list(
			LIST *role_operation_list,
			boolean viewonly,
			char *form_delete_warning_javascript );

#endif
