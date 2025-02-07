/* $APPASERVER_HOME/library/post_dictionary.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_DICTIONARY_H
#define POST_DICTIONARY_H

#include <stdio.h>
#include "boolean.h"
#include "dictionary.h"

#define POST_DICTIONARY_SPOOL_PREFIX	"sspool_"

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
		char *datum );

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

/* Returns static memory */
/* --------------------- */
char *post_dictionary_attribute_name(
		char *input );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *post_dictionary_input_filename(
		LIST *upload_filename_list,
		char *input,
		char *post_dictionary_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_dictionary_spool_filename(
		char *application_name,
		char *upload_directory,
		char *post_dictionary_input_filename );

/* Usage */
/* ----- */

/* Example: BA|1 returns 1 and datum = BA */
/* -------------------------------------- */
int post_dictionary_row_number(
		char *datum /* in/out */,
		char widget_multi_left_right_delimiter );

/* Usage */
/* ----- */
void post_dictionary_spool_file(
		char *spool_filename,
		char *apache_key );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_DICTIONARY *post_dictionary_string_new(
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		char *dictionary_string );

#endif
