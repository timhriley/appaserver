/* library/dictionary_appaserver.h			   */
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
#define SORT_BUTTON_PREFIX		"ssort_button_"
#define LOOKUP_BEFORE_DROP_DOWN_PREFIX	"llookup_before_drop_down_"
#define PAIR_ONE2M_PREFIX		"ppair_one2m_"

/* Structures */
/* ---------- */
typedef struct
{
	DICTIONARY *original_post_dictionary;
	DICTIONARY *working_post_dictionary;
	DICTIONARY *sort_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *preprompt_dictionary;
	DICTIONARY *lookup_before_drop_down_dictionary;
	DICTIONARY *row_dictionary;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *pair_one2m_dictionary;
	DICTIONARY *non_prefixed_dictionary;
	DICTIONARY *send_dictionary;
} DICTIONARY_APPASERVER;

DICTIONARY_APPASERVER *dictionary_appaserver_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list,
			LIST *operation_name_list );

DICTIONARY *dictionary_appaserver_row_dictionary_row_zero(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_name_list );

DICTIONARY *dictionary_appaserver_row_dictionary_multi_row(
			DICTIONARY *non_prefixed_dictionary,
			LIST *attribute_name_list,
			LIST *operation_name_list );

DICTIONARY *dictionary_appaserver_working_post_dictionary(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list );

DICTIONARY *dictionary_appaserver_query_dictionary(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_sort_dictionary(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_preprompt_dictionary(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_lookup_before_dictionary(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *lookup_before_drop_down_dictionary,
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

DICTIONARY *dictionary_appaserver_ignore_dictionary(
			DICTIONARY *working_post_dictionary );

DICTIONARY *dictionary_appaserver_pair_one2m_dictionary(
			DICTIONARY *working_post_dictionary );

char *dictionary_appaserver_escaped_send_dictionary_string(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary );

char *dictionary_appaserver_escaped_dictionary_string(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary );

DICTIONARY *dictionary_appaserver_non_prefixed_dictionary(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_list,
			char *application_name );

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
			LIST *primary_attribute_name_list,
			char delimiter );

DICTIONARY *dictionary_appaserver_prefixed_dictionary(
			DICTIONARY *post_dictionary,
			char *prefix );

#endif
