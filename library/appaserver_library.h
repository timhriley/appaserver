/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/appaserver_library.h			*/
/* --------------------------------------------------------------------	*/
/*									*/
/* These are the generic Appaserver functions.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#ifndef APPASERVER_LIBRARY_H
#define APPASERVER_LIBRARY_H

/* Includes */
/* -------- */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "dictionary.h"
#include "list.h"
#include "boolean.h"
#include "attribute.h"
#include "role.h"

/* Enumerated types */
/* ---------------- */
enum preupdate_change_state {		from_null_to_something,
					from_something_to_null,
					from_something_to_something_else,
					no_change };

/* Constants */
/* ========= */

/* ---------------------------------------------------- */
/* After changing this:					*/
/* $ cd $APPASERVER_HOME/src_appaserver			*/
/* $ make						*/
/* ---------------------------------------------------- */
#define DIFFERENT_DESTINATION_APPLICATION_OK	0

#define FOLDER_DATA_DELIMITER			'^'
#define SQL_DELIMITER				'^'
#define PREDICTIVE_LOGO_FILENAME_KEY		"logo_filename"
#define TEMPLATE_APPLICATION			"template"
#define APPASERVER_UMASK			7
#define HORIZONTAL_MENU_RELATIVE_DIRECTORY	"appaserver/zmenu"
#define CALENDAR_RELATIVE_DIRECTORY		"appaserver/zscal2"
#define IMAGE_RELATIVE_DIRECTORY		"appaserver/zimages"
#define PATH_DELIMITER				':'
#define OUTPUT_TEXT_FILE_DELIMITER		'|'
#define PREPROMPT_PREFIX			"ppreprompt_"
#define RESIDUAL_STANDARD_ESTIMATE		"Error StdDev"
#define COLUMNS_UPDATED_KEY			"ccolumns_updated"
#define COLUMNS_UPDATED_CHANGED_FOLDER_KEY	"ccolumns_updated_changed"
#define CONTENT_TYPE_YN				"output_content_type_yn"
#define MAX_QUERY_ROWS_FORCE_DROP_DOWNS		5
#define ROWS_INSERTED_COUNT_KEY			"rrows_inserted_count"
#define DEFAULT_TARGET_FRAME			EDIT_FRAME
#define MYSQL_DUPLICATE_ERROR_MESSAGE_KEY	"Duplicate entry"
#define MULTI_ATTRIBUTE_KEY_DELIMITER			'|'
#define MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER		'^'
#define MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER		'|'
#define APPASERVER_DATABASE_ENVIRONMENT_VARIABLE	"APPASERVER_DATABASE"
#define HORIZONTAL_MENU_CLASS			"menu"
#define VERTICAL_MENU_CLASS			"vertical_menu"
#define SUBMIT_BUTTON_LABEL			"|    Submit    |"
#define TRANSMIT_PROMPT				"Link to file."
#define SYNCHRONIZE_CURRENT_LOGFILE_NAME	"synchronize_current.txt"
#define FROM_PREPROMPT				"ffrom_preprompt"
#define ONE2M_FOLDER_NAME_FOR_PROCESS		"one2m_folder_name_for_process"
#define CHOOSE_FOLDER_MENU_NAME			"choose_folder_menu"
#define CHOOSE_PROCESS_MENU_NAME		"choose_process_menu"
#define MENU_FRAME				"menu_frame"
#define PROMPT_FRAME				"prompt_frame"
#define EDIT_FRAME				"edit_frame"
#define CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME	"choose_role_drop_down"
#define MAX_INPUT_LINE				131072
#define MENU_VERTICAL_PIXEL_COLUMNS		200
#define MENU_HORIZONTAL_PIXEL_ROWS		35
#define ERROR_HEADING				"appaserver error"
#define INSERT_IDENTIFIER			"is_insert_row"
#define MULTI_ATTRIBUTE_NON_DATE_DISPLAY_DELIMITER \
						"-"
#define ROWS_FOR_SUBMIT_AT_BOTTOM		6
#define PROCESS_PROMPT_DEFAULT_WIDTH		25
#define BOTTOM_FRAME_NAME			"bottomframe"
#define IGNORE_PUSH_BUTTON_PREFIX		"ignore_"
#define NO_DISPLAY_PUSH_BUTTON_PREFIX		"no_display_"
#define NO_DISPLAY_PUSH_BUTTON_HEADING		"no_display"
#define INSERT_NEW_TEXT_ITEM_PREFIX		"insert_"
#define RADIO_NEW_PUSH_BUTTON_NAME		"new_button"
#define AJAX_FILL_PUSH_BUTTON_PREFIX		"ajax_fill_"
#define AJAX_FILL_PUSH_BUTTON_LABEL		"Fill"
#define TRANSMIT_PUSH_BUTTON_NAME		"transmit_button"
#define TRANSMIT_PUSH_BUTTON_LABEL		"Spreadsheet"
#define INSERT_NEW_PUSH_BUTTON_LABEL		"Insert"
#define GRACE_CHART_PUSH_BUTTON_NAME		"grace_chart_button"
#define GOOGLE_CHART_PUSH_BUTTON_NAME		"google_chart_button"
#define HISTOGRAM_PUSH_BUTTON_NAME		"histogram_button"
#define GROUP_PUSH_BUTTON_NAME			"group_button"
#define SORT_ORDER_ATTRIBUTE_NAME		"sort_order"
#define SORT_ORDER_ELEMENT_NAME			"sort_order_1"
#define DISPLAY_ORDER_ATTRIBUTE_NAME		"display_order"
#define SEQUENCE_NUMBER_ATTRIBUTE_NAME		"sequence_number"
#define SORT_ORDER_PUSH_BUTTON_NAME		"sort_order_button"
#define QUANTUM_CHART_PUSH_BUTTON_NAME		"quantum_chart_button"
#define STATISTICS_PUSH_BUTTON_NAME		"statistics_button"
#define DELETE_PUSH_BUTTON_NAME			"delete_button"
#define LOOKUP_PUSH_BUTTON_NAME			"lookup_button"
#define BOGUS_SESSION				"-999"
#define EMAIL_OUTPUT_NAME			"email_output"
#define EDIT_RADIO_GROUP			"edit_radio"
#define INSERT_PUSH_BUTTON_NAME			"insert"
#define LOOKUP_OPTION_RADIO_BUTTON_NAME		"lookup_option_radio_button"
#define PROMPT_NEW_BUTTON_FOLDER_NAME		"prompt_new_button_folder_name"
#define PRIMARY_DATA_LIST_KEY			"primary_data_list"

#define EQUAL_OPERATOR				"equals"
#define NOT_EQUAL_OPERATOR			"not_equal"
#define NOT_EQUAL_OR_NULL_OPERATOR		"not_equal_or_empty"
#define LESS_THAN_OPERATOR			"less_than"
#define LESS_THAN_EQUAL_TO_OPERATOR		"less_than_equal_to"
#define GREATER_THAN_OPERATOR			"greater_than"
#define GREATER_THAN_EQUAL_TO_OPERATOR		"greater_than_equal_to"
#define BETWEEN_OPERATOR			"between"
#define BEGINS_OPERATOR				"begins"
#define CONTAINS_OPERATOR			"contains"
#define NOT_CONTAINS_OPERATOR			"not_contains"
#define OR_OPERATOR				"or_,"
#define NULL_OPERATOR				"is_empty"
#define NOT_NULL_OPERATOR			"not_empty"
#define SELECT_OPERATOR				"select"

#ifndef APPASERVER_DATABASE_ERROR_FILE
#define APPASERVER_DATABASE_ERROR_FILE		"/var/log/appaserver_%s.err"
#endif

/* Data structures */
/* --------------- */
typedef struct
{
	char *process;
	char *command_line;
	char *output_new_window_yn;
} PROCESS_RECORD;

/* Protoypes */
/* --------- */
LIST *get_attribute4table_list( char *table_name );
void subtract_attributes_in_dictionary( 
				LIST *attribute_record_list,
				DICTIONARY *post_dictionary,
				int piece_offset );
LIST *get_primary_data_list(	char *entity,
				char *folder );

/* ------------------------------ */
/* Returns heap memory [strdup()] */
/* ------------------------------ */
char *get_table_name( 		char *application_name,
				char *folder );

char *get_folder_name(		char *application_name,
				char *table_name );
void output_insert_database_row(char *data_pipe_string,
				int row,
				DICTIONARY *post_dictionary,
				DICTIONARY *frame_dictionary,
				LIST *attribute_name_list,
				FILE *f );
void output_dictionary_string_as_hidden( char *dictionary );
LIST *get_relation_operator_list(
				char *datatype );
char *get_operator_character( 	char *operator_string );
boolean is_system_folder(	char *folder_name );
boolean appaserver_library_is_system_folder(
				char *folder_name );
LIST *get_relation_operator_equal_list(
				void );

LIST *get_role_list(		char *application_name,
				char *person,
				char *error_file );

int get_attribute_width(	char *entity,
				char *attribute_name );

LIST *appaserver_library_get_ignore_pressed_attribute_name_list( 	
				DICTIONARY *posted_dictionary,
				LIST *attribute_name_list,
				DICTIONARY *query_dictionary );

LIST *folder_name_list2table_name_list( 	
				char *entity,
				char *application,
				LIST *folder_name_list );

void populate_no_display_button_for_ignore(
				DICTIONARY *dictionary );
void populate_ignore_button_for_no_display_pressed(
				DICTIONARY *dictionary );
LIST *appaserver_library_get_datatype_name_list(
				char *application );
char *get_displayable_primary_attribute_list_string(
				LIST *primary_attribute_name_list );
LIST *get_attribute4folder_list(char *entity, 
				char *folder_name );
LIST *get_insert_ignore_pressed_attribute_name_list( 	
				DICTIONARY *posted_dictionary,
				LIST *attribute_name_list );
LIST *appaserver_library_get_no_display_pressed_attribute_name_list( 	
				DICTIONARY *ignore_dictionary,
				LIST *attribute_name_list );

LIST *remove_related_folders_attribute_name_list(
				LIST *related_mto1_folder_list,
				LIST *attribute_name_list );
void m(				char *message );

void populate_ignore_button_for_attributes_not_inserted(
				DICTIONARY *post_dictionary,
				char *entity,
				char *folder_name );

char **get_system_folder_list(	void );

char **appaserver_library_get_system_folder_list(
				void );

void output_dictionary_as_hidden(
				DICTIONARY *dictionary );
void appaserver_library_output_dictionary_as_hidden(
				DICTIONARY *dictionary );
char *get_full_attribute_name( 	char *application_name,
				char *folder_name,
				char *attribute_name );
char *build_multi_attribute_key(LIST *key_list, char delimiter );
char *get_multiple_table_names(
				char *application_name,
				char *folder_name_list_comma_string );
char *get_multi_table_name(	char *application_name,
				char *multi_folder_name_list_string );
LIST *appaserver_library_get_folder_name_list(
				char *application_name,
				char *attribute_name );

LIST *appaserver_library_get_insert_attribute_element_list(
					int *objects_outputted,
					LIST *attribute_list,
					char *attribute_name,
					LIST *posted_attribute_name_list,
					boolean is_primary_attribute,
					char *folder_post_change_javascript,
					char *application_name );

LIST *appaserver_library_get_update_attribute_element_list(
					int *objects_outputted,
					ATTRIBUTE *attribute,
					char update_yn,
					LIST *primary_attribute_name_list,
					boolean is_primary_attribute,
					char *folder_post_change_javascript,
					boolean prompt_data_element_only );

boolean appaserver_library_validate_begin_end_date(
					char **begin_date,
					char **end_date,
					DICTIONARY *post_dictionary );

LIST *appaserver_library_get_primary_data_list(
					DICTIONARY *post_dictionary,
					LIST *attribute_list );

char *appaserver_library_get_server_address(
			void );

char *appaserver_library_server_address(
			void );

void appaserver_library_output_ftp_prompt(
					char *output_filename,
					char *prompt,
					char *target,
					char *mime_type );

char *appaserver_library_get_http_prompt(
				 	char *cgi_directory,
				 	char *server_address,
					char ssl_support_yn,
					char prepend_http_protocol_yn );

char *appaserver_library_get_whoami(	void );
boolean appaserver_library_get_from_preprompt(
					DICTIONARY *dictionary );
LIST *appaserver_library_get_omit_insert_prompt_attribute_name_list(
					LIST *attribute_list );

LIST *appaserver_library_get_omit_insert_attribute_name_list(
					LIST *attribute_list );

LIST *appaserver_library_get_prompt_data_element_list(
					char *attribute_name,
					boolean is_primary_attribute );

void appaserver_library_set_no_display_pressed(
					DICTIONARY *ignore_dictionary, 
					LIST *key_list );
void appaserver_library_output_style_sheet(
					FILE *output_file, 
					char *application_name );
LIST *appaserver_library_trim_carrot_number(
					LIST *data_list );

LIST *appaserver_library_get_attribute_name_list(
					char *application, 
					char *folder_name );

char *appaserver_library_get_verify_attribute_widths_submit_control_string(
					LIST *element_list,
					char *source_form );

void appaserver_library_post_dictionary_database_convert_dates(
					DICTIONARY *post_dictionary,
					char *application_name,
					LIST *attribute_list );

void appaserver_library_dictionary_database_convert_begin_end_dates(
					DICTIONARY *dictionary,
					char *application_name );

void appaserver_library_list_database_convert_dates(
					LIST *data_list,
					char *application_name,
					LIST *attribute_list );

void appaserver_library_dictionary_convert_date(
					DICTIONARY *dictionary,
					char *application_name,
					char *date_string,
					char *key );

void appaserver_library_output_calendar_javascript(
					void );

int appaserver_library_get_reference_number(
				char *application_name,
				int insert_rows_number );

char *appaserver_library_prelookup_button_control_string(
			char *application_name,
			char *cgi_directory,
			char *server_address,
			char *login_name,
			char *session,
			char *folder_name,
			char *lookup_before_drop_down_base_folder_name,
			char *role_name,
			char *state );

void appaserver_library_populate_last_foreign_attribute_key(
				DICTIONARY *post_dictionary,
				LIST *mto1_related_folder_list,
				LIST *primary_attribute_name_list );

void appaserver_library_dictionary_convert_dates(
				DICTIONARY *dictionary,
				char *application_name );

char *appaserver_get_error_filename(
				char *application_name );

/*
void appaserver_output_starting_argv_append_file(
				int argc,
				char **argv,
				char *application_name );
*/

FILE *appaserver_open_append_file(
				char *application_name );

boolean appaserver_library_get_from_php(
				DICTIONARY *post_dictionary );

boolean appaserver_library_application_exists(
				char *application,
				char *appaserver_error_directory );

char *appaserver_library_get_default_role_name(
				char *application_name,
				char *login_name );

LIST *appaserver_library_with_attribute_get_insert_attribute_element_list(
				char *attribute_name,
				char *datatype,
				int width,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				LIST *posted_attribute_name_list,
				boolean is_primary_attribute,
				boolean omit_update );

int appaserver_library_add_operations(
				LIST *element_list,
				int objects_outputted,
				LIST *operation_list,
				char *delete_isa_only_folder_name );

boolean appaserver_library_exists_javascript_folder(
				char *application_name,
				char *folder_name );

void appaserver_library_purge_temporary_files(
				char *application_name );

void appaserver_library_automatically_set_login_name(
				DICTIONARY *query_dictionary,
				char *login_name,
				LIST *mto1_related_folder_list,
				LIST *attribute_list,
				LIST *role_attribute_exclude_list );

enum preupdate_change_state appaserver_library_get_preupdate_change_state(
				char *preupdate_data,
				char *postupdate_data,
				char *preupdate_placeholder_name );

char *appaserver_library_preupdate_change_state_display(
			enum preupdate_change_state preupdate_change_state );

char *appaserver_library_change_state_display(
			enum preupdate_change_state preupdate_change_state );

char *appaserver_library_get_sort_attribute_name(
				LIST *attribute_list );

LIST *appaserver_library_get_update_lookup_attribute_element_list(
					char update_yn,
					LIST *primary_attribute_name_list,
					LIST *exclude_permission_list,
					char *attribute_name,
					char *datatype,
					int width,
					char *post_change_javascript,
					char *on_focus_javascript_function,
					boolean is_primary_attribute );

char *appaserver_library_get_folder_foreign_translation(
			char *attribute_name,
			LIST *folder_foreign_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary );

LIST *appaserver_library_get_application_name_list(
			char *appaserver_error_directory );

char *appaserver_library_get_table_name(
					char *application_name,
					char *folder_name );

#endif
