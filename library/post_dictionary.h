/* $APPASERVER_HOME/library/post_dictionary.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_DICTIONARY_H
#define POST_DICTIONARY_H

#include <stdio.h>
#include "boolean.h"
#include "dictionary.h"

#define POST_DICTIONARY_FILE_NAME_PREFIX	"post_dictionary_file"

typedef struct
{
	char *date_name;
	char *specification;
	char *specification_key;
} POST_DICTIONARY_FILE;

/* Usage */
/* ----- */
POST_DICTIONARY_FILE *post_dictionary_file_new(
		const char *security_escape_character_string,
		char *application_name,
		char *upload_directory,
		char *post_dictionary_apache_key,
		char *post_dictionary_attribute_name,
		char *post_dictionary_file_datum );

/* Process */
/* ------- */
POST_DICTIONARY_FILE *post_dictionary_file_calloc(
		void );

/* Returns heap memory */
/* ------------------ */
char *post_dictionary_file_specification_key(
		const char *post_dictionary_file_name_prefix,
		char *post_dictionary_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_dictionary_file_clean_name(
		const char *security_escape_character_string,
		char *post_dictionary_file_datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_file_date_name(
		char *post_dictionary_file_clean_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_file_specification(
		char *application_name,
		char *upload_directory,
		char *post_dictionary_file_date_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *post_dictionary_file_datum(
		LIST *upload_filename_list,
		char *post_dictionary_attribute_name,
		char *input );

/* Usage */
/* ----- */
void post_dictionary_file_write(
		char *post_dictionary_apache_key,
		char *post_dictionary_file_specification );

typedef struct
{
	char *apache_key;
	DICTIONARY *original_post_dictionary;
} POST_DICTIONARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_DICTIONARY *post_dictionary_stdin_new(
		/* ---------------------------------- */
		/* Used when expecting a spooled file */
		/* ---------------------------------- */
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list );

/* Process */
/* ------- */
POST_DICTIONARY *post_dictionary_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DICTIONARY *post_dictionary_fetch(
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *post_dictionary_apache_key );

/* Usage */
/* ----- */
void post_dictionary_datum_set(
		DICTIONARY *original_post_dictionary /* out */,
		char *post_dictionary_attribute_name,
		char *post_dictionary_datum );

/* Usage */
/* ----- */

/* Returns heap memory or NULL */
/* --------------------------- */
char *post_dictionary_apache_key(
		char *input );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_dictionary_apache_label(
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_attribute_name(
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *post_dictionary_datum(
		const char *widget_select_operator,
		FILE *input_stream,
		char *post_dictionary_apache_key );

/* Usage */
/* ----- */

/* Example: BA|1 returns 1 and datum = BA */
/* -------------------------------------- */
int post_dictionary_row_number(
		char *datum /* in/out */,
		char widget_multi_left_right_delimiter );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_DICTIONARY *post_dictionary_string_new(
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		char *dictionary_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_row_attribute_name(
		char *post_dictionary_attribute_name,
		int post_dictionary_row_number );

#endif
