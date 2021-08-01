/* $APPASERVER_HOME/library/list.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

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

/* -------------------------------------------- */
/* Used to determine whether or not to trim off	*/
/* the indices when parsing browser posts.	*/
/* -------------------------------------------- */
#define NO_TRIM_INDICES			0
#define TRIM_INDICES			1

struct LINKTYPE {
        struct LINKTYPE *next;
        struct LINKTYPE *previous;
        char *item;
        int num_bytes;
};

typedef struct {
        struct LINKTYPE *head;
        struct LINKTYPE *tail;
        struct LINKTYPE *current;
        struct LINKTYPE *mark;
        int num_in_list;
	boolean past_end;
} LIST;


/* LIST Function Prototypes */
/* ------------------------ */
void list_load_from_pipe( LIST *list, char *pipe_string );
void list_get_from_pipe( LIST *list, char *pipe_string );
void list_append_string( LIST *list, char *string );

boolean list_append_pointer( LIST *list, void *this_item );

boolean list_set( LIST *list, void *this_item );

boolean list_set_pointer( LIST *list, void *this_item );

boolean list_prepend_pointer( LIST *list, void *this_item );

int list_set_first(	LIST *list,
			void *this_item );

int list_add_head(	LIST *list,
			void *this_item );

void list_append_string_list( LIST *list, LIST *string_list );
void list_append( LIST *list, void *this_item, int num_bytes );
char *append( LIST *list, char *this_item, int num_bytes );

char *list_append_unique(	LIST *list, 
				char *this_item,
				int num_bytes,
				int (*match_fn)() );

int delete_item();

int num_items();

int list_count(		LIST *list );

int list_length(	LIST *list );

int next_item(		LIST *list );

int list_next(		LIST *list );

int list_item_exists(	LIST *list,
			char *item,
			int (*match_fn)() );

void *list_match_seek(	LIST *list,
			char *string,
			int (*match_fn)() );

boolean item_exists(	LIST *list,
			char *item,
			int (*match_fn)() );

boolean list_exists(	LIST *list,
			char *item,
			int (*match_fn)() );

boolean list_exists_string(
			char *string,
			LIST *list );

boolean list_string_exists(
			char *string,
			LIST *list );

boolean list_exists_string_beginning(
			LIST *list,
			char *string );

LIST *create_list(	void );

LIST *list_new(		void );

LIST *list_new_list(	void );

LIST *new_list(		void );

boolean go_head(	LIST * );

boolean list_reset(	LIST *list );

boolean list_rewind(	LIST *list );

void *list_get(		LIST *list );

void *list_pointer(		LIST *list );

void *list_get_pointer(		LIST *list );

void *list_get_current_pointer( LIST *list );

boolean go_tail();

int next_node();
int previous_node();
int retrieve_item( char *ret_item, LIST *list );
char *retrieve_item_ptr();

char *add_item(	LIST *list,
		void *this_item,
		int num_bytes,
		int head_or_tail_or_current );

int destroy_list( LIST *list );

void list_free( LIST *list );
struct LINKTYPE *get_current_record( LIST *list );
void go_record( LIST *list, struct LINKTYPE *l );
int list_at_end( LIST *list );
boolean list_still_more(LIST *list );
boolean list_past_end(	LIST *list );
int list_at_start( LIST *list );
int list_at_head( LIST *list );
int list_at_first( LIST *list );
int at_head( LIST *list );
int at_end( LIST *list );

LIST *list_pipe_fetch(	char *sys_string );

LIST *sys_string2list(	char *sys_string );

LIST *pipe2list(	char *sys_string );

LIST *list_string_list(	char *list_string,
			char delimiter );

LIST *string2list(	char *list_string,
			char delimiter );

LIST *list_delimiter_string_to_list(
			char *list_string,
			char delimiter );

/* offset is zero based */
/* -------------------- */
char *list_get_offset(	LIST *list,
			int offset );

/* offset is zero based */
/* -------------------- */
char *list_seek_offset(	LIST *list,
			int offset );

/* index is one based */
/* ------------------ */
char *list_seek_index(	LIST *list,
			int index );

/* Returns -1 if not found */
/* ----------------------- */
int list_seek(		char *item,
			LIST *list );

LIST *list_subtract( LIST *big_list, LIST *subtract_this );

LIST *subtract_list( LIST *big_list, LIST *subtract_this );

LIST *list_subtract_list( LIST *big_list, LIST *subtract_this );

LIST *list_subtract_string_list( LIST *big_list, LIST *subtract_this );

char *list2comma_string( LIST *list );

char *two_lists_to_dictionary_string( LIST *list1, LIST *list2 );

char *list_string_display( LIST *list );

void list_subtract_dictionary_string( LIST *list, char *dictionary_string );

void list_subtract_string( LIST *list, char *string );

int delete_current( LIST *list );

int list_delete_current( LIST *list );

char *list_display_quoted( char *destination, LIST *list );

char *list_display_quoted_delimiter( 
			char *destination, 
			LIST *list,
			char delimiter );

char *list_display_quoted_delimited(
			char *destination, 
			LIST *list,
			char delimiter );

char *list_display(	LIST *list );

void list_display_lines(
			LIST *list );

/* Returns heap memory */
/* ------------------- */
char *list_display_delimited(
			LIST *list,
			char delimiter );

/* Returns heap memory or empty string */
/* ----------------------------------- */
char *list_display_limited(
			LIST *list,
			char delimiter,
			int length );

char *list_display_delimited_plus_space( LIST *list, char delimiter );

boolean is_subset_of( LIST *subset, LIST *set );

boolean list_is_subset_of( LIST *subset, LIST *set );
void list_delete_from_list( LIST *set, LIST *delete_list );
void list_delete_string( LIST *list, char *string );
LIST *dictionary_string2list( char *dictionary_string, 
			      int attribute_or_data );
LIST *list_trim_indices( LIST *string_list );

LIST *list_unique(	LIST *string_list );

char *list_get_string( LIST *list );

void *list_first(	LIST *list );

void *list_get_first(	LIST *list );

void *list_get_first_pointer(
			LIST *list );

void *list_last(	LIST *list );

void *list_get_last(	LIST *list );

void *list_get_last_pointer( LIST *list );

LIST *list_string_to_list( char *list_string, char delimiter );
LIST *list_string2list( char *list_string, char delimiter );
LIST *list_quote_comma_string2list( char *list_string );
LIST *string_list_duplicate( LIST *list );
LIST *list_duplicate_string_list( LIST *list );
LIST *list_duplicate( LIST *list );
void list_replace_last_string( 	LIST *list, char *string );
void list_replace_string( 	LIST *list, char *old, char *new );

LIST *list_set_list(		LIST *destination_list,
				LIST *source_list );

LIST *list_append_list( 	LIST *destination_list, 
				LIST *source_list );

void *list_get_first_item(	LIST *list );

int list_get_max_string_width( LIST *list );

LIST *pipe2list_append( 	LIST *source_list, 
				char *pipe_string );

LIST *list_copy_string_list(	LIST *source );

LIST *list_copy(		LIST *source );

void list_push_current(		LIST *list );
void list_pop_current(		LIST *list );
void list_mark_current(		LIST *list );
void list_restore_current(	LIST *list );
void list_save(			LIST *list );
void list_restore(		LIST *list );

boolean list_tail( 		LIST *list );

boolean list_go_tail( 		LIST *list );

boolean list_go_last( 		LIST *list );

void list_interpolate_string_record( 
				LIST *list,
				char delimiter );

void list_set_current_string(	LIST *list, char *s );

void list_set_string(		LIST *list, char *s );

boolean list_add_string_in_order(
				LIST *list,
				char *string );

void list_add_string( 		LIST *list, char *string );

int list_add_in_order(		LIST *list, 
				void *this_item, 
				int num_bytes, 
				int (*match_fn)() );

boolean list_add_pointer_in_order(
				LIST *list, 
				void *this_item, 
				int (*match_fn)() );

int add_in_order( 		LIST *list, 
				void *this_item, 
				int num_bytes,
				int (*match_fn)() );

struct LINKTYPE *create_node(	void );
void list_double_forward( 	LIST *double_list, double here );

boolean list_previous( 		LIST *list );

boolean list_prior( 		LIST *list );

char *list_to_string( 		LIST *list, char delimiter );

char *list2string_delimited(	LIST *list, char delimiter );

int list_go_offset( 		LIST *list, int offset );

char *list_get_last_string( 	LIST *list );

char *list_display_delimited_prefixed(	
				LIST *list, 
				char delimiter, 
				char *prefix );

int list_search_string( 	LIST *list, char *string );

void list_toupper(		LIST *list );


char *list_get_first_string(	LIST *list );

int list_strcmp(		char *s1, char *s2 );

int list_string_index_compare(	char *s1, char *s2 );

void free_string_list( 		LIST *string_list );
void list_free_string_list( 	LIST *string_list );
LIST *list_double_quotes_around_string_list(
				LIST *list );

LIST *list_replicate_string_list(
				char *string, int how_many );

LIST *string_array2string_list( char **string_array,
				int max_array_elements );

void *list_get_first_element( 	LIST *list );

int list_at_tail(		LIST *list );
int at_tail(			LIST *list );
void list_bye(			char *s );
int num_in_list(		LIST *list );
int previous_item(		LIST *list );
void moveitem(			char *to, char *from, int num_bytes );

void list_remove_string(	LIST *string_list,
				char *remove_string );

void list_insert_current(	LIST *list, void *item );
void list_set_current(		LIST *list, void *item );
void list_set_current_pointer(	LIST *list, void *item );
void list_pop(			LIST *list );
void list_push(			LIST *list );

LIST *list_intersect_string_list(
				LIST *list1, LIST *list2 );

LIST *list_append_unique_string_list(
			LIST *destination_list,
			LIST *source_list );

LIST *list_unique_list(
			LIST *destination_list,
			LIST *source_list );

void list_unique_set(
			LIST *list,
			char *this_item );

void list_set_unique(
			LIST *list,
			char *this_item );

void list_append_unique_string(
			LIST *list,
			char *this_item );

char *list_buffered_display(	char *destination,
				LIST *list, 
				char delimiter );

LIST *list_merge_string_list(	LIST *list1,
				LIST *list2,
				char delimiter );

/* This will generate an in clause */
/* ------------------------------- */
char *list_display_quote_comma_delimited(
				char *destination, 
				LIST *list );

char *list_display_double_quote_comma_delimited(
				char *destination, 
				LIST *list );

boolean list_equals_string_list(LIST *list1,
				LIST *list2 );

boolean list_equivalent_string_list(
				LIST *list1,
				LIST *list2 );

int list_free_container(	LIST *list );
int list_free_string_container(	LIST *list );

LIST *list_get_position_list(	LIST *list,
				LIST *subset_list );

void list_format_initial_capital(
				LIST *list );

boolean list_go_head(		LIST *list );

boolean list_string_list_same(	LIST *list1,
				LIST *list2 );

boolean list_string_list_match( LIST *list1,
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

LIST *list_rotate(	LIST *list );

LIST *list_purge_duplicates(
			LIST *string_list );

void list_set_position_string(
			LIST *list,
			char *string,
			int position );

void list_free_data(	LIST *list );

LIST *list_delimiter_string_to_integer_list(
			char *list_string,
			char delimiter );

void list_insert_head(	LIST *list,
			void *item,
			int num_bytes );

LIST *list_tail_list(	LIST *list,
			int include_tail_count );

LIST *list_copy_count(	LIST *list,
			int count );

LIST *list_delimited_string_to_list(
			char *delimited_string );

LIST *list_delimiter_list_piece_list(
			LIST *list,
			char delimiter,
			int piece_offset );

LIST *list_string_to_double_list(
			LIST *string_list );

char *list_double_list_display(	char *destination,
				LIST *double_list );

char *list_double_display(	LIST *double_list, 
				char delimiter );


/* -------------------------------------------- */
/* Returns zero-based index of matching double. */
/* Returns -1 if no match.			*/
/* -------------------------------------------- */
int list_double_list_match(	LIST *double_list,
				double match );

LIST *list_append_current_list(	LIST *destination_list,
				LIST *source_list );

void list_html_display(
			LIST *list );

void list_html_table_display(
			LIST *list,
			char *heading_string );

LIST *list_cycle_right(		LIST *list );

void *list_data(		LIST *list );

char *list_integer_display(	LIST *integer_list,
				char delimiter );

void list_linktype_free(	struct LINKTYPE *linktype );

LIST *list_string_new(		char *string );

char *list_length_display(
			LIST *string_list,
			int how_many );

#endif
