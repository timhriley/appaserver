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
#include "dictionary.h"
#include "boolean.h"

/* Constants */
/* --------- */
/* #define ELEMENT_LONG_DASH_DELIMITER		"&#151;" */
#define ELEMENT_LONG_DASH_DELIMITER		"---"

#define ELEMENT_MULTI_DROP_DOWN_ORIGINAL_PREFIX	"original_"
#define ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER '|'
#define ELEMENT_NAME_LOOKUP_STATE		"lookup_button"
#define ELEMENT_TITLE_NOTEPAD_PADDING_EM	3
#define ELEMENT_TEXTAREA_WRAP			"soft"
#define ELEMENT_DICTIONARY_DELIMITER		'~'
#define ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER	'|'
#define ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER '~'
#define ELEMENT_TEXT_LOOKUP_LENGTH 		100
#define ELEMENT_TEXT_LARGE_WIDGET_SIZE		25
#define ELEMENT_TEXT_SMALL_WIDGET_SIZE		10
#define ELEMENT_TEXT_MAX_DISPLAY_SIZE		30
#define ELEMENT_NOTEPAD_COLUMNS			30
#define ELEMENT_NOTEPAD_ROWS			4
#define ELEMENT_LARGE_NOTEPAD_THRESHOLD		10000
#define ELEMENT_LARGE_NOTEPAD_COLUMNS		70
#define ELEMENT_LARGE_NOTEPAD_ROWS		12
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
			text,
			radio_button,
			notepad,
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
	char *on_click;
	int tab_order;
	char *image_source;
	boolean remember;

	/* Public */
	/* ------ */
	char *html;
} ELEMENT_CHECKBOX;

/* ELEMENT_CHECKBOX operations */
/* --------------------------- */
ELEMENT_CHECKBOX *element_checkbox_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *element_checkbox_heading_string(
			char *attribute_name );

/* Returns name */
/* ------------ */
boolean element_checkbox_checked(
			char *attribute_name,
			DICTIONARY *row_dictionary );

/* Returns heap memory */
/* ------------------- */
char *element_checkbox_html(
			char *attribute_name,
			char *prompt_display,
			boolean checked,
			char *on_click,
			int tab_order,
			char *image_source );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *name;
	LIST *attribute_name_list;
	LIST *delimited_list;
	LIST *display_list;
	boolean no_initial_capital;
	boolean output_null_option;
	boolean output_not_null_option;
	boolean output_select_option;
	int column_span;
	int tab_order;
	boolean multi_select;
	char *post_change_javascript;
	boolean remember;

	/* Public */
	/* ------ */
	char *key_string;
	char *value;
	int display_size;
	char *html;

	/* Private */
	/* ------- */
	char *heading;
} ELEMENT_DROP_DOWN;

/* ELEMENT_DROP_DOWN operations */
/* ---------------------------- */
ELEMENT_DROP_DOWN *element_drop_down_calloc(
			void );

ELEMENT_DROP_DOWN *element_drop_down_new(
			char *name,
			LIST *attribute_name_list,
			LIST *delimited_list,
			LIST *display_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
			int tab_order,
			boolean multi_select,
			char *post_change_javascript,
			boolean remember );

ELEMENT_DROP_DOWN *element_drop_down_empty_new(
			char *name,
			int size,
			boolean multi_select,
			char *post_change_javascript,
			char *state );

/* Returns row_dictionary->hash_table->other_data */
/* ---------------------------------------------- */
char *element_drop_down_value(
			char *key_string,
			DICTIONARY *row_dictionary );

/* Returns static memory */
/* --------------------- */
char *element_drop_down_name(
			LIST *attribute_name_list,
			int row_number );

int element_drop_down_display_size(
			int delimited_list_length );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_html( 	
			char *name,
			char *value,
			LIST *delimited_list,
			LIST *display_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
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

/* Private */
/* ------- */
LIST *element_drop_down_display_list(
			LIST *delimited_list,
			boolean no_initial_capital );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_value_display(
			char *value,
			boolean no_initial_capital );

/* Returns static memory */
/* --------------------- */
char *element_drop_down_option_value_html(
			char *value,
			char *value_display );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_option_value_list_html(
			LIST *delimited_list,
			LIST *display_list,
			char *value );

/* Returns static memory. */
/* ---------------------- */
char *element_drop_down_close_html(
			boolean output_null_option,
			boolean output_not_null_option );

/* Returns heap memory */
/* ------------------- */
char *element_drop_down_heading(
			LIST *attribute_name_list );

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
	char *attribute_name;
	char *message;

	/* Public */
	/* ------ */
	char *initial_capital;

	/* Private */
	/* ------- */
	char *heading_string;
} ELEMENT_NON_EDIT_TEXT;

/* ELEMENT_NON_EDIT_TEXT operations */
/* -------------------------------- */
ELEMENT_NON_EDIT_TEXT *element_non_edit_text_calloc(
			void );

ELEMENT_NON_EDIT_TEXT *element_non_edit_text_new(
			char *attribute_name,
			char *message );

/* Public */
/* ------ */

/* Returns heap memory or null */
/* ---------------------------- */
char *element_non_edit_text_initial_capital(
			char *value,
			char *message );

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	LIST *attribute_name_list;

	/* Public */
	/* ------ */
	char *key_string;
	char *element_name;
	char *value;
} ELEMENT_HIDDEN;

/* ELEMENT_HIDDEN operations */
/* ------------------------- */
ELEMENT_HIDDEN *element_hidden_calloc(
			void );

ELEMENT_HIDDEN *element_hidden_new(
			char *attribute_name,
			LIST *attribute_name_list );

/* Returns static memory */
/* --------------------- */
char *element_hidden_key_string(
			char *attribute_name,
			LIST *attribute_name_list );

/* Returns static memory */
/* --------------------- */
char *element_hidden_name(
			char *key_string,
			int row_number );

/* Returns heap memory */
/* ------------------- */
char *element_hidden_html(
			char *element_name,
			char *value );

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
	/* Attributes */
	/* ---------- */
	boolean align_right;
	int column_span;

	/* Public */
	/* ------ */
	char *html;
} ELEMENT_TABLE_DATA;

/* ELEMENT_TABLE_DATA operations */
/* ----------------------------- */
ELEMENT_TABLE_DATA *element_table_data_calloc(
			void );

ELEMENT_TABLE_DATA *element_table_data_new(
			boolean align_right,
	 		int column_span );

/* Returns heap memory */
/* ------------------- */
char *element_table_data_html(
			boolean align_right,
			int column_span );

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

int element_multi_display_size(
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
	/* Input */
	/* ----- */
	char *attribute_name;
	int attribute_size;
	int columns;
	int rows;
	boolean null_to_slash;
	int tab_index;

	/* Public */
	/* ------ */
	char *value;

	/* Private */
	/* ------- */
	char *heading_string;
} ELEMENT_NOTEPAD;

/* ELEMENT_NOTEPAD operations */
/* -------------------------- */
ELEMENT_NOTEPAD *element_notepad_calloc(
			void );

ELEMENT_NOTEPAD *element_notepad_new(
			char *attribute_name,
			int attribute_size,
			int columns,
			int rows,
			boolean null_to_slash,
			int tab_index );

/* Returns heap memory or null */
/* --------------------------- */
char *element_notepad_html(
			char *element_name,
			char *value,
			int attribute_size,
			int element_notepad_columns,
			int element_notepad_rows,
			boolean null_to_slash,
			int tab_index );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *attribute_name;
	char *datatype_name;
	int max_length;
	int size;
	boolean null_to_slash;
	boolean prevent_carrot;
	char *on_change;
	char *on_focus;
	char *on_keyup;
	int tab_index;
	boolean remember;

	/* Public */
	/* ------ */
	char *value;
	char *javascript_replace_on_change;
	char *javascript_replace_on_focus;
	char *prevent_carrot_on_keyup;

	/* Private */
	/* ------- */
	char *heading_string;
} ELEMENT_TEXT;

/* ELEMENT_TEXT operations */
/* ----------------------- */
ELEMENT_TEXT *element_text_calloc(
			void );

ELEMENT_TEXT *element_text_new(
			char *attribute_name,
			char *datatype_name,
			int max_length,
			int size,
			boolean null_to_slash,
			boolean prevent_carrot,
			char *on_change,
			char *on_focus,
			char *on_keyup,
			int tab_index,
			boolean remember );

/* Returns heap memory or null */
/* --------------------------- */
char *element_text_value(
			char *attribute_name,
			DICTIONARY *row_dictionary,
			boolean is_number );

/* Returns heap memory or null */
/* --------------------------- */
char *element_text_javascript_replace_on_change(
			char *on_change,
			int row_number,
			char *state,
			boolean null_to_slash );

/* Returns heap memory or null */
/* --------------------------- */
char *element_text_javascript_replace_on_focus(
			char *on_focus,
			int row_number,
			char *state );

/* Returns heap memory or null */
/* --------------------------- */
char *element_text_prevent_carrot_on_keyup(
			char *on_keyup,
			boolean prevent_carrot );

boolean element_text_autocomplete_off(
			char *attribute_name );

/* Returns heap memory or null */
/* --------------------------- */
char *element_text_html(
			char *element_name,
			char *value,
			int attribute_width_max_length,
			int element_text_max_display_size,
			char *on_change,
			char *on_focus,
			char *on_keyup,
			boolean autocomplete_off,
			int tab_index,
			char *background_color );

typedef struct
{
	char *data;
	ELEMENT_TEXT_ITEM *update_text_item;
} ELEMENT_HTTP_FILENAME;

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	int attribute_width;
	int tab_index;

	/* Private */
	/* ------- */
	char *heading_string;
} ELEMENT_PASSWORD;

/* ELEMENT_PASSWORD operations */
/* --------------------------- */
ELEMENT_PASSWORD *element_password_calloc(
			void );

ELEMENT_PASSWORD *element_password_new(
			char *attribute_name,
			int attribute_width_max_length,
			int element_text_max_display_size,
			int tab_index );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *element_password_compare_element_name(
			char *attribute_name,
			int row_number );

/* Returns heap memory or null */
/* --------------------------- */
char *element_password_html(
			char *element_name,
			char *compare_element_name,
			char *value,
			int attribute_width_max_length,
			int element_text_max_display_size,
			int tab_index );

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
	ELEMENT_TEXT *text;
	ELEMENT_NOTEPAD *notepad;

/*
	ELEMENT_RADIO_BUTTON *radio_button;
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

/* Public */
/* ------ */
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
			char *attribute_name,
			int row_number );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_list_html(
			LIST *appaserver_element_list /* in/out */,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_hidden_element_list_html(
			LIST *appaserver_element_list /* in/out */,
			int row_number,
			DICTIONARY *row_dictionary );


/* Returns static memory */
/* --------------------- */
char *appaserver_element_javascript_html(
			char *appaserver_element_javascript );

/* Returns static memory */
/* --------------------- */
char *appaserver_element_background_color_html(
			char *background_color );

/* Returns data or heap memory */
/* --------------------------- */
char *appaserver_element_number_commas_string(
			char *element_name,
			char *data );

/* Returns static memory or null */
/* ----------------------------- */
char *appaserver_element_key_string(
			LIST *attribute_name_list );

/* Returns row_dictionary->hash_table->other_data or null */
/* ------------------------------------------------------ */
char *appaserver_element_value(
			char *key_string,
			DICTIONARY *row_dictionary );

/* Private */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_element_heading_string(
			char *name );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_html(
			APPASERVER_ELEMENT *appaserver_element /* in/out */,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_drop_down_html(
			ELEMENT_DROP_DOWN *drop_down,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_text_html(
			ELEMENT_TEXT *text,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_hidden_html(
			ELEMENT_HIDDEN *hidden,
			int row_number,
			DICTIONARY *row_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_notepad_html(
			ELEMENT_NOTEPAD *notepad,
			int row_number,
			DICTIONARY *row_dictionary );

#endif
