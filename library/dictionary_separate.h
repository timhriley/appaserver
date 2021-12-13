/* $APPASERVER_HOME/library/dictionary_separate.h	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef DICTIONARY_SEPARATE_H
#define DICTIONARY_SEPARATE_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define DICTIONARY_SEPARATE_QUERY_PREFIX	"qquery_"
#define DICTIONARY_SEPARATE_SORT_PREFIX		"ssort_"
#define DICTIONARY_SEPARATE_DRILLTHRU_PREFIX	"ddrillthru_"
#define DICTIONARY_SEPARATE_IGNORE_PREFIX	"iignore_"
#define DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX	"ppair_one2m_"

#define IGNORE_SELECT_PUSH_BUTTON_PREFIX	"ignore_"
#define NO_DISPLAY_PUSH_BUTTON_PREFIX		"no_display_"
#define FROM_PREFIX				"from_"
#define TO_PREFIX				"to_"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	LIST *folder_attribute_date_name_list;
	LIST *operation_name_list;
	char *login_name;

	/* Process */
	/* ------- */
	DICTIONARY *original_post_dictionary;
	DICTIONARY *working_post_dictionary;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *row_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *send_dictionary;
	LIST *ignore_select_attribute_name_list;
} DICTIONARY_SEPARATE;

DICTIONARY_SEPARATE *dictionary_separate_calloc(
			void );

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_folder_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *operation_name_list );

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_row_new(
			DICTIONARY *original_row_dictionary,
			char *application_name,
			char *login_name,
			LIST *attribute_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *operation_name_list );

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_string_new(
			DICTIONARY *original_post_dictionary );

DICTIONARY *dictionary_separate_row_dictionary_row_zero(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_name_list );

DICTIONARY *dictionary_separate_row_dictionary_multi_row(
			DICTIONARY *non_prefixed_dictionary,
			LIST *attribute_name_list,
			LIST *operation_name_list );

DICTIONARY *dictionary_separate_working_post(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *folder_attribute_date_name_list,
			char *login_name );

DICTIONARY *dictionary_separate_query(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_sort(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_drillthru(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary );

void dictionary_separate_parse_multi_attribute_keys(
			DICTIONARY *source_destination,
			char *prefix );

DICTIONARY *dictionary_separate_row(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row );

DICTIONARY *dictionary_separate_ignore(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_pair_one2m(
			DICTIONARY *working_post_dictionary );

char *dictionary_separate_escaped_send_dictionary_string(
			DICTIONARY_SEPARATE *dictionary_separate,
			boolean with_non_prefixed_dictionary );

char *dictionary_separate_escaped_dictionary_string(
			DICTIONARY_SEPARATE *dictionary_separate,
			boolean with_non_prefixed_dictionary );

DICTIONARY *dictionary_separate_non_prefixed(
			DICTIONARY *working_post_dictionary );

void dictionary_separate_output_as_hidden(
			DICTIONARY_SEPARATE *dictionary_separate,
			boolean with_non_prefixed_dictionary );

LIST *dictionary_separate_posted_attribute_name_list(
			DICTIONARY *query_dictionary, 
			LIST *attribute_name_list );

void dictionary_separate_remove_from_starting_label(
			DICTIONARY *dictionary );

void dictionary_separate_set_primary_data_list_string(
			DICTIONARY *row_dictionary,
			char *primary_data_list_string,
			LIST *primary_key_list,
			char delimiter );

DICTIONARY *dictionary_separate_prefixed(
			DICTIONARY *post_dictionary,
			char *prefix );

LIST *dictionary_separate_ignore_select_attribute_name_list(
			DICTIONARY *ignore_dictionary );

void dictionary_separate_date_convert(
			DICTIONARY *working_post_dictionary,
			char *application_name,
			LIST *folder_attribute_date_name_list,
			char *login_name );

void dictionary_separate_string_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			char *date_string,
			char *key,
			char *login_name );

void dictionary_separate_trim_multi_drop_down_index(
			DICTIONARY *working_post_dictionary,
			char multi_select_move_left_right_index_delimiter );

void dictionary_separate_trim_double_bracked_data(
			DICTIONARY *working_post_dictionary );

char *dictionary_separate_send_string(
			DICTIONARY *send_dictionary );

DICTIONARY *dictionary_separate_original_post_dictionary(
			char *dictionary_string );

DICTIONARY *dictionary_separate_prefixed(
			DICTIONARY *post_dictionary,
			char *prefix );

/* Returns heap memory or null */
/* --------------------------- */
char *dictionary_separate_hidden_html(
			char *prefix,
			DICTIONARY *dictionary );

#endif
