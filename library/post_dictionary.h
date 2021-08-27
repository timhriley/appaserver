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
	char *post_dictionary_apache_key;
} POST_DICTIONARY;

/* Operations */
/* ---------- */

/* Always succeeds */
/* --------------- */
POST_DICTIONARY *post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			char *appaserver_data_directory,
			char *session_key );

POST_DICTIONARY *post_dictionary_string_new(
			char *dictionary_string );

/* Returns heap memory or NULL */
/* --------------------------- */
char *post_dictionary_apache_key(
			char *input );

DICTIONARY *post_dictionary_fetch(
			FILE *stdin,
			char *appaserver_data_directory,
			char *session_key,
			char *apache_key );

/* Returns static memory */
/* --------------------- */
char *post_dictionary_apache_label(
			char *input );

/* Returns static memory */
/* --------------------- */
char *post_dictionary_appaserver_key(
			char *input );

/* Returns static memory */
/* --------------------- */
char *post_dictionary_input_filename(
			char *input );

/* Returns static memory */
/* --------------------- */
char *post_dictionary_appaserver_key(
			char *input );

void post_dictionary_stream_set(
			DICTIONARY *dictionary,
			FILE *stdin,
			char *appaserver_key,
			char *apache_key );

/* Returns heap memory */
/* ------------------- */
char *post_dictionary_spool_filename(
			char *input_filename,
			char *appaserver_data_directory,
			char *session_key );

void post_dictionary_spool_file(
			FILE *stdin,
			char *apache_key,
			char *spool_filename );

POST_DICTIONARY *post_dictionary_calloc(
			void );

#endif
