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
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define FORM_COLOR1				"#dccdde"
#define FORM_COLOR2				"#ffe6ca"
#define FORM_COLOR3				"#d2ecf2"
#define FORM_COLOR4				"#f7ffce"
#define FORM_COLOR5				"#f2e0b8"

#define FORM_ROWS_BETWEEN_HEADINGS		10
#define FORM_SORT_LABEL				"sort_"
#define FORM_DESCENDING_LABEL			"descend_"

#define FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER	'~'

#define FORM_LOOKUP_SUBTITLE_MESSAGE	\
	"Enter only the minimum information sufficient to search."

typedef struct
{
	char *title_html;
	char *message_html;
	char *tag_html;
	LIST *element_list;
	LIST *insert_isa_button_list;
} FORM_PROMPT_INSERT_ISA;
	
typedef struct
{
	/* Input */
	/* ----- */
	char *button_label;
	char *action_string;
	char *image_source;

	/* Process */
	/* ------- */
	char *html;
} FORM_BUTTON;

typedef struct
{
	LIST *value_list;
	char *html;
} FORM_RADIO_VALUE;

typedef struct
{
	char *radio_name;
	LIST *value_list;
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
	char *folder_name;
	char *state;
	char *title_string;
	LIST *regular_element_list;
	LIST *viewonly_element_list;
} FORM_TABLE;

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

/* FORM operations */
/* --------------- */

/* Safely returns heap memory */
/* -------------------------- */
char *form_title_html(
			char *title );

/* Safely returns heap memory */
/* -------------------------- */
char *form_message_html(
			char *message );

/* Safely returns heap memory */
/* -------------------------- */
char *form_tag_html(
			char *action_string,
			char *target_frame );

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

/* FORM_BUTTON operations */
/* ---------------------- */
FORM_BUTTON *form_button_calloc(
			void );

FORM_BUTTON *form_button_new(
			char *button_label,
			char *action_string,
			char *additional_action_string,
			char *image_source );

FORM_BUTTON *form_button_submit(
			int form_number,
			char *additional_action_string );

FORM_BUTTON *form_button_reset(
			int form_number );

FORM_BUTTON *form_button_back(
			void );

FORM_BUTTON *form_button_forward(
			void );

/* Returns heap memory or null */
/* --------------------------- */
char *form_button_html(
			char *button_label,
			char *action_string,
			char *additional_action_string,
			char *image_source );

void form_button_list_output(
			FILE *output_stream,
			LIST *button_list );

/* FORM_PROMPT operations */
/* ---------------------- */
FORM_PROMPT *form_new(
			char *state,
			char *application_title );

FORM_PROMPT *form_prompt_calloc(
			void );

/* FORM_PROMPT_INSERT_ISA operations */
/* --------------------------------- */
FORM_PROMPT_INSERT_ISA *form_prompt_insert_isa_calloc(
			void );

FORM_PROMPT_INSERT_ISA *form_prompt_insert_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list,
			char *action_string );

LIST *form_prompt_insert_isa_element_list(
			char *one2m_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list );

LIST *form_prompt_insert_isa_button_list(
			void );

void form_prompt_insert_isa_output(
			FILE *output_stream,
			char *title_html,
			char *message_html,
			char *tag_html,
			LIST *element_list,
			LIST *button_list );

#endif
