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
#define DICTIONARY_SEPARATE_IGNORE_PREFIX	"ignore_"
#define DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX	"ppair_one2m_"
#define DICTIONARY_SEPARATE_SORT_BUTTON_PREFIX	"ssort_button_"

#define DICTIONARY_SEPARATE_NO_DISPLAY_PUSH_BUTTON_PREFIX \
						"no_display_"

#define DICTIONARY_SEPARATE_FROM_PREFIX		"from_"
#define DICTIONARY_SEPARATE_TO_PREFIX		"to_"

typedef struct
{
	DICTIONARY *dictionary;
} DICTIONARY_SEPARATE_DATE_CONVERT;

/* DICTIONARY_SEPARATE_DATE_CONVERT operations */
/* ------------------------------------------- */

/* Usage */
/* ----- */
DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_calloc(
			void );

void dictionary_separate_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			char *dictionary_separate_from_prefix,
			char *dictionary_separate_to_prefix );

/* Private */
/* ------- */
void dictionary_separate_date_convert_string(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *date_string,
			char *key );

typedef struct
{
	DICTIONARY *dictionary;
} DICTIONARY_SEPARATE_TRIM_MULTI;

/* DICTIONARY_SEPARATE_TRIM_MULTI operations */
/* ----------------------------------------- */

/* Usage */
/* ----- */
DICTIONARY_SEPARATE_TRIM_MULTI *
	dictionary_separate_trim_multi_new(
			DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_TRIM_MULTI *
	dictionary_separate_trim_multi_calloc(
			void );

void dictionary_separate_trim_multi(
			DICTIONARY *dictionary,
			char element_multi_move_left_right_delimiter );

typedef struct
{
	DICTIONARY *dictionary;
} DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET;

/* Usage */
/* ----- */
DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
	dictionary_separate_trim_double_bracket_new(
			DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
	dictionary_separate_trim_double_bracket_calloc(
			void );

void dictionary_separate_trim_double_bracket(
			DICTIONARY *dictionary );

typedef struct
{
	DICTIONARY *dictionary;
} DICTIONARY_SEPARATE_PARSE_MULTI;

/* Usage */
/* ----- */
DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_new(
			DICTIONARY *original_post_dictionary,
			LIST *relation_mto1_non_isa_list,
			char sql_delimiter );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_calloc(
			void );

void dictionary_separate_parse_multi(
			DICTIONARY *dictionary,
			LIST *mto1_non_isa_list,
			char sql_delimiter );

typedef struct
{
	DICTIONARY *original_post_dictionary;
	DICTIONARY *parse_multi_attribute_dictionary;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *row_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	LIST *ignore_select_attribute_name_list;
} DICTIONARY_SEPARATE;

/* DICTIONARY_SEPARATE operations */
/* ------------------------------ */

/* Private */
/* ------- */
DICTIONARY_SEPARATE *dictionary_separate_calloc(
			void );

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_string_new(
			DICTIONARY *original_post_dictionary );

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_post_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *operation_name_list,
			LIST *folder_attribute_date_name_list );

/* Followed by dictionary_append_dictionar() */
/* ----------------------------------------- */
DICTIONARY *dictionary_separate_parse_multi_attribute_dictionary(
			DICTIONARY *original_post_dictionary,
			LIST *relation_mto1_non_isa_list,
			char sql_delimiter );

/* Returns original_post_dictionary */
/* -------------------------------- */
DICTIONARY *dictionary_separate_working_post(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list );

DICTIONARY *dictionary_separate_query(
			char *dictionary_separate_query_prefix,
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_sort(
			char *dictionary_separate_sort_prefix,
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_drillthru(
			char *dictionary_separate_drillthru_prefix,
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary );

DICTIONARY *dictionary_separate_row(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row );

DICTIONARY *dictionary_separate_ignore(
			char *dictionary_separate_ignore_prefix,
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_separate_pair_one2m(
			char *dictionary_separate_pair_one2m_prefix,
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

/* Returns heap memory */
/* ------------------- */
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
