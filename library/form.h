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
#include "remember.h"
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

typedef struct
{
	LIST *value_list;
	char *html;
} FORM_RADIO_VALUE;

typedef struct
{
	char *radio_name;
	LIST *value_list;
	char *html;
} FORM_RADIO;

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
	LIST *form_radio_list;
	REMEMBER *remember;
	LIST *element_list;
} FORM_PROMPT;

/* FORM_RADIO_VALUE operations */
/* --------------------------- */
FORM_RADIO_VALUE *form_radio_value_calloc(
			void );

LIST *form_radio_value_list(
			char *radio_value,
			char *initial_value,
			LIST *value_string_list );

FORM_RADIO_VALUE *form_radio_value_new(
			char *radio_name,
			char *initial_value,
			char *value_string );

/* Returns heap memory or null */
/* --------------------------- */
char *form_radio_value_html(
			char *radio_name,
			char *initial_value,
			char *value_string );

/* FORM_RADIO operations */
/* --------------------- */
FORM_RADIO *form_radio_calloc(
			void );

FORM_RADIO *form_radio_new(
			char *radio_name,
			char *initial_value,
			LIST *value_string_list,
			char *set_all_push_buttons_html );

/* Returns heap memory or null */
/* --------------------------- */
char *form_radio_html(
			LIST *value_list,
			char *set_all_push_buttons_html );

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
			char *choose_isa_subtitle_html,
			char *tag_html,
			LIST *element_list,
			LIST *button_element_list );

/* FORM external operations */
/* ------------------------ */

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
char *form_close_html(
			void );

char *form_next_reference_number(
			int *form_current_reference_number );

#endif
