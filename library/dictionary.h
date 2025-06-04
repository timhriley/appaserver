/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/dictionary.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "hash_table.h"
#include "boolean.h"
#include "list.h"

#define DICTIONARY_ATTRIBUTE_DATUM_DELIMITER	'='
#define DICTIONARY_ELEMENT_DELIMITER		'&'

typedef struct
{
	HASH_TABLE *hash_table;
} DICTIONARY;

/* Usage */
/* ----- */
DICTIONARY *dictionary_new(
		void );

DICTIONARY *dictionary_small(
		void );

DICTIONARY *dictionary_medium(
		void );

DICTIONARY *dictionary_large(
		void );

DICTIONARY *dictionary_huge(
		void );

DICTIONARY *dictionary_super(
		void );

DICTIONARY *dictionary_duper(
		void );

DICTIONARY *dictionary_string_resolve(
		char *dictionary_string );

DICTIONARY *dictionary_index_string2dictionary(
		char *dictionary_string );

DICTIONARY *dictionary_string_dictionary(
		char *dictionary_string );

DICTIONARY *dictionary_string2dictionary(
		char *dictionary_string );

/* Process */
/* ------- */
DICTIONARY *dictionary_calloc(
		void );

/* Usage */
/* ----- */

/* Returns dictionary_large() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_string_new(
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		char *dictionary_string );

/* Usage */
/* ----- */
void dictionary_replace_command_line( 	
		char *command_line,
		DICTIONARY *dictionary );

/* Process */
/* ------- */

LIST *dictionary_indexed_data_list(
		DICTIONARY *dictionary,
		int highest_index,
		char *key_prefix );

/* Usage */
/* ----- */
void dictionary_index_set(
		DICTIONARY *dictionary /* out */,
		DICTIONARY *multi_row_dictionary,
		LIST *dictionary_key_list,
		int index );

/* Usage */
/* ----- */
LIST *dictionary_index_populated_name_list(
		LIST *folder_attribute_non_primary_name_list,
		DICTIONARY *multi_row_dictionary,
		int index );

/* Usage */
/* ----- */
void dictionary_prefixed_dictionary_set(
		DICTIONARY *dictionary /* out */,
		char *prefix,
		DICTIONARY *source_dictionary );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *dictionary_prefixed_key(
		char *prefix,
		char *key );

/* Usage */
/* ----- */
void dictionary_set(
		DICTIONARY *dictionary,
		char *key, 
		char *datum );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *dictionary_get(
		char *key,
		DICTIONARY *dictionary );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *dictionary_list_get(
		DICTIONARY *dictionary,
		LIST *key_list );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *dictionary_index_get(
		char *key,
		DICTIONARY *dictionary,
		int index );

/* Usage */
/* ----- */
int dictionary_length(
		DICTIONARY *dictionary );

/* Usage */
/* ----- */
LIST *dictionary_key_list(
		DICTIONARY *dictionary );

/* Usage */
/* ----- */
LIST *dictionary_ordered_key_list(
		DICTIONARY *dictionary );

/* Usage */
/* ----- */
LIST *dictionary_data_list(
		LIST *key_list,
		DICTIONARY *dictionary );

/* Usage */
/* ----- */
DICTIONARY *dictionary_row_fetch(
		LIST *attribute_name_list,
		char *folder_table_name,
		char *where );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *dictionary_select_attribute_string(
		LIST *attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *dictionary_system_string(
		char *dictionary_select_attribute_string,
		char *folder_table_name,
		char *where );

DICTIONARY *dictionary_parse(
		char sql_delimiter,
		LIST *attribute_name_list,
		char *input );

/* Usage */
/* ----- */

/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_name_list_fetch(
		char *system_string,
		LIST *name_list,
		char delimiter );

/* Usage */
/* ----- */

/* Returns >= 0 */
/* ------------ */
int dictionary_highest_index(
		DICTIONARY *dictionary );

/* Usage */
/* ----- */
int dictionary_key_list_highest_index(
		const char attribute_multi_key_delimiter,
		LIST *key_list,
		DICTIONARY *dictionary,
		int dictionary_highest_index );

/* Usage */
/* ----- */

/* --------------------------------------------------------------------------- 
Two notes:
1) No memory gets allocated. Both the key and data still exist
   in the append_dictionary dictionary.
2) If a they share a key, then it doesn't clobber the source_destinatation.
--------------------------------------------------------------------------- */
DICTIONARY *dictionary_append_dictionary(
		DICTIONARY *source_destination,
		DICTIONARY *append_dictionary );

/* Usage */
/* ----- */

/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_remove_index(
		DICTIONARY *source_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */ 
char *dictionary_display_delimited(
		DICTIONARY *dictionary,
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter );

/* Public */
/* ------ */
DICTIONARY *dictionary_prefix_extract(
	    	const char *prefix,
		DICTIONARY *source_dictionary );

/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *dictionary_remove_prefix(
		const char *prefix,
		DICTIONARY *dictionary );

LIST *dictionary_prefix_key_list(
		const char *prefix,
		DICTIONARY *dictionary );

char *dictionary_pointer(
		char *key,
		DICTIONARY *dictionary );

void dictionary_free(
		DICTIONARY *dictionary );

void dictionary_free_container(
		DICTIONARY *dictionary );

void dictionary_free_data(
		DICTIONARY *dictionary,
		LIST *key_list );

char **get_key_array(
		DICTIONARY *dictionary );

int dictionary_all_null_data(
		DICTIONARY *dictionary );

void dictionary_add(
		DICTIONARY *dictionary,
		char *key,
		char *data,
		int size );

char *dictionary_list_display(
		LIST *dictionary_list );

/* Returns heap memory */
/* -------------------- */ 
char *dictionary_display(
		DICTIONARY *dictionary );

int dictionary_key_highest_index(
		DICTIONARY *dictionary );

boolean dictionary_exists_key_index(
		DICTIONARY *dictionary,
		char *search_key,
		int row );

DICTIONARY *dictionary_copy(
		DICTIONARY *dictionary );

DICTIONARY *dictionary_small_copy(
		DICTIONARY *dictionary );

DICTIONARY *dictionary_medium_copy(
		DICTIONARY *dictionary );

DICTIONARY *dictionary_large_copy(
		DICTIONARY *dictionary );

DICTIONARY *copy_dictionary(
		DICTIONARY *dictionary );

DICTIONARY *dictionary_copy_dictionary(
		DICTIONARY *dictionary );

DICTIONARY *copy_dictionary_dictionary(
		DICTIONARY *dictionary );

DICTIONARY *dictionary_set_dictionary(
		DICTIONARY *source_destination,
		DICTIONARY *append_dictionary );

int dictionary_exists_key_in_list(
		LIST *key_list,
		DICTIONARY *dictionary );

void dictionary_add_pointer(
		DICTIONARY *d, 
		char *key, 
		void *data );

boolean dictionary_exists_key(
		DICTIONARY *dictionary,
		char *search_key );

boolean dictionary_key_exists(
		DICTIONARY *dictionary,
		char *search_key );

boolean dictionary_key_boolean(
		DICTIONARY *dictionary,
		char *search_key );

/* Returns dictionary_large() */
/* -------------------------- */
DICTIONARY *dictionary_pipe(
		char *system_string,
		char delimiter );

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

int dictionary_remove_key(
		DICTIONARY *d,
		char *key );

void dictionary_new_index_key_list_for_data_list(
		DICTIONARY *indexed_dictionary,
		LIST *old_key_list,
		LIST *new_key_list,
		int index );

/* Forbidden characters: `\" */
/* ------------------------- */
DICTIONARY *dictionary_search_replace_special_characters(
		DICTIONARY *dictionary );

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

char *dictionary_delimited_data(
		DICTIONARY *dictionary,
		LIST *attribute_name_list,
		char delimiter );

void dictionary_replace_double_quote_with_single(
		DICTIONARY *dictionary );

char *dictionary_get_pointer(
		DICTIONARY *d,
		char *key );

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
		char *starting_prefix,
		DICTIONARY *dictionary );

void dictionary_delete_prefixed_keys(
		DICTIONARY *dictionary,
		char *prefix_string );

int dictionary_remove(
		DICTIONARY *dictionary,
		char *key );

int dictionary_delete(	DICTIONARY *dictionary,
		char *key );

int dictionary_delete_key(
		DICTIONARY *dictionary,
		char *key );

int dictionary_remove_data(
		DICTIONARY *d,
		char *key );

/*
DICTIONARY *dictionary_load_record_dictionary(
		FILE *input_pipe,
		int delimiter );
*/

void dictionary_increment_count(
		DICTIONARY *dictionary,
		char *key );

/* --------------------------------------- */
/* Returns component of dictionary or null */
/* --------------------------------------- */
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

void dictionary_convert_index_to_index_zero(
		DICTIONARY *dictionary,
		int index );

/* Returns dictionary_medium() or null */
/* ----------------------------------- */
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

LIST *dictionary_attribute_name_list(
		LIST *attribute_list );

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

int dictionary_prefix_highest_row(
		char *prefix,
		DICTIONARY *d );

char *dictionary_row(
		char *attribute_name,
		int row,
		DICTIONARY *dictionary );

LIST *dictionary_attribute_data_list(
		LIST *attribute_name_list,
		DICTIONARY *dictionary );

/* Returns heap memory or null. */
/* Note: has trailing CR.	*/
/* ---------------------------- */
char *dictionary_attribute_name_list_string(
		DICTIONARY *dictionary,
		LIST *attribute_name_list,
		char sql_delimiter,
		int row_number );

/* Returns static memory */
/* --------------------- */
char *dictionary_attribute_name_append_row_number(
		char *attribute_name,
		int row_number );

DICTIONARY *dictionary_file_fetch(
		char *filename,
		char delimiter );

/* ------------------------------------------------------------ */
/* Sample:							*/
/* From:  "station^datatype_1=BA^stage"				*/
/* To:    "station_1=BA and datatype_1=stage"			*/
/* ------------------------------------------------------------ */
void dictionary_parse_multi_attribute_keys(
		DICTIONARY *dictionary,
		char delimiter );

DICTIONARY *dictionary_single_row(
		LIST *key_list,
		int row_number,
		DICTIONARY *multi_row_dictionary );

DICTIONARY *dictionary_append(
		DICTIONARY *destination_dictionary,
		DICTIONARY *source_dictionary );

#endif
