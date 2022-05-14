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
#define FORM_MAX_BACKGROUND_COLOR_ARRAY	10
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

#define FORM_NO_DISPLAY_PREFIX		"nno_display_"

#define FORM_NO_DISPLAY_PUSH_BUTTON_HEADING \
					"no_display"

#define FORM_IGNORE_PREFIX		"iignore_"
#define FORM_IGNORE_PUSH_BUTTON_HEADING "ignore"

#define FORM_PROMPT_ATTRIBUTE_RELATIONAL_PREFIX \
					"rrelational_"

#define FORM_ATTRIBUTE_FROM_PREFIX \
					"ffrom_"

#define FORM_ATTRIBUTE_TO_PREFIX	"tto_"

#define FORM_PROMPT_ORIGINAL_PREFIX	"original_"
#define FORM_PROMPT_RELATION_PREFIX	"relation_"

#define FORM_FROM_ATTRIBUTE_WIDTH	100

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

/* Returns heap memory */
/* ------------------- */
char *form_ignore_name(	char *form_ignore_prefix,
			char *relation_name );

/* Returns heap memory */
/* ------------------- */
char *form_no_display_name(
			char *form_no_display_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_relational_name(
			char *form_prompt_attribute_relational_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_from_name(
			char *form_prompt_attribute_from_prefix,
			char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_to_name(
			char *form_prompt_attribute_to_prefix,
			char *attribute_name );

char **form_background_color_array(
			int *background_color_array_length );

char *form_background_color(
			void );

#endif
