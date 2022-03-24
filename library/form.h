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

#define FORM_ROWS_BETWEEN_HEADINGS	10
#define FORM_SORT_ASCEND_LABEL		"assend_"
#define FORM_SORT_DESCEND_LABEL		"descend_"

#define FORM_LOOKUP_SUBTITLE_MESSAGE	\
	"Enter only the minimum information sufficient to search."

#define FORM_RADIO_LIST_NAME		"form_radio_list"

typedef struct
{
	char *folder_name;
	char *state;
	char *title_string;
	LIST *element_list;
} FORM_DETAIL;

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

/* Returns static memory */
/* --------------------- */
char *form_cookie_key(	char *form_name,
			char *folder_name );

/* Returns static memory */
/* --------------------- */
char *form_cookie_multi_key(
			char *form_name,
			char *folder_name );

/* Returns heap memory or null */
/* --------------------------- */
char *form_keystrokes_save_javascript(
			char *form_name,
			char *form_cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_keystrokes_multi_save_javascript(
			char *form_name,
			char *form_cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_keystrokes_recall_javascript(
			char *form_name,
			char *form_cookie_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_keystrokes_multi_recall_javascript(
			char *form_name,
			char *form_cookie_multi_key,
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_multi_select_all_javascript(
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *form_verify_notepad_widths_javascript(
			char *form_name,
			LIST *element_list );

#endif
