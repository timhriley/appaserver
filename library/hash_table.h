/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/hash_table.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "list.h"

/* Should be prime */
/* --------------- */
#define HASH_TABLE_SMALL		1019
#define HASH_TABLE_MEDIUM		10007
#define HASH_TABLE_LARGE		99991
#define HASH_TABLE_HUGE			299993
#define HASH_TABLE_SUPER		610639
#define HASH_TABLE_DUPER		1019071

enum hash_table_size	{		hash_table_small,
					hash_table_medium,
					hash_table_large,
					hash_table_huge,
					hash_table_super,
					hash_table_duper };

typedef struct
{
	char *key;
	void *other_data;
	int tried_to_insert_duplicate_key;
} HASH_TABLE_CELL;

typedef struct
{
	LIST **cell_array;
	HASH_TABLE_CELL *last_cell_retrieved;
	int cell_array_size;
} HASH_TABLE;

/* Usage */
/* ----- */
HASH_TABLE *hash_table_new(
		enum hash_table_size );

/* Public */
/* ------ */
LIST *hash_table_key_list(
		HASH_TABLE *h );

LIST *hash_table_get_key_list(
		HASH_TABLE *h );

LIST *hash_table_ordered_key_list(
		HASH_TABLE *h );

LIST *hash_table_get_ordered_key_list(
		HASH_TABLE *h );

HASH_TABLE *hash_table_init(
		enum hash_table_size );

int hash_table_len(
		HASH_TABLE *h );

int hash_table_length(
		HASH_TABLE *h );

int hash_table_insert(
		HASH_TABLE *h,
		char *key,
		void *other_data );

int hash_table_set_pointer(
		HASH_TABLE *h,
		char *key,
		void *other_data );

int hash_table_add_pointer(
		HASH_TABLE *h,char *key,
		void *other_data );

int hash_table_insert_back(
		HASH_TABLE *h,
		char *key,
		void *other_data );

void display_hash_table(
		HASH_TABLE *h,
		char *output_file );

void *hash_table_get(
		HASH_TABLE *h,
		char *key );

void *hash_table_get_pointer(
		HASH_TABLE *h,
		char *key );

void *hash_table_seek(
		HASH_TABLE *h,
		char *key );

void *hash_table_fetch(
		HASH_TABLE *h,
		char *key );

void *hash_table_fetch_pointer(
		HASH_TABLE *h,
		char *key );

/* --------------------------------------- */
/* Returns component of hash_table or null */
/* --------------------------------------- */
void *hash_table_retrieve_other_data(
		HASH_TABLE *hash_table,
		char *key,
		int *duplicate_indicator );

int hash_table_free(
		HASH_TABLE *h);

int hash_table_all_null_data(
		HASH_TABLE *h );

LIST *get_hashtbl_key_list(
		HASH_TABLE *h );

char *hash_table_display(
		char *destination,
		HASH_TABLE *h );

void hash_table_display_key_list(
		HASH_TABLE *h );

char *hash_table_display_delimiter( 
		char *destination, 
		HASH_TABLE *h,
		const char *attribute_datum_delimiter,
		const char *element_delimiter,
		unsigned long buffer_size );

char *hash_table_index_zero_display_delimiter(
		char *destination, 
		HASH_TABLE *h,
		int delimiter );

boolean hash_table_exists_key(
		HASH_TABLE *h,
		char *key );

boolean hash_table_exists(
		HASH_TABLE *h,
		char *key );

char *hash_table_get_string(
		HASH_TABLE *h,
		char *key );

int hash_table_set_string(
		HASH_TABLE *h,
		char *key,
		char *s );

int hash_table_set(
		HASH_TABLE *h, 
		char *key, 
		void *other_data );

int compare_key(HASH_TABLE_CELL *cell_ptr,
		char *key );

int hash_table_remove(
		HASH_TABLE *h,
		char *key );

unsigned int hash_table_hash_value(
		char *key,
		int cell_array_size );

int hash_table_remove_data(
		HASH_TABLE *h,
		char *key );

HASH_TABLE *hash_table_new_hash_table(
		enum hash_table_size );

int hash_table_set_unique_key(
		HASH_TABLE *h,
		char *key,
		void *other_data );

void hash_table_free_container(
		HASH_TABLE *h );

#endif
