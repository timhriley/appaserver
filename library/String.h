/* $APPASERVER_HOME/library/String.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef STRING_H
#define STRING_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */
#define STRING_LF	10
#define STRING_CR	13

/* Structures */
/* ---------- */
typedef struct
{
	char *ptr;
	int occurrance;
} STRING_OCCURRANCE;

/* Prototypes */
/* ---------- */
char *string_enforce_utf16(	char *destination,
				char *source );

LIST *string_negative_sequence_integer_list(
				char *source );

char *string_occurrance_list_display(
				char *destination,
				LIST *occurrance_list );

LIST *string_negative_sequence_occurrance_list(
				char *source );

/* Returns input_buffer or (char *)0 if all done. */
/* ---------------------------------------------- */
char *string_input(	char *input_buffer,
			FILE *infile,
			int buffer_size );

void string_reset_get_line_check_utf_16(
			void );

char *string_escape_quote(
			char *destination,
			char *source );

char *string_escape_quote_dollar(
			char *destination,
			char *source );

char *string_escape_full(
			char *destination,
			char *source );

char *string_escape(
			char *destination,
			char *source,
			char *character_array );

char *string_escape_character_array(
			char *destination,
			char *source,
			char *character_array );

int string_strlen(	char *s );

boolean string_strcmp(	char *s1,
			char *s2 );

boolean string_strcpy(	char *d,
			char *s,
			int buffer_size );

char *string_strcat(	char *d,
			char *s );

int string_strncmp(	char *s1,
			char *s2 );

boolean string_loose_strcmp(
			char *s1,
			char *s2 );

char *string_remove_control(
			char *input );

char *string_escape_character(
			char *destination,
			char *data,
			int character_to_escape );

char *string_commas_rounded_dollar(
			double d );

char *string_commas_dollar(
			double d );

char *string_commas_money(
			double d );

/* Returns heap memory of 3 decimal places */
/* --------------------------------------- */
char *string_commas_double(
			double d );

char *string_format_mnemonic(
			char *mnemonic,
			char *string );

boolean string_character_exists(
			char *buffer,
			char c );

/* Position is 1 based */
/* ------------------- */
int string_character_position(
			char *buffer,
			char c );

char *string_trim_character(
			char *buffer,
			char delimiter );

char *string_trim_right(
			char *buffer,
			int length );

int string_character_count(
			char ch,
			char *source );

char *string_rtrim(	char *buffer );

boolean string_exists_substr(
			char *string,
			char *substring );

int string_instr(	char *substr,
			char *string,
			int occurrence );

char *string_input_tmp(	char *input_buffer,
			FILE *infile,
			int buffer_size );

char *string_pipe_fetch( char *sys_string );

LIST *string_pipe_list(	char *system_string );

char *string_repeat(	char *string,
			int number_times );

#endif
