/* $APPASERVER_HOME/library/dictionary.h		*/
/* ==================================================== */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "hash_table.h"
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */
#define RELATION_OPERATOR_PREFIX		"relation_operator_"
#define DICTIONARY_ALTERNATIVE_DELIMITER	'~'

typedef struct
{
	HASH_TABLE *hash_table;
} DICTIONARY;

/* Function prototypes */
/* ------------------- */
DICTIONARY *dictionary_calloc();
DICTIONARY *dictionary_new();
DICTIONARY *dictionary_small();
DICTIONARY *dictionary_medium();
DICTIONARY *dictionary_large();
DICTIONARY *dictionary_huge();
DICTIONARY *dictionary_super();
DICTIONARY *dictionary_duper();

DICTIONARY *dictionary_string_resolve(
			char *dictionary_string );

DICTIONARY *dictionary_index_string2dictionary(
			char *dictionary_string );

DICTIONARY *dictionary_string_dictionary(
			char *dictionary_string );

DICTIONARY *dictionary_string2dictionary(
			char *dictionary_string );

int dictionary_len(	DICTIONARY *d );

int dictionary_length(	DICTIONARY *d );

void dictionary_set( 	DICTIONARY *d, 
			char *key, 
			char *data );

char *dictionary_pointer(
			char *key,
			DICTIONARY *d );

char *dictionary_get(	char *key,
			DICTIONARY *d );

void dictionary_free(	DICTIONARY *d );

void dictionary_free_data(
			DICTIONARY *d,
			LIST *key_list );

void dictionary_free_container(
			DICTIONARY *d );

char **get_key_array(	DICTIONARY *d );

int dictionary_all_null_data(
			DICTIONARY *d );

void dictionary_add(	DICTIONARY *d,
			char *key,
			char *data,
			int size );

LIST *dictionary_key_list(
			DICTIONARY *d );

LIST *dictionary_ordered_key_list(
			DICTIONARY *d );

char *dictionary_display(
			DICTIONARY *d );

char *dictionary_list_display(
			LIST *dictionary_list );

char *dictionary_display_delimiter(
			DICTIONARY *d,
			char delimiter );

int dictionary_key_highest_index(
			DICTIONARY *d );

LIST *dictionary_index_data_list(
			DICTIONARY *dictionary,
			char *key );

int dictionary_index_data_if_changed(
			char **destination,
			DICTIONARY *dictionary_1,
			DICTIONARY *dictionary_2,
			char *key, 
			int index );

/* ----------------------------------------------------------- */
/* Returns: 	1 if the data comes from the index	       */
/*		0 if the data comes from index = 0 or no index */
/*	       -1 if the data is not found	   	       */
/* ----------------------------------------------------------- */
int dictionary_index_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index );

boolean dictionary_exists_key_index(
			DICTIONARY *dictionary,
			char *search_key,
			int row );

void dictionary_parse_multi_attribute_keys(
			DICTIONARY *dictionary, 
			char key_delimiter,
			char data_delimiter,
			char *prefix,
			boolean dont_include_relational_operators );

void dictionary_parse_multi_attribute_relation_operator_keys(
			DICTIONARY *dictionary, 
			char multi_attribute_drop_down_delimiter );

void dictionary_add_string(
			DICTIONARY *d,
			char *key, 
			char *data_string );

void dictionary_set_string(
			DICTIONARY *d,
			char *key, 
			char *data_string );

void dictionary_set_index_zero(
			DICTIONARY *d,
			char *key, 
			char *data_string );

void dictionary_set_string_index_key(
			DICTIONARY *d,
			char *key, 
			char *data_string,
			int index );

void dictionary_search_replace_command_arguments( 	
			char *source_destination,
			DICTIONARY *dictionary, 
			int index );

int dictionary_index_data_multi(
			char *destination,
			DICTIONARY *dictionary,
			char *key_multi,
			int index,
			char delimiter );

DICTIONARY *dictionary_prepend_key(
			DICTIONARY *dictionary,
			char *label );

DICTIONARY *dictionary_extract_prepended_key(
			DICTIONARY *dictionary,
			char *label,
			int with_strip );

boolean dictionary_non_populated_key_exists_index_zero(
			DICTIONARY *indexed_dictionary,
			char *key );

boolean dictionary_key_exists_index_zero(
			DICTIONARY *indexed_dictionary,
			char *key );

boolean dictionary_key_exists_index_zero_or_one(
			DICTIONARY *indexed_dictionary,
			char *key );

char *dictionary_display_delimited(
			DICTIONARY *d,
			char delimiter );

char *dictionary_index_zero_display_delimited(
			DICTIONARY *d,
			char delimiter );

void dictionary_add_elements_by_removing_prefix(
		    	DICTIONARY *dictionary,
		    	char *starting_prefix );

void dictionary_add_elements_by_removing_index_zero(
		    	DICTIONARY *dictionary );

DICTIONARY *dictionary_remove_prefix(
			DICTIONARY *dictionary,
		    	char *starting_prefix );

DICTIONARY *dictionary_zap_with_prefix(
			DICTIONARY *dictionary,
		    	char *starting_prefix );

DICTIONARY *dictionary_zap_index_zero_attribute_name_list(
			DICTIONARY *dictionary,
			LIST *attribute_name_list );

void dictionary_zap_where_key_contains_character(
			DICTIONARY *dictionary,
			char character );

DICTIONARY *dictionary_with_prefix(
			DICTIONARY *source_dictionary,
		    	char *starting_prefix );

DICTIONARY *dictionary_without_prefix(
			DICTIONARY *dictionary,
		    	char *starting_prefix );

DICTIONARY *dictionary_remove_index(
			DICTIONARY *dictionary );

DICTIONARY *dictionary_remove_index_zero(
			DICTIONARY *dictionary );

DICTIONARY *dictionary_extract_key_prefix( 
			DICTIONARY *dictionary,
			char *starting_prefix );

DICTIONARY *where_clause_to_dictionary(
			char *where_clause );

int dictionary_extract_where_variable(
			char *variable, 
			char *where_portion );

void dictionary_extract_where_data(
			char *data,
			char *where_portion );

void dictionary_extract_where_operator(
			char *operator,
			char *where_portion );

char *dictionary_with_prefix_at_index_zero(
			DICTIONARY *dictionary,
			char *prefix,
			char *key );

DICTIONARY *dictionary_copy(
			DICTIONARY *dictionary );

DICTIONARY *dictionary_large_copy(
			DICTIONARY *dictionary );

DICTIONARY *copy_dictionary(
			DICTIONARY *dictionary );

DICTIONARY *dictionary_copy_dictionary(
			DICTIONARY *dictionary );

DICTIONARY *copy_dictionary_dictionary(
			DICTIONARY *dictionary );

void dictionary_append_dictionary(
			DICTIONARY *source_destination,
			DICTIONARY *append_dictionary );

void dictionary_append_row_zero(
			DICTIONARY *source_destination,
			DICTIONARY *append_dictionary );

void dictionary_add_suffix_zero(
			DICTIONARY *dictionary );

void dictionary_add_index_zero(
			DICTIONARY *dictionary );

char *dictionary_index_zero(
			DICTIONARY *dictionary,
			char *key );

char *dictionary_fetch_index_zero(
			DICTIONARY *dictionary,
			char *key );

char *dictionary_index_zero_data(
			DICTIONARY *dictionary,
			char *key );

boolean dictionary_index_data_if_populated(
			char **destination,
			DICTIONARY *dictionary,
			char *key, 
			int index );

int dictionary_exists_key_in_list(
			LIST *key_list,
			DICTIONARY *dictionary );

LIST *dictionary_index_zero_key_list(
			DICTIONARY *dictionary,
			LIST *attribute_name_list );

void dictionary_add_pointer(
			DICTIONARY *d, 
			char *key, 
			void *data );

char *dictionary_new_index_key(
			DICTIONARY *d,
			char *key );

boolean dictionary_exists_key(
			DICTIONARY *dictionary,
			char *search_key );

boolean dictionary_key_exists(
			DICTIONARY *dictionary,
			char *search_key );

boolean dictionary_exists_index_key(
			DICTIONARY *d, 
			char *key_with_unknown_index );

LIST *dictionary_index_list(
			DICTIONARY *d,
			char *search_key_without_prefix );

LIST *dictionary_positive_index_list(
			DICTIONARY *d,
			char *search_key_without_prefix );

DICTIONARY *pipe2dictionary(
			char *sys_string,
			char delimiter );

DICTIONARY *dictionary_pipe2dictionary(
			char *sys_string,
			char delimiter );

DICTIONARY *dictionary_pipe(
			char *sys_string,
			char delimiter );

DICTIONARY *dictionary_new_dictionary(
			void );

DICTIONARY *new_dictionary(
			void );

DICTIONARY *dictionary_index_data2dictionary(
			DICTIONARY *source_dictionary,
			char *key_wo_index );

DICTIONARY *dictionary_merge_dictionary(
			DICTIONARY *dictionary1,
			DICTIONARY *dictionary2 );

DICTIONARY *dictionary_merge_lists2dictionary(
			LIST *key_list,
			LIST *data_list );

void dictionary_add_constant(
			DICTIONARY *dictionary,
			LIST *data_list,
			char *constant );

boolean dictionary_data_exists_index_zero(
			DICTIONARY *indexed_dictionary,
			char *key );

int dictionary_string_list_highest_index(
			LIST *string_list,
			DICTIONARY *d );

int dictionary_remove_key(
			DICTIONARY *d,
			char *key );

void dictionary_new_index_key_list_for_data_list(
			DICTIONARY *indexed_dictionary,
			LIST *old_key_list,
			LIST *new_key_list,
			int index );

void dictionary_search_replace_special_characters(
			DICTIONARY *post_dictionary );

void dictionary_set_pointer(
			DICTIONARY *d,
			char *key,
			char *data );

DICTIONARY *dictionary_subtract_dictionary(
			DICTIONARY *d1,
			DICTIONARY *d2 );

boolean dictionary_exists_index_in_key(
			char *key );

boolean dictionary_exists_suffix(
			char *key );

LIST *dictionary_non_populated_index_zero_key_list(
			DICTIONARY *dictionary, 
			LIST *required_attribute_name_list );

LIST *dictionary_populated_index_zero_key_list(
			DICTIONARY *dictionary, 
			LIST *attribute_name_list );

boolean dictionary_populated_key_exists_index_zero(
			DICTIONARY *dictionary,
			char *key );

boolean dictionary_populated_key_exists_index_one(
			DICTIONARY *dictionary,
			char *key );

boolean dictionary_non_populated_key_exists_index_zero(
			DICTIONARY *dictionary,
			char *key );

LIST *dictionary_index_zero_data_list(
			DICTIONARY *dictionary,
			LIST *attribute_name_list );

char *dictionary_delimited_data(
			DICTIONARY *dictionary,
			LIST *attribute_name_list,
			char delimiter );

void dictionary_replace_double_quote_with_single(
			DICTIONARY *dictionary );

char *dictionary_get_pointer(
			DICTIONARY *d,
			char *key );

LIST *dictionary_indexed_data_list(
			DICTIONARY *dictionary,
			char *key_prefix );

LIST *dictionary_populated_index_list(
			DICTIONARY *dictionary,
			char *key_prefix );

LIST *dictionary_with_populated_index_list_index_data_list(
			DICTIONARY *dictionary,
			LIST *populated_index_list,
			char *key_prefix );

DICTIONARY *dictionary_record_to_dictionary(
			LIST *key_list,
			char *record,
			char delimiter );

void dictionary_trim_double_bracked_data(
			DICTIONARY *dictionary );

char *dictionary_trim_double_bracked_string(
			char *string );

LIST *dictionary_extract_prefixed_key_list(
			DICTIONARY *dictionary,
			char *starting_prefix );

LIST *dictionary_extract_and_remove_prefixed_key_list(
			DICTIONARY *dictionary,
			char *starting_prefix );

void dictionary_delete_prefixed_keys(
			DICTIONARY *dictionary,
			char *prefix_string );

int dictionary_remove(	DICTIONARY *dictionary,
			char *key );

int dictionary_delete(	DICTIONARY *dictionary,
			char *key );

int dictionary_delete_key(
			DICTIONARY *dictionary,
			char *key );

int dictionary_remove_data(
			DICTIONARY *d,
			char *key );

DICTIONARY *dictionary_load_record_dictionary(
			FILE *input_pipe,
			int delimiter );

void dictionary_increment_count(
			DICTIONARY *dictionary,
			char *key );

char *dictionary_fetch(	char *key,
			DICTIONARY *d );

char *dictionary_seek(	char *key,
			DICTIONARY *d );

char *dictionary_safe_fetch(
			char *key,
			DICTIONARY *d );

void dictionary_output_with_prefix(
			DICTIONARY *dictionary,
			char *prefix );

void dictionary_output_html_table(
			DICTIONARY *dictionary,
			char *heading1,
			char *heading2,
			boolean align_right );

LIST *dictionary_key_list_index_data_list(
			DICTIONARY *dictionary,
			LIST *key_list,
			int index );

void dictionary_convert_index_to_index_zero(
			DICTIONARY *dictionary,
			int index );

DICTIONARY *dictionary_prefix(
			DICTIONARY *dictionary,
			char *prefix );

void dictionary_delete_keys_with_prefix(
			DICTIONARY *dictionary, 
			char *prefix );


LIST *dictionary_key_data_list(
			DICTIONARY *dictionary,
			LIST *key_list,
			char delimiter );

void dictionary_clear_key_list(
			DICTIONARY *dictionary,
			LIST *key_list );

void dictionary_clear_key_list_index_zero(
			DICTIONARY *dictionary,
			LIST *key_list );

void dictionary_trim_prefix(
			DICTIONARY *dictionary,
			char *prefix );

void dictionary_escape_single_quotes(
			DICTIONARY *dictionary );

boolean dictionary_index_list_string(
			char *destination,
			DICTIONARY *dictionary,
			char *key_without_index );

LIST *dictionary_prefixed_indexed_data_list(
			DICTIONARY *dictionary,
			char *prompt_prefix,
			int dictionary_index,
			char delimiter );

void dictionary_remove_keys_with_prefix(
			DICTIONARY *dictionary,
			char *prefix );

void dictionary_set_delimited_string(
			DICTIONARY *dictionary,
			LIST *key_list,
			char *delimited_string,
			char delimiter );

boolean dictionary_index_data_strict_index(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index );

DICTIONARY *dictionary_huge(
			void );

LIST *dictionary_data_list(
			LIST *attribute_name_list,
			DICTIONARY *dictionary );

LIST *dictionary_key_list_fetch(
			LIST *key_list,
			DICTIONARY *dictionary );

void dictionary_list_output_to_file(
			char *output_filename, 
			LIST *row_dictionary_list,
			LIST *attribute_name_list,
			char *heading_display );

void dictionary_row_output_to_file(
			FILE *output_file, 
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list );

void dictionary_output_heading_to_file(
			FILE *output_file,
			LIST *attribute_name_list );

/* Set the current time, if expected but not there. */
/* ------------------------------------------------ */
void dictionary_set_indexed_date_time_to_current(
			DICTIONARY *dictionary,
			LIST *attribute_list );

void dictionary_set_date_time_to_current(
			DICTIONARY *dictionary,
			LIST *attribute_list );

LIST *dictionary_attribute_name_list(
			LIST *attribute_list );

DICTIONARY *dictionary_row_zero_dictionary(
			DICTIONARY *row_dictionary );

LIST *dictionary_data_list_attribute_name_list_merge_dictionary_list(
			LIST *primary_data_list,
			LIST *primary_key_list,
			char delimiter );

LIST *dictionary_seek_delimited_list(
			DICTIONARY *dictionary,
			char *key );

LIST *dictionary_non_indexed_key_list(
			DICTIONARY *dictionary );

void dictionary_remove_symbols_in_numbers(
			DICTIONARY *dictionary,
			LIST *attribute_list );

DICTIONARY *dictionary_key_piece(
			DICTIONARY *source_dictionary,
			char delimiter,
			int piece_offset );

char *dictionary_data(	DICTIONARY *d,
			char *key );

void dictionary_set_other_data(
			DICTIONARY *d, 
			char *key, 
			void *other_data, 
			int sizeof_other_data );

void dictionary_output_as_hidden(
			DICTIONARY *dictionary );

char *dictionary_starting_label_get(
			char *key,
			char *starting_label,
			DICTIONARY *dictionary );

int dictionary_highest_row(
			DICTIONARY *dictionary );

char *dictionary_row_get(
			char *attribute_name,
			int row,
			DICTIONARY *dictionary );

LIST *dictionary_attribute_data_list(
			LIST *attribute_name_list,
			DICTIONARY *dictionary );

#endif
