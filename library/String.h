/* $APPASERVER_HOME/library/string.h			   */
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

#endif
