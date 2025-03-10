/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/dictionary_separate.h	   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DICTIONARY_SEPARATE_H
#define DICTIONARY_SEPARATE_H

#include "boolean.h"
#include "list.h"
#include "folder_attribute.h"
#include "dictionary.h"

#define DICTIONARY_SEPARATE_SORT_PREFIX		"ssort_"
#define DICTIONARY_SEPARATE_QUERY_PREFIX	"qquery_"
#define DICTIONARY_SEPARATE_DRILLTHRU_PREFIX	"ddrillthru_"
#define DICTIONARY_SEPARATE_IGNORE_PREFIX	"iignore_"
#define DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX	"nno_display_"
#define DICTIONARY_SEPARATE_PAIR_PREFIX		"ppair_one2m_"

typedef struct
{
	DICTIONARY *dictionary;
} DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_new(
		DICTIONARY *original_post_dictionary /* in/out */,
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

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_prompt_new(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		DICTIONARY *dictionary /* in/out */ );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_calloc(
		void );

/* Usage */
/* ----- */
void dictionary_separate_date_convert_prompt_frame(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		DICTIONARY *dictionary /* in/out */ );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list );

/* Usage */
/* ----- */
void dictionary_separate_date_convert_edit_frame(
		DICTIONARY *dictionary /* in/out */,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		int dictionary_highest_index );

/* Usage */
/* ----- */
boolean dictionary_separate_date_convert(
		DICTIONARY *dictionary /* in/out */,
		char *application_name,
		char *login_name,
		char *date_string,
		char *key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *dictionary_separate_date_convert_string(
		char *application_name,
		char *login_name,
		char *date_string );

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

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_new(
		const char *dictionary_separate_sort_prefix,
		const char *dictionary_separate_query_prefix,
		const char *dictionary_separate_drillthru_prefix,
		const char *dictionary_separate_ignore_prefix,
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_pair_prefix,
		const char attribute_multi_key_delimiter,
		DICTIONARY *dictionary /* in/out */ );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_calloc(
		void );

/* Usage */
/* ----- */

/* ------------------------------------------------------------ */
/* Sample:							*/
/* From:  "station^datatype_1=BA^stage"				*/
/* To:    "station_1=BA and datatype_1=stage"			*/
/* ------------------------------------------------------------ */
void dictionary_separate_parse_multi(
		char *prefix,
		const char attribute_multi_key_delimiter,
		DICTIONARY *dictionary /* in/out */,
		LIST *dictionary_key_list );

/* Process */
/* ------- */

/* Returns multi_key or a component of multi_key */
/* --------------------------------------------- */
char *dictionary_separate_multi_prefix_skipped(
		char *prefix,
		char *multi_key );

/* Usage */
/* ----- */
void dictionary_separate_parse_multi_key(
		char *prefix,
		const char attribute_multi_key_delimiter,
		DICTIONARY *dictionary /* in/out */,
		int string_index,
		char *multi_key_without_prefix_index,
		char *multi_data );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *dictionary_separate_parse_multi_single_key(
		char *prefix,
		char *single_key );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *dictionary_separate_parse_multi_index_key(
		char *prefix,
		int string_index,
		char *single_key );

typedef struct
{
	DICTIONARY *original_post_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE *dictionary_separate_string_new(
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
DICTIONARY_SEPARATE *dictionary_separate_calloc(
		void );

/* Usage */
/* ----- */

/* Returns destination_dictionary */
/* ------------------------------ */
DICTIONARY *dictionary_separate_row_number(
		DICTIONARY *destination_dictionary,
		LIST *folder_attribute_name_list,
		DICTIONARY *source_dictionary,
		int row_number );

/* Usage */
/* ----- */

/* Returns dictionary_small() */
/* -------------------------- */
DICTIONARY *dictionary_separate_prompt(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *dictionary );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *dictionary_separate_from_to_key(
		const char *prefix,
		char *attribute_name );

/* Usage */
/* ----- */
DICTIONARY *dictionary_separate_multi_row(
		LIST *folder_attribute_name_list,
		DICTIONARY *dictionary );

/* Usage */
/* ----- */

/* ---------------------------------- */
/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_separate_no_display(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_ignore_prefix,
		DICTIONARY *sql_injection_escape_dictionary );

/* Usage */
/* ----- */
DICTIONARY *dictionary_separate_query(
		const char *dictionary_separate_query_prefix,
		DICTIONARY *sql_injection_escape_dictionary,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */
LIST *dictionary_separate_no_display_name_list(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_ignore_prefix,
		DICTIONARY *no_display_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *dictionary_separate_send_string(
		DICTIONARY *dictionary_separate_send_dictionary );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *dictionary_separate_key(
		char *attribute_name,
		int row_number );

/* ---------------------------------- */
/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_separate_remove_prefix(
		const char *prefix,
		DICTIONARY *dictionary );

/* ---------------------------------- */
/* Returns dictionary_large() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_separate_non_prefixed(
		const char *dictionary_separate_sort_prefix,
		const char *dictionary_separate_query_prefix,
		const char *dictionary_separate_drillthru_prefix,
		const char *dictionary_separate_ignore_prefix,
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_pair_prefix,
		DICTIONARY *dictionary );

DICTIONARY *dictionary_separate_operation(
		LIST *role_operation_name_list,
		DICTIONARY *dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *dictionary_separate_hidden_html(
		char *prefix,
		DICTIONARY *dictionary );

/* Safely returns */
/* -------------- */
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
char *dictionary_separate_relation_key(
		char *query_relation_operator_prefix,
		char *single_key );

typedef struct
{
	DICTIONARY *prompt_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE_PROMPT_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_PROMPT_LOOKUP *
	dictionary_separate_prompt_lookup_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PROMPT_LOOKUP *
	dictionary_separate_prompt_lookup_calloc(
		void );

typedef struct
{
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	LIST *ignore_name_list;
} DICTIONARY_SEPARATE_POST_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
	dictionary_separate_post_prompt_insert_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
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
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *no_display_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	LIST *no_display_name_list;
} DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
	dictionary_separate_post_prompt_lookup_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
	dictionary_separate_post_prompt_lookup_calloc(
		void );

typedef struct
{
	DICTIONARY *ignore_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *multi_row_dictionary;
	DICTIONARY *drillthru_dictionary;
	LIST *ignore_name_list;
} DICTIONARY_SEPARATE_POST_TABLE_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_POST_TABLE_INSERT *
	dictionary_separate_post_table_insert_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_POST_TABLE_INSERT *
	dictionary_separate_post_table_insert_calloc(
		void );

typedef struct
{
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *no_display_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *multi_row_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *operation_dictionary;
	LIST *no_display_name_list;
} DICTIONARY_SEPARATE_POST_TABLE_EDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_POST_TABLE_EDIT *
	dictionary_separate_post_table_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *role_operation_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_POST_TABLE_EDIT *
	dictionary_separate_post_table_edit_calloc(
		void );

typedef struct
{
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE_DRILLTHRU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
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
	DICTIONARY *original_post_dictionary;
	DICTIONARY *sort_dictionary;
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *no_display_dictionary;
	LIST *no_display_name_list;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *query_dictionary;
} DICTIONARY_SEPARATE_TABLE_EDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_TABLE_EDIT *
	dictionary_separate_table_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_TABLE_EDIT *
	dictionary_separate_table_edit_calloc(
		void );

typedef struct
{
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	LIST *ignore_name_list;
	LIST *prompt_name_list;
} DICTIONARY_SEPARATE_TABLE_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_TABLE_INSERT *
	dictionary_separate_table_insert_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_name_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_TABLE_INSERT *
	dictionary_separate_table_insert_calloc(
		void );

typedef struct
{
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE_PROMPT_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *prompt_process_date_name_list,
		LIST *folder_attribute_name_list_attribute_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_calloc(
		void );

typedef struct
{
	DICTIONARY *query_dictionary;
	DICTIONARY *non_prefixed_dictionary;
} DICTIONARY_SEPARATE_LOOKUP_DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY_SEPARATE_LOOKUP_DELETE *
	dictionary_separate_lookup_delete_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list );

/* Process */
/* ------- */
DICTIONARY_SEPARATE_LOOKUP_DELETE *
	dictionary_separate_lookup_delete_calloc(
		void );

#endif
