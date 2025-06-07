/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_copy.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RELATION_COPY_H
#define RELATION_COPY_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "relation_mto1.h"
#include "process.h"

typedef struct
{
	LIST *folder_attribute_non_primary_name_list;
} RELATION_COPY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION_COPY *relation_copy_new(
		DICTIONARY *dictionary /* in/out */,
		LIST *folder_attribute_list,
		LIST *relation_mto1_list,
		int row_number );

/* Process */
/* ------- */
RELATION_COPY *relation_copy_calloc(
		void );

/* Usage */
/* ----- */
void relation_copy_set(
		DICTIONARY *dictionary /* in/out */,
		LIST *folder_attribute_non_primary_name_list,
		RELATION_MTO1 *relation_mto1,
		int row_number );

/* Usage */
/* ----- */
LIST *relation_copy_where_query_cell_list(
		DICTIONARY *dictionary,
		LIST *relation_foreign_key_list,
		LIST *folder_attribute_primary_key_list,
		int row_number );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_copy_system_string(
		const char sql_delimiter,
		LIST *select_name_list,
		char *folder_name,
		char *where_string );

/* Usage */
/* ----- */
LIST *relation_copy_result_query_cell_list(
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */

/* Returns query_cell_attribute_name or heap memory */
/* ------------------------------------------------ */
char *relation_copy_attribute_name(
		char *query_cell_attribute_name,
		int row_number );

#endif
