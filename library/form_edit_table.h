/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_edit_table.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_EDIT_TABLEH
#define FORM_EDIT_TABLEH

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define FORM_EDIT_TABLE_NAME	"form_edit_table"

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
	char *ignore_dictionary_hidden_html;
	char *html;
	char *trailer_html;
} FORM_EDIT_TABLE;

/* FORM_EDIT_TABLE operations */
/* -------------------------- */
FORM_EDIT_TABLE *form_edit_table_calloc(
			void );

/* Always succeeds */
/* --------------- */
FORM_EDIT_TABLE *form_edit_table_new(
			char *folder_name,
			char *post_change_javascript,
			int dictionary_list_length,
			char *post_edit_table_action_string,
			LIST *role_operation_list,
			LIST *edit_table_heading_name_list,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary );

/* Returns heap memory */
/* ------------------- */
char *form_edit_table_tag(
			char *form_edit_table_name,
			char *post_edit_table_action_string,
			char *target_frame,
			char *element_table_open_html );

LIST *form_edit_table_button_element_list(
			char *post_change_javascript,
			int dictionary_list_length );

LIST *form_edit_table_sort_checkbox_element_list(
			char *folder_name,
			int operation_list_length,
			LIST *edit_table_heading_name_list );

LIST *form_edit_table_heading_element_list(
			LIST *role_operation_list,
			LIST *edit_table_heading_name_list );

/* Returns heap memory */
/* ------------------- */
char *form_edit_table_html(
			char *form_edit_table_tag,
			char *top_button_element_list_html,
			char *sort_checkbox_element_list_html,
			char *heading_element_list_html );

/* Returns heap memory */
/* ------------------- */
char *form_edit_table_trailer_html(
			char *bottom_button_element_list_html,
			char *element_table_close_html,
			char *query_dictionary_hidden_html,
			char *sort_dictionary_hidden_html,
			char *drillthru_dictionary_hidden_html,
			char *ignore_dictionary_hidden_html,
			char *form_close_html );

#endif
