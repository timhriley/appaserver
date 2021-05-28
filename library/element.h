/* $APPASERVER_HOME/library/element.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ELEMENT_APPASERVER_H
#define ELEMENT_APPASERVER_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "date_convert.h"
#include "boolean.h"

/* Constants */
/* --------- */
/* #define ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER	"&#151;" */
#define ELEMENT_LONG_DASH_DELIMITER			"&#151;"
#define ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER	"---"

#define ELEMENT_TITLE_NOTEPAD_PADDING_EM	3
#define ELEMENT_TEXTAREA_WRAP			"soft"
#define ELEMENT_DICTIONARY_DELIMITER		'~'
#define ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER '|'
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

enum element_type {	drop_down,
			toggle_button,
			push_button,
			radio_button,
			notepad,
			password,
			non_edit_text,
			linebreak,
			text_item,
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
			table_row,
			table_closing,
			prompt,
			prompt_heading,
			anchor,
			non_edit_multi_select };

typedef struct
{
	char *text;
	int column_span;
	int padding_em;
} ELEMENT_NON_EDIT_TEXT;

typedef struct
{
	char *heading;
	char *initial_data;
	LIST *option_data_list;
	LIST *option_label_list;
	int number_columns;
	boolean output_null_option;
	boolean output_not_null_option;
	boolean output_select_option;
	int multi_select;
	char *folder_name;
	char *post_change_javascript;
	int max_drop_down_size;
	char *multi_select_element_name;
	char *onblur_javascript_function;
	int date_piece_offset;
	boolean no_initial_capital;
	boolean readonly;
	char *state;
	int attribute_width;
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
	char *label;
	char *onclick_function;
} ELEMENT_PUSH_BUTTON;

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
} ELEMENT_LINEBREAK;

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
	enum element_type element_type;
	char *name;
	boolean omit_heading_sort_button;
	ELEMENT_DROP_DOWN *drop_down;
	ELEMENT_TOGGLE_BUTTON *toggle_button;
	ELEMENT_PUSH_BUTTON *push_button;
	ELEMENT_RADIO_BUTTON *radio_button;
	ELEMENT_NOTEPAD *notepad;
	ELEMENT_LINEBREAK *linebreak;
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
	boolean remember_keystrokes_ok;
	int tab_index;
} ELEMENT_APPASERVER;

ELEMENT_APPASERVER *element_appaserver_new(
				enum element_type,
				char *name );

void element_simple_output(	ELEMENT_APPASERVER *element,
				int row );

void element_output( 		DICTIONARY *hidden_name_dictionary,
				ELEMENT_APPASERVER *e,
				int row,
				int with_toggle_buttons,
				FILE *output_file,
				char *background_color,
				char *application_name,
				char *login_name );

void element_prompt_output(	FILE *output_file,
				char *element_name,
				boolean with_heading_format );

void element_output_non_element(	char *s,
					FILE *output_file );

void element_set_data( 			ELEMENT_APPASERVER *e, char *s );

char *element_get_heading(
				char **toggle_button_set_all_control_string,
				ELEMENT_APPASERVER *e,
				int form_number );

int element_get_attribute_width( 	ELEMENT_APPASERVER *e );

void element_set_datatype(		ELEMENT_APPASERVER *e,
					char *datatype );

ELEMENT_PUSH_BUTTON *element_push_button_new(
					void );

ELEMENT_TOGGLE_BUTTON *element_toggle_button_new(
					void );

char *element_toggle_button_get_heading(char *element_name,
					char *heading );

void element_toggle_button_set_heading(	ELEMENT_TOGGLE_BUTTON *e,
					char *heading );

void element_toggle_button_output( 	FILE *output_file,
					char *element_name,
					char *heading,
					boolean checked,
					int row,
					char onchange_submit_yn,
					char *form_name,
					char *image_source,
					char *onclick_keystrokes_save_string,
					char *onclick_function );

void element_toggle_button_set_checked(	ELEMENT_TOGGLE_BUTTON *e );

ELEMENT_RADIO_BUTTON *element_radio_button_new(	void );

char *element_radio_button_get_heading( 	char *heading,
						char *name );

void element_radio_button_set_heading(		ELEMENT_RADIO_BUTTON *e,
						char *heading );

void element_radio_button_output(		FILE *output_file,
						char onchange_submit_yn,
						char *form_name,
						char *image_source,
						char *value,
						int checked,
						char *heading,
						char *name,
						char *onclick,
						int row,
						char *state,
						char *post_change_javascript );

ELEMENT_NOTEPAD *element_notepad_new(		void );

void element_notepad_set_data(			ELEMENT_NOTEPAD *e,
						char *s );

void element_notepad_set_attribute_width(	ELEMENT_NOTEPAD *e,
						int w );

void element_notepad_set_field_width(		ELEMENT_NOTEPAD *e,
						int w );

char *element_notepad_get_heading( 		ELEMENT_NOTEPAD *e );

void element_notepad_output( 			FILE *output_file,
						int attribute_width,
						char *element_name,
						char *data,
						int row,
						int number_rows,
						char onchange_null2slash_yn,
						int tab_index );

void element_notepad_set_onchange_null2slash(	ELEMENT_NOTEPAD *e );


ELEMENT_TEXT_ITEM *element_text_item_new(	void );

void element_text_item_set_data(		ELEMENT_TEXT_ITEM *e,
						char *s );

void element_text_item_set_default(		ELEMENT_TEXT_ITEM *e,
						char *s );

void element_text_item_set_attribute_width(	ELEMENT_TEXT_ITEM *e,
						int w );

void element_text_item_set_field_width(		ELEMENT_TEXT_ITEM *e,
						int w );

void element_text_item_set_heading(		ELEMENT_TEXT_ITEM *e,
						char *s );

char *element_text_item_get_heading(		char *element_name,
						char *heading );

void element_text_item_output( 		FILE *output_file,
					char *element_name,
					char *data,
					int attribute_width,
					int row, 
					char onchange_null2slash_yn,
					char *post_change_javascript,
					char *on_focus_javascript_function,
					int widget_size,
					char *background_color,
					int tab_index,
					boolean without_td_tags,
					boolean readonly,
					char *state,
					boolean is_numeric );

void element_text_item_set_onchange_null2slash(
						ELEMENT_TEXT_ITEM *e );

ELEMENT_PASSWORD *element_password_new( 	void );

void element_password_set_data(			ELEMENT_PASSWORD *e,
						char *s );

void element_password_set_default(		ELEMENT_PASSWORD *e,
						char *s );

void element_password_set_attribute_width(	ELEMENT_PASSWORD *e,
						int w );

void element_password_set_heading(		ELEMENT_PASSWORD *e,
						char *s );

char *element_password_get_heading(		char *element_name,
						char *heading );

void element_password_output( 			FILE *output_file,
						char *element_name,
						char *data,
						int attribute_width,
						int row,
						int tab_index );

ELEMENT_REFERENCE_NUMBER *element_reference_number_new(
						void );

void element_reference_number_set_data(		ELEMENT_REFERENCE_NUMBER *e,
						char *s );

void element_reference_number_set_attribute_width(	
						ELEMENT_REFERENCE_NUMBER *e,
						int w );

void element_reference_number_set_heading(	ELEMENT_REFERENCE_NUMBER *e,
						char *s );

char *element_reference_number_get_heading( 	char *element_name,
						char *heading );

void element_reference_number_output( 		FILE *output_file,
						char *element_name,
						char *data,
						int row,
						int attribute_width,
						boolean omit_update );

ELEMENT_EMPTY_COLUMN *element_empty_column_new(
			void );

void element_empty_column_output(
			FILE *output_file );

ELEMENT_HIDDEN *element_hidden_new(
			void );

void element_hidden_set_data(
			ELEMENT_HIDDEN *e,
			char *s );

void element_hidden_name_dictionary_output(
			FILE *output_file,
			DICTIONARY *hidden_name_dictionary,
			int row,
			char *name,
			char *data );

ELEMENT_PROMPT_DATA *element_prompt_data_new( 	void );

char *element_prompt_data_get_heading( 		char *element_name,
						char *heading );

void element_prompt_data_set_data(		ELEMENT_PROMPT_DATA *e,
						char *s );

void element_prompt_data_set_heading( 		ELEMENT_PROMPT_DATA *e,
						char *heading );

void element_prompt_data_output(		FILE *output_file,
						char *element_name,
						char *align,
						char *data,
						boolean format_initial_capital);

ELEMENT_DROP_DOWN *element_drop_down_new(	void );

void element_drop_down_set_option_data_option_label_list(
						LIST **option_data_list,
						LIST **option_label_list,
						LIST *source_list );

void element_drop_down_output(
			FILE *output_file,
			char *element_name,
			LIST *option_data_list,
			LIST *option_label_list,
			int number_columns,
			boolean multi_select,
			int row,
			char *initial_data,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			char *post_change_javascript,
			int max_drop_down_size,
			char *multi_select_element_name,
			char *onblur_javascript_function,
			char *background_color,
			int date_piece_offset,
			boolean no_initial_capital,
			boolean readonly,
			int tab_index,
			char *state,
			int attribute_width );

void element_drop_down_set_output_null_option(
						ELEMENT_DROP_DOWN *e );
void element_drop_down_set_output_not_null_option(
						ELEMENT_DROP_DOWN *e );

ELEMENT_NON_EDIT_MULTI_SELECT *element_non_edit_multi_select_new(
						void );

void element_non_edit_multi_select_set_option_label_list(
					ELEMENT_NON_EDIT_MULTI_SELECT *e,
					LIST *l );

char *element_non_edit_multi_select_get_heading(
					char *element_name );

void element_non_edit_multi_select_output(
					FILE *output_file,
					char *element_name,
					LIST *option_label_list );

void element_non_edit_multi_select_set_option_label_list(
					ELEMENT_NON_EDIT_MULTI_SELECT *e,
					LIST *l );

ELEMENT_LINEBREAK *element_linebreak_new( void );

void element_linebreak_output( 			FILE *output_file );


void element_blank_data_output(			FILE *output_file );

char *element_drop_down_get_heading(		char *element_name,
						char *heading );

ELEMENT_HTTP_FILENAME *element_http_filename_new(
						void );
ELEMENT_ANCHOR *element_anchor_new(		void );

void element_http_filename_output(	FILE *output_file,
					ELEMENT_HTTP_FILENAME *http_filename,
					int row,
					char *background_color,
					char *element_name,
					char *application_name );

void element_anchor_output(			FILE *output_file,
						char *prompt,
						char *href );

void element_table_opening_output(		FILE *output_file );

void element_table_row_output(			FILE *output_file );

void element_table_closing_output(		FILE *output_file );

ELEMENT_UPLOAD_FILENAME *element_upload_filename_new(
						void );

void element_upload_filename_output(		FILE *output_file,
						int attribute_width,
						int row,
						char *name );

void element_javascript_filename_output(	FILE *output_file,
						char *filename );

ELEMENT_APPASERVER *element_non_edit_text_new_element(
						char *name,
						char *text,
						int column_span,
						int padding_em );

ELEMENT_NON_EDIT_TEXT *element_new_non_edit_text(
						void );

void element_non_edit_text_output(		FILE *output_file,
						char *text,
						int column_span,
						int padding_em );

int element_type_count(				LIST *element_list,
						enum element_type );

ELEMENT_APPASERVER *element_get_yes_no_element(	char *attribute_name,
						char *prepend_folder_name,
						char *post_change_javascript,
						boolean with_is_null,
						boolean with_not_null );

LIST *element_get_shifted_option_label_list(	char **shifted_initial_label,
						LIST *option_data_list,
						char delimiter );

int element_exists_delimiter(			char *option_data,
						char delimiter );

char *element_shift_first_label(		char *data,
						char delimiter );

char *element_data2label(			char *destination,
						char *data,
						boolean no_initial_capital );

char *element_replace_javascript_variables(
						char *destination,
						char *source,
						int row,
						char *state );

char *element_delimit_drop_down_data(		char *destination,
						char *source,
						int date_piece_offset );

void element_output_as_dictionary(
				FILE *output_file,
				ELEMENT_APPASERVER *element, 
				int row );

void element_notepad_output_as_dictionary(
						FILE *output_file,
						char *element_name,
						char *data,
						int row );
void element_text_item_output_as_dictionary(
						FILE *output_file,
						char *element_name,
						char *element_data,
						int row );
void element_password_output_as_dictionary(
						FILE *output_file,
						char *element_name,
						char *data,
						int row );

void element_drop_down_output_as_dictionary(
				FILE *output_file,
				char *element_name,
				int row,
				char *initial_data,
				char *folder_name,
				int length_option_data_list,
				int max_drop_down_size );

void element_reference_number_output_as_dictionary(
						FILE *output_file,
						char *element_name,
						char *data,
						int row );

void element_hidden_output(
				FILE *output_file,
				char *name,
				char *data,
				int row );

ELEMENT_APPASERVER *element_text_item_variant_element(
				char *attribute_name,
				char *datatype,
				int width,
				char *post_change_javascript,
				char *on_focus_javascript_function );

void element_non_edit_text_output_as_dictionary(
					FILE *output_file,
					char *name,
					char *text,
					int row );

ELEMENT_APPASERVER *element_hidden_new_element(
					char *name,
					char *data );

char *element_appaserver_list_display(
			LIST *element_list );

char *element_appaserver_display(
			ELEMENT_APPASERVER *element );

char *element_get_type_string(		enum element_type element_type );

char *element_get_destination_multi_select_element_name(
					LIST *element_list );

boolean element_exists_upload_filename( LIST *element_list );

boolean element_exists_reference_number(LIST *element_list );

LIST *element_list2remember_keystrokes_non_multi_element_name_list(
					LIST *element_list );

LIST *element_list2remember_keystrokes_multi_element_name_list(
					LIST *element_list );

char *element_get_element_type_string(	enum element_type );

void element_date_output( 		FILE *output_file,
					char *element_name,
					char *data,
					int attribute_width,
					int row, 
					char onchange_null2slash_yn,
					char *post_change_javascript,
					char *on_focus_javascript_function,
					int widget_size,
					char *background_color,
					char *application_name,
					char *login_name,
					boolean with_calendar_popup,
					boolean readonly,
					int tab_index );

char *element_data_delimiter2label_delimiter(
					char *source_destination );

void element_password_erase_data(	char *data );

boolean element_combined_option_data_list(
					LIST *option_data_list );

LIST *element_get_combined_option_label_list(
					LIST *option_data_list );

void element_drop_down_get_initial_label(
					char *initial_label,
					char *initial_data,
					boolean no_initial_capital,
					LIST *option_data_list,
					LIST *option_label_list );

void element_data_list_to_label(	char *label,
					char *data,
					LIST *option_data_list,
					LIST *option_label_list );

void element_list_set_omit_heading_sort_button(
					LIST *element_list,
					LIST *join_1tom_related_folder_list );

char *element_get_date_format_string(
					enum date_convert_format );

void element_list_set_readonly(
					LIST *element_list );

void element_push_button_output( 	FILE *output_file,
					char *element_label,
					int row,
					char *onclick_function );

char *element_seek_initial_data(	char **initial_label,
					char *initial_data,
					LIST *option_data_list,
					LIST *option_label_list );

char *element_place_commas_in_number_string(
					char *element_name,
					char *data );

#endif
