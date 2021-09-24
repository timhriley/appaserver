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
#define FORM_CAPTION_HEADING_TAG		"h3"
#define FORM_COLOR1				"#dccdde"
#define FORM_COLOR2				"#ffe6ca"
#define FORM_COLOR3				"#d2ecf2"
#define FORM_COLOR4				"#f7ffce"
#define FORM_COLOR5				"#f2e0b8"

#define FORM_MAX_BACKGROUND_COLOR_ARRAY		10
#define FORM_ROWS_BETWEEN_HEADINGS		10
#define FORM_SORT_LABEL				"sort_"
#define FORM_DESCENDING_LABEL			"descend_"
#define FORM_SUBMIT_CONTROL_STRING_SIZE		8192

#define FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER	'~'

#define FORM_LOOKUP_SUBTITLE_MESSAGE	\
	"Enter only the minimum information sufficient to search."

typedef struct
{
	char *button_label;
	char *onclick_control_string;
} FORM_BUTTON;

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
	char *folder_name;
	char *state;
	char *title_string;
	LIST *element_list;
} FORM_PROMPT;

/* FORM_BUTTON operations */
/* ---------------------- */
FORM_BUTTON *form_button_new(
			char *button_label,
			char *onclick_control_string );

/* FORM_PROMPT operations */
/* ---------------------- */
FORM_PROMPT *form_new(
			char *state,
			char *application_title );

FORM_PROMPT *form_prompt_calloc(
			void );

#endif
