/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/optional_column.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef OPTIONAL_COLUMN_H
#define OPTIONAL_COLUMN_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"

typedef struct
{
	/* Heap memory */
	/* ----------- */
	char *return_string;
	char *prior_return_string;
} OPTIONAL_COLUMN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
OPTIONAL_COLUMN *optional_column_new(
		const char delimiter,
		char *base_string,
		char *component /* column or datum */,
		boolean escape_boolean,
		boolean set_boolean );

/* Process */
/* ------- */
OPTIONAL_COLUMN *optional_column_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *optional_column_return_string(
		const char delimiter,
		char *base_string,
		char *component );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
OPTIONAL_COLUMN *optional_column_money_new(
		const char delimiter,
		char *base_string,
		double money,
		boolean set_boolean );

#endif
