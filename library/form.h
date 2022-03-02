/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_H
#define FORM_H

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
#define FORM_COLOR1			"#dccdde"
#define FORM_COLOR2			"#ffe6ca"
#define FORM_COLOR3			"#d2ecf2"
#define FORM_COLOR4			"#f7ffce"
#define FORM_COLOR5			"#f2e0b8"

#define FORM_MAX_BACKGROUND_COLOR_ARRAY	10
#define FORM_ROWS_BETWEEN_HEADINGS	10
#define FORM_SORT_ASCEND_LABEL		"assend_"
#define FORM_SORT_DESCEND_LABEL		"descend_"

#define FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER	'~'

#define FORM_LOOKUP_SUBTITLE_MESSAGE	\
	"Enter only the minimum information sufficient to search."

#define FORM_PROMPT_EDIT_RELATIONAL_PREFIX \
					"form_prompt_edit_relational_"

#define FORM_PROMPT_EDIT_FROM_PREFIX \
					"form_prompt_edit_from_"

#define FORM_PROMPT_EDIT_TO_PREFIX \
					"form_prompt_edit_to_"

#define FORM_PROMPT_EDIT_ORIGINAL_PREFIX \
					"original_"

#define FORM_PROMPT_EDIT_RELATION_PREFIX \
					"relation_"

#define FORM_PROMPT_EDIT_FROM_ATTRIBUTE_WIDTH 100

typedef struct
{
	char *folder_name;
	char *state;
	char *title_string;
	LIST *element_list;
} FORM_DETAIL;

typedef struct
{
	/* Process */
	/* ------- */
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
			char *javascript_replace,
			int dictionary_list_length,
			char *edit_table_submit_action_string,
			LIST *operation_list,
			LIST *edit_table_heading_name_list,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary );

/* Returns heap memory */
/* ------------------- */
char *form_edit_table_tag(
			char *edit_table_submit_action_string,
			char *target_frame );

LIST *form_edit_table_button_element_list(
			char *javascript_replace,
			int dictionary_list_length );

LIST *form_edit_table_sort_checkbox_element_list(
			char *folder_name,
			int operation_list_length,
			LIST *edit_table_heading_name_list );

LIST *form_edit_table_heading_element_list(
			LIST *operation_list,
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

typedef struct
{
	char *title;
	char *subtitle;
	char *action_string;
	LIST *form_button_bottom_list;
	RADIO_LIST *radio_list;
	LIST *element_list;
} FORM_PROMPT;

/* FORM_PROMPT operations */
/* ---------------------- */
FORM_PROMPT *form_new(
			char *state,
			char *application_title );

FORM_PROMPT *form_prompt_calloc(
			void );

typedef struct
{
	DICTIONARY *row_dictionary;
	LIST *element_list;
	int row_number;
	char *background_color;
} FORM_TABLE_ROW;

/* FORM_TABLE_ROW operations */
/* ------------------------- */
FORM_TABLE_ROW *form_table_row_new(
			DICTIONARY *row_dictionary,
			LIST *element_list,
			int row_number,
			char *background_color );

char *form_table_row_background_color(
			void );

char **form_table_row_background_color_array(
			int *background_color_array_length,
			char *application_name );

void form_table_row_output(
			FILE *output_stream,
			FORM_TABLE_ROW *form_table_row );

void form_table_row_free(
			FORM_TABLE_ROW *form_table_row );

typedef struct
{
	/* Process */
	/* ------- */
	char *tag_html;
	char *drop_down_onchange_javascript;
	LIST *element_list;
	char *html;
} FORM_CHOOSE_ROLE;

/* FORM_CHOOSE_ROLE operations */
/* --------------------------- */
FORM_CHOOSE_ROLE *form_choose_role_calloc(
			void );

FORM_CHOOSE_ROLE *form_choose_role_new(
			LIST *role_name_list,
			char *post_action_string,
			char *target_frame,
			char *form_name,
			char *drop_down_element_name );

/* Returns static memory */
/* --------------------- */
char *form_choose_role_drop_down_onchange_javascript(
			char *form_name );

LIST *form_choose_role_element_list(
			LIST *role_name_list,
			char *drop_down_onchange_javascript,
			char *drop_down_element_name );

/* Returns heap memory or null */
/* --------------------------- */
char *form_choose_role_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html );

typedef struct
{
	/* Process */
	/* ------- */
	char *message_html;
	char *tag_html;
	LIST *element_list;
	LIST *button_element_list;
	char *html;
} FORM_CHOOSE_ISA;

/* FORM_CHOOSE_ISA operations */
/* -------------------------- */
FORM_CHOOSE_ISA *form_choose_isa_calloc(
			void );

FORM_CHOOSE_ISA *form_choose_isa_new(
			char *prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *choose_isa_post_action_string );

LIST *form_choose_isa_element_list(
			char *choose_isa_prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital );

LIST *form_choose_isa_button_element_list(
			void );

/* Returns heap memory */
/* ------------------- */
char *form_choose_isa_html(
			char *tag_html,
			LIST *element_list,
			LIST *button_element_list,
			char *form_close_tag_html );

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
char *form_prompt_edit_relation_original_name(
			char *form_prompt_edit_original_prefix,
			char *form_prompt_edit_relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_relation_element_name(
			char *form_prompt_edit_relation_prefix,
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
			char *attribute_name,
			boolean primary_key_index,
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
	LIST *element_list;
	LIST *keystrokes_save_string;
	char *keystrokes_recall_string;
	LIST *button_element_list;
	char *html;
} FORM_PROMPT_EDIT;

/* FORM_PROMPT_EDIT operations */
/* --------------------------- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_EDIT *form_prompt_edit_new(
			char *prompt_edit_action_string,
			boolean prompt_edit_omit_insert_button,
			boolean prompt_edit_omit_delete_button,
			boolean prompt_edit_omit_new_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished );

/* Process */
/* ------- */
LIST *form_prompt_edit_radio_pair_list(
			boolean prompt_edit_omit_insert_button,
			boolean prompt_edit_omit_delete_button,
			boolean prompt_edit_omit_new_button,
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

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_keystrokes_save_string(
			LIST *form_prompt_edit_element_list );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_keystrokes_recall_string(
			LIST *form_prompt_edit_element_list );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_edit_html(
			char *tag_html,
			char *radio_list_html,
			char *element_list_html,
			char *button_element_list_html,
			char *form_close_html );

/* Private */
/* ------- */
FORM_PROMPT_EDIT *form_prompt_edit_calloc(
			void );

/* --------------- */
/* FORM operations */
/* --------------- */

/* Returns heap memory */
/* ------------------- */
char *form_title_html(
			char *title );

/* Returns heap memory */
/* ------------------- */
char *form_tag_html(	char *form_name,
			char *action_string,
			char *target_frame );

/* Returns program memory */
/* ---------------------- */
char *form_close_html(	void );

char *form_next_reference_number(
			int *form_current_reference_number );

/* Returns heap memory */
/* ------------------- */
char *form_element_list_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html );

#endif
