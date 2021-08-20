/* $APPASERVER_HOME/library/dictionary_appaserver.h	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef DICTIONARY_APPASERVER_H
#define DICTIONARY_APPASERVER_H

/* Includes */
/* -------- */
#include "dictionary.h"
#include "list.h"

/* Constants */
/* --------- */
#define QUERY_PREFIX				"qqery_"
#define SORT_BUTTON_PREFIX			"ssort_button_"
#define DRILLDOWN_PREFIX			"ddrilldown_"
#define PAIR_ONE2M_PREFIX			"ppair_one2m_"
#define IGNORE_SELECT_PUSH_BUTTON_PREFIX	"ignore_"
#define NO_DISPLAY_PUSH_BUTTON_PREFIX		"no_display_"
#define FROM_PREFIX				"from_"
#define TO_PREFIX				"to_"
#define RELATION_OPERATOR_PREFIX		"relation_operator_"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	LIST *attribute_date_name_list;
	LIST *operation_list;
	char *login_name;

	/* Process */
	/* ------- */
	DICTIONARY *original_post_dictionary;
	DICTIONARY *working_post_dictionary;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *drilldown_dictionary;
	DICTIONARY *row_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *send_dictionary;
	LIST *ignore_select_attribute_name_list;
} DICTIONARY_APPASERVER;

DICTIONARY_APPASERVER *dictionary_appaserver_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_name_list,
			LIST *attribute_date_name_list,
			LIST *operation_name_list,
			char *login_name );

DICTIONARY *dictionary_appaserver_row_dictionary_row_zero(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_name_list );

DICTIONARY *dictionary_appaserver_row_dictionary_multi_row(
			DICTIONARY *non_prefixed_dictionary,
			LIST *attribute_name_list,
			LIST *operation_name_list );

DICTIONARY *dictionary_appaserver_working_post(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_date_name_list,
			char *login_name );

DICTIONARY *dictionary_appaserver_query(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_sort(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_drilldown(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drilldown_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary );

void dictionary_appaserver_parse_multi_attribute_keys(
			DICTIONARY *source_destination,
			char *prefix );

DICTIONARY *dictionary_appaserver_row_dictionary_row(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row );

DICTIONARY *dictionary_appaserver_ignore(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_pair_one2m(
			DICTIONARY *working_post_dictionary );

char *dictionary_appaserver_escaped_send_dictionary_string(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary );

char *dictionary_appaserver_escaped_dictionary_string(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary );

DICTIONARY *dictionary_appaserver_non_prefixed(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_date_name_list,
			char *application_name,
			char *login_name );

void dictionary_appaserver_output_as_hidden(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary );

LIST *dictionary_appaserver_posted_attribute_name_list(
			DICTIONARY *query_dictionary, 
			LIST *attribute_name_list );

void dictionary_appaserver_remove_from_starting_label(
			DICTIONARY *dictionary );

void dictionary_appaserver_set_primary_data_list_string(
			DICTIONARY *row_dictionary,
			char *primary_data_list_string,
			LIST *primary_key_list,
			char delimiter );

DICTIONARY *dictionary_appaserver_prefixed(
			DICTIONARY *post_dictionary,
			char *prefix );

LIST *dictionary_appaserver_ignore_select_attribute_name_list(
			DICTIONARY *ignore_dictionary );

void dictionary_appaserver_dictionary_date_convert(
			DICTIONARY *post_dictionary,
			char *application_name,
			LIST *attribute_date_name_list,
			char *login_name );

void dictionary_appaserver_string_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			char *date_string,
			char *key,
			char *login_name );

void dictionary_appaserver_trim_multi_drop_down_index(
			DICTIONARY *working_post_dictionary,
			char multi_select_move_left_right_index_delimiter );

void dictionary_appaserver_trim_double_bracked_data(
			DICTIONARY *working_post_dictionary );

char *dictionary_appaserver_send_string(
			DICTIONARY *send_dictionary );

#endif
