/* $APPASERVER_HOME/library/post_dictionary.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_DICTIONARY_H
#define POST_DICTIONARY_H

#include <stdio.h>
#include "boolean.h"
#include "dictionary.h"
#include "filename.h"

#define POST_DICTIONARY_FILE_NAME_MAXSIZE	80

typedef struct
{
char *post_dictionary_attribute_datum;
	FILENAME *filename;
	char *post_dictionary_file_specification;
	char *attribute_name;
} POST_DICTIONARY_RECALL;

/* Usage */
/* ----- */
POST_DICTIONARY_RECALL *post_dictionary_recall_new(
		char *widget_upload_recall_prefix,
		const int post_dictionary_file_name_maxsize,
		char *application_name,
		char *upload_directory,
		char *post_dictionary_apache_key,
		char *post_dictionary_attribute_name );

/* Process */
/* ------- */
boolean post_dictionary_recall_boolean(
		char *widget_upload_recall_prefix,
		char *post_dictionary_attribute_name );

POST_DICTIONARY_RECALL *post_dictionary_recall_calloc(
		void );

/* Usage */
/* ----- */

/* Returns component of parameter */
/* ------------------------------ */
char *post_dictionary_recall_attribute_name(
		char *widget_upload_recall_prefix,
		char *post_dictionary_attribute_name );

typedef struct
{
	char *name_string;
	FILENAME *filename;
	char *specification;
} POST_DICTIONARY_FILE;

/* Usage */
/* ----- */
POST_DICTIONARY_FILE *post_dictionary_file_new(
		const int post_dictionary_file_name_maxsize,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *post_dictionary_apache_key,
		char *input,
		char *post_dictionary_attribute_name );

/* Process */
/* ------- */
POST_DICTIONARY_FILE *post_dictionary_file_calloc(
		void );

/* Usage */
/* ----- */
boolean post_dictionary_file_boolean(
		LIST *upload_filename_list,
		char *post_dictionary_attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_file_specification(
		char *application_name,
		char *upload_directory,
		char *filename_return_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *post_dictionary_file_name_string(
		char *input );

/* Usage */
/* ----- */
void post_dictionary_file_write(
		char *post_dictionary_apache_key,
		char *post_dictionary_file_specification );

typedef struct
{
	char *name;
	POST_DICTIONARY_FILE *post_dictionary_file;
	POST_DICTIONARY_RECALL *post_dictionary_recall;
	char *datum;
} POST_DICTIONARY_ATTRIBUTE;

/* Usage */
/* ----- */
POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute_new(
		const char *widget_select_operator,
		char *widget_upload_recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *post_dictionary_apache_key );

/* Process */
/* ------- */
POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *post_dictionary_attribute_apache_marker(
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
char *post_dictionary_attribute_datum(
		const char *widget_select_operator,
		FILE *input_stream,
		char *post_dictionary_apache_key );

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
		char *widget_upload_recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list );

/* Process */
/* ------- */
POST_DICTIONARY *post_dictionary_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or NULL */
/* --------------------------- */
char *post_dictionary_apache_key(
		char *input );

/* Usage */
/* ----- */
void post_dictionary_datum_set(
		const char widget_multi_left_right_delimiter,
		DICTIONARY *original_post_dictionary /* out */,
		char *post_dictionary_attribute_name,
		char *post_dictionary_attribute_datum /* in/out */ );

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
		const char *dictionary_attribute_datum_delimiter,
		const char *dictionary_element_delimiter,
		char *dictionary_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_row_attribute_name(
		char *post_dictionary_attribute_name,
		int post_dictionary_row_number );

#endif
