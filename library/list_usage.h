/* list_usage.h */
/* ------------ */

#ifndef LIST_USAGE_H
#define LIST_USAGE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"

char *search_replace_list(
			char *source_destination, 
			LIST *list, 
			DICTIONARY *dictionary );

char *search_replace_list_index_prepend_single_quoted(
			char *source_destination, 
			LIST *list,
			DICTIONARY *dictionary,
			int dictionary_key_offset,
			char *search_key_prepend,
			char replace_char_prepend );

int list_string_item_offset(LIST *list, 
			char *search_string );

char *search_replace_list_index_prepend_double_quoted(
			char *source_destination, 
			LIST *list,
			DICTIONARY *dictionary,
			int dictionary_key_offset,
			char *search_key_prepend,
			char search_char_prepend );

void list_separate(	LIST *list,
			int separate_character );

char *list_usage_attribute_data_list2where_clause(
			LIST *attribute_name_list,
			LIST *attribute_data_list );

LIST *list_usage_add_prefix(
			LIST *source_list,
			char *prefix );

LIST *list_usage_remove_prefix(
			LIST *source_list,
			char *prefix );

LIST *list_usage_column_list(
			char *string );

LIST *list_usage_piece_list(	LIST *list,
				char delimiter,
				int offset );

LIST *list_usage_pipe2dictionary_list(	
				char *sys_string, 
				LIST *attribute_name_list,
				LIST *date_attribute_name_list,
				char field_delimiter,
				char *application_name,
				char *login_name );

LIST *pipe2dictionary_list(	
			char *sys_string, 
			LIST *attribute_name_list,
			LIST *date_attribute_name_list,
			char field_delimiter,
			char *application_name,
			char *login_name );

char *list_usage_concat(
			LIST *attribute_name_list );

char *list_usage_in_clause(
			LIST *column_string_list );

LIST *list_usage_attribute_data_dictionary_merge_list(
			LIST *primary_key_list,
			LIST *primary_data_list,
			char delimiter );

LIST *list_usage_file2list(
			char *filename );

void list_usage_stdout_display(
			LIST *list );

boolean list_usage_lists_equal(
			LIST *list1,
			LIST *list2 );

/* Assume comma and dash delimited string. */
/* --------------------------------------- */
LIST *list_usage_expression2number_list(
			char *expression );

#endif
