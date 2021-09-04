/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_H
#define FORM_H

#include <unistd.h>
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "boolean.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define FORM_CAPTION_HEADING_TAG		"h3"
#define FORM_COLOR1				"#dccdde"
#define FORM_COLOR2				"#ffe6ca"
#define FORM_COLOR3				"#d2ecf2"
#define FORM_COLOR4				"#f7ffce"
#define FORM_COLOR5				"#f2e0b8"

/*
#define FORM_COLOR1				"#73d2ff"
#define FORM_COLOR2				"#8cdaff"
#define FORM_COLOR3				"#a6e2ff"
#define FORM_COLOR4				"#ccefff"
#define FORM_COLOR5				"#e6f7ff"
*/

#define FORM_INSERT_UPDATE_KEY_PLACEHOLDER	"insert_update_key"
#define FORM_DEFAULT_INSERT_ROWS_NUMBER		5
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
	/* Input */
	/* ----- */
	char *state;
	char *application_title;
	LIST *regular_element_list;
	LIST *viewonly_element_list;

	char *insert_update_key;
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *target_frame;
	char *post_process;
	int output_pairs;
	int submit_button_in_heading;
	int submit_button_in_trailer;
	int remember_keystrokes_button_in_heading;
	int remember_keystrokes_button_in_trailer;
	int output_table_heading;
	int output_content_type;
	int output_row_zero_only;
	int insert_rows_number;
	int current_row;
	int drop_down_number_columns;
	LIST *row_dictionary_list;
	LIST *operation_list;
	char *post_dictionary_string;
	int output_multi_selects_first;
	int output_dynamic_drop_down;
	char *element_name_string_array_string;
	char *option_value_string_array_string;
	char *option_display_string_array_string;
	int omit_output_html;
	int omit_output_body;
	int dont_set_data_as_current_value;
	char *operation_button_in_trailer_sys_string;
	char *operation_button_in_trailer_prompt;
	int dont_output_document_heading;
	boolean dont_output_operations;
	char *form_title;
	char *action_string;
	int current_reference_number;
	char *onload_control_string;
	char submit_control_string[ FORM_SUBMIT_CONTROL_STRING_SIZE ];
	char *remember_keystrokes_button_control_string;
	int table_border;
	int omit_folder_name_in_title;
	char *submit_process;
	char *html_help_file_anchor;
	pid_t process_id;
	char *optional_related_attribute_name;
	char *onclick_keystrokes_save_string;
	char *subtitle_string;
	DICTIONARY *hidden_name_dictionary;
	LIST *form_button_list;
} FORM;

/* Operations */
/* ---------- */
FORM_BUTTON *form_button_new(
			char *button_label,
			char *onclick_control_string );

FORM *form_new(		char *state,
			char *application_title );

FORM *form_calloc(	void );

void form_set_output_pairs( 		FORM *form );
void form_set_submit_button_in_heading(	FORM *form );
void form_set_submit_button_in_trailer(	FORM *form );
void form_set_remember_keystrokes_button_in_trailer(
					FORM *form );
void form_set_remember_keystrokes_button_in_heading(
					FORM *form );

void form_set_output_content_type(	FORM *form );

void form_set_drop_down_number_columns(	FORM *form,
					int drop_down_number_columns );

void form_set_post_process(		FORM *form,
					char *post_process );

void form_output_html_heading(		FORM *form,
					char *application_name );

void form_output_heading(
			char *login_name,
			char *application_name,
			char *session,
			char *form_name,
			char *post_process,
			char *action_string,
			char *folder_name,
			char *role_name,
			char *state,
			char *insert_update_key,
			char *target_frame,
			boolean output_submit_reset_buttons,
			boolean with_prelookup_skip_button,
			char *submit_control_string,
			int table_border,
			char *caption_string,
			char *html_help_file_anchor,
			pid_t process_id,
			char *server_address,
			char *optional_related_attribute_name,
			char *remember_keystrokes_onload_control_string,
			char *post_change_javascript );

int form_output_body(	int *form_current_reference_number,
			DICTIONARY *hidden_name_dictionary,
			int output_row_zero_only,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			char *spool_filename,
			boolean row_level_non_owner_view_only,
			char *application_name,
			char *login_name,
			char *attribute_not_null_string,
			char *appaserver_user_foreign_login_name,
			LIST *non_edit_folder_name_list );

void form_output_trailer_post_change_javascript(
			boolean output_submit_reset_buttons,
			boolean output_insert_button,
			char *submit_control_string,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *prelookup_button_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			int form_number,
			char *post_change_javascript );

void form_output_trailer(
			boolean output_submit_reset_buttons,
			boolean output_insert_button,
			char *submit_control_string,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *prelookup_button_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			int form_number,
			LIST *form_button_list,
			char *post_change_javascript );

void form_output_row(	int *form_current_reference_number,
			DICTIONARY *hidden_name_dictionary,
			LIST *element_list,
			DICTIONARY *dictionary,
			int row,
			int with_push_buttons,
			FILE *spool_file,
			boolean row_level_non_owner_view_only,
			char *application_name,
			char *login_name,
			char *background_color,
			char *appaserver_user_foreign_login_name,
			LIST *non_edit_folder_name_list );

int form_output_insert_rows(
			int *form_current_reference_number,
			DICTIONARY *hidden_name_dictionary,
			LIST *element_list,
			int current_row,
			int insert_rows_number,
			DICTIONARY *post_dictionary,
			char *application_name,
			char *login_name );

int form_output_all_rows(
			int *form_current_reference_number,
			DICTIONARY *hidden_name_dictionary,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			int current_row,
			FILE *spool_file,
			boolean row_level_non_owner_view_only,
			char *application_name,
			char *login_name,
			char *attribute_not_null_string,
			char *appaserver_user_foreign_login_name,
			LIST *non_edit_folder_name_list );

void form_output_prompt( 		ELEMENT_APPASERVER *element );
void form_output_table_heading(		LIST *element_list,
					int form_number );
void form_set_output_table_heading(	FORM *form );

void form_output_submit_button(
			char *submit_control_string,
			char *button_label,
			int form_number );

void form_output_submit_reset_buttons(
			char *submit_control_string,
			char *button_label,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			boolean with_prelookup_skip_button,
			int form_number,
			char *post_change_javascript );

void form_set_insert_rows_number( 	FORM *form, 
					int insert_rows_number );

void form_set_starting_row(	 	FORM *form, 
					int starting_row );

void form_set_insert_update_key(	FORM *form, char *insert_update_key );

void form_fetch_row_dictionary_list( 	FORM *form, 
					LIST *attribute_name_list );

int form_output_prompt_edit( 		FORM *form );

void form_set_row_dictionary_list( 	FORM *form, 
					LIST *row_dictionary_list );

void form_set_omit_output_html(		FORM *form );

void form_set_omit_output_body(		FORM *form );

void form_set_query_dictionary_string(	FORM *form,
					char *query_dictionary_string );

void form_set_output_multi_selects_first( 	
					FORM *form );
void form_set_output_dynamic_drop_down(	
				FORM *form,
				char *element_name_string_array_string,
				char *option_value_string_array_string,
				char *option_display_string_array_string );
void form_set_operation_button_in_trailer( 
				FORM *form, 
				char *operation_button_in_trailer_sys_string,
				char *operation_button_in_trailer_prompt );
void form_output_operation_button( 
				FORM *form );
LIST *form_get_hydrology_validation_element_list(
				HASH_TABLE *attribute_hash_table,
				LIST *attribute_name_list );
void form_dont_output_document_heading( FORM *form );
void form_set_title(		FORM *form, char *title );
LIST *form_dictionary2hidden_element_list( 
				DICTIONARY *dictionary );
void form_set_action(		FORM *form, char *action_string );
void form_set_reference_number(
				FORM *form,
				char *reference_number );

char *form_get_next_reference_number(
				int *form_current_reference_number );

void form_set_folder_parameters(FORM *form,
				char *state,
				char *login_name,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name );

void form_output_content_type( 	void );
void form_set_dont_output_operations(
				FORM *form );
void form_set_populate_default_from_cookies( 
				FORM *form );

/* Appends to form->submit_control_string */
/* -------------------------------------- */
void form_append_remember_keystrokes_submit_control_string( 
				char **onclick_keystrokes_save_string,
				FORM *form,
				LIST *non_multi_element_name_list,
				LIST *multi_element_name_list,
				char *cookie_key_prefix,
				char *cookie_key );

char *form_get_onload_control_string( 
				FORM *form );

char *form_get_load_button_control_string( 
				FORM *form );

char *form_get_submit_control_string( 
				FORM *form );

void form_set_table_border(	FORM *form );

void form_set_remember_keystrokes_load_button_control_string(
				FORM *form,
				LIST *element_name_list );

void form_output_remember_keystrokes_button(
				char *control_string );

char *form_get_remember_keystrokes_onload_control_string(
				char *form_name,
				LIST *non_multi_element_name_list,
				LIST *multi_element_name_list,
				char *post_change_javascript,
				char *cookie_key_prefix,
				char *cookie_key );

void form_set_current_row(	FORM *form,
				int current_row );

void form_output_closing(	char *submit_control_string );

void form_output_title(		char *application_title,
				char *state,
				char *form_title,
				char *folder_name,
				char *subtitle_string,
				boolean omit_format_initial_capital );

void form_output_sort_buttons(	FILE *output_file,
				LIST *element_list,
				char *insert_update_key );

void form_append_submit_control_string(
				char *submit_control_string,
				char *control_string );

char *form_get_background_color(char *application_name );

void form_build_cookie_key_buffer(
				char *cookie_key_buffer,
				char *cookie_key_prefix,
				char *cookie_key );

void form_set_new_button_onclick_keystrokes_save_string(
			LIST *element_list,
			char *onclick_keystrokes_save_string );

void form_output_prelookup_button(
				char *control_string,
				boolean with_back_to_top_button );

void form_execute_output_prompt_edit_form(
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *error_file,
				DICTIONARY_SEPARATE *dictionary_separate,
				char *target_frame );

void form_execute_output_prompt_insert_form(
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *error_file,
				DICTIONARY_SEPARATE *dictionary_separate );

void form_output_back_to_top_button(
				void );

char **form_get_background_color_array(
			int *background_color_array_length,
			char *application_name );

void form_output_prelookup_skip_button(
			int form_number );

void form_output_reset_button(	char *post_change_javascript,
			int form_number );

void form_output_insert_pair_one2m_submit_buttons(
			char *submit_control_string,
			LIST *pair_one2m_related_folder_name_list );

void form_output_html_help_file_anchor(
			char *application_name,
			char *html_help_file_anchor );

void form_output_generic_button(char *onclick_control_string,
			char *button_label );

char *form_set_post_change_javascript_row_zero(
			char *post_change_javascript );

void form_output_back_forward_buttons(
			void );

void form_output_prompt_insert_trailer(
			char *submit_control_string,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *prelookup_button_control_string,
			char *application_name,
			char *reset_post_change_javascript,
			LIST *pair_one2m_related_folder_name_list );

void form_output_prompt_insert_submit_buttons(
			char *submit_control_string,
			char *button_label,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *application_name,
			char *reset_post_change_javascript,
			LIST *pair_one2m_related_folder_name_list );

void form_output_prompt_insert_submit_button(
			char *submit_control_string,
			char *button_label,
			LIST *pair_one2m_related_folder_name_list );

void form_output_prompt_insert_reset_button(
			char *reset_post_change_javascript );

#endif

