/* $APPASERVER_HOME/library/attribute.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/* Includes */
/* -------- */
#include "list.h"
#include "boolean.h"
#include "dictionary.h"

/* Enumerated types */
/* ---------------- */
enum attribute_primary_attribute_fetch {
					attribute_primary_only,
					attribute_non_primary,
					attribute_fetch_either };

/* Constants */
/* --------- */
#define ATTRIBUTE_FOLDER_PIECE				0
#define ATTRIBUTE_ATTRIBUTE_PIECE			1
#define ATTRIBUTE_DATATYPE_PIECE			2
#define ATTRIBUTE_WIDTH_PIECE				3
#define ATTRIBUTE_FLOAT_DECIMAL_PLACES_PIECE		4
#define ATTRIBUTE_PRIMARY_KEY_INDEX_PIECE		5
#define ATTRIBUTE_DISPLAY_ORDER_PIECE			6
#define ATTRIBUTE_OMIT_INSERT_YN_PIECE			7
#define ATTRIBUTE_OMIT_INSERT_PROMPT_YN_PIECE		8
#define ATTRIBUTE_OMIT_UPDATE_YN_PIECE			9
#define ATTRIBUTE_HINT_MESSAGE_PIECE			10
#define ATTRIBUTE_POST_CHANGE_JAVASCRIPT_PIECE		11
#define ATTRIBUTE_ON_FOCUS_JAVASCRIPT_PIECE		12
#define ATTRIBUTE_ADDITIONAL_UNIQUE_INDEX_PIECE		13
#define ATTRIBUTE_ADDITIONAL_INDEX_PIECE		14
#define ATTRIBUTE_LOOKUP_REQUIRED_PIECE			15
#define ATTRIBUTE_INSERT_REQUIRED_PIECE			16
#define ATTRIBUTE_LOOKUP_HISTOGRAM_OUTPUT_YN_PIECE	17
#define ATTRIBUTE_LOOKUP_TIME_CHART_OUTPUT_YN_PIECE	18
#define ATTRIBUTE_APPASERVER_YN_PIECE			19

#define ATTRIBUTE_DELIMITER				'^'

/* Objects */
/* ------- */
typedef struct
{
	char *datatype;
	char *attribute_name;
	int primary_key_index;
	int display_order;
	boolean omit_insert_prompt;
	boolean omit_insert;
	boolean omit_update;
	int width;
	int float_decimal_places;
	char *hint_message;
	char *folder_name;
	boolean lookup_required;
	boolean insert_required;
	LIST *exclude_permission_list;
	char *post_change_javascript;
	char *on_focus_javascript_function;
	boolean additional_unique_index;
	boolean additional_index;
	boolean lookup_histogram_output;
	boolean lookup_time_chart_output;
	boolean appaserver;
	LIST *attribute_list;
} ATTRIBUTE;

/* Operations */
/* ---------- */
ATTRIBUTE *attribute_calloc(
			void );
ATTRIBUTE *attribute_new(
			char *attribute_name );

ATTRIBUTE *attribute_new_attribute(
			char *folder_name,
			char *attribute_name,
			char *datatype,
			int width,
			int primary_key_index,
			int display_order,
			int float_decimal_places,
			boolean omit_insert,
			boolean omit_insert_prompt,
			boolean omit_update,
			char *hint_message,
			char *post_change_javascript,
			char *on_focus_javascript_function,
			boolean additional_unique_index,
			boolean additional_index,
			boolean lookup_histogram_output,
			boolean lookup_time_chart_output,
			boolean lookup_required,
			boolean insert_required,
			boolean appaserver );

ATTRIBUTE *attribute_load_attribute(	char *application_name,
					char *attribute_name );

ATTRIBUTE *attribute_load_folder_attribute(
					char *application_name,
					char *folder_attribute,
					char *attribute_name );

ATTRIBUTE *attribute_seek(
			char *attribute_name,
			LIST *attribute_list );

ATTRIBUTE *attribute_seek_attribute(
			char *attribute_name,
			LIST *attribute_list );

char *attribute_list_display(
			LIST *attribute_list );

char *attribute_display(
			ATTRIBUTE *attribute );

char *attribute_get_database_datatype(	char *datatype,
					int width,
					int float_decimal_places,
					int primary_key_index );

char *attribute_get_last_primary_attribute_name(
					LIST *attribute_list );

LIST *attribute_get_exclude_attribute_name_list(
			char *application_name,
			char *role_name,
			char *exclude_permission );

LIST *attribute_list_get_datatype_attribute_string_list(
					LIST *attribute_list,
					char *datatype_string_list );

LIST *attribute_list_get_primary_datatype_attribute_string_list(
					LIST *attribute_list,
					char *datatype_string_list );

LIST *attribute_get_non_primary_attribute_list(
					LIST *attribute_list );

LIST *attribute_using_name_list_extract_attribute_list(
					LIST *attribute_list,
					LIST *attribute_name_list );

LIST *attribute_get_list(		char *application_name,
					char *folder_name,
					char *role_name );

LIST *attribute_get_attribute_list(	char *application_name,
					char *folder_name,
					char *attribute_name,
					LIST *mto1_isa_related_folder_list,
					char *role_name );

LIST *attribute_get_attribute_record_list(
					char *application_name, 
					char *folder_name,
					char *attribute_name );

void attribute_append_attribute_list(
				LIST *attribute_list,
				char *application_name,
				char *folder_name,
				char *attribute_name,
				char *role_name,
				enum attribute_primary_attribute_fetch );

boolean attribute_exists_omit_insert_login_name(
					LIST *attribute_list );

boolean attribute_exists_date_attribute(LIST *attribute_list );

boolean attribute_exists_omit_insert_prompt(
					LIST *attribute_list );

LIST *attribute_get_omit_insert_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_get_omit_update_attribute_name_list(
					LIST *attribute_list );

char *attribute_get_last_primary_attribute_name(
					LIST *attribute_list );

LIST *attribute_get_common_non_primary_attribute_name_list(
					char *application_name,
					char *folder_name,
					char *related_folder_name );

boolean attribute_list_exists_lookup_histogram_output(
					LIST *attribute_list );

boolean attribute_exists_reference_number(
					LIST *attribute_list );

boolean attribute_list_exists_lookup_time_chart_output(
					LIST *attribute_list );

LIST *attribute_get_histogram_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_get_time_chart_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_get_date_attribute_position_list(
					LIST *attribute_list );

LIST *attribute_get_date_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_get_lookup_allowed_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_folder_name_list(
			LIST *attribute_list,
			char *folder_name );

LIST *attribute_name_list(
			LIST *attribute_list );

LIST *attribute_get_name_list(
			LIST *attribute_list );

LIST *attribute_get_attribute_name_list(
			LIST *attribute_list );

LIST *attribute_get_primary_attribute_name_list(
			LIST *attribute_list );

LIST *attribute_get_lookup_required_attribute_name_list(
			LIST *attribute_list );

LIST *attribute_get_insert_required_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_get_update_lookup_exclude_attribute_name_list(
					LIST *attribute_list );

LIST *attribute_list_subtract(		LIST *attribute_list,
					LIST *subtract_attribute_name_list );

void attribute_remove_attribute_list(	LIST *attribute_list,
					char *attribute_name );

int attribute_get_primary_attribute_list_length(
					LIST *attribute_list );

LIST *attribute_get_attribute_element_list(
					char *attribute_name,
					char *prepend_folder_name,
					char *datatype,
					char *post_change_javascript,
					int width,
					char *hint_message,
					boolean is_primary_attribute,
					boolean omit_push_buttons );

int attribute_get_date_piece_offset(	LIST *attribute_list,
					LIST *exclude_attribute_name_list );

char *attribute_get_reference_number_attribute_name(
					LIST *attribute_list,
					LIST *attribute_name_list );

boolean attribute_list_exists(		LIST *attribute_list,
					char *attribute_name );

char *attribute_append_post_change_javascript(
					char *original_post_change_javascript,
					char *post_change_javascript,
					boolean place_first );

LIST *attribute_get_non_primary_float_list(
					LIST *attribute_list );

boolean attribute_list_exists_name(	LIST *attribute_list,
					char *attribute_name );

void attribute_set_dictionary_date_international(
					DICTIONARY *dictionary,
					LIST *attribute_list );

char *attribute_get_select_clause(	char *application_name,
					char *folder_name,
					LIST *attribute_name_list );

LIST *attribute_get_additional_unique_index_attribute_name_list(
					LIST *attribute_list );

void attribute_list_remove_exclude_permission_list(
					LIST *attribute_list );

int attribute_get_width(		char *application_name,
					char *attribute_name );

boolean attribute_exists(
			char *application_name,
			char *folder_name,
			char *attribute_name );

boolean attribute_record_parse(
			char *fetched_folder_name,
			char *fetched_attribute_name,
			char *attribute_datatype,
			char *width,
			char *float_decimal_places,
			char *primary_key_index,
			char *display_order,
			char *omit_insert_yn,
			char *omit_insert_prompt_yn,
			char *omit_update_yn,
			char *hint_message,
			char *post_change_javascript,
			char *on_focus_javascript_function,
			char *additional_unique_index_yn,
			char *additional_index_yn,
			char *lookup_required_yn,
			char *insert_required_yn,
			char *lookup_histrogram_output_yn,
			char *lookup_time_chart_output_yn,
			char *appaserver_yn,
			char *record,
			char *folder_name,
			char *attribute_name );

LIST *attribute_append_isa_attribute_list(
			char *application_name,
			char *folder_name,
			LIST *mto1_isa_related_folder_list,
			char *role_name );

LIST *attribute_distinct_folder_name_list(
			LIST *attribute_list );

LIST *attribute_list(	char *folder_name );

LIST *attribute_system_list(
			char *sys_string );

ATTRIBUTE *attribute_parse(
			char *input );

char *attribute_list_sys_string(
			char *folder_name );

LIST *attribute_folder_attribute_name_list(
			LIST *attribute_list );

LIST *attribute_primary_attribute_name_list(
			LIST *attribute_list );

#endif
