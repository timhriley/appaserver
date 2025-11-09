/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/list.h			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include "boolean.h"

/* Add A Node To Either Head Or Tail or Current */
/* -------------------------------------------- */
#define ADD_HEAD        0
#define ADD_TAIL        1
#define ADD_CURRENT     2

/* --------------------------------------------- */
/* Used to determine what to return from parsing */
/* string dictionaries.				 */
/* --------------------------------------------- */
#define LIST_POPULATED_ATTRIBUTE		0
#define LIST_DATA				1

struct LINKTYPE {
        struct LINKTYPE *next;
        struct LINKTYPE *previous;
        char *item;
        int num_bytes;
};

/* Usage */
/* ----- */
struct LINKTYPE *create_node(
			void );

/* Process */
/* ------- */

/* Public */
/* ------ */

typedef struct {
        struct LINKTYPE *head;
        struct LINKTYPE *tail;
        struct LINKTYPE *current;
        struct LINKTYPE *mark;
        int num_in_list;
	boolean past_end;
} LIST;

/* Usage */
/* ----- */
LIST *list_new( void );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *list_stdin(
		void );

/* Public */
/* ------ */
void list_load_from_pipe( LIST *list, char *pipe_string );
void list_get_from_pipe( LIST *list, char *pipe_string );
void list_append_string( LIST *list, char *string );

void list_set( LIST *list, void *this_item );

void list_append_pointer( LIST *list, void *this_item );

void list_set_pointer( LIST *list, void *this_item );

void list_prepend_pointer( LIST *list, void *this_item );

void list_set_first(	LIST *list,
			void *this_item );

void list_add_head(	LIST *list,
			void *this_item );

void list_set_head(	LIST *list,
			void *this_item );

void list_append_string_list( LIST *list, LIST *string_list );
void list_append( LIST *list, void *this_item, int num_bytes );
char *append( LIST *list, char *this_item, int num_bytes );

char *list_append_unique(
		LIST *list, 
		char *this_item,
		int num_bytes,
		int (*compare_fn)() );

void delete_item(
		LIST *list );

int num_items(	void );

int list_count(	LIST *list );

int list_length(LIST *list );

boolean list_next(
		LIST *list );

boolean next_item(
		LIST *list );

boolean list_item_exists(
		LIST *list,
		char *item,
		int (*compare_fn)() );

boolean list_item_boolean(
		LIST *list,
		char *item,
		int (*compare_fn)() );

int list_item_offset(
		LIST *list,
		char *item,
		int (*compare_fn)() );

void *list_match_seek(
		LIST *list,
		char *string,
		int (*compare_fn)() );

boolean item_exists(
		LIST *list,
		char *item,
		int (*compare_fn)() );

boolean list_exists(
		LIST *list,
		char *item,
		int (*compare_fn)() );

boolean list_exists_string(
		char *string,
		LIST *list );

boolean list_string_exists(
		char *string,
		LIST *list );

boolean list_string_boolean(
		char *string,
		LIST *list );

boolean list_exists_string_beginning(
		LIST *list,
		char *string );

LIST *create_list(
		void );

LIST *list_new_list(
		void );

LIST *new_list(	void );

boolean go_head(LIST * );

boolean list_reset(
		LIST *list );

boolean list_rewind(
		LIST *list );

char *list_string(
		LIST *list );

char *list_get_string(
		LIST *list );

void *list_get(	LIST *list );

void *list_get_pointer(
		LIST *list );

boolean go_tail(
		LIST *list );

int next_node( void );

int previous_node(void );

int retrieve_item(
		char *ret_item,
		LIST *list );

char *retrieve_item_ptr(
		LIST *list );

char *add_item(	LIST *list,
		void *this_item,
		int num_bytes,
		int head_or_tail_or_current );

int destroy_list( LIST *list );

void list_free(	LIST *list );

boolean list_at_end(
		LIST *list );

boolean list_at_last(
		LIST *list );

boolean list_last_boolean(
		LIST *list );

boolean list_still_more(
		LIST *list );

boolean list_past_end(
		LIST *list );

boolean list_at_start(
		LIST *list );

boolean list_at_head(
		LIST *list );

boolean list_at_first(
		LIST *list );

boolean list_first_boolean(
		LIST *list );

boolean at_head(LIST *list );

boolean at_end(	LIST *list );

LIST *list_pipe(
		char *system_string );

LIST *list_pipe_fetch(
		char *system_string );

LIST *list_stream_fetch(
		FILE *stream );

LIST *list_file_fetch(
		FILE *file );

LIST *list_fetch_pipe(
		char *system_string );

LIST *sys_string2list(
		char *sys_string );

LIST *pipe2list(char *sys_string );

/* Safely returns */
/* -------------- */
LIST *list_string_list(
		char *list_string,
		char delimiter );

LIST *list_string_extract(
		char *list_string,
		char delimiter );

LIST *string2list(
		char *list_string,
		char delimiter );

LIST *list_delimiter_string_to_list(
		char *list_string,
		char delimiter );

/* offset is zero based */
/* -------------------- */
void *list_offset_seek(
		int offset,
		LIST *list );

/* index is one based */
/* ------------------ */
void *list_index_seek(
		int index,
		LIST *list );

/* Returns -1 if not found */
/* ----------------------- */
int list_seek_offset(
		char *item,
		LIST *list );

LIST *list_subtract(
		LIST *list,
		LIST *subtract_this );

LIST *subtract_list(
		LIST *list,
		LIST *subtract_this );

LIST *list_subtract_list(
		LIST *list,
		LIST *subtract_this );

LIST *list_subtract_string_list(
		LIST *list,
		LIST *subtract_this );

char *list2comma_string(
		LIST *list );

char *two_lists_to_dictionary_string(
		LIST *list1,
		LIST *list2 );

char *list_string_display(
		LIST *list );

void list_subtract_dictionary_string(
		LIST *list,
		char *dictionary_string );

/* Returns list */
/* ------------ */
LIST *list_subtract_string(
		LIST *list,
		char *string );

void delete_current(
		LIST *list );

/* ------------------------------------------------------------ */
/* This function deletes the link that current is pointing to	*/
/* but not the datum.						*/
/* ------------------------------------------------------------ */
void list_delete(
		LIST *list );

char *list_display_quoted(
		char *destination,
		LIST *list );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display(
		LIST *list );

void list_display_lines(
		LIST *list );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display_line(
		LIST *list );

/* Returns heap memory or null */
/* --------------------------- */
char *list_delimited_string(
		LIST *list,
		char delimiter );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display_delimited(
		LIST *list,
		char delimiter );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_delimited(
		LIST *list,
		char delimiter );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display_string_delimited(
		LIST *list,
		char *delimited_string );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_string_delimited(
		LIST *list,
		char *delimited_string );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display_limited(
		LIST *list,
		char delimiter,
		int length );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_display_delimited_plus_space(
		LIST *list,
		char delimiter );

boolean is_subset_of(
		LIST *subset,
		LIST *set );

boolean list_is_subset_of(
		LIST *subset,
		LIST *set );

boolean list_subset_boolean(
		LIST *subset,
		LIST *set );

void list_delete_from_list(
		LIST *set,
		LIST *delete_list );

void list_delete_string(
		LIST *list,
		char *string );

LIST *dictionary_string2list(
		char *dictionary_string, 
		int attribute_or_data );

LIST *list_trim_indices(
		LIST *string_list );

LIST *list_unique(
		LIST *string_list );


/* Returns the first element or null */
/* --------------------------------- */
void *list_first(
		LIST *list );

/* Returns the last element or null */
/* -------------------------------- */
void *list_last(
		LIST *list );

/* Returns list with new heap memory */
/* --------------------------------- */
LIST *list_string_to_list(
		char *list_string,
		char delimiter );

LIST *list_string2list(
		char *list_string,
		char delimiter );

LIST *list_quote_comma_string2list(
		char *list_string );

LIST *list_duplicate_string_list(
		LIST *list );

LIST *list_duplicate(
		LIST *list );

void list_replace_last_string( 
		LIST *list,
		char *string );

void list_replace_last( 
		LIST *list,
		void *datum );

void list_replace_string(
		LIST *list,
		char *old,
		char *new );

LIST *list_set_list(
		LIST *destination_list,
		LIST *source_list );

LIST *list_append_list(
		LIST *destination_list, 
		LIST *source_list );

void *list_first_item(
		LIST *list );

int list_max_string_width(
		LIST *list );

LIST *pipe2list_append(
		LIST *source_list, 
		char *pipe_string );

LIST *list_strdup_copy(
		LIST *list );

LIST *list_copy(
		LIST *list );

void list_push_current(
		LIST *list );

void list_pop_current(
		LIST *list );

void list_mark_current(
		LIST *list );


void list_push(	LIST *list );
void list_save(	LIST *list );

void list_pop(	LIST *list );
void list_restore(
		LIST *list );


boolean list_tail(
		LIST *list );

boolean list_go_tail(
		LIST *list );

boolean list_go_last(
		LIST *list );

void list_interpolate_string_record( 
		LIST *list,
		char delimiter );

void list_set_current_string(
		LIST *list,
		char *s );

void list_set_string(
		LIST *list,
		char *s );

boolean list_add_string_in_order(
		LIST *list,
		char *string );

boolean list_set_string_in_order(
		LIST *list,
		char *string );

boolean list_string_in_order(
		LIST *list,
		char *string );

void list_add_string(
		LIST *list,
		char *string );

int list_add_in_order(
		LIST *list, 
		void *this_item, 
		int num_bytes, 
		int (*compare_fn)() );

boolean list_add_pointer_in_order(
		LIST *list, 
		void *this_item, 
		int (*compare_fn)() );

boolean list_set_order(
		LIST *list /* in/out */,
		void *this_item, 
		int (*compare_fn)() );

int add_in_order(
		LIST *list, 
		void *this_item, 
		int num_bytes,
		int (*compare_fn)() );

void list_double_forward(
		LIST *double_list,
		double here );

boolean list_previous(
		LIST *list );

boolean list_prior(
		LIST *list );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_to_string(
		LIST *list,
		char delimiter );

char *list2string_delimited(
		LIST *list,
		char delimiter );

int list_go_offset(
		LIST *list,
		int offset );

char *list_last_string(
		LIST *list );

boolean list_search_string(
		LIST *list,
		char *string );

void list_toupper(
		LIST *list );


int list_strcmp(char *s1,
		char *s2 );

int list_string_index_compare(
		char *s1,
		char *s2 );

void free_string_list(
		LIST *string_list );

void list_free_string_list(
		LIST *string_list );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_single_quotes_around_string(
		LIST *list );

LIST *list_replicate_string_list(
		char *string,
		int how_many );

LIST *string_array2string_list(
		char **string_array,
		int max_array_elements );

boolean list_at_tail(
		LIST *list );

boolean list_tail_boolean(
		LIST *list );

boolean at_tail(LIST *list );

void list_bye(	char *s );

int num_in_list(LIST *list );

int previous_item(
		LIST *list );

void moveitem(	char *to,
		char *from,
		int num_bytes );

/* Returns string_list */
/* ------------------- */
LIST *list_remove_string(
		LIST *string_list,
		char *remove_string );

void list_insert_current(
		LIST *list,
		void *item );

void list_set_current(
		LIST *list,
		void *item );

void list_set_current_pointer(
		LIST *list,
		void *item );


LIST *list_intersect_string_list(
		LIST *list1,
		LIST *list2 );

LIST *list_append_unique_string_list(
		LIST *destination_list /* in/out */,
		LIST *source_list );

LIST *list_unique_list(
		LIST *destination_list /* in/out */,
		LIST *source_list );

void list_unique_set(
		LIST *list,
		char *this_item );

void list_set_unique(
		LIST *list,
		char *string );

void list_append_unique_string(
		LIST *list,
		char *this_item );

char *list_buffered_display(
		char *destination,
		LIST *list, 
		char delimiter );

LIST *list_merge_string_list(
		LIST *list1,
		LIST *list2,
		char delimiter );

/* Returns destination */
/* ------------------- */
char *list_single_quote_comma_display(
		char *destination, 
		LIST *list );

/* Returns destination */
/* ------------------- */
char *list_double_quote_comma_display(
		char *destination, 
		LIST *list );

boolean list_equals_string_list(
		LIST *list1,
		LIST *list2 );

boolean list_equivalent_string_list(
		LIST *list1,
		LIST *list2 );

/* ------------------------------- */
/* Free the list but keep the data */
/* ------------------------------- */
void list_free_container(
		LIST *list );

/* ------------------------------- */
/* Free the list but keep the data */
/* ------------------------------- */
void list_free_string_container(
		LIST *list );

LIST *list_position_list(
		LIST *list,
		LIST *subset_list );

LIST *list_initial_capital(
		LIST *list );

boolean list_head(
		LIST *list );

boolean list_go_head(
		LIST *list );

boolean list_string_list_same(
		LIST *list1,
		LIST *list2 );

boolean list_string_list_match(
		LIST *list1,
		LIST *list2 );

boolean list_exists_any_index_string(
		LIST *list,
		char *string );

boolean list_exists_index_string(
		LIST *list,
		char *string,
		int index );

boolean list_string_list_all_populated(
		LIST *list );

boolean list_string_list_all_empty(
		LIST *list );

LIST *list_rotate(
		LIST *list );

LIST *list_purge_duplicates(
		LIST *string_list );

void list_set_position_string(
		LIST *list,
		char *string,
		int position );

void list_free_datum(
		LIST *list );

LIST *list_delimiter_string_to_integer_list(
		char *list_string,
		char delimiter );

void list_insert_head(
		LIST *list,
		void *item,
		int num_bytes );

LIST *list_tail_list(
		LIST *list,
		int include_tail_count );

LIST *list_copy_count(
		LIST *list,
		int count );

LIST *list_delimited_string_to_list(
		char *delimited_string );

LIST *list_delimiter_extract(
		char *delimited_string,
		char delimiter );

LIST *list_delimiter_list_piece_list(
		LIST *list,
		char delimiter,
		int piece_offset );

LIST *list_string_to_double_list(
		LIST *string_list );

char *list_double_list_display(
		char *destination,
		LIST *double_list );

/* Returns heap memory or "" */
/* ------------------------- */
char *list_double_display(
		LIST *double_list, 
		char delimiter );

/* -------------------------------------------- */
/* Returns zero-based index of matching double. */
/* Returns -1 if no match.			*/
/* -------------------------------------------- */
int list_double_list_match(
		LIST *double_list,
		double match );

LIST *list_append_current_list(
		LIST *destination_list,
		LIST *source_list );

void list_html_display(
		LIST *list );

void list_html_table_display(
		LIST *list,
		char *heading_string );

LIST *list_cycle_right(
		LIST *list );

void *list_data(LIST *list );

void *list_datum(
		LIST *list );

char *list_integer_display(
		LIST *integer_list,
		char delimiter );

LIST *list_string_new(
		char *string );

char *list_length_display(
		LIST *string_list,
		int how_many );

char *list_string_search(
		char *string,
		LIST *list );

struct LINKTYPE *get_current_record(
		LIST *list );

void go_record(	LIST *list,
		struct LINKTYPE *l );

LIST *list_argv(int argc,
		char **argv );

void list_write(
		LIST *list,
		char *filename );

LIST *list_string_initial_capital(
		LIST *string_list );

void list_display_stream(
		LIST *list,
		FILE *stream );

#endif
