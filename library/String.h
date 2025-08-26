/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/String.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef STRING_H
#define STRING_H

#include "boolean.h"
#include "list.h"

#define STRING_LF			10
#define STRING_CR			13
#define STRING_TAB			9
#define STRING_1K			1024
#define STRING_2K			2048
#define STRING_4K			4096
#define STRING_8K			8192
#define STRING_16K			16384
#define STRING_32K			32768
#define STRING_64K			65536
#define STRING_65K			66560
#define STRING_66K			67584
#define STRING_128K			131072
#define STRING_129K			132096
#define STRING_192K			196608
#define STRING_256K			262144
#define STRING_512K			524288
#define STRING_640K			655360
#define STRING_704K			720896
#define STRING_768K			786432
#define STRING_769K			787456
#define STRING_770K			788480

/* Unpredictable behavior occurred
#define STRING_ONE_MEG			1048576
#define STRING_TWO_MEG			2097152
#define STRING_THREE_MEG		3145728
#define STRING_FOUR_MEG			4194304
*/

#define STRING_SIZE_HASH_TABLE		70645
#define STRING_SYSTEM_BUFFER		STRING_704K
#define STRING_WHERE_BUFFER		STRING_704K
#define STRING_COMMAND_BUFFER		STRING_704K
#define STRING_INPUT_BUFFER		STRING_704K

#define STRING_OVERFLOW_TEMPLATE	"Memory size of %d exceeded."

typedef struct
{
	char *ptr;
	int occurrance;
} STRING_OCCURRANCE;

/* Usage */
/* ----- */
void string_replace_command_line(
		char *command_line /* in/out */,
		char *string,
		char *placeholder );

/* Usage */
/* ----- */
char *string_with_space_search_replace(
		char *source_destination /* in/out */,
		char *search_string,
		char *replace_string );

/* Public */
/* ------ */
char *string_enforce_utf16(
		char *destination,
		char *source );

LIST *string_negative_sequence_integer_list(
		char *source );

char *string_occurrance_list_display(
		char *destination,
		LIST *occurrance_list );

LIST *string_negative_sequence_occurrance_list(
		char *source );

/* Returns input_buffer or null if all done. */
/* ----------------------------------------- */
char *string_input(
		char *input_buffer,
		FILE *infile,
		unsigned int buffer_size );

void string_reset_get_line_check_utf_16(
		void );

char *string_escape_quote(
		char *destination,
		char *source );

char *string_escape_quote_dollar(
		char *destination,
		char *source );

/* Returns heap memory */
/* ------------------- */
char *string_escape_dollar(
		char *source );

/* Safely returns destination */
/* -------------------------- */
char *string_escape_full(
		char *destination,
		char *source );

/* Safely returns destination */
/* -------------------------- */
char *string_escape(
		char *destination,
		char *source,
		char *character_array );

/* Returns heap memory */
/* ------------------- */
char *string_escape_array(
		char *character_array,
		char *source );

/* Safely returns destination */
/* -------------------------- */
char *string_escape_character_array(
		char *destination,
		char *source,
		char *character_array );

/* Safely returns destination */
/* -------------------------- */
char *string_unescape_character_array(
		char *destination,
		char *source,
		char *character_array );

int string_strlen(
		char *string );

int string_strcmp(
		char *s1,
		char *s2 );

int string_strcasecmp(
		char *s1,
		char *s2 );

/* Returns destination */
/* ------------------- */
char *string_strcpy(
		char *destination,
		char *source,
		unsigned int buffer_size );

/* Returns destination */
/* ------------------- */
char *string_strcat(
		char *destination,
		char *source );

int string_strncmp(
		char *large_string,
		char *small_string );

boolean string_loose_strcmp(
		char *s1,
		char *s2 );

/* Returns static memory */
/* --------------------- */
char *string_remove_control(
		char *input );

/* Returns source_destination */
/* -------------------------- */
char *string_remove_character(
		char *source_destination /* in/out */,
		char character );

/* Returns source_destination */
/* -------------------------- */
char *string_remove_character_string(
		char *source_destination /* in/out */,
		const char *character_string );

/* Returns destination */
/* ------------------- */
char *string_escape_character(
		char *destination,
		char *data,
		char character );

/* Returns static memory */
/* ----------------------*/
char *string_commas_rounded_dollar(
		double d );

/* Returns static memory */
/* ----------------------*/
char *string_commas_dollar(
		double d );

/* Returns static memory. */
/* Doesn't trim pennies.  */
/* ---------------------- */
char *string_commas_money(
		double d );

/* Returns static memory */
/* ----------------------*/
char *string_commas_double(
		double d,
		int decimal_count );

/* Returns static memory */
/* ----------------------*/
char *string_commas_float(
		double d,
		int decimal_places );

/* Returns static memory or "" */
/* --------------------------- */
char *string_commas_number_string(
		char *string );

/* Returns static memory or "" */
/* --------------------------- */
char *string_commas_integer(
		int i );

/* Returns static memory or "" */
/* --------------------------- */
char *string_commas_long(
		long n );

/* Returns static memory or "" */
/* --------------------------- */
char *string_commas_unsigned_long(
		unsigned long n );

/* Returns static memory or "" */
/* --------------------------- */
char *string_commas_unsigned_long_long(
		unsigned long long n );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *string_mnemonic(
		char *string );

/* ---------------- */
/* Returns mnemonic */
/* ---------------- */
char *string_format_mnemonic(
		char *mnemonic,
		char *string );

boolean string_character_exists(
		char *datum,
		char c );

boolean string_character_boolean(
		char *datum,
		char c );

/* Position is 1 based */
/* ------------------- */
int string_character_position(
		char *data,
		char c );

/* Returns datum */
/* ------------- */
char *string_trim_character(
		char *datum,
		char character );

/* Returns buffer with begin shortened (maybe) */
/* ------------------------------------------- */
char *string_trim_left_spaces(
		char *buffer );

/* Returns buffer with end shortened (maybe) */
/* ----------------------------------------- */
char *string_trim_right_spaces(
		char *buffer );

/* Returns someplace in input */
/* -------------------------- */
char *string_right(
		char *input,
		int length );

int string_character_count(
		char ch,
		char *source );

/* Returns buffer with end shortened (maybe) */
/* ----------------------------------------- */
char *string_rtrim(
		char *buffer );

boolean string_exists(
		char *string,
		char *substring );

boolean string_exists_substring(
		char *string,
		char *substring );

/* Returns -1 if not found. */
/* ------------------------ */
int string_instr(
		char *substr,
		char *string,
		int occurrence );

/* Returns heap memory or null */
/* --------------------------- */
char *string_pipe(
		char *system_string );

/* Returns heap memory or null */
/* --------------------------- */
char *string_pipe_fetch(
		char *system_string );

/* Returns heap memory or null */
/* --------------------------- */
char *string_fetch_pipe(
		char *system_string );

/* Returns heap memory or null */
/* --------------------------- */
char *string_fetch(
		char *system_string );

/* Returns heap memory or null */
/* --------------------------- */
char *string_pipe_input(
		char *system_string );

LIST *string_pipe_list(
		char *system_string );

/* Returns heap memory */
/* ------------------- */
char *string_repeat(
		char *string,
		int number_times );

/* Returns static memory */
/* --------------------- */
char *string_itoa(
		int i );

/* Returns static memory */
/* --------------------- */
char *string_ftoa(
		double f,
		int decimal_places );

/* Returns datum */
/* ------------- */
char *string_trim_character_array(
		char *datum,
		char *character_array );

char *string_delimiter_repeat(
		char *string,
		char delimiter,
		int number_times );

char *string_strncpy(
		char *destination,
		char *source,
		int count );

/* Returns heap memory */
/* ------------------- */
char *string_in_clause(
		LIST *data_list );

int string_length(
		char *string );

/* Trims leading and trailing spaces.	*/
/* Returns buffer.			*/
/* ------------------------------------ */
char *string_trim(
		char *buffer );

char *string_extract_lt_gt_delimited(
		char *destination,
		char *source );

/* -------------------------------------------------- */
/* Sample: attribute_name = "station" <-- returns -1  */
/* Sample: attribute_name = "station_0" <-- returns 0 */
/* Sample: attribute_name = "station_1" <-- returns 1 */
/* -------------------------------------------------- */
int string_index(
		char *attribute_name );

char *string_trim_leading_character(
		char *data,
		char character );

/* Returns destination */
/* ------------------- */
char *string_trim_index(
		char *destination,
		char *source );

boolean string_exists_character(
		char *source,
		char character );

boolean string_exists_character_string(
		const char *character_string,
		char *string );

char string_delimiter(
		char *string );

/* Returns destination */
/* ------------------- */
char *string_initial_capital(
		char *destination,
		char *source );

/* Returns destination */
/* ------------------- */
char *string_separate_delimiter(
		char *destination,
		char *source );

/* Returns source_destination */
/* -------------------------- */
char *string_search_replace_character(
		char *source_destination,
		char search_character,
		char replace_character );

/* Returns source_destination */
/* -------------------------- */
char *string_search_replace(
		char *source_destination,
		char *search_string,
		char *replace_string );

/* Returns string */
/* -------------- */
char *string_insert(
		char *string,
		char *substring,
		int pos );

/* Returns string */
/* -------------- */
char *string_delete(
		char *string,
		int start,
		int num_chars );

/* Sample: attribute_name = "station_1" */
/* ------------------------------------ */
int string_row_number(
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *string_append(
		char *message_list_string,
		char *message_string,
		char *delimiter );

char string_last_character(
		char *string );

/* Returns string */
/* -------------- */
char *string_up(
		char *string );

/* Returns string */
/* -------------- */
char *string_low(
		char *string );

/* Returns heap memory */
/* ------------------- */
char *string_strdup(
		char *string );

/* Safely returns */
/* -------------- */
double string_atof(
		char *string );

/* Safely returns */
/* -------------- */
int string_atoi(
		char *string );

/* Returns static memory */
/* --------------------- */
char *string_double_quote_comma(
		char *source,
		char delimiter );

/* Returns heap memory or null */
/* --------------------------- */
char *string_file_fetch(
		char *filename,
		char *delimiter );

boolean string_file_write(
		char *filename,
		char *string );

/* Returns destination */
/* ------------------- */
char *string_unescape_character(
		char *destination,
		char *datum,
		char character_to_unescape );

/* Returns static memory */
/* --------------------- */
char *string_double_quotes_around(
		char *s );

/* Returns static memory */
/* --------------------- */
char *string_quotes_around(
		char *s,
		char c );

char *string_reverse(
		char *destination,
		char *source );

/* Returns source_destination */
/* -------------------------- */
char *string_trim_trailing_character(
		char *source_destination,
		char character );

char *string_trim_right(
		char *source_destination,
		int length );

char *string_search_replace_special_characters(
		char *buffer );

/* Returns static memory */
/* --------------------- */
char *string_escape_single_quotes(
		char *source );

char *string_remove_thousands_separator(
		char *destination,
		char *source );

boolean string_is_number(
		char *string );

int string_instr_character(
		char character,
		char *string );

char *string_midstr(
		char *destination,
		char *source,
		int start,
		int how_many );

LIST *string_list(
		char *string );

/* Returns string */
/* -------------- */
char *string_character_fill(
		char *string /* in/out */,
		char character );

void string_free(
		char *string );

/* Returns string or component of string */
/* ------------------------------------- */
char *string_skip_prefix(
		const char *prefix,
		char *string );

boolean string_mnemonic_boolean(
		char *string );

boolean string_email_address_boolean(
		char *string );

/* Returns destination */
/* ------------------- */
char *string_left(
		char *destination,
		char *source,
		int how_many );

/* Returns destination */
/* ------------------- */
char *string_middle(
		char *destination,
		char *source,
		int start,
		int how_many );

/* Returns destination */
/* ------------------- */
char *string_substr(
		char *destination,
		char *source,
		int start,
		int how_many );

int string_strict_case_instr(
		char *substr,
		char *string,
		int occurrence );

int string_strict_case_strcmp(
		char *s1,
		char *s2 );

#endif
