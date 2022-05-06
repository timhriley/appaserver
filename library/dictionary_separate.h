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
#include "folder_attribute.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define DICTIONARY_SEPARATE_SORT_PREFIX		"ssort_"
#define DICTIONARY_SEPARATE_QUERY_PREFIX	"qquery_"
#define DICTIONARY_SEPARATE_DRILLTHRU_PREFIX	"ddrillthru_"
#define DICTIONARY_SEPARATE_IGNORE_PREFIX	"iignore_"
#define DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX	"nno_display_"
#define DICTIONARY_SEPARATE_PAIR_PREFIX		"ppair_one2m_"

#define DICTIONARY_SEPARATE_FROM_PREFIX		"from_"
#define DICTIONARY_SEPARATE_TO_PREFIX		"to_"

typedef struct
{
	DICTIONARY *dictionary;
	LIST *dictionary_key_list;
	char *key;
	char *data;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *security_sql_injection_escape;
} DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE;

/* DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE operations */
/* --------------------------------------------------- */

/* Usage */
/* ----- */
DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_new(
			DICTIONARY *original_post_dictionary,
			LIST *folder_attribute_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_calloc(
			void );

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
			char sql_delimiter );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_calloc(
			void );

/* ------------------------------------------------------------ */
/* Sample:							*/
/* From:  "station^datatype_1=BA^stage"				*/
/* To:    "station_1=BA and datatype_1=stage"			*/
/* ------------------------------------------------------------ */
void dictionary_separate_parse_multi(
			DICTIONARY *dictionary,
			char sql_delimiter );

typedef struct
{
	DICTIONARY *original_post_dictionary;
} DICTIONARY_SEPARATE;

/* DICTIONARY_SEPARATE operations */
/* ------------------------------ */

/* Private */
/* ------- */
DICTIONARY_SEPARATE *dictionary_separate_calloc(
			void );

/* Returns destination_dictionary */
/* ------------------------------ */
DICTIONARY *dictionary_separate_multi_row_attribute(
			DICTIONARY *destination_dictionary,
			LIST *folder_attribute_name_list,
			DICTIONARY *source_dictionary,
			int row_number );

/* Public */
/* ------ */
DICTIONARY *dictionary_separate_remove_prefix(
			char *prefix,
			DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_non_prefixed(
			char *dictionary_separate_sort_prefix,
			char *dictionary_separate_query_prefix,
			char *dictionary_separate_drillthru_prefix,
			char *dictionary_separate_ignore_prefix,
			char *dictionary_separate_no_display_prefix,
			char *dictionary_separate_pair_prefix,
			DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_multi_row(
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_row_zero(
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_row(
			LIST *folder_attribute_name_list,
			DICTIONARY *multi_row_dictionary,
			int row_number );

DICTIONARY *dictionary_separate_operation(
			LIST *role_operation_name_list,
			DICTIONARY *dictionary );

char *dictionary_separate_hidden_html(
			char *prefix,
			DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_send_dictionary(
			DICTIONARY *sort_dictionary,
			char *dictionary_separate_sort_prefix,
			DICTIONARY *query_dictionary,
			char *dictionary_separate_query_prefix,
			DICTIONARY *drillthru_dictionary,
			char *dictionary_separate_drillthru_prefix,
			DICTIONARY *ignore_dictionary,
			char *dictionary_separate_ignore_prefix,
			DICTIONARY *no_display_dictionary,
			char *dictionary_separate_no_display_prefix,
			DICTIONARY *pair_one2m_dictionary,
			char *dictionary_separate_pair_prefix,
			DICTIONARY *non_prefixed_dictionary );

/* Returns heap memory */
/* ------------------- */
char *dictionary_separate_send_string(
			DICTIONARY *replaced_send_dictionary );

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE *dictionary_separate_string_new(
			DICTIONARY *original_post_dictionary );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *row_zero_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	LIST *ignore_name_list;

	/* Not fetched.							*/
	/* Instead, later set by post_prompt_insert_query_dictionary()  */
	/* ------------------------------------------------------------ */
	DICTIONARY *query_dictionary;
} DICTIONARY_SEPARATE_POST_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
	dictionary_separate_post_prompt_insert_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
	dictionary_separate_post_prompt_insert_calloc(
			void );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *no_display_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *multi_row_dictionary;
	DICTIONARY *row_zero_dictionary;
	DICTIONARY *operation_dictionary;
	LIST *no_display_name_list;
} DICTIONARY_SEPARATE_POST_EDIT_TABLE;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_POST_EDIT_TABLE *
	dictionary_separate_post_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_name_list,
			LIST *role_operation_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_POST_EDIT_TABLE *
	dictionary_separate_post_edit_table_calloc(
			void );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *drillthru_dictionary;
} DICTIONARY_SEPARATE_DRILLTHRU;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_DRILLTHRU *
	dictionary_separate_drillthru_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_DRILLTHRU *
	dictionary_separate_drillthru_calloc(
			void );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *no_display_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	LIST *no_display_name_list;
} DICTIONARY_SEPARATE_EDIT_TABLE;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_EDIT_TABLE *
	dictionary_separate_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_EDIT_TABLE *
	dictionary_separate_edit_table_calloc(
			void );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *multi_row_dictionary;
	DICTIONARY *row_zero_dictionary;
	LIST *ignore_name_list;
} DICTIONARY_SEPARATE_INSERT_TABLE;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_INSERT_TABLE *
	dictionary_separate_insert_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list,
			LIST *folder_attribute_name_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_INSERT_TABLE *
	dictionary_separate_insert_table_calloc(
			void );

typedef struct
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE_PROMPT_PROCESS;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_name_list_attribute_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_calloc(
			void );

#endif
