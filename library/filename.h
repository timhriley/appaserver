/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/filename.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FILENAME_H
#define FILENAME_H

#include <stdio.h>
#include "boolean.h"

#define FILENAME_APPEND_DATE_LENGTH	9

typedef struct
{
	char *extension;
	int extension_length;
	char *basename;
	char *clean;
	int clean_length;
	int string_left_size;
	char *trim;
	char *date;
	char *return_string /* heap memory */;
} FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILENAME *filename_new(
		char *filename_string,
		int filename_max_size,
		boolean append_date_boolean );

/* Process */
/* ------- */
FILENAME *filename_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *filename_extension(
		char *filename_string );

int filename_extension_length(
		char *filename_extension );

/* Returns static memory */
/* --------------------- */
char *filename_basename(
		char *filename_string );

/* Returns static memory */
/* --------------------- */
char *filename_clean(
		const char *security_escape_character_string,
		char *filename_basename );

int filename_clean_length(
		char *filename_clean );

int filename_string_left_size(
		const int filename_append_date_length,
		int name_max_size,
		boolean append_date_boolean,
		int extension_length );

/* Returns heap memory */
/* ------------------- */
char *filename_trim(
		char *filename_clean,
		int filename_string_left_size );

/* Returns parameter or heap memory */
/* -------------------------------- */
char *filename_date(
		boolean append_date_boolean,
		char *filename_trim );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *filename_return_string(
		char *filename_extension /* static memory */,
		char *filename_date );

#endif
