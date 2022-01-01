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
#include "button.h"
#include "boolean.h"

/* Constants */
/* --------- */
/* #define ELEMENT_LONG_DASH_DELIMITER		"&#151;" */
#define ELEMENT_LONG_DASH_DELIMITER		"---"

#define ELEMENT_CHECKBOX_REMEMBER		1
#define ELEMENT_DROP_DOWN_REMEMBER		1
#define ELEMENT_MULTI_DROP_DOWN_REMEMBER	1
#define ELEMENT_MULTI_DROP_DOWN_ORIGINAL_PREFIX	"original_"
#define ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER '|'
#define ELEMENT_NAME_LOOKUP_STATE		"lookup_button"
#define ELEMENT_TITLE_NOTEPAD_PADDING_EM	3
#define ELEMENT_TEXTAREA_WRAP			"soft"
#define ELEMENT_DICTIONARY_DELIMITER		'~'
#define ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER	'|'
#define ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER '~'
#define ELEMENT_TEXT_ITEM_LOOKUP_LENGTH 	100
#define ELEMENT_TEXT_ITEM_LARGE_WIDGET_SIZE	25
#define ELEMENT_TEXT_ITEM_SMALL_WIDGET_SIZE	10
#define ELEMENT_MAX_TEXT_WIDTH			30
#define ELEMENT_NOTEPAD_DEFAULT_NUMBER_ROWS	4
#define ELEMENT_LARGE_NOTEPAD_THRESHOLD		10000
#define ELEMENT_LARGE_NOTEPAD_WIDTH		70
#define ELEMENT_LARGE_NOTEPAD_NUMBER_ROWS	12
#define ELEMENT_NULL_OPERATOR			"is_empty"
#define ELEMENT_NOT_NULL_OPERATOR		"not_empty"
#define ELEMENT_SELECT_OPERATOR			"select"

enum element_type {	table_open,
			table_row,
			table_close,
			checkbox,
			drop_down,
			button,
			non_edit_text,
			hidden,
			line_break,
			table_data,
			multi_drop_down,
			radio_button,
			notepad,
			text_item,
			password,
			upload_filename,
			reference_number,
			javascript_filename,
			element_date,
			element_current_date,
			element_time,
			element_current_time,
			element_date_time,
			element_current_date_time,
			http_filename,
			timestamp,
			empty_column };

typedef struct
{
	char *html;
} ELEMENT_TABLE_OPEN;

/* ELEMENT_TABLE_OPEN operations */
/* ----------------------------- */
ELEMENT_TABLE_OPEN *element_table_open_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_table_open_html(
			void );

typedef struct
{
	char *html;
} ELEMENT_TABLE_ROW;

/* ELEMENT_TABLE_ROW operations */
/* ---------------------------- */
ELEMENT_TABLE_ROW *element_table_row_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_table_row_html(
			void );

typedef struct
{
	char *html;
} ELEMENT_TABLE_CLOSE;

/* ELEMENT_TABLE_CLOSE operations */
/* ------------------------------ */
ELEMENT_TABLE_CLOSE *element_table_close_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_table_close_html(
			void );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *name;
	char *prompt_string;
	boolean checked;
	char *action_string;
	int tab_order;
	char *value;
	char *image_source;

	/* External */
	/* -------- */
	char *html;
} ELEMENT_CHECKBOX;

/* ELEMENT_CHECKBOX operations */
/* --------------------------- */
ELEMENT_CHECKBOX *element_checkbox_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_checkbox_html(
			char *element_name,
			char *prompt_display,
			boolean checked,
			char *action_string,
			int tab_order,
			char *value,
			char *image_source );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *name;
	LIST *attribute_name_list;
	LIST *delimited_list;
	boolean no_initial_capital;
	boolean output_null_option;
	boolean output_not_null_option;
	boolean output_select_option;
	int column_span;
	int tab_order;
	boolean multi_select;
	char *post_change_javascript;
	char *state;
	char *initial_data;

	/* Process */
	/* ------- */
	char *heading;
	int size;
	char *html;
} ELEMENT_DROP_DOWN;

/* ELEMENT_DROP_DOWN operations */
/* ---------------------------- */
ELEMENT_DROP_DOWN *element_drop_down_calloc(
			void );

ELEMENT_DROP_DOWN *element_drop_down_new(
			char *name,
			LIST *attribute_name_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			int tab_order,
			boolean multi_select,
			char *post_change_javascript,
			char *state );

ELEMENT_DROP_DOWN *element_drop_down_empty_new(
			char *name,
			int size,
			boolean multi_select,
			char *post_change_javascript,
			char *state );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_html( 	
			char *element_drop_down_name,
			char *initial_data,
			LIST *delimited_list,
			LIST *display_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			boolean multi_select,
			int tab_order,
			char *appaserver_element_javascript,
			char *background_color );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_empty_html(
			char *drop_down_name,
			int drop_down_size,
			boolean multi_select,
			char *appaserver_element_javascript,
			char *background_color );

LIST *element_drop_down_display_list(
			LIST *delimited_list,
			boolean no_initial_capital );

/* Returns static memory or null. */
/* ------------------------------ */
char *element_drop_down_initial_data_html(
			char *initial_data,	
			char *initial_display,
			boolean no_initial_capital );

/* Returns heap memory or null. */
/* ---------------------------- */
char *element_drop_down_option_value_list_html(
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
			boolean output_not_null_option );

/* Returns static memory */
/* --------------------- */
char *element_drop_down_name(
			LIST *attribute_name_list,
			int row_number );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_heading(
			LIST *attribute_name_list );

int element_drop_down_size(
			int delimited_list_length );

typedef struct
{
	/* Process */
	/* ------- */
	BUTTON *button;
} ELEMENT_BUTTON;

/* ELEMENT_BUTTON operations */
/* ------------------------- */
ELEMENT_BUTTON *element_button_calloc(
			void );

ELEMENT_BUTTON *element_button_new(
			char *label,
			char *action_string );

/* Returns heap memory */
/* ------------------- */
char *element_button_html(
			char *html );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *name;
	char *text_html;
	int column_span;
	int padding_em;

	/* External */
	/* -------- */
	char *message;
	char *html;
	char *heading;
} ELEMENT_NON_EDIT_TEXT;

/* ELEMENT_NON_EDIT_TEXT operations */
/* -------------------------------- */
ELEMENT_NON_EDIT_TEXT *element_non_edit_text_calloc(
			void );

char *element_non_edit_text_message(
			char *message,
			char *name );

/* Safely returns heap memory */
/* -------------------------- */
char *element_non_edit_text_html(
			char *message );

typedef struct
{
	char *name;
	char *data;
} ELEMENT_HIDDEN;

/* ELEMENT_HIDDEN operations */
/* ------------------------- */
ELEMENT_HIDDEN *element_hidden_calloc(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *element_hidden_html(
			char *name,
			char *data,
			int row_number );

typedef struct
{
	char *html;
} ELEMENT_LINE_BREAK;

/* ELEMENT_LINE_BREAK operations */
/* ----------------------------- */
ELEMENT_LINE_BREAK *element_line_break_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_line_break_html(
			void );

typedef struct
{
	char *html;
} ELEMENT_TABLE_DATA;

/* ELEMENT_TABLE_DATA operations */
/* ----------------------------- */
ELEMENT_TABLE_DATA *element_table_data_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_table_data_html(
			void );

typedef struct
{
	/* Input */
	/* ----- */
	LIST *attribute_name_list;
	LIST *delimited_list;
	boolean no_initial_capital;
	char *post_change_javascript;
	char *state;

	/* Process */
	/* ------- */
	ELEMENT_DROP_DOWN *original_drop_down;
	ELEMENT_TABLE_DATA *table_data;
	ELEMENT_BUTTON *move_right_button;
	ELEMENT_LINE_BREAK *element_line_break;
	ELEMENT_BUTTON *move_left_button;
	ELEMENT_DROP_DOWN *empty_drop_down;
	char *name;
	char *empty_html;
	char *html;
	char *heading;
} ELEMENT_MULTI_DROP_DOWN;

/* ELEMENT_MULTI_DROP_DOWN operations */
/* ---------------------------------- */
ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down_calloc(
			void );

ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down_new(
			LIST *attribute_name_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *post_change_javascript,
			char *state );

int element_multi_drop_down_size(
			void );

/* Returns program memory */
/* ---------------------- */
char *element_multi_drop_down_move_right_label(
			void );

/* Returns program memory */
/* ---------------------- */
char *element_multi_drop_down_move_left_label(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_multi_drop_down_move_right_action_string(
			LIST *attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *element_multi_drop_down_move_left_action_string(
			LIST *attribute_name_list );


/* Returns static memory */
/* --------------------- */
char *element_multi_drop_down_original_name(
			LIST *attribute_name_list,
			char *element_name_prefix );

/* Returns static memory */
/* --------------------- */
char *element_multi_drop_down_name(
			LIST *attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *element_multi_drop_down_html(
			ELEMENT_DROP_DOWN *original_drop_down,
			ELEMENT_TABLE_DATA *table_data,
			ELEMENT_BUTTON *move_right_button,
			ELEMENT_LINE_BREAK *element_line_break,
			ELEMENT_BUTTON *move_left_button,
			ELEMENT_DROP_DOWN *empty_drop_down,
			char *appaserver_element_javascript );

/* Returns heap memory */
/* ------------------- */
char *element_anchor_html(
			char *image_source,
			char *action_string,
			boolean with_table_data_tag );

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
	int attribute_width;
} ELEMENT_UPLOAD_FILENAME;

typedef struct
{
	char *heading;
	char *data;
	char *align;
	boolean format_initial_capital;
} ELEMENT_PROMPT_DATA;

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

/* ELEMENT_RADIO_BUTTON operations */
/* ------------------------------- */
ELEMENT_RADIO_BUTTON *element_radio_button_calloc(
			void );

typedef struct
{
	/* Attributes */
	/* ---------- */
	enum element_type element_type;
	char *element_name;
	char *heading_string;
	char *element_key;

	/* Attributes */
	/* ---------- */
	ELEMENT_TABLE_OPEN *table_open;
	ELEMENT_TABLE_ROW *table_row;
	ELEMENT_TABLE_CLOSE *table_close;
	ELEMENT_CHECKBOX *checkbox;
	ELEMENT_DROP_DOWN *drop_down;
	ELEMENT_BUTTON *button;
	ELEMENT_NON_EDIT_TEXT *non_edit_text;
	ELEMENT_HIDDEN *hidden;
	ELEMENT_LINE_BREAK *line_break;
	ELEMENT_TABLE_DATA *table_data;
	ELEMENT_MULTI_DROP_DOWN *multi_drop_down;

/*
	ELEMENT_RADIO_BUTTON *radio_button;
	ELEMENT_NOTEPAD *notepad;
	ELEMENT_TEXT_ITEM *text_item;
	ELEMENT_PASSWORD *password;
	ELEMENT_UPLOAD_FILENAME *upload_filename;
	ELEMENT_HTTP_FILENAME *http_filename;
	ELEMENT_EMPTY_COLUMN *empty_column;
	ELEMENT_REFERENCE_NUMBER *reference_number;
	ELEMENT_NON_EDIT_MULTI_SELECT *non_edit_multi_select;
*/
} APPASERVER_ELEMENT;

/* APPASERVER_ELEMENT operations */
/* ----------------------------- */
APPASERVER_ELEMENT *appaserver_element_new(
			enum element_type element_type,
			char *element_name );

APPASERVER_ELEMENT *appaserver_element_name_seek(
			char *element_name,
			LIST *element_list );

APPASERVER_ELEMENT *appaserver_element_key_seek(
			char *element_key,
			LIST *element_list );

int appaserver_element_tab_order(
			int tab_order );

/* Returns program memory */
/* ---------------------- */
char *appaserver_element_background_color(
			int row_number );

/* Returns static memory or null */
/* ----------------------------- */
char *appaserver_element_javascript(
			char *javascript,
			char *state,
			int row_number );

/* Returns static memory */
/* --------------------- */
char *appaserver_element_name(
			char *name,
			int row_number );

void appaserver_element_list_output(
			FILE *output_stream,
			LIST *element_list );

void appaserver_element_output(
			FILE *output_stream,
			APPASERVER_ELEMENT *element );

char *appaserver_element_heading(
			APPASERVER_ELEMENT *element );

/* Returns heap memory */
/* ------------------- */
char *appaserver_element_heading_string(
			char *name );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_list_html(
			char *background_color,
			char *state,
			int row_number,
			LIST *appaserver_element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_hidden_element_list_html(
			int row_number,
			LIST *appaserver_element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_html(
			char *background_color,
			char *state,
			int row_number,
			APPASERVER_ELEMENT *appaserver_element );

/* Returns static memory */
/* --------------------- */
char *appaserver_element_javascript_html(
			char *appaserver_element_javascript );

/* Returns static memory */
/* --------------------- */
char *appaserver_element_background_color_html(
			char *background_color );

#endif
