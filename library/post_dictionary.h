/* $APPASERVER_HOME/library/post_dictionary.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST_DICTIONARY_H
#define POST_DICTIONARY_H

/* Includes */
/* -------- */
#include <stdio.h>
#include "boolean.h"
#include "dictionary.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	DICTIONARY *dictionary;
	char *post_dictionary_key;
} POST_DICTIONARY;

/* Operations */
/* ---------- */

/* Always returns */
/* -------------- */
POST_DICTIONARY *post_dictionary_fetch(
			FILE *input_stream,
			char *dictionary_string,
			char *appaserver_data_directory,
			char *session_key );

POST_DICTIONARY *post_dictionary_calloc(
			void );

DICTIONARY *post_dictionary_input_stream(
			FILE *input_stream,
			char *appaserver_data_directory,
			char *session_key );

char *post_dictionary_key(
			char *input );

void post_dictionary_line_input(
			DICTIONARY *dictionary,
			char *input,
			char *post_dictionary_key,
			char *appaserver_data_directory,
			char *session_key,
			FILE *input_stream );

char *post_dictionary_spool_filename(
			char *appaserver_data_directory,
			char *base_filename,
			char *session_key );

void post_dictionary_spool_file(
			char *spool_filename,
			FILE *input_stream,
			char *post_dictionary_key );

void post_dictionary_input(
			char *data,
			FILE *input_stream,
			char *post_dictionary_key );

void post_dictionary_set(
			DICTIONARY *dictionary,
			char *key,
			char *data );

#endif
