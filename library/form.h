/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_H
#define FORM_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "radio.h"
#include "dictionary_separate.h"

#define FORM_MAX_BACKGROUND_COLOR_ARRAY	10
#define FORM_COLOR1			"#dccdde"
#define FORM_COLOR2			"#ffe6ca"
#define FORM_COLOR3			"#d2ecf2"
#define FORM_COLOR4			"#f7ffce"
#define FORM_COLOR5			"#f2e0b8"

#define FORM_ROWS_BETWEEN_HEADINGS	10
#define FORM_SORT_ASCEND_LABEL		"assend_"
#define FORM_SORT_DESCEND_LABEL		"descend_"

#define FORM_APPASERVER_PROMPT		"prompt"
#define FORM_APPASERVER_TABLE		"table"

#define FORM_LOOKUP_SUBTITLE_MESSAGE	\
	"Enter only the minimum information sufficient to search."

#define FORM_RADIO_LIST_NAME		"form_radio_list"
#define FORM_NO_DISPLAY_HEADING 	"No Display"
#define FORM_IGNORE_PUSH_BUTTON_HEADING "ignore"

#define FORM_PROMPT_ORIGINAL_PREFIX	"original_"
#define FORM_PROMPT_RELATION_PREFIX	"relation_"

#define FORM_FROM_ATTRIBUTE_WIDTH	100

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *form_multi_select_all_javascript(
		LIST *widget_container_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_title_tag(
		char *title );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_tag(	char *form_name,
		char *action_string,
		char *target_frame );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *form_close_tag(
		void );

/* Usage */
/* ----- */

/* Returns static memory and increments form_current_reference_number */
/* ------------------------------------------------------------------ */
char *form_next_reference_number(
		int *form_current_reference_number );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_cookie_key(
		char *form_name,
		char *folder_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_cookie_multi_key(
		char *form_name,
		char *folder_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *form_verify_notepad_widths_javascript(
		char *form_name,
		LIST *widget_container_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_ignore_name(
		char *dictionary_separate_ignore_prefix,
		char *relation_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_no_display_name(
		char *dictionary_separate_no_display_prefix,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_relational_name(
		char *form_prompt_attribute_relational_prefix,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_from_name(
		char *form_prompt_attribute_from_prefix,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prompt_attribute_to_name(
		char *form_prompt_attribute_to_prefix,
		char *attribute_name );

/* Usage */
/* ----- */

char **form_background_color_array(
		int *background_color_array_length );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_background_color(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_prefix_widget_name(
		const char *prefix,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_html(
		char *form_tag,
		char *widget_container_list_html,
		char *hidden_html,
		char *form_close_tag );

/* Usage */
/* ----- */
int form_increment_number(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_name(
		const char *label,
		int form_increment_number );

#endif
