/* $APPASERVER_HOME/library/element.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_ELEMENT_H
#define APPASERVER_ELEMENT_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "date_convert.h"
#include "folder_attribute.h"
#include "boolean.h"

/* Constants */
/* --------- */
/* #define ELEMENT_LONG_DASH_DELIMITER		"&#151;" */
#define ELEMENT_LONG_DASH_DELIMITER		"---"

#define ELEMENT_TABLE_ROW_REMEMBER		0
#define ELEMENT_PROMPT_REMEMBER			0
#define ELEMENT_CHECKBOX_REMEMBER		1
#define ELEMENT_DROP_DOWN_REMEMBER		1
#define ELEMENT_MULTI_DROP_DOWN_REMEMBER	1

#define ELEMENT_NAME_LOOKUP_STATE		"lookup_button"
#define ELEMENT_TITLE_NOTEPAD_PADDING_EM	3
#define ELEMENT_TEXTAREA_WRAP			"soft"
#define ELEMENT_DICTIONARY_DELIMITER		'~'
#define ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER	'|'
#define ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER '~'
#define ELEMENT_MULTI_SELECT_ADD_LABEL		"Add ->"
#define ELEMENT_MULTI_SELECT_REMOVE_LABEL	"<- Remove"
#define ELEMENT_TEXT_ITEM_LOOKUP_LENGTH 	100
#define ELEMENT_TEXT_ITEM_LARGE_WIDGET_SIZE	25
#define ELEMENT_TEXT_ITEM_SMALL_WIDGET_SIZE	10
#define ELEMENT_MAX_TEXT_WIDTH			30
#define ELEMENT_NOTEPAD_DEFAULT_NUMBER_ROWS	4
#define ELEMENT_LARGE_NOTEPAD_THRESHOLD		10000
#define ELEMENT_LARGE_NOTEPAD_WIDTH		70
#define ELEMENT_LARGE_NOTEPAD_NUMBER_ROWS	12
#define ELEMENT_MULTI_SELECT_ROW_COUNT		10
#define ELEMENT_NON_EDIT_MULTI_SELECT_ROW_COUNT	3

#define ELEMENT_NULL_OPERATOR			"is_empty"
#define ELEMENT_NOT_NULL_OPERATOR		"not_empty"
#define ELEMENT_SELECT_OPERATOR			"select"

enum element_type {	table_row,
			prompt,
			checkbox,
			drop_down,
			multi_drop_down,
			radio_button,
			notepad,
			text_item,
			password,
			non_edit_text,
			javascript_filename,
			upload_filename,
			prompt_data,
			prompt_data_plus_hidden,
			reference_number,
			hidden,
			empty_column,
			blank,
			element_date,
			element_current_date,
			element_time,
			element_current_time,
			element_date_time,
			element_current_date_time,
			http_filename,
			timestamp,
			table_opening,
			table_closing,
			prompt_heading,
			anchor,
			non_edit_multi_select };

typedef struct
{
	char *html;
} ELEMENT_TABLE_ROW;

typedef struct
{
	char *html;
} ELEMENT_PROMPT;

typedef struct
{
	char *text;
	int column_span;
	int padding_em;
} ELEMENT_NON_EDIT_TEXT;

typedef struct
{
	char *html;
} ELEMENT_MULTI_DROP_DOWN;

typedef struct
{
	char *html;
} ELEMENT_DROP_DOWN;

typedef struct
{
	LIST *option_label_list;
} ELEMENT_NON_EDIT_MULTI_SELECT;

typedef struct
{
	char *null;
} ELEMENT_EMPTY_COLUMN;

typedef struct
{
	char *data;
} ELEMENT_HIDDEN;

typedef struct
{
	int attribute_width;
} ELEMENT_UPLOAD_FILENAME;

typedef struct
{
	char *prompt;
	char *href;
} ELEMENT_ANCHOR;

typedef struct
{
	char *heading;
	char *data;
	char *align;
	boolean format_initial_capital;
} ELEMENT_PROMPT_DATA;

typedef struct
{
	/* Attribute */
	/* --------- */
	char *name;

	/* External */
	/* -------- */
	char *html;
} ELEMENT_CHECKBOX;

typedef struct
{
	char *heading;
	boolean checked;
	char onchange_submit_yn;
	char *form_name;
	char *image_source;
	char *onclick_keystrokes_save_string;
	char *onclick_function;
} ELEMENT_TOGGLE_BUTTON;

typedef struct
{
	char *heading;
	int checked;
	char onchange_submit_yn;
	char *form_name;
	char *image_source;
	char *value;
	char *onclick;
	char *state;
	char *post_change_javascript;
} ELEMENT_RADIO_BUTTON;

typedef struct
{
	char *data;
	int attribute_width;
	int number_rows;
	char *heading;
	char onchange_null2slash_yn;
	char *state;
} ELEMENT_NOTEPAD;

typedef struct
{
	char *data;
	int attribute_width;
	int widget_size;
	char *heading;
	char onchange_null2slash_yn;
	char *post_change_javascript;
	char *on_focus_javascript_function;
	char *state;
	boolean dont_create_current_date;
	boolean readonly;
	boolean is_numeric;
} ELEMENT_TEXT_ITEM;

typedef struct
{
	char *data;
	ELEMENT_TEXT_ITEM *update_text_item;
} ELEMENT_HTTP_FILENAME;

typedef struct
{
	char *data;
	int attribute_width;
	char *heading;
	char *state;
} ELEMENT_PASSWORD;

typedef struct
{
	char *data;
	int attribute_width;
	char *heading;
	boolean omit_update;
} ELEMENT_REFERENCE_NUMBER;

typedef struct
{
	/* Input */
	/* ----- */
	enum element_type element_type;

	/* Attributes */
	/* ---------- */
	ELEMENT_TABLE_ROW *table_row;
	ELEMENT_PROMPT *prompt;
	ELEMENT_CHECKBOX *checkbox;
	ELEMENT_DROP_DOWN *drop_down;
	ELEMENT_MULTI_DROP_DOWN *multi_drop_down;

/*
	ELEMENT_TOGGLE_BUTTON *toggle_button;
	ELEMENT_PUSH_BUTTON *push_button;
	ELEMENT_RADIO_BUTTON *radio_button;
	ELEMENT_NOTEPAD *notepad;
	ELEMENT_TEXT_ITEM *text_item;
	ELEMENT_PASSWORD *password;
	ELEMENT_PROMPT_DATA *prompt_data;
	ELEMENT_HIDDEN *hidden;
	ELEMENT_UPLOAD_FILENAME *upload_filename;
	ELEMENT_HTTP_FILENAME *http_filename;
	ELEMENT_ANCHOR *anchor;
	ELEMENT_EMPTY_COLUMN *empty_column;
	ELEMENT_REFERENCE_NUMBER *reference_number;
	ELEMENT_NON_EDIT_TEXT *non_edit_text;
	ELEMENT_NON_EDIT_MULTI_SELECT *non_edit_multi_select;
*/
} APPASERVER_ELEMENT;

/* APPASERVER_ELEMENT operations */
/* ----------------------------- */
APPASERVER_ELEMENT *appaserver_element_new(
			enum element_type element_type );

int appaserver_element_tab_order(
			int tab_order );

/* Returns program memory */
/* ---------------------- */
char *appaserver_element_background_color(
			int row_number );

/* Safely returns heap memory.			    */
/* Note: sourse is assumed to be heap and is freed. */
/* ------------------------------------------------ */
char *appaserver_element_post_change_javascript(
			char *source,
			int row_number,
			char *state );

char *appaserver_element_name(
			char *name,
			int row_number );

void appaserver_element_list_output(
			FILE *output_stream,
			LIST *element_list );

void appaserver_element_output(
			FILE *output_stream,
			APPASERVER_ELEMENT *element );

/* ELEMENT_TABLE_ROW operations */
/* ---------------------------- */
ELEMENT_TABLE_ROW *element_table_row_calloc(
			void );

/* Returns program memory */
/* ---------------------- */
char *element_table_row_html(
			void );

/* ELEMENT_PROMPT operations */
/* ------------------------- */
ELEMENT_PROMPT *element_prompt_calloc(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *element_prompt_html(
			char *prompt_string );

/* ELEMENT_CHECKBOX operations */
/* --------------------------- */
ELEMENT_CHECKBOX *element_checkbox_calloc(
			void );

/* Returns heap memory or null. */
/* ---------------------------- */
char *element_checkbox_html(
			char *element_name,
			char *prompt_display,
			boolean checked,
			char *action_string,
			int tab_order,
			char *value );

/* ELEMENT_DROP_DOWN operations */
/* ---------------------------- */
ELEMENT_DROP_DOWN *element_drop_down_calloc(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *element_drop_down_name(
			LIST *element_name_list,
			int row_number );

LIST *element_drop_down_display_list(
			LIST *delimited_list,
			boolean no_initial_capital );

int element_drop_down_size(
			int delimited_list_length );

char *element_drop_down_html( 	
			char *element_drop_down_name,
			char *initial_data,
			LIST *delimited_list,
			LIST *display_list,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			char *post_change_javascript,
			char *background_color,
			int tab_order );

/* Zaps row in delimited_list.  */
/* Returns heap memory or null.	*/
/* ---------------------------- */
char *element_drop_down_initial_data_html(
			LIST *delimited_list,
			LIST *display_list,
			char *initial_data );

/* Returns static memory */
/* --------------------- */
char *element_drop_down_option_value_html(
			char *data,
			char *display );

/* Returns static memory. */
/* ---------------------- */
char *element_drop_down_close_html(
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option );

/* Returns heap memory or null */
/* --------------------------- */
char *element_drop_down_data_list_display(
			LIST *data_list );

/* ELEMENT_MULTI_DROP_DOWN operations */
/* ---------------------------------- */
ELEMENT_DROP_DOWN *element_drop_down_calloc(
			void );

#endif
