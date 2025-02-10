/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/widget.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef WIDGET_H
#define WIDGET_H

#include <stdio.h>
#include "list.h"
#include "String.h"
#include "dictionary.h"
#include "date_convert.h"
#include "security.h"
#include "role.h"
#include "folder.h"
#include "folder_attribute.h"
#include "button.h"
#include "dictionary.h"
#include "relation_join.h"
#include "boolean.h"

#define WIDGET_TEXTAREA_WRAP			"soft"
#define WIDGET_TEXT_SMALL_WIDGET_SIZE		10
#define WIDGET_SELECT_OPERATOR			"select"
#define WIDGET_LOOKUP_CHECKBOX_NAME		"lookup_checkbox"

#define WIDGET_DROP_DOWN_LABEL_DELIMITER	'|'
#define WIDGET_DROP_DOWN_EXTRA_DELIMITER	'['

#define WIDGET_DROP_DOWN_DASH_DELIMITER		"&#151;"
/* #define WIDGET_DROP_DOWN_DASH_DELIMITER	"---" */

#define WIDGET_MULTI_DISPLAY_SIZE		10
#define WIDGET_MULTI_ORIGINAL_PREFIX		"original_"
#define WIDGET_MULTI_LEFT_RIGHT_DELIMITER	'|'
#define WIDGET_MULTI_SELECT_REMEMBER_DELIMITER	'~'
#define WIDGET_MULTI_SELECT_REMEMBER_DELIMITER	'~'

#define WIDGET_MULTI_DROP_DOWN_RIGHT_MESSAGE	\
"Include the selected item for lookup."

#define WIDGET_MULTI_DROP_DOWN_LEFT_MESSAGE	\
"Remove the selected item from lookup."

#define WIDGET_DATE_DISPLAY_SIZE 	11
#define WIDGET_DATE_TIME_DISPLAY_SIZE 	19
#define WIDGET_DATE_DATATYPE_NAME	"date"

#define WIDGET_NOTEPAD_COLUMNS			30
#define WIDGET_NOTEPAD_ROWS			4

#define WIDGET_ENCRYPT_COMPARE_LABEL	"compare"

enum widget_type {	table_open,
			table_heading,
			table_row,
			table_close,
			checkbox,
			drop_down,
			multi_drop_down,
			button,
			non_edit_text,
			line_break,
			hidden,
			table_data,
			notepad,
			text,
			encrypt,
			yes_no,
			widget_date,
			widget_time,
			upload,
			blank,
			anchor,
			unordered_list_open,
			unordered_list_close,
			list_item,
			widget_radio,
			widget_ajax,
			widget_password };

typedef struct
{
	LIST *ajax_client_widget_container_list;
} WIDGET_AJAX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_AJAX *widget_ajax_new(
		LIST *ajax_client_widget_container_list );

/* Process */
/* ------- */
WIDGET_AJAX *widget_ajax_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_ajax_html(
		LIST *ajax_client_widget_container_list,
		LIST *query_row_cell_list,
		int row_number,
		char *background_color );

typedef struct
{
	char *hypertext_reference;
	char *target_frame;
	char *prompt;
} WIDGET_ANCHOR;

/* Usage */
/* ----- */
WIDGET_ANCHOR *widget_anchor_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_anchor_html(
		char *hypertext_reference,
		char *target_frame,
		char *prompt );

typedef struct
{
	boolean border_boolean;

	/* Set externally */
	/* -------------- */
	int cell_spacing;
	int cell_padding;
} WIDGET_TABLE_OPEN;

/* Usage */
/* ----- */
WIDGET_TABLE_OPEN *widget_table_open_new(
		boolean border_boolean );

/* Process */
/* ------- */
WIDGET_TABLE_OPEN *widget_table_open_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_table_open_tag(
		boolean border_boolean,
		int cell_spacing,
		int cell_padding );

typedef struct
{
	/* stub */
} WIDGET_TABLE_ROW;

/* Usage */
/* ----- */
WIDGET_TABLE_ROW *widget_table_row_calloc(
		void );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *widget_table_row_tag(
		char *background_color );

typedef struct
{
	/* stub */
} WIDGET_TABLE_CLOSE;

/* Usage */
/* ----- */
WIDGET_TABLE_CLOSE *widget_table_close_calloc(
		void );

/* Public */
/* ------ */

/* Returns program memory */
/* ---------------------- */
char *widget_table_close_tag(
		void );

typedef struct
{
	char *widget_name;
	char *prompt;
	char *onclick;
	int tab_order;
	char *image_source;
} WIDGET_CHECKBOX;

/* Usage */
/* ----- */
WIDGET_CHECKBOX *widget_checkbox_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_CHECKBOX *widget_checkbox_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_checkbox_submit_javascript(
		char *form_name,
		char *recall_save_javascript );

/* Usage */
/* ----- */
boolean widget_checkbox_dictionary_checked(
		char *widget_name,
		DICTIONARY *dictionary );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_checkbox_html(
		char *widget_name,
		char *prompt,
		char *onclick,
		int tab_order,
		char *image_source,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		char *background_color );

/* Process */
/* ------- */
boolean widget_checkbox_checked(
		LIST *query_row_cell_list,
		char *widget_name );

/* Returns heap memory */
/* ------------------- */
char *widget_checkbox_html_string(
		char *prompt,
		int tab_order,
		char *image_source,
		char *background_color,
		char *widget_container_key,
		boolean widget_checkbox_checked,
		char *javascript_replace );

typedef struct
{
	char *widget_name;
	char *value;
	char *prompt;
	char *onclick;
} WIDGET_RADIO;

/* Usage */
/* ----- */
WIDGET_RADIO *widget_radio_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_RADIO *widget_radio_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_radio_html(
		char *widget_name,
		char *value,
		char *prompt,
		char *onclick );

typedef struct
{
	char *value_string;
	char *display_string;
	char *tag;
} WIDGET_DROP_DOWN_OPTION;

/* Usage */
/* ----- */
LIST *widget_drop_down_option_list(
		const char sql_delimiter,
		const char widget_drop_down_label_delimiter /* '|' */,
		const char widget_drop_down_extra_delimiter /* '[' */,
		const char *widget_drop_down_dash_delimiter,
		LIST *delimited_list,
		boolean no_initial_capital );

/* Usage */
/* ----- */
WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_delimited_new(
		const char widget_drop_down_label_delimiter /* '|' */,
		const char widget_drop_down_extra_delimiter /* '[' */,
		const char *widget_drop_down_dash_delimiter,
		boolean no_initial_capital,
		char *sql_delimiter_string,
		char *delimited_string );

/* Process */
/* ------- */
WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_option_tag(
		char *widget_drop_down_option_value_string,
		char *widget_drop_down_option_display_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_option_value_string(
		char widget_drop_down_label_delimiter /* '|' */,
		char widget_drop_down_extra_delimiter /* '[' */,
		char *delimited_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_drop_down_option_display_string(
		const char widget_drop_down_label_delimiter /* '|' */,
		const char *widget_drop_down_dash_delimiter,
		boolean no_initial_capital,
		char *sql_delimiter_string,
		char *delimited_string );

/* Usage */
/* ----- */
WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_new(
		char *string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_drop_down_option_list_html(
		LIST *widget_drop_down_option_list,
		char *widget_drop_down_value );

/* Process */
/* ------- */
WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_seek(
		char *widget_drop_down_value,
		LIST *widget_drop_down_option_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_option_static_tag(
		char *widget_operator );

typedef struct
{
	char *widget_name /* mutually exclusive */;

	/* Set externally */
	/* -------------- */
	LIST *attribute_name_list /* mutually exclusive */;
	char *many_folder_name;
	boolean top_select_boolean;
	boolean null_boolean;
	boolean not_null_boolean;
	boolean bottom_select_boolean;
	int display_size;
	int tab_order;
	boolean multi_select_boolean;
	char *post_change_javascript;
	LIST *widget_drop_down_option_list;
	LIST *foreign_key_list;
} WIDGET_DROP_DOWN;

/* Usage */
/* ----- */
WIDGET_DROP_DOWN *widget_drop_down_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_DROP_DOWN *widget_drop_down_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_drop_down_html(
		char *widget_name /* mutually exclusive */,
		LIST *attribute_name_list /* mutually exclusive */,
		boolean top_select_boolean,
		boolean null_boolean,
		boolean not_null_boolean,
		boolean bottom_select_boolean,
		int display_size,
		int tab_order,
		boolean multi_select_boolean,
		char *post_change_javascript,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *widget_drop_down_option_list,
		LIST *foreign_key_list,
		char *background_color,
		char *widget_drop_down_id );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_name(
		char sql_delimiter,
		char *widget_name /* mutually exclusive */,
		LIST *attribute_name_list /* mutually exclusive */ );

/* Returns static memory or widget_container_key */
/* --------------------------------------------- */
char *widget_drop_down_row_number_id(
		char *widget_drop_down_id,
		int row_number,
		char *widget_container_key );

int widget_drop_down_display_size(
		int widget_multi_display_size,
		int display_size,
		boolean multi_select_boolean );

boolean widget_drop_down_top_select_boolean(
		boolean top_select_boolean,
		char *widget_drop_down_value );

boolean widget_drop_down_bottom_select_boolean(
		boolean bottom_select_boolean,
		boolean widget_drop_down_top_select_boolean );

boolean widget_drop_down_null_boolean(
		boolean null_boolean,
		boolean widget_drop_down_top_select_boolean );

/* Returns heap memory */
/* ------------------- */
char *widget_drop_down_html_string(
		char *widget_drop_down_select_tag,
		char *widget_drop_down_relation_join_html,
		char *widget_drop_down_option_list_html,
		char *widget_drop_down_select_close_tag );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *widget_drop_down_value(
		char sql_delimiter,
		char *widget_name,
		LIST *query_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *foreign_key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_drop_down_select_tag(
		int widget_drop_down_display_size,
		int tab_order,
		boolean multi_select_boolean,
		char *widget_container_key,
		char *widget_drop_down_row_number_id,
		char *javascript_replace,
		char *background_color,
		boolean top_select_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_select_close_tag(
		boolean null_boolean,
		boolean not_null_boolean,
		boolean bottom_select_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_drop_down_relation_join_html(
		char *widget_name,
		RELATION_JOIN_ROW *relation_join_row );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_drop_down_id(
		char *many_folder_name,
		char *widget_name );

typedef struct
{
	BUTTON *button;
} WIDGET_BUTTON;

/* Usage */
/* ----- */
WIDGET_BUTTON *widget_button_new(
		char *label );

/* Process */
/* ------- */
WIDGET_BUTTON *widget_button_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *widget_button_submit_reset_container_list(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_button_html(
		int row_number,
		WIDGET_BUTTON *widget_button );

typedef struct
{
	char *widget_name;
	boolean no_initial_capital_boolean;
} WIDGET_NON_EDIT_TEXT;

/* Usage */
/* ----- */
WIDGET_NON_EDIT_TEXT *widget_non_edit_text_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_NON_EDIT_TEXT *widget_non_edit_text_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_non_edit_text_html(
		LIST *query_row_cell_list,
		char *widget_name,
		boolean no_initial_capital_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_non_edit_text_html_string(
		char *widget_name,
		boolean no_initial_capital_boolean,
		char *widget_container_value );

typedef struct
{
	char *widget_name;
	char *value_string;
} WIDGET_HIDDEN;

/* Usage */
/* ----- */
WIDGET_HIDDEN *widget_hidden_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_HIDDEN *widget_hidden_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_hidden_html(
		int row_number,
		LIST *query_cell_list,
		WIDGET_HIDDEN *widget_hidden );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_hidden_html_string(
		char *widget_hidden_container_key,
		char *widget_hidden_value );

/* Usage */
/* ----- */

/* Returns value_string, heap memory, or null */
/* ------------------------------------------ */
char *widget_hidden_value(
		LIST *query_row_cell_list,
		char *widget_name,
		char *value_string );

/* Usage */
/* ----- */

/* Returns widget_name or static memory */
/* ------------------------------------ */
char *widget_hidden_container_key(
		char *widget_name,
		int row_number );

typedef struct
{
	char *heading_string;
} WIDGET_TABLE_HEADING;

/* Usage */
/* ----- */
WIDGET_TABLE_HEADING *widget_table_heading_new(
		char *heading_string );

/* Process */
/* ------- */
WIDGET_TABLE_HEADING *widget_table_heading_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_table_heading_html(
		char *heading_string );

typedef struct
{
	char *tag;
} WIDGET_LINE_BREAK;

/* Usage */
/* ----- */
WIDGET_LINE_BREAK *widget_line_break_new(
		void );

/* Process */
/* ------- */
WIDGET_LINE_BREAK *widget_line_break_calloc(
		void );

typedef struct
{
	boolean align_right;
	int column_span;
	int width_em;
	char *style;
} WIDGET_TABLE_DATA;

/* Usage */
/* ----- */
WIDGET_TABLE_DATA *widget_table_data_calloc(
		void );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *widget_table_data_tag(
		boolean align_right,
		int column_span,
		int width_em,
		char *style );

typedef struct
{
	char *widget_drop_down_select_tag;
	char *widget_drop_down_select_close_tag;
	char *html;
} WIDGET_MULTI_EMPTY_DROP_DOWN;

/* Usage */
/* ----- */
WIDGET_MULTI_EMPTY_DROP_DOWN *widget_multi_empty_drop_down_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_MULTI_EMPTY_DROP_DOWN *widget_multi_empty_drop_down_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *widget_multi_empty_drop_down_html(
		char *widget_drop_down_select_tag,
		char *widget_drop_down_select_close_tag );

typedef struct
{
	char *original_name;
	WIDGET_DROP_DOWN *original_drop_down;
	WIDGET_TABLE_DATA *table_data;
	WIDGET_BUTTON *move_right_button;
	WIDGET_LINE_BREAK *widget_line_break;
	WIDGET_BUTTON *move_left_button;
	WIDGET_MULTI_EMPTY_DROP_DOWN *multi_empty_drop_down;
	boolean null_boolean;
	boolean not_null_boolean;
} WIDGET_MULTI_DROP_DOWN;

/* Usage */
/* ----- */
WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *widget_multi_drop_down_original_name(
		char *widget_multi_original_prefix,
		char *widget_name );

/* Returns program memory */
/* ---------------------- */
char *widget_multi_drop_down_right_label(
		void );

/* Returns heap memory */
/* ------------------- */
char *widget_multi_drop_down_right_action_string(
		char *widget_multi_drop_down_original_name,
		char *widget_name,
		char widget_multi_move_left_right_delimiter );

/* Returns program memory */
/* ---------------------- */
char *widget_multi_drop_down_left_label(
		void );

/* Returns heap memory */
/* ------------------- */
char *widget_multi_drop_down_left_action_string(
		char *widget_multi_drop_down_original_name,
		char *widget_name,
		char widget_multi_move_left_right_delimiter );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_multi_drop_down_html(
		char *state,
		WIDGET_DROP_DOWN *original_drop_down,
		WIDGET_TABLE_DATA *table_data,
		WIDGET_BUTTON *move_right_button,
		WIDGET_LINE_BREAK *widget_line_break,
		WIDGET_BUTTON *move_left_button,
		WIDGET_MULTI_EMPTY_DROP_DOWN *multi_empty_drop_down,
		boolean null_boolean,
		boolean not_null_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_multi_drop_down_html_string(
		char *widget_drop_down_html,
		char *widget_table_data_tag,
		char *widget_table_open_tag,
		char *widget_table_row_tag,
		char *move_right_html,
		char *widget_line_break_tag,
		char *move_left_html,
		char *widget_table_close_tag,
		char *widget_multi_empty_drop_down_html );

typedef struct
{
	char *widget_name;

	/* Set externally */
	/* -------------- */
	int attribute_size;
	boolean null_to_slash;
	char *post_change_javascript;
	int tab_order;
} WIDGET_NOTEPAD;

/* Usage */
/* ----- */
WIDGET_NOTEPAD *widget_notepad_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_NOTEPAD *widget_notepad_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_notepad_html(
		char *widget_name,
		int attribute_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_notepad_html_string(
		int tab_order,
		char *background_color,
		char *widget_container_key,
		char *widget_container_value,
		int attribute_size,
		int widget_notepad_columns,
		int widget_notepad_rows,
		char *widget_change_replace_javascript );

typedef struct
{
	char *widget_name;
	int attribute_width_max_length;
	int widget_text_display_size;
	boolean null_to_slash;
	char *post_change_javascript;
	int tab_order;
} WIDGET_TIME;

/* Usage */
/* ----- */
WIDGET_TIME *widget_time_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_TIME *widget_time_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_time_html(
		char *widget_name,
		int attribute_width_max_length,
		int widget_text_display_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		char *background_color );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_time_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		char *background_color,
		char *widget_container_key,
		char *widget_container_value,
		char *widget_text_replace_javascript );

typedef struct
{
	char *widget_name;

	/* Set externally */
	/* -------------- */
	char *datatype_name;
	char *application_name;
	char *login_name;
	int attribute_width_max_length;
	int display_size;
	boolean null_to_slash;
	char *post_change_javascript;
	int tab_order;
} WIDGET_DATE;

/* Usage */
/* ----- */
WIDGET_DATE *widget_date_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_DATE *widget_date_calloc(
		void );

/* Usage */
/* ----- */
boolean widget_date_calendar_boolean(
		char *datatype_name,
		char *state );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_date_html(
		char *widget_name,
		char *datatype_name,
		int attribute_width_max_length,
		int display_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *application_name,
		char *login_name,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *widget_date_format_template(
		char *appaserver_user_date_format );

/* Returns static memory */
/* --------------------- */
char *widget_date_calendar_image_tag(
		char *widget_container_key,
		char *widget_date_format_template );

/* Returns heap memory */
/* ------------------- */
char *widget_date_html_string(
		int attribute_width_max_length,
		int display_size,
		char *widget_container_key,
		char *widget_container_value,
		char *widget_text_replace_javascript,
		char *widget_date_calendar_image_tag,
		int tab_order,
		char *background_color );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_date_login_name_now_date_string(
		int attribute_width_max_length,
		char *application_name,
		char *login_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_date_portion_date(
		char *date_now_string );

/* Returns heap memory */
/* ------------------- */
char *widget_date_portion_time(
		char *date_now_string );

/* Returns heap memory */
/* ------------------- */
char *widget_date_combine_date_string(
		char *date_convert_source_international,
		char *widget_date_portion_time );

#define WIDGET_TEXT_DEFAULT_DISPLAY_SIZE	10
#define WIDGET_TEXT_DEFAULT_MAX_LENGTH		30

typedef struct
{
	char *widget_name;
	char *datatype_name;
	int widget_text_display_size;
	int attribute_width_max_length;
	boolean null_to_slash;
	char *post_change_javascript;
	int tab_order;
	char *value_string;
	boolean viewonly;
} WIDGET_TEXT;

/* Usage */
/* ----- */
WIDGET_TEXT *widget_text_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_TEXT *widget_text_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_text_html(
		char *widget_name,
		int attribute_width_max_length,
		int widget_text_display_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *value_string,
		boolean viewonly,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_change_replace_javascript(
		char *post_change_javascript,
		int row_number,
		char *state,
		boolean null_to_slash );

boolean widget_text_autocomplete_off(
		char *widget_name );

/* Returns heap memory */
/* ------------------- */
char *widget_text_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		boolean viewonly,
		char *background_color,
		char *widget_container_name,
		char *widget_text_value_string,
		char *widget_text_replace_javascript,
		boolean widget_text_autocomplete_off );

/* Usage */
/* ----- */
int widget_text_display_size(
		int widget_text_default_max_length,
		int attribute_width );

/* Usage */
/* ----- */

/* Returns value_string, component of query_cell_list, heap memory, or null */
/* ------------------------------------------------------------------------ */
char *widget_text_value_string(
		LIST *query_cell_list,
		char *value_string,
		char *widget_name );

typedef struct
{
	char *widget_password_display_name;
	WIDGET_TEXT *text;
	/* -------------- */
	/* Set externally */
	/* -------------- */
	char *attribute_name;
} WIDGET_PASSWORD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_PASSWORD *widget_password_new(
		char *widget_password_display_name );

/* Process */
/* ------- */
WIDGET_PASSWORD *widget_password_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_password_html(
		char *widget_password_display_name,
		char *attribute_name,
		WIDGET_TEXT *widget_text,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		char *background_color );

/* Usage */
/* ----- */

/* Returns program memory or null */
/* ------------------------------ */
char *widget_password_value_string(
		LIST *query_row_cell_list,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_password_display_name(
		char *attribute_name );

typedef struct
{
	char *widget_name;
	int attribute_width_max_length;
	int tab_order;
} WIDGET_ENCRYPT;

/* Usage */
/* ----- */
WIDGET_ENCRYPT *widget_encrypt_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_ENCRYPT *widget_encrypt_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_encrypt_html(
		int widget_text_display_size,
		int row_number,
		LIST *query_cell_list,
		char *widget_name,
		int attribute_width_max_length,
		int tab_order );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *widget_encrypt_compare_name(
		const char *widget_encrypt_compare_label,
		char *widget_name,
		int row_number );

/* Returns heap memory */
/* ------------------- */
char *widget_encrypt_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		char *widget_encrypt_compare_name,
		char *widget_container_key,
		char *widget_container_value );

typedef struct
{
	char *application_name;
	char *session_key;
	char *widget_name;
	int tab_order;
	char *post_change_javascript;
} WIDGET_UPLOAD;

/* Usage */
/* ----- */
WIDGET_UPLOAD *widget_upload_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_UPLOAD *widget_upload_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_upload_prompt_frame_html(
		char *widget_name,
		int tab_order );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_upload_prompt_frame_html_string(
		char *widget_container_key,
		int tab_order );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_upload_edit_frame_html(
		char *application_name,
		char *session_key,
		char *widget_name,
		LIST *query_cell_list,
		char *background_color );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *widget_upload_source_filespecification(
		char *application_name,
		char *appaserver_parameter_upload_directory,
		char *widget_container_value );

/* Returns static memory */
/* --------------------- */
char *widget_upload_filename_basename(
		char *widget_container_value );


/* Returns static memory or null */
/* ----------------------------- */
char *widget_upload_filename_extension(
		char *widget_container_value );


/* Returns static memory */
/* --------------------- */
char *widget_upload_link_system_string(
		char *appaserver_parameter_document_root,
		char *widget_upload_source_filespecification,
	       	char *widget_upload_hypertext );

/* Returns heap memory */
/* ------------------- */
char *widget_upload_edit_frame_html_string(
		char *background_color,
		char *widget_container_value,
		char *widget_upload_hypertext );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_upload_hypertext(
		char *application_name,
		char *session_key,
		char *widget_upload_filename_basename,
		char *widget_upload_filename_extension );

typedef struct
{
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option;
} WIDGET_YES_NO_OPTION;

/* Usage */
/* ----- */
WIDGET_YES_NO_OPTION *widget_yes_no_option_new(
		boolean yes_boolean,
		boolean no_boolean,
		boolean select_boolean );

/* Process */
/* ------- */
WIDGET_YES_NO_OPTION *widget_yes_no_option_calloc(
		void );

typedef struct
{
	char *widget_name;
	char *post_change_javascript;
	int tab_order;
} WIDGET_YES_NO;

/* Usage */
/* ----- */
WIDGET_YES_NO *widget_yes_no_new(
		char *widget_name );

/* Process */
/* ------- */
WIDGET_YES_NO *widget_yes_no_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_yes_no_html(
		char *widget_name,
		char *post_change_javascript,
		int tab_order,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color );

/* Process */
/* ------- */
boolean widget_yes_no_affirmative_boolean(
		char *widget_container_value );

boolean widget_yes_no_not_affirmative_boolean(
		char *widget_container_value );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_yes_no_html_string(
		char *widget_container_key,
		boolean widget_yes_no_affirmative_boolean,
		boolean widget_yes_no_not_affirmative_boolean,
		char *javascript_replace,
		int tab_order,
		char *background_color );

typedef struct
{
	char *data;
	int attribute_width;
	char *heading;
	boolean omit_update;
} WIDGET_REFERENCE_NUMBER;

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
} WIDGET_RADIO_BUTTON;

WIDGET_RADIO_BUTTON *widget_radio_button_calloc(
			void );

typedef struct
{
	enum widget_type widget_type;
	char *widget_name;
	char *heading_string;
	char *widget_key;
	boolean recall_boolean;

	union {
		WIDGET_TABLE_OPEN *table_open;
		WIDGET_TABLE_HEADING *table_heading;
		WIDGET_TABLE_ROW *table_row;
		WIDGET_TABLE_CLOSE *table_close;
		WIDGET_CHECKBOX *checkbox;
		WIDGET_DROP_DOWN *drop_down;
		WIDGET_BUTTON *widget_button;
		WIDGET_NON_EDIT_TEXT *non_edit_text;
		WIDGET_HIDDEN *hidden;
		WIDGET_LINE_BREAK *line_break;
		WIDGET_TABLE_DATA *table_data;
		WIDGET_MULTI_DROP_DOWN *multi_drop_down;
		WIDGET_NOTEPAD *notepad;
		WIDGET_TEXT *text;
		WIDGET_ENCRYPT *encrypt;
		WIDGET_YES_NO *yes_no;
		WIDGET_DATE *date;
		WIDGET_TIME *time;
		WIDGET_UPLOAD *upload;
		WIDGET_ANCHOR *anchor;
		WIDGET_RADIO *radio;
		WIDGET_AJAX *ajax;
		WIDGET_PASSWORD *password;
	};
} WIDGET_CONTAINER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *widget_container_new(
		enum widget_type widget_type,
		char *widget_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_container_key(
		char *widget_name,
		int row_number );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *widget_container_name(
		char *attribute_name,
		int row_number );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_container_value(
		LIST *query_row_cell_list,
		char *widget_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_container_list_html(
		char *state,
		int row_number /* < 1 to omit index suffix */,
		char *background_color,
		LIST *widget_container_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_container_list_hidden_html(
		int row_number,
		LIST *widget_container_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_html(
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		char *background_color,
		WIDGET_CONTAINER *widget_container );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_checkbox_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_CHECKBOX *checkbox );

/* Returns heap memory */
/* ------------------- */
char *widget_container_multi_drop_down_html(
		char *state,
		WIDGET_MULTI_DROP_DOWN *multi_drop_down );

/* Returns heap memory */
/* ------------------- */
char *widget_container_notepad_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_NOTEPAD *notepad );

/* Returns heap memory */
/* ------------------- */
char *widget_container_yes_no_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_YES_NO *yes_no );

/* Returns heap memory */
/* ------------------- */
char *widget_container_time_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_TIME *time );

/* Returns heap memory */
/* ------------------- */
char *widget_container_non_edit_text_html(
		LIST *query_cell_list,
		WIDGET_NON_EDIT_TEXT *non_edit_text );

/* Returns heap memory */
/* ------------------- */
char *widget_container_encrypt_html(
		int row_number,
		LIST *query_cell_list,
		WIDGET_ENCRYPT *encrypt );

/* Returns heap memory */
/* ------------------- */
char *widget_container_anchor_html(
		WIDGET_ANCHOR *anchor );

/* Returns heap memory */
/* ------------------- */
char *widget_container_radio_html(
		WIDGET_RADIO *radio );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_text_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_TEXT *text );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_date_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_DATE *date );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_ajax_html(
		LIST *ajax_widget_container_list,
		LIST *query_row_cell_list,
		int row_number,
		char *background_color );

/* Usage */
/* ----- */
LIST *widget_container_dictionary_hidden_list(
		DICTIONARY *dictionary );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *widget_lookup_checkbox_container(
		char *checkbox_label );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *widget_confirm_checkbox_container(
		char *checkbox_label,
		char *checkbox_prompt );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *widget_submit_button_container(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_drop_down_html(
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		char *background_color,
		WIDGET_DROP_DOWN *drop_down );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_container_heading_html(
		LIST *widget_container_list );

/* Usage */
/* ----- */
LIST *widget_container_back_to_top_list(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *widget_container_upload_html(
		LIST *query_row_cell_list,
		char *background_color,
		WIDGET_UPLOAD *upload );

/* Usage */
/* ----- */
LIST *widget_container_heading_label_list(
		LIST *widget_container_list );

/* Usage */
/* ----- */
LIST *widget_container_heading_list(
		LIST *widget_container_heading_label_list );

/* Usage */
/* ----- */
LIST *widget_container_heading_name_list(
		LIST *widget_container_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *widget_container_ajax_new(
		char *widget_name,
		LIST *ajax_client_widget_container_list );

/* Public */
/* ------ */
WIDGET_CONTAINER *widget_container_name_seek(
		char *widget_name,
		LIST *widget_container_list );

int widget_container_tab_order(
		void );

/* Returns static memory */
/* --------------------- */
char *widget_container_tab_order_html(
		int widget_container_tab_order );

/* Returns static memory */
/* --------------------- */
char *widget_container_background_color_html(
		char *background_color );

/* Returns static memory */
/* --------------------- */
char *widget_container_javascript_html(
		char *javascript_replace );

/* Returns data or heap memory */
/* --------------------------- */
char *widget_container_number_commas_string(
		char *widget_name,
		char *data );

typedef struct
{
	/* stub */
} WIDGET;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_table_edit_row_html(
		char *table_edit_state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *table_edit_apply_widget_container_list,
		char *form_background_color,
		boolean hidden_only );

/* Usage */
/* ----- */
LIST *widget_hidden_container_list(
		char *dictionary_separate_prefix,
		DICTIONARY *dictionary );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *widget_hidden_key(
		char *dictionary_separate_prefix,
		char *key );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *widget_dictionary_hidden_html(
		DICTIONARY *dictionary );

#endif
