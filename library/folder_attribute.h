/* $APPASERVER_HOME/library/folder_attribute.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_ATTRIBUTE_H
#define FOLDER_ATTRIBUTE_H

/* Includes */
/* -------- */
#include "list.h"
#include "boolean.h"
#include "attribute.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define FOLDER_ATTRIBUTE_TABLE	"folder_attribute"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	char *attribute_name;
	int primary_key_index;
	int display_order;
	boolean omit_insert;
	boolean omit_insert_prompt;
	boolean omit_update;
	boolean additional_unique_index;
	boolean additional_index;
	boolean insert_required;
	boolean lookup_required;

	/* Process */
	/* ------- */
	ATTRIBUTE *attribute;
} FOLDER_ATTRIBUTE;

/* Operations */
/* ---------- */
FOLDER_ATTRIBUTE *folder_attribute_new(
			char *folder_name,
			char *attribute_name );

LIST *folder_attribute_list(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list );

/* Returns static memory */
/* --------------------- */
char *folder_attribute_where(
			char *folder_name );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_system_string(
			char *where );

FOLDER_ATTRIBUTE *folder_attribute_parse(
			char *input );

#endif
