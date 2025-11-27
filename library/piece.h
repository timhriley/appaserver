/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/piece.h					   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	   */
/* ----------------------------------------------------------------------- */

#ifndef PIECE_H
#define PIECE_H

#include "boolean.h"
#include "list.h"

#define MAX_FIELD_LEN				80
#define MAX_NUM_FIELDS				500
#define PIECE_QUOTE_COMMA_DELIMITER_CODE	1
#define PIECE_SPACES_DELIMITER_CODE		2
#define PIECE_QUOTE				'"'
#define PIECE_COMMA				','
 
typedef struct
{
	char delimiter;
	int count;
} PIECE_DELIMITER_COUNT;

/* Returns destination or null if not enough delimiters */
/* ---------------------------------------------------- */
char *piece(	char *destination,
		char delimiter,
		char *source,
		int offset );

LIST *piece_list(
		LIST *source_list,
		char delimiter,
		int offset );

char piece_delimiter_search_highest(
		LIST *piece_delimiter_count_list );

char piece_delimiter_search(
		char *source );

PIECE_DELIMITER_COUNT *piece_delimiter_count_new(
		char delimiter );

char *piece_unknown(
		char *destination,
		char *delimiter,
		char *source,
		int offset );

char *piece_quoted(
		char *destination,
		char delimiter,
		char *source,
		int offset,
		char quote_character );

/* Returns destination or NULL */
/* --------------------------- */
char *piece_quote(
		char *destination,
		char *source,
		int offset );

/* Returns destination or NULL */
/* --------------------------- */
char *piece_quote_comma(
		char *destination,
		char *source,
		int offset );

char *piece_quote_comma_delimited(
		char *destination,
		char *source,
		int offset );

char *piece_multiple(
		char *destination, 
		char delimiter, 
		char *source, 
		int columns_to_piece );

int piece_string_to_record(
		char*, char );

char *piece_field(
		int );

char *replace_piece(
		char *source_destination, 
		char delimiter, 
		char *new_data, 
		int piece_offset );

char *piece_replace(
		char *source_destination, 
		char delimiter, 
		char *new_data, 
		int piece_offset );

void piece_set_notrim(	void );
void piece_set_trim(	void );

char *piece_inverse(
		char *source_destination,
		char delimiter,
		int piece_offset );

char *piece_delete( char *, char, int );
void piece_bar_or_error( char *d, char *s, int piece_buffer );
int exists_escaped_delimiter( char *source, char delimiter );
char *piece_trim( char *s_here, char *s, int piece_offset );

char *piece_swap(
		char *source_destination, 
		char delimiter, 
		int piece1_offset,
		int piece2_offset );

char *piece_shift_left(
		char *source_destination, 
		char delimiter );

char *piece_shift_right(
		char *source_destination, 
		char delimiter );

char *piece_last(
		char *destination,
		char delimiter,
		char *source );

char *piece_insert(
		char *source_destination, 
		char delimiter, 
		char *insert_this,
		int piece_offset );

char *piece_grep(
		char *source,
		char delimiter,
		char *pattern_to_search,
		int piece_offset,
		boolean grep_exclude );

int piece_record_to_string(
		char *str,
		char delimiter );

char *piece_split_in_two(
		char *destination, 
		char delimiter, 
		char *source, 
		int offset );

int piece_delimiter_position(
		char *source,
		char delimiter,
		int piece_offset );

char *piece_double_quoted(
		char *destination,
		char delimiter,
		char *source,
		int offset );

char *piece_string(
		char *destination,
		char *delimiter,
		char *source,
		int piece_offset );

char *piece_delete_quote_comma(
		char *destination,
		char *source,
		int piece_offset );

char *piece_insert_quote_comma(
		char *destination,
		char *source,
		char *string,
		int piece_offset );

char *piece_replace_quote_comma(
		char *destination, 
		char *source, 
		char *new_data, 
		int piece_offset );

char *piece_delete_multiple(
		char *source_destination,
		char delimiter,
		int columns_to_piece );

int piece_quote_comma_seek(
		char *quote_comma_row,
		char *label );

boolean piece_boolean(
		char *search_string,
		char *delimited_string,
		char delimiter );
#endif
